//

//	| File:			dcompiler.h
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dcompiler.h"

#include <string.h>

#include "dlexer.h"
#include "dmemory.h"
#include "dopcodes.h"

#define CompileInstructionParseAssert() { if (delta_Parse(L) != PARSE_OK) return DELTA_SYNTAX_ERROR; }
#define ParseAssert() { if (delta_Parse(L) != PARSE_OK) return DELTA_SYNTAX_ERROR; }
#define StatusAssert(exp) { delta_EStatus status = (exp); if (status != DELTA_OK) { return status; }}
#define PushAssert(exp) { if ((exp) == dfalse) { return DELTA_ALLOCATOR_ERROR; }}

					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * PushBytecodeByte
 */
static delta_TBool	PushBytecodeByte(delta_SState* D, delta_SBytecode* BC, delta_TByte byte);

/* ====================================
 * PushBytecodeWord
 */
static delta_TBool	PushBytecodeWord(delta_SState* D, delta_SBytecode* BC, delta_TWord word);

/* ====================================
 * PushBytecodeDoubleWord
 */
static delta_TBool	PushBytecodeDWord(delta_SState* D, delta_SBytecode* BC, delta_TDWord dword);

/* ====================================
 * PushBytecodeNumber
 */
static delta_TBool	PushBytecodeNumber(delta_SState* D, delta_SBytecode* BC, delta_TNumber number);

/* ====================================
 * ExpandBytecodeBuffer
 */
static delta_TBool	ExpandBytecodeBuffer(delta_SState* D, delta_SBytecode* BC);

// ------------------------------------------------------------------------- //

/* ====================================
 * IsMathSymbol
 */
static delta_TBool IsMathSymbol(delta_TChar ch);

/* ====================================
 * GetMathPriority
 */
static size_t GetMathPriority(delta_EOpcodes op);

// ------------------------------------------------------------------------- //

/* ====================================
 * CompileInstruction
 */
static delta_EStatus CompileInstruction(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC);

/* ====================================
 * delta_EMathStatus
 */
typedef enum {
	MATH_UNEXPECTED_CLOSING_BRACKET = DELTA_MATH_STATUS,
	MATH_NOT_ENOUGH_VALUES,
} delta_EMathStatus;

/* ====================================
 * CompileNumericMath
 */
static delta_EMathStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, size_t* outVariables);

/* ====================================
 * CollapseMath
 */
static delta_EMathStatus CollapseMath(delta_SState* D, delta_EOpcodes* ops, size_t* nOps, delta_SBytecode* BC, size_t* outVariables);

/* ====================================
 * CompileStringMath
 */
static delta_EMathStatus CompileStringMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, size_t* outVariables);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Compile
 */
delta_EStatus delta_Compile(delta_SState* D) {
	if (D->head == NULL)
		return DELTA_OK;

	D->ip			= 0;
	D->currentLine	= NULL;

	delta_SBytecode bc;
	bc.bytecodeSize	= D->bytecodeSize;
	bc.index		= DELTABASIC_EXEC_BYTECODE_SIZE;
	bc.bytecode		= D->bytecode;
	bc.bCanResize	= dtrue;

	for (delta_SLine* node = D->head; node != NULL; node = node->next) {
		delta_EStatus status = delta_CompileLine(D, node, &bc);
		if (status != DELTA_OK)
			return status;
	}

	if (bc.bytecode[bc.index - 1] != OPCODE_HLT) {
		PushAssert(PushBytecodeByte(D, &bc, OPCODE_HLT));
	}

	D->bytecodeSize	= bc.bytecodeSize;
	D->bytecode		= bc.bytecode;
	D->ip			= DELTABASIC_EXEC_BYTECODE_SIZE;
	D->currentLine	= D->head;

	return DELTA_OK;
}

/* ====================================
 * delta_CompileLine
 */
delta_EStatus delta_CompileLine(delta_SState* D, delta_SLine* L, delta_SBytecode* BC) {
	L->offset = BC->index;

	delta_SLexerState lexem = { 0 };
	lexem.buffer = L->str;

	while (lexem.type != LEXEM_EOL) {
		if (delta_Parse(&lexem) != PARSE_OK)
			return DELTA_SYNTAX_ERROR;

		delta_EStatus status = CompileInstruction(D, &lexem, BC);
		if (status != DELTA_OK)
			return status;

		if (lexem.type == LEXEM_SYMBOL) {
			if (lexem.symbol != ':')
				return DELTA_SYNTAX_ERROR;
		}
	}

	PushAssert(PushBytecodeByte(D, BC, OPCODE_NEXTL));
	return DELTA_OK;
}

