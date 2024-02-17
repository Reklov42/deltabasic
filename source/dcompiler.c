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
 * CompileNumericMath
 */
static delta_EStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC);

/* ====================================
 * CollapseNumericMath
 */
static delta_EStatus CollapseNumericMath(delta_SState* D, delta_EOpcodes* ops, size_t* nOps, delta_SBytecode* BC);

/* ====================================
 * CompileStringMath
 */
static delta_EStatus CompileStringMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC);

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

	if (bc.bytecode[bc.index - 1] != OPCODE_HLT)
		PushBytecodeByte(D, &bc, OPCODE_HLT);

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

				}
				else
					return DELTA_SYNTAX_ERROR;
			}
			else if (L->symbol == '=') { // Numeric Assignment
				StatusAssert(CompileNumericMath(D, L, BC));

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->op == OP_PRINT) {
			while (dtrue) {
				const delta_TChar* head = L->head;
				delta_EStatus status = CompileNumericMath(D, L, BC);
				if (status == DELTA_ALLOCATOR_ERROR)
					return DELTA_ALLOCATOR_ERROR;

				if (status == DELTA_OK) {
					if (L->type == LEXEM_EOL) {
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTNT));
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));
						break;
					} else if (L->type == LEXEM_SYMBOL) {
						if (L->symbol == ',') {
							PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTNT));
						} else if (L->symbol == ';') {
							PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTN));
						} else if (L->symbol == ':') {
							PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTNT));
							PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));
							break;
						} else
							return DELTA_SYNTAX_ERROR;
					}
				}
				else {
					L->head = head;
				}
			}
		}
		else if (L->op == OP_END) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_HLT));
		}
		else
			return DELTA_SYNTAX_ERROR;
	}
	else if (L->type == LEXEM_NAME) {
		delta_SLexemString name = L->string;

	}
	else
		return DELTA_SYNTAX_ERROR;

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * CompileNumericMath
 */
delta_EStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC) {
	delta_EOpcodes ops[DELTABASIC_COMPILER_MAX_MATH_OPS];
	size_t nOps = 0;

	delta_TBool bUnaryMinus = dfalse;
	delta_ELexemType lastLexemType = LEXEM_SYMBOL;

	size_t nVariables = 0;
	while (dtrue) {
		ParseAssert();

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
								}
							}
						}
				};
				
				if (nOps != 0) { // Pop operations
					if (GetMathPriority(opcode) < GetMathPriority(ops[nOps - 1])) {
						StatusAssert(CollapseNumericMath(D, ops, &nOps, BC));
					}
				}

				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;

				ops[nOps++] = opcode;
			}
			
			if (symbol == '(') {
				if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
					return DELTA_ALLOCATOR_ERROR;
					
				ops[nOps++] = '(';
			}
			else if (symbol == ')') {
				if (GetMathPriority(symbol) < GetMathPriority(ops[nOps - 1])) {
					StatusAssert(CollapseNumericMath(D, ops, &nOps, BC));
				}

				//lastLexemType = (nVariables != 0) ? ELexemType::INTEGER : ELexemType::KEYSYMBOL;
				continue;
			}
			else if ((symbol == ':') || (symbol == ',') || (symbol == ';')) {
				break;
			}
		}
		
		if ((L->type == LEXEM_INTEGER) || (L->type == LEXEM_FLOAT)) {
			delta_TNumber number = (L->type == LEXEM_INTEGER) ? (delta_TNumber)(L->integerValue) : (delta_TNumber)(L->floatValue);
			if (bUnaryMinus == dtrue)
				number = -number;
			
			PushAssert(PushBytecodeByte(D, BC, OPCODE_PUSHN));
			PushAssert(PushBytecodeNumber(D, BC, number));

			++nVariables;
			bUnaryMinus = dfalse;
		}
		else if (L->type == LEXEM_STRING) {
			return DELTA_SYNTAX_ERROR;
		}
		else if (L->type == LEXEM_NAME) { // TODO: functions
			PushAssert(PushBytecodeByte(D, BC, OPCODE_GETN));
			PushAssert(PushBytecodeWord(D, BC, L->string.offset));
			PushAssert(PushBytecodeWord(D, BC, L->string.size));

			if (bUnaryMinus == dtrue)
				PushAssert(PushBytecodeByte(D, BC, OPCODE_NEG));
			
			bUnaryMinus = dfalse;
		}
		else if (L->type == LEXEM_EOL) {
			break;
		}

		lastLexemType = L->type;
	}
	
	StatusAssert(CollapseNumericMath(D, ops, &nOps, BC));

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
 * CompileNumericMath
 */
delta_EStatus CollapseNumericMath(delta_SState* D, delta_EOpcodes* ops, size_t* nOps, delta_SBytecode* BC) {
	while (*nOps != 0) {
		const delta_EOpcodes opcodeOnTop = ops[*nOps - 1];
		--(*nOps);
		if (opcodeOnTop == '(')
			break;

		PushAssert(PushBytecodeByte(D, BC, opcodeOnTop));
	}

	return DELTA_OK;
}


/* ====================================
 * CompileNumericMath
 */
delta_EStatus CompileStringMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC) {
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
