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

#define DELTABASIC_COMPILER_MATH_WINDOW_SIZE				3

#define CompileInstructionParseAssert() { if (delta_Parse(L) != PARSE_OK) return DELTA_SYNTAX_ERROR; }
#define ParseAssert() { if (delta_Parse(L) != PARSE_OK) return DELTA_SYNTAX_ERROR; }
#define StatusAssert(exp) { delta_EStatus status = (delta_EStatus)(exp); if (status != DELTA_OK) { return status; }}
#define MathStatusAssert(exp, ret) { delta_EStatus status = (delta_EStatus)(exp); if (status != (delta_EStatus)ret) { return status; }}
#define PushAssert(exp) { if ((delta_TBool)(exp) == dfalse) { return DELTA_ALLOCATOR_ERROR; }}

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
typedef enum delta_EMathStatus {
	MATH_UNEXPECTED_CLOSING_BRACKET = DELTA_MATH_STATUS,
	MATH_OK_UNDEF,
	MATH_OK_STRING,
	MATH_OK_NUMERIC,
	MATH_NOT_ENOUGH_VALUES,
} delta_EMathStatus;

/* ====================================
 * CompileMathUnary
 */
delta_EMathStatus CompileMathUnary(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, delta_EMathStatus* mathStatus);

/* ====================================
 * CompileMath
 */
delta_EMathStatus CompileMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, delta_EMathStatus startingMathStatus);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Compile
 */
delta_EStatus delta_Compile(delta_SState* D) {
	if (D->head == NULL) {
		D->bCompiled = dtrue;
		return DELTA_OK;
	}

	D->ip			= 0;
	D->currentLine	= NULL;

	delta_SBytecode bc;
	bc.bytecodeSize	= D->bytecodeSize;
	bc.index		= DELTABASIC_EXEC_BYTECODE_SIZE;
	bc.bytecode		= D->bytecode;
	bc.bCanResize	= dtrue;

	for (delta_SLine* node = D->head; node != NULL; node = node->next) {
		delta_EStatus status = delta_CompileLine(D, node, NULL, &bc);
		if (status != DELTA_OK) {
			D->bytecodeSize = bc.bytecodeSize;
			D->bytecode = bc.bytecode;

			return status;
		}
	}

	if (bc.bytecode[bc.index - 1] != OPCODE_HLT) {
		PushAssert(PushBytecodeByte(D, &bc, OPCODE_HLT));
	}

	D->bytecodeSize	= bc.bytecodeSize;
	D->bytecode		= bc.bytecode;
	D->ip			= DELTABASIC_EXEC_BYTECODE_SIZE;
	D->currentLine	= D->head;
	D->bCompiled	= dtrue;

	return DELTA_OK;
}

/* ====================================
 * delta_CompileLine
 */