/* ====================================
 * CompileNumericMath
 */
delta_EStatus CompileInstruction(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC) {
	if (L->type == LEXEM_OP) {
		if (L->op == OP_LET) {
			CompileInstructionParseAssert();
			if (L->type != LEXEM_NAME)
				return DELTA_SYNTAX_ERROR;

			delta_SLexemString name = L->string;
			CompileInstructionParseAssert();

			if (L->type != LEXEM_SYMBOL)
				return DELTA_SYNTAX_ERROR;

			if (L->symbol == '$') { // String
				CompileInstructionParseAssert();

				if (L->type != LEXEM_SYMBOL)
					return DELTA_SYNTAX_ERROR;

				if (L->symbol == '=') { // String Assignment
					size_t nVariables = 0;
					StatusAssert(CompileStringMath(D, L, BC, &nVariables));
					if (nVariables != 1)
						return DELTA_SYNTAX_ERROR;

					PushAssert(PushBytecodeByte(D, BC, OPCODE_SETS));
					PushAssert(PushBytecodeWord(D, BC, name.offset));
					PushAssert(PushBytecodeWord(D, BC, name.size));
				}
				else
					return DELTA_SYNTAX_ERROR;
			}
			else if (L->symbol == '=') { // Numeric Assignment
				size_t nVariables = 0;
				StatusAssert(CompileNumericMath(D, L, BC, &nVariables));
				if (nVariables != 1)
					return DELTA_SYNTAX_ERROR;

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->op == OP_PRINT) {
			delta_TChar lastSymbol = ' ';
			while (dtrue) {
				delta_TBool bNumeric = dtrue;
				size_t nVariables = 0;
				{
					const delta_TChar* head = L->head;
					delta_EStatus status = CompileNumericMath(D, L, BC, &nVariables);
					if (status == DELTA_ALLOCATOR_ERROR)
						return DELTA_ALLOCATOR_ERROR;

					if (status == DELTA_OK) {
						if (nVariables > 1)
							return DELTA_SYNTAX_ERROR;
					}
					else {
						bNumeric = dfalse;

						L->head = head;
						delta_EStatus status = CompileStringMath(D, L, BC, &nVariables);
						if (status != DELTA_OK)
							return status;
						
						if (nVariables > 1)
							return DELTA_SYNTAX_ERROR;
					}
				}

				if (L->type == LEXEM_EOL) {
					if (nVariables != 0)
						PushAssert(PushBytecodeByte(D, BC, bNumeric ? OPCODE_PRINTNT : OPCODE_PRINTST));
					
					if ((lastSymbol != ';') || (nVariables != 0))
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));

					break;
				} else if (L->type == LEXEM_SYMBOL) {
					if (nVariables != 0) {
						if (L->symbol == ',') {
							PushAssert(PushBytecodeByte(D, BC, bNumeric ? OPCODE_PRINTNT : OPCODE_PRINTST));
						} else if (L->symbol == ';') {
							PushAssert(PushBytecodeByte(D, BC, bNumeric ? OPCODE_PRINTN : OPCODE_PRINTS));
						} else if (L->symbol == ':') {
							PushAssert(PushBytecodeByte(D, BC, bNumeric ? OPCODE_PRINTNT : OPCODE_PRINTST));
						}
					}

					if (L->symbol == ':') {
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));
						break;
					}

					lastSymbol = L->symbol;
				} else
					return DELTA_SYNTAX_ERROR;
			}
		}
		else if (L->op == OP_END) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_HLT));
		}
		else if (L->op == OP_STOP) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_STOP));
		}
		else
			return DELTA_SYNTAX_ERROR;
	}
	else if (L->type == LEXEM_NAME) {
		delta_SLexemString name = L->string;
		CompileInstructionParseAssert();

		if (L->type != LEXEM_SYMBOL)
			return DELTA_SYNTAX_ERROR;

		if (L->symbol == '$') { // String
			CompileInstructionParseAssert();

			if (L->type != LEXEM_SYMBOL)
				return DELTA_SYNTAX_ERROR;

			if (L->symbol == '=') { // String Assignment
				size_t nVariables = 0;
				StatusAssert(CompileStringMath(D, L, BC, &nVariables));
				if (nVariables != 1)
					return DELTA_SYNTAX_ERROR;

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETS));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else if (L->symbol == '(') { // String Function call

			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->symbol == '=') { // Numeric Assignment
			size_t nVariables = 0;
			StatusAssert(CompileNumericMath(D, L, BC, &nVariables));
			if (nVariables != 1)
				return DELTA_SYNTAX_ERROR;

			PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
			PushAssert(PushBytecodeWord(D, BC, name.offset));
			PushAssert(PushBytecodeWord(D, BC, name.size));
		}
		else if (L->symbol == '(') { // Numeric Function call

		}
		else
			return DELTA_SYNTAX_ERROR;
	}
	else if (L->type == LEXEM_EOL) {
	}
	else
		return DELTA_SYNTAX_ERROR;

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * CompileNumericMath
 */
