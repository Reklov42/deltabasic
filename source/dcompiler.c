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

#define CompileLineParseAssert() { if (delta_Parse(&lexem) != PARSE_OK) return DELTA_SYNTAX_ERROR; }
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
static size_t GetMathPriority(delta_TChar ch);

// ------------------------------------------------------------------------- //

/* ====================================
 * CompileNumericMath
 */
static delta_EStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC);

/* ====================================
 * CollapseNumericMath
 */
static delta_EStatus CollapseNumericMath(delta_SState* D, delta_TChar* ops, size_t* nOps, delta_SBytecode* BC);

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

#include <stdio.h>

/* ====================================
 * delta_CompileLine
 */
delta_EStatus delta_CompileLine(delta_SState* D, delta_SLine* L, delta_SBytecode* BC) {
	L->offset = BC->index;

	const char* str = L->str;


	delta_SLexerState lexem = { 0 };
	lexem.buffer = L->str;
	
	CompileLineParseAssert();
	if (lexem.type == LEXEM_OP) {
		if (lexem.op == OP_LET) {
			CompileLineParseAssert();
			if (lexem.type != LEXEM_NAME)
				return DELTA_SYNTAX_ERROR;

			delta_SLexemString name = lexem.string;
			CompileLineParseAssert();

			if (lexem.type != LEXEM_SYMBOL)
				return DELTA_SYNTAX_ERROR;

			if (lexem.symbol == '$') { // String
				CompileLineParseAssert();

				if (lexem.type != LEXEM_SYMBOL)
					return DELTA_SYNTAX_ERROR;

				if (lexem.symbol == '=') { // Assignment

				}
				else if (lexem.symbol == '(') { // TODO: function

				}
				else
					return DELTA_SYNTAX_ERROR;
			}
			else if (lexem.symbol == '=') { // Assignment
				StatusAssert(CompileNumericMath(D, &lexem, BC));

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else if (lexem.symbol == '(') { // TODO: function

			}
			else
				return DELTA_SYNTAX_ERROR;
		}
	}
	else if (lexem.type == LEXEM_NAME) {
		delta_SLexemString name = lexem.string;

	}

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * CompileNumericMath
 */
delta_EStatus CompileNumericMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC) {
	delta_TChar ops[DELTABASIC_COMPILER_MAX_MATH_OPS];
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
				
				if (nOps != 0) { // Pop operations
					if (GetMathPriority(symbol) < GetMathPriority(ops[nOps - 1])) {
						StatusAssert(CollapseNumericMath(D, ops, &nOps, BC));
					}
				}

				ops[nOps++] = symbol;
			}
			else if (symbol == '(') {
				ops[nOps++] = '(';
			}
			else if (symbol == ')') {
				if (GetMathPriority(symbol) < GetMathPriority(ops[nOps - 1])) {
					StatusAssert(CollapseNumericMath(D, ops, &nOps, BC));
				}

				//lastLexemType = (nVariables != 0) ? ELexemType::INTEGER : ELexemType::KEYSYMBOL;
				continue;
			}
			else if ((symbol == ':') || (symbol == ',')) {
				break;
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if ((L->type == LEXEM_INTEGER) || (L->type == LEXEM_FLOAT)) {
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
		else if (L->type == LEXEM_NAME) { // TODO:
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
delta_EStatus CollapseNumericMath(delta_SState* D, delta_TChar* ops, size_t* nOps, delta_SBytecode* BC) {
	while (*nOps != 0) {
		const delta_TChar symbolOnTop = ops[*nOps - 1];
		--(*nOps);
		if (symbolOnTop == '(')
			break;

		delta_EOpcodes opcode;
		switch (symbolOnTop) {
			case '+': opcode = OPCODE_ADD; break;
			case '-': opcode = OPCODE_SUB; break;
			case '*': opcode = OPCODE_MUL; break;
			case '/': opcode = OPCODE_DIV; break;
			case '^': opcode = OPCODE_POW; break;
			case '%': opcode = OPCODE_MOD; break;

		/*
			case EKeysymbol::EQUAL:					EmplaceOp(EOpCode::ET);		break;
			case EKeysymbol::NOT_EQUAL:				EmplaceOp(EOpCode::NET);	break;
			case EKeysymbol::LESS_THAN:				EmplaceOp(EOpCode::LT);		break;
			case EKeysymbol::GREATER_THAN:			EmplaceOp(EOpCode::GT);		break;
			case EKeysymbol::LESS_THAN_OR_EQUAL:	EmplaceOp(EOpCode::LET);	break;
			case EKeysymbol::GREATER_THAN_OR_EQUAL:	EmplaceOp(EOpCode::GET);	break;
		*/
			default:
				return DELTA_SYNTAX_ERROR;
		}

		PushAssert(PushBytecodeByte(D, BC, opcode));
	}

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

	return dfalse;
}

/* ====================================
 * GetMathPriority
 */
inline size_t GetMathPriority(delta_TChar ch) {
	if (ch == '+') return 1;
	if (ch == '-') return 1;

	if (ch == '*') return 2;
	if (ch == '/') return 2;

	if (ch == '^') return 4;
	if (ch == '%') return 3;

	return 0;
}