delta_EStatus delta_CompileLine(delta_SState* D, delta_SLine* L, delta_TChar* str, delta_SBytecode* BC) {
	D->lineNumber = L->line;
	L->offset = BC->index;

	delta_SLexerState lexem = { 0 };
	lexem.buffer = (str == NULL) ? L->str : str;

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
 * CompileCall
 */
delta_EStatus CompileCall(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, size_t index, delta_ECFuncArgType returnType, delta_TBool bReturn) {
	delta_SCFunction* func = D->cfuncVector.array[index];
	if (func->retType != returnType)
		return DELTA_SYNTAX_ERROR;

	for (delta_TByte i = 0; i < func->argCount; ++i) {
		if (i != 0) {
			if ((L->type != LEXEM_SYMBOL) || (L->symbol != ','))
				return DELTA_SYNTAX_ERROR;
		}

		delta_EMathStatus math = MATH_OK_NUMERIC;
		if ((((func->argsMask) >> i) & 0x01) == DELTA_CFUNC_ARG_STRING)
			math = MATH_OK_STRING;
		
		MathStatusAssert(CompileMath(D, L, BC, math), math);
	}

	if (func->argCount == 0) {
		CompileInstructionParseAssert();
	}

	if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
		return DELTA_SYNTAX_ERROR;

	PushAssert(PushBytecodeByte(D, BC, (bReturn == dtrue) ? OPCODE_CALLR : OPCODE_CALL));
	PushAssert(PushBytecodeWord(D, BC, index));

	CompileInstructionParseAssert();

	return DELTA_OK;
}

/* ====================================
 * CompileInstruction
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
					MathStatusAssert(CompileMath(D, L, BC, MATH_OK_STRING), MATH_OK_STRING);

					PushAssert(PushBytecodeByte(D, BC, OPCODE_SETS));
					PushAssert(PushBytecodeWord(D, BC, name.offset));
					PushAssert(PushBytecodeWord(D, BC, name.size));
				}
				else
					return DELTA_SYNTAX_ERROR;
			}
			else if (L->symbol == '=') { // Numeric Assignment
				MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->op == OP_DIM) {
			CompileInstructionParseAssert();

			if (L->type != LEXEM_NAME)
				return DELTA_SYNTAX_ERROR;

			delta_SLexemString name = L->string;
			CompileInstructionParseAssert();
			
			if (L->type != LEXEM_SYMBOL)
				return DELTA_SYNTAX_ERROR;

			delta_TBool bString = dfalse;
			if (L->symbol == '$') { // String Array
				bString = dtrue;
				CompileInstructionParseAssert();
			}

			if ((L->type != LEXEM_SYMBOL) || (L->symbol != '('))
				return DELTA_SYNTAX_ERROR;

			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

			if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
				return DELTA_SYNTAX_ERROR;

			PushAssert(PushBytecodeByte(D, BC, (bString == dtrue) ? OPCODE_ALLOCS : OPCODE_ALLOCN));
			PushAssert(PushBytecodeWord(D, BC, name.offset));
			PushAssert(PushBytecodeWord(D, BC, name.size));

			CompileInstructionParseAssert();
		}
		else if (L->op == OP_PRINT) {
			delta_TChar lastSymbol = ' ';
			while (dtrue) {
				delta_EStatus status = CompileMath(D, L, BC, MATH_OK_UNDEF);
				if ((status != MATH_OK_NUMERIC) && (status != MATH_OK_STRING) && (status != MATH_OK_UNDEF))
					return status;

				if (L->type == LEXEM_EOL) {
					if (status == MATH_OK_NUMERIC) { PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTNT)); }
					else if (status == MATH_OK_STRING) { PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTST)); }
					
					if ((lastSymbol != ';') || (status != MATH_OK_UNDEF))
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));

					break;
				}
				else if (L->type == LEXEM_SYMBOL) {
					if (status != MATH_OK_UNDEF) {
						if ((L->symbol == ',') || (L->symbol == ':')) {
							PushAssert(PushBytecodeByte(D, BC, (status == MATH_OK_NUMERIC) ? OPCODE_PRINTNT : OPCODE_PRINTST));
						} else if (L->symbol == ';') {
							PushAssert(PushBytecodeByte(D, BC, (status == MATH_OK_NUMERIC) ? OPCODE_PRINTN : OPCODE_PRINTS));
						}
					}

					if (L->symbol == ':') {
						PushAssert(PushBytecodeByte(D, BC, OPCODE_PRINTLN));
						break;
					}

					lastSymbol = L->symbol;
				}
				else
					return DELTA_SYNTAX_ERROR;
			}
		}
		else if (L->op == OP_INPUT) {
			CompileInstructionParseAssert();
			while (dtrue) {
				if (L->type == LEXEM_SYMBOL) {
					if (L->symbol == ':')
						break;
					else if (L->symbol == ',') {
						// Nothing
					}
					
					return DELTA_SYNTAX_ERROR;
				}
				else if (L->type == LEXEM_EOL)
					break;

				if (L->type != LEXEM_NAME)
					return DELTA_SYNTAX_ERROR;

				delta_SLexemString name = L->string;
				CompileInstructionParseAssert();

				delta_EMathStatus status = MATH_OK_UNDEF;
				if (L->type == LEXEM_SYMBOL) {
					if (L->symbol == '$') // String
						status = MATH_OK_STRING;
					else if (L->symbol == ',') // Numeric
						status = MATH_OK_NUMERIC;

					CompileInstructionParseAssert();
				}
				else if ((L->type == LEXEM_NAME) || (L->type == LEXEM_EOL)) // Numeric
					status = MATH_OK_NUMERIC;

				if (status != MATH_OK_UNDEF) {
					PushAssert(PushBytecodeByte(D, BC, (status == MATH_OK_NUMERIC) ? OPCODE_INPUTN : OPCODE_INPUTS));
					PushAssert(PushBytecodeWord(D, BC, name.offset));
					PushAssert(PushBytecodeWord(D, BC, name.size));
				}
			}
		}
		else if (L->op == OP_END) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_HLT));
		}
		else if ((L->op == OP_GOTO) || (L->op == OP_GOSUB)) {
			delta_TBool bGoto = (L->op == OP_GOTO);
			CompileInstructionParseAssert();

			if (L->type != LEXEM_INTEGER)
				return DELTA_SYNTAX_ERROR;

			if (L->integerValue < 0)
				return DELTA_SYNTAX_ERROR;

			size_t number = (size_t)(L->integerValue);
			delta_SLine* line = D->head;
			while (line != NULL) {
				if (line->line == number)
					break;

				if (line->line > number) {
					if (line->prev != NULL)
						line = line->prev;
					else
						line = NULL;

					break;
				}

				line = line->next;
			}

			if (line == NULL)
				return DELTA_OUT_OF_LINES_RANGE;
			
			PushAssert(PushBytecodeByte(D, BC, (bGoto == dtrue) ? OPCODE_JMP : OPCODE_GOSUB));
			PushAssert(PushBytecodeWord(D, BC, (delta_TWord)(line->line)));
		}
		else if (L->op == OP_IF) {
			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

			if (L->type != LEXEM_OP)
				return DELTA_SYNTAX_ERROR;

			if (L->op != OP_THEN)
				return DELTA_SYNTAX_ERROR;

			PushAssert(PushBytecodeByte(D, BC, OPCODE_JNLNZ));
		}
		else if (L->op == OP_FOR) {
			CompileInstructionParseAssert();

			if (L->type != LEXEM_NAME) // FOR var
				return DELTA_SYNTAX_ERROR;

			delta_SLexemString name = L->string;
			CompileInstructionParseAssert();

			if (L->type != LEXEM_SYMBOL) // FOR var =
				return DELTA_SYNTAX_ERROR;

			if (L->symbol != '=') // FOR var =
				return DELTA_SYNTAX_ERROR;

			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC); // FOR var = math

			if (L->type != LEXEM_OP) // FOR var = math TO
				return DELTA_SYNTAX_ERROR;

			if (L->op != OP_TO) // FOR var = math TO
				return DELTA_SYNTAX_ERROR;

			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC); // FOR var = math TO math

			if ((L->type == LEXEM_EOL) || ((L->type == LEXEM_SYMBOL) && (L->symbol == ':'))) {
				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETFOR));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else if (L->type == LEXEM_OP) {
				if (L->op != OP_STEP) // FOR var = math TO math STEP
					return DELTA_SYNTAX_ERROR;

				MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC); // FOR var = math TO math STEP math

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETSTEPFOR));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->op == OP_NEXT) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_NEXTFOR));
		}
		else if (L->op == OP_RETURN) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_RETURN));
		}
		else if (L->op == OP_STOP) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_STOP));
		}
		else if (L->op == OP_RUN) {
			PushAssert(PushBytecodeByte(D, BC, OPCODE_RUN));
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
			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_STRING), MATH_OK_STRING);

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETS));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
			else if (L->symbol == '(') { // String Function call or String Array
				size_t index;
				if (delta_FindCFunction(D, L->buffer + name.offset, name.size, &index) == dtrue) { // String Function call
					StatusAssert(CompileCall(D, L, BC, index, DELTA_CFUNC_ARG_STRING, dfalse));
				}
				else { // String Array
					MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

					if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
						return DELTA_SYNTAX_ERROR;

					CompileInstructionParseAssert();

					if ((L->type != LEXEM_SYMBOL) || (L->symbol != '='))
						return DELTA_SYNTAX_ERROR;

					MathStatusAssert(CompileMath(D, L, BC, MATH_OK_STRING), MATH_OK_STRING);

					PushAssert(PushBytecodeByte(D, BC, OPCODE_SETIS));
					PushAssert(PushBytecodeWord(D, BC, name.offset));
					PushAssert(PushBytecodeWord(D, BC, name.size));
				}
			}
			else
				return DELTA_SYNTAX_ERROR;
		}
		else if (L->symbol == '=') { // Numeric Assignment
			MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

			PushAssert(PushBytecodeByte(D, BC, OPCODE_SETN));
			PushAssert(PushBytecodeWord(D, BC, name.offset));
			PushAssert(PushBytecodeWord(D, BC, name.size));
		}
		else if (L->symbol == '(') { // Numeric Function call or Numeric Array
			size_t index;
			if (delta_FindCFunction(D, L->buffer + name.offset, name.size, &index) == dtrue) { // Numeric Function call
				StatusAssert(CompileCall(D, L, BC, index, DELTA_CFUNC_ARG_NUMERIC, dfalse));
			}
			else { // Numeric Array
				MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

				if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
					return DELTA_SYNTAX_ERROR;

				CompileInstructionParseAssert();

				if ((L->type != LEXEM_SYMBOL) || (L->symbol != '='))
					return DELTA_SYNTAX_ERROR;

				MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

				PushAssert(PushBytecodeByte(D, BC, OPCODE_SETIN));
				PushAssert(PushBytecodeWord(D, BC, name.offset));
				PushAssert(PushBytecodeWord(D, BC, name.size));
			}
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
 * CompileMathUnary
 */
delta_EMathStatus CompileMathUnary(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, delta_EMathStatus* mathStatus) {
	delta_TBool bMinus = dfalse;
	if (L->type == LEXEM_SYMBOL) {
		if (L->symbol == '(') {
			delta_EMathStatus status = CompileMath(D, L, BC, *mathStatus);
			if (status == MATH_OK_UNDEF) {
				if (*mathStatus != status) // Empty
					return DELTA_SYNTAX_ERROR;
			}
			else if (status == MATH_OK_NUMERIC) {
				if (*mathStatus == MATH_OK_STRING)
					return DELTA_SYNTAX_ERROR;

				*mathStatus = MATH_OK_NUMERIC;
			}
			else if (status == MATH_OK_STRING) {
				if (*mathStatus == MATH_OK_NUMERIC)
					return DELTA_SYNTAX_ERROR;

				*mathStatus = MATH_OK_STRING;
			}
			else
				return status;

			return DELTA_OK;
		}

		if (*mathStatus == MATH_OK_STRING)
			return DELTA_SYNTAX_ERROR;

		*mathStatus = MATH_OK_NUMERIC;

		if (L->symbol == '-') {
			bMinus = dtrue;
			ParseAssert();
		}
		else if (L->symbol == '+') {
			ParseAssert();
		}
		else
			return DELTA_SYNTAX_ERROR;
	}
	
	if ((L->type == LEXEM_INTEGER) || (L->type == LEXEM_FLOAT)) {
		if (*mathStatus == MATH_OK_STRING)
			return DELTA_SYNTAX_ERROR;

		*mathStatus = MATH_OK_NUMERIC;

		delta_TNumber number = (L->type == LEXEM_INTEGER) ? (delta_TNumber)(L->integerValue) : (delta_TNumber)(L->floatValue);
		if (bMinus == dtrue)
			number = -number;

		PushAssert(PushBytecodeByte(D, BC, OPCODE_PUSHN));
		PushAssert(PushBytecodeNumber(D, BC, number));

		return DELTA_OK;
	}
	else if (L->type == LEXEM_STRING) {
		if (*mathStatus == MATH_OK_NUMERIC)
			return DELTA_SYNTAX_ERROR;

		*mathStatus = MATH_OK_STRING;

		PushAssert(PushBytecodeByte(D, BC, OPCODE_PUSHS));
		PushAssert(PushBytecodeWord(D, BC, L->string.offset));
		PushAssert(PushBytecodeWord(D, BC, L->string.size));

		return DELTA_OK;
	}
	else if (L->type == LEXEM_NAME) { // TODO: functions
		delta_SLexemString str = L->string;
		
		if (*mathStatus == MATH_OK_UNDEF) {
			const delta_TChar* head = L->head;
			ParseAssert();
			if ((L->type == LEXEM_SYMBOL) && (L->symbol == '$'))
				*mathStatus = MATH_OK_STRING;
			else
				*mathStatus = MATH_OK_NUMERIC;
			
			L->head = head;
		}

		if (*mathStatus == MATH_OK_STRING) {
			ParseAssert();
			if ((L->type != LEXEM_SYMBOL) || (L->symbol != '$'))
				return DELTA_SYNTAX_ERROR;

			const delta_TChar* head = L->head;
			ParseAssert();
			if ((L->type == LEXEM_SYMBOL) && (L->symbol == '(')) { // Function or Array
				size_t index;
				if (delta_FindCFunction(D, L->buffer + str.offset, str.size, &index) == dtrue) { // Function
					StatusAssert(CompileCall(D, L, BC, index, DELTA_CFUNC_ARG_STRING, dtrue));
				}
				else {
					MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

					if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
						return DELTA_SYNTAX_ERROR;

					PushAssert(PushBytecodeByte(D, BC, OPCODE_GETIS));
					PushAssert(PushBytecodeWord(D, BC, str.offset));
					PushAssert(PushBytecodeWord(D, BC, str.size));
				}
			}
			else {
				L->head = head;

				PushAssert(PushBytecodeByte(D, BC, OPCODE_GETS));
				PushAssert(PushBytecodeWord(D, BC, str.offset));
				PushAssert(PushBytecodeWord(D, BC, str.size));
			}
		}
		else {
			const delta_TChar* head = L->head;
			ParseAssert();
			if ((L->type == LEXEM_SYMBOL) && (L->symbol == '(')) { // Function or Array
				size_t index;
				if (delta_FindCFunction(D, L->buffer + str.offset, str.size, &index) == dtrue) { // Function
					StatusAssert(CompileCall(D, L, BC, index, DELTA_CFUNC_ARG_NUMERIC, dtrue));
				}
				else {
					MathStatusAssert(CompileMath(D, L, BC, MATH_OK_NUMERIC), MATH_OK_NUMERIC);

					if ((L->type != LEXEM_SYMBOL) || (L->symbol != ')'))
						return DELTA_SYNTAX_ERROR;

					PushAssert(PushBytecodeByte(D, BC, OPCODE_GETIN));
					PushAssert(PushBytecodeWord(D, BC, str.offset));
					PushAssert(PushBytecodeWord(D, BC, str.size));
				}
			}
			else {
				L->head = head;

				PushAssert(PushBytecodeByte(D, BC, OPCODE_GETN));
				PushAssert(PushBytecodeWord(D, BC, str.offset));
				PushAssert(PushBytecodeWord(D, BC, str.size));
			}

			if (bMinus == dtrue)
				PushAssert(PushBytecodeByte(D, BC, OPCODE_NEG));
		}

		return DELTA_OK;
	}

	return DELTA_SYNTAX_ERROR;
}

/* ====================================
 * CompileMath
 */
delta_EMathStatus CompileMath(delta_SState* D, delta_SLexerState* L, delta_SBytecode* BC, delta_EMathStatus startingMathStatus) {
	delta_EOpcodes ops[DELTABASIC_COMPILER_MAX_MATH_OPS];
	size_t nOps = 0;
	size_t outVariables = 0;

	delta_EMathStatus mathStatus = startingMathStatus;

	ParseAssert();
	if (L->type == LEXEM_EOL) // Empty
		return MATH_OK_UNDEF;

	StatusAssert(CompileMathUnary(D, L, BC, &mathStatus));
	++outVariables;
	ParseAssert();
	while (dtrue) {
		if (L->type == LEXEM_SYMBOL) {
			if (L->symbol == '$')
				return DELTA_SYNTAX_ERROR;

			if ((L->symbol == ')') || (L->symbol == ':') || (L->symbol == ';') || (L->symbol == ','))
				break;

			// TODO: check UNDEF?

			const delta_TChar symbol = L->symbol;
			delta_EOpcodes opcode = OPCODE_HLT;
			if (mathStatus == MATH_OK_NUMERIC) {
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

						ParseAssert();
					}
				}
				else {
					switch (symbol) {
						case '+': opcode = OPCODE_ADD; break;
						case '-': opcode = OPCODE_SUB; break;
						case '*': opcode = OPCODE_MUL; break;
						case '/': opcode = OPCODE_DIV; break;
						case '^': opcode = OPCODE_POW; break;
						case '%': opcode = OPCODE_MOD; break;
						case '=': opcode = OPCODE_ET;  break;
						default:
							return DELTA_SYNTAX_ERROR;
					};
							
					ParseAssert();
				}
			}
			else { // MATH_OK_STRING
				if (symbol == '+')
					opcode = OPCODE_CONCAT;
				else
					return DELTA_SYNTAX_ERROR;

				ParseAssert();
			}

			if (nOps != 0) { // Pop operations
				if (GetMathPriority(opcode) < GetMathPriority(ops[nOps - 1])) {
					while (nOps != 0) {
						const delta_EOpcodes opcodeOnTop = ops[--nOps];

						if (outVariables == 0)
							return MATH_NOT_ENOUGH_VALUES;

						--outVariables;
						PushAssert(PushBytecodeByte(D, BC, opcodeOnTop));
					}
				}
			}

			if (nOps >= DELTABASIC_COMPILER_MAX_MATH_OPS)
				return DELTA_ALLOCATOR_ERROR;

			ops[nOps++] = opcode;

			StatusAssert(CompileMathUnary(D, L, BC, &mathStatus));
			++outVariables;
			ParseAssert();
		}
		else if (L->type == LEXEM_OP) {
			break;
		}
		else if (L->type == LEXEM_EOL) {
			break;
		}
		else
			return DELTA_SYNTAX_ERROR;
	}

	while (nOps != 0) {
		const delta_EOpcodes opcodeOnTop = ops[--nOps];

		if (outVariables == 0)
			return MATH_NOT_ENOUGH_VALUES;

		--outVariables;
		PushAssert(PushBytecodeByte(D, BC, opcodeOnTop));
	}
	
	return mathStatus;
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
	// Just because GCC's "warning: dereferencing type-punned pointer will break strict-aliasing rules"
	// on the hack-ish *((delta_TDWord*)(&number)) and blah-blah-blah

	union {
		delta_TNumber number;
		delta_TDWord dword;
	} cast;

	cast.number = number;

	return PushBytecodeDWord(D, BC, cast.dword);
}

/* ====================================
 * ExpandBytecodeBuffer
 */
inline delta_TBool ExpandBytecodeBuffer(delta_SState* D, delta_SBytecode* BC) {
	if (BC->bCanResize == dfalse)
		return dfalse;

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