delta_EMathStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, size_t* outVariables) {
	delta_EOpcodes ops[DELTABASIC_COMPILER_MAX_MATH_OPS];
	size_t nOps = 0;

	delta_TBool bUnaryMinus = dfalse;
	delta_ELexemType lastLexemType = LEXEM_SYMBOL;

	*outVariables = 0;
	size_t nBrackets = 0;
	delta_TBool bSkipParse = dfalse;
	while (dtrue) {
		if (bSkipParse == dfalse) {
			ParseAssert();
		}
		else
			bSkipParse = dfalse;

		if (L->type == LEXEM_SYMBOL) {
			const delta_TChar symbol = L->symbol;
			if (IsMathSymbol(symbol) == dtrue) {
				if (lastLexemType == LEXEM_SYMBOL) { // Unary
					if (symbol == '-') {
						bUnaryMinus = dtrue;
						continue;
					}
					else if (symbol == '+') 
						continue;

					return DELTA_SYNTAX_ERROR;
				}

				delta_EOpcodes opcode;
				switch (symbol) {
					case '+': opcode = OPCODE_ADD; break;
					case '-': opcode = OPCODE_SUB; break;
					case '*': opcode = OPCODE_MUL; break;
					case '/': opcode = OPCODE_DIV; break;
					case '^': opcode = OPCODE_POW; break;
					case '%': opcode = OPCODE_MOD; break;
					case '=': opcode = OPCODE_ET;  break;
					default:
						if ((symbol == '<') || (symbol == '>')) {
							ParseAssert();
							bSkipParse = dtrue;
							if (L->type != LEXEM_SYMBOL) {
								if (symbol == '<') opcode = OPCODE_LT;
								else if (symbol == '>') opcode = OPCODE_GT;
							}
							else {
								if ((symbol == '<') && (L->symbol == '>')) opcode = OPCODE_NET;
								else if ((symbol == '<') && (L->symbol == '=')) opcode = OPCODE_LET;
								else if ((symbol == '>') && (L->symbol == '=')) opcode = OPCODE_GET;
								else if (L->symbol == '-') {
									bUnaryMinus = dtrue;
									ParseAssert();
									bSkipParse = dtrue;
								}
							}
						}
				};
				
				if (nOps != 0) { // Pop operations
					if (GetMathPriority(opcode) < GetMathPriority(ops[nOps - 1])) {
						StatusAssert(CollapseMath(D, ops, &nOps, BC, outVariables));
					}
				}

				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;

				ops[nOps++] = opcode;
			}
			else if (symbol == '(') {
				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;
				
				++nBrackets;
				ops[nOps++] = '(';
			}
			else if (symbol == ')') {
				if (GetMathPriority(symbol) < GetMathPriority(ops[nOps - 1])) {
					StatusAssert(CollapseMath(D, ops, &nOps, BC, outVariables));
				}

				if (nBrackets == 0)
					return MATH_UNEXPECTED_CLOSING_BRACKET;

				--nBrackets;
				//lastLexemType = (nVariables != 0) ? ELexemType::INTEGER : ELexemType::KEYSYMBOL;
				continue;
			}
			else if ((symbol == ':') || (symbol == ',') || (symbol == ';')) {
				break;
			}
			else
				DELTA_SYNTAX_ERROR;
		}
		else if ((L->type == LEXEM_INTEGER) || (L->type == LEXEM_FLOAT)) {
			delta_TNumber number = (L->type == LEXEM_INTEGER) ? (delta_TNumber)(L->integerValue) : (delta_TNumber)(L->floatValue);
			if (bUnaryMinus == dtrue)
				number = -number;
			
			PushAssert(PushBytecodeByte(D, BC, OPCODE_PUSHN));
			PushAssert(PushBytecodeNumber(D, BC, number));

			++(*outVariables);
			bUnaryMinus = dfalse;
		}
		else if (L->type == LEXEM_STRING) {
			return DELTA_SYNTAX_ERROR;
		}
		else if (L->type == LEXEM_NAME) { // TODO: functions
			delta_SLexemString str = L->string;
			ParseAssert();
			bSkipParse = dtrue;
			if ((L->type == LEXEM_SYMBOL) && (L->symbol == '$'))
				return DELTA_SYNTAX_ERROR;

			PushAssert(PushBytecodeByte(D, BC, OPCODE_GETN));
			PushAssert(PushBytecodeWord(D, BC, str.offset));
			PushAssert(PushBytecodeWord(D, BC, str.size));

			if (bUnaryMinus == dtrue)
				PushAssert(PushBytecodeByte(D, BC, OPCODE_NEG));
			
			++(*outVariables);
			bUnaryMinus = dfalse;
		}
		else if (L->type == LEXEM_OP)
			return DELTA_SYNTAX_ERROR;
		else if (L->type == LEXEM_EOL) {
			break;
		}

		lastLexemType = L->type;
	}
	
	StatusAssert(CollapseMath(D, ops, &nOps, BC, outVariables));

/*
	if (nVariables != 1) {
		if (nVariables > 1)
			PRINT_ERROR("Too many unnecessary values (%zu)\n", nVariables);
		else if (bZeroOutError == true)
			PRINT_ERROR("Expression does not contain a resultant value\n");

		return false;
	}
*/

	return DELTA_OK;
}

/* ====================================
 * CollapseMath
 */
delta_EMathStatus CollapseMath(delta_SState* D, delta_EOpcodes* ops, size_t* nOps, delta_SBytecode* BC, size_t* outVariables) {
	while (*nOps != 0) {
		const delta_EOpcodes opcodeOnTop = ops[*nOps - 1];
		--(*nOps);
		if (opcodeOnTop == '(')
			break;

		if (*outVariables == 0)
			return MATH_NOT_ENOUGH_VALUES;

		--(*outVariables);
		PushAssert(PushBytecodeByte(D, BC, opcodeOnTop));
	}

	return DELTA_OK;
}


/* ====================================
 * CompileStringMath
 */
delta_EMathStatus CompileStringMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, size_t* outVariables) {
	delta_EOpcodes ops[DELTABASIC_COMPILER_MAX_MATH_OPS];
	size_t nOps = 0;

	*outVariables = 0;
	delta_ELexemType lastLexemType = LEXEM_SYMBOL;

	while (dtrue) {
		ParseAssert();

		if (L->type == LEXEM_SYMBOL) {
			const delta_TChar symbol = L->symbol;
			if (IsMathSymbol(symbol) == dtrue) {
				if (symbol != '+')
					return DELTA_SYNTAX_ERROR;

				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;

				ops[nOps++] = OPCODE_CONCAT;
			}
			
			if (symbol == '(') {
				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;
					
				ops[nOps++] = '(';
			}
			else if (symbol == ')') {
				if (GetMathPriority(symbol) < GetMathPriority(ops[nOps - 1])) {
					StatusAssert(CollapseMath(D, ops, &nOps, BC, outVariables));
				}

				//lastLexemType = (nVariables != 0) ? ELexemType::INTEGER : ELexemType::KEYSYMBOL;
				continue;
			}
			else if ((symbol == ':') || (symbol == ',') || (symbol == ';')) {
				break;
			}
			else
				DELTA_SYNTAX_ERROR;
		}
		else if ((L->type == LEXEM_INTEGER) || (L->type == LEXEM_FLOAT)) {
			return DELTA_SYNTAX_ERROR;
		}
		else if (L->type == LEXEM_STRING) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_PUSHS));
			PushAssert(PushBytecodeWord(D, BC, L->string.offset));
			PushAssert(PushBytecodeWord(D, BC, L->string.size));
			
			++(*outVariables);
		}
		else if (L->type == LEXEM_NAME) { // TODO: functions
			delta_SLexemString str = L->string;
			ParseAssert();
			if (L->type != LEXEM_SYMBOL)
				return DELTA_SYNTAX_ERROR;

			if (L->symbol != '$')
				return DELTA_SYNTAX_ERROR;

			PushAssert(PushBytecodeByte(D, BC, OPCODE_GETS));
			PushAssert(PushBytecodeWord(D, BC, str.offset));
			PushAssert(PushBytecodeWord(D, BC, str.size));
			
			++(*outVariables);
		}
		else if (L->type == LEXEM_OP) {
			if (L->op == OP_TAB) {
				CompileInstructionParseAssert();

				if (L->type != LEXEM_SYMBOL)
					return DELTA_SYNTAX_ERROR;

				if (L->symbol == '(') { // String Assignment
					size_t nVariables = 0;
					delta_EMathStatus ms = CompileNumericMath(D, L, BC, &nVariables);
					if (nVariables != 1)
						return DELTA_SYNTAX_ERROR;

					if (ms != MATH_UNEXPECTED_CLOSING_BRACKET)
						return DELTA_SYNTAX_ERROR;
				}
				else
					return DELTA_SYNTAX_ERROR;
					
				++(*outVariables);
				PushAssert(PushBytecodeByte(D, BC, OPCODE_TAB));
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->type == LEXEM_EOL) {
			break;
		}

		lastLexemType = L->type;
	}
	
	StatusAssert(CollapseMath(D, ops, &nOps, BC, outVariables));

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * PushBytecodeByte
 */
delta_TBool PushBytecodeByte(delta_SState* D, delta_SBytecode* BC, delta_TByte byte) {
	if (BC->index + 1 >= BC->bytecodeSize) {
		if (ExpandBytecodeBuffer(D, BC) == dfalse)
			return dfalse;
	}

	BC->bytecode[BC->index] = byte;
	BC->index += 1;

	return dtrue;
}

/* ====================================
 * PushBytecodeWord
 */
inline delta_TBool PushBytecodeWord(delta_SState* D, delta_SBytecode* BC, delta_TWord word) {
	if (BC->index + 2 >= BC->bytecodeSize) {
		if (ExpandBytecodeBuffer(D, BC) == dfalse)
			return dfalse;
	}

	*((delta_TWord*)(BC->bytecode + BC->index)) = word;
	BC->index += 2;

	return dtrue;
}

/* ====================================
 * PushBytecodeDWord
 */
inline delta_TBool PushBytecodeDWord(delta_SState* D, delta_SBytecode* BC, delta_TDWord dword) {
	if (BC->index + 4 >= BC->bytecodeSize) {
		if (ExpandBytecodeBuffer(D, BC) == dfalse)
			return dfalse;
	}

	*((delta_TDWord*)(BC->bytecode + BC->index)) = dword;
	BC->index += 4;

	return dtrue;
}

/* ====================================
 * PushBytecodeNumber
 */
inline delta_TBool PushBytecodeNumber(delta_SState* D, delta_SBytecode* BC, delta_TNumber number) {
	return PushBytecodeDWord(D, BC, *((delta_TDWord*)(&number)));
}

/* ====================================
 * ExpandBytecodeBuffer
 */
inline delta_TBool ExpandBytecodeBuffer(delta_SState* D, delta_SBytecode* BC) {
	const size_t newSize = BC->bytecodeSize * 2;

	BC->bytecode = DELTA_Realloc(
		D,
		BC->bytecode,
		sizeof(delta_TByte) * BC->bytecodeSize,
		sizeof(delta_TByte) * newSize
	);

	if (BC->bytecode == NULL)
		return dfalse;

	BC->bytecodeSize = newSize;

	return dtrue;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * IsMathSymbol
 */
inline delta_TBool IsMathSymbol(delta_TChar ch) {
	if (ch == '+') return dtrue;
	if (ch == '-') return dtrue;

	if (ch == '*') return dtrue;
	if (ch == '/') return dtrue;

	if (ch == '^') return dtrue;
	if (ch == '%') return dtrue;

	if (ch == '=') return dtrue;
	if (ch == '<') return dtrue;
	if (ch == '>') return dtrue;

	return dfalse;
}

/* ====================================
 * GetMathPriority
 */
inline size_t GetMathPriority(delta_EOpcodes op) {
	switch (op) {
		case OPCODE_ET:
		case OPCODE_NET:
		case OPCODE_LT:
		case OPCODE_GT:
		case OPCODE_LET:
		case OPCODE_GET:
			return 1;
		case OPCODE_ADD:
		case OPCODE_SUB:
			return 2;
		case OPCODE_MUL:
		case OPCODE_DIV:
			return 3;
		case OPCODE_MOD:
		case OPCODE_POW:
			return 4;
		default:
			return 0;
	}
}
