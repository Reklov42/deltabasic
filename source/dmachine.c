//

//	| File:			dcompiler.h
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dmachine.h"

#include <string.h>
#include <math.h>

#include "dmemory.h"
#include "dstate.h"
#include "dopcodes.h"
#include "dstring.h"
#include "dlexer.h"
					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineHalt
 */
delta_EStatus MachineHalt(delta_SState* D);

/* ====================================
 * MachineNextLine
 */
delta_EStatus MachineNextLine(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachinePushNumeric
 */
delta_EStatus MachinePushNumeric(delta_SState* D);

/* ====================================
 * MachineSetNumeric
 */
delta_EStatus MachineSetNumeric(delta_SState* D);

/* ====================================
 * MachineGetNumeric
 */
delta_EStatus MachineGetNumeric(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachinePushString
 */
delta_EStatus MachinePushString(delta_SState* D);

/* ====================================
 * MachineSetString
 */
delta_EStatus MachineSetString(delta_SState* D);

/* ====================================
 * MachineGetString
 */
delta_EStatus MachineGetString(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineConcat
 */
delta_EStatus MachineConcat(delta_SState* D);

/* ====================================
 * MachineAdd
 */
delta_EStatus MachineAdd(delta_SState* D);

/* ====================================
 * MachineSub
 */
delta_EStatus MachineSub(delta_SState* D);

/* ====================================
 * MachineMul
 */
delta_EStatus MachineMul(delta_SState* D);

/* ====================================
 * MachineDiv
 */
delta_EStatus MachineDiv(delta_SState* D);

/* ====================================
 * MachineMod
 */
delta_EStatus MachineMod(delta_SState* D);

/* ====================================
 * MachinePow
 */
delta_EStatus MachinePow(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachinePrintNumeric
 */
delta_EStatus MachinePrintNumeric(delta_SState* D);

/* ====================================
 * MachinePrintNumericT
 */
delta_EStatus MachinePrintNumericT(delta_SState* D);

/* ====================================
 * MachinePrintString
 */
delta_EStatus MachinePrintString(delta_SState* D);

/* ====================================
 * MachinePrintStringT
 */
delta_EStatus MachinePrintStringT(delta_SState* D);

/* ====================================
 * MachinePrintNewLine
 */
delta_EStatus MachinePrintNewLine(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineEqualTo
 */
delta_EStatus MachineEqualTo(delta_SState* D);

/* ====================================
 * MachineNotEqualTo
 */
delta_EStatus MachineNotEqualTo(delta_SState* D);

/* ====================================
 * MachineLessThan
 */
delta_EStatus MachineLessThan(delta_SState* D);

/* ====================================
 * MachineGreaterThan
 */
delta_EStatus MachineGreaterThan(delta_SState* D);

/* ====================================
 * MachineLessOrEqualTo
 */
delta_EStatus MachineLessOrEqualTo(delta_SState* D);

/* ====================================
 * MachineGreaterOrEqualTo
 */
delta_EStatus MachineGreaterOrEqualTo(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineNeg
 */
delta_EStatus MachineNeg(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineStop
 */
delta_EStatus MachineStop(delta_SState* D);

/* ====================================
 * MachineTab
 */
delta_EStatus MachineTab(delta_SState* D);

/* ====================================
 * MachineJump
 */
delta_EStatus MachineJump(delta_SState* D);

/* ====================================
 * MachineGoSub
 */
delta_EStatus MachineGoSub(delta_SState* D);

/* ====================================
 * MachineReturn
 */
delta_EStatus MachineReturn(delta_SState* D);

/* ====================================
 * MachineJumpNextLineIfNotZero
 */
delta_EStatus MachineJumpNextLineIfNotZero(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineSetFor
 */
delta_EStatus MachineSetFor(delta_SState* D);

/* ====================================
 * MachineSetStepFor
 */
delta_EStatus MachineSetStepFor(delta_SState* D);

/* ====================================
 * MachineNextFor
 */
delta_EStatus MachineNextFor(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineInputNumeric
 */
delta_EStatus MachineInputNumeric(delta_SState* D);

/* ====================================
 * MachineInputString
 */
delta_EStatus MachineInputString(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * AllocNumericArray
 */
delta_EStatus AllocNumericArray(delta_SState* D, delta_SNumericArray* array);

/* ====================================
 * AllocStringArray
 */
delta_EStatus AllocStringArray(delta_SState* D, delta_SStringArray* array);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineAllocNumericArray
 */
delta_EStatus MachineAllocNumericArray(delta_SState* D);

/* ====================================
 * MachineAllocStringArray
 */
delta_EStatus MachineAllocStringArray(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineGetNumericArray
 */
delta_EStatus MachineGetNumericArray(delta_SState* D);

/* ====================================
 * MachineGetStringArray
 */
delta_EStatus MachineGetStringArray(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineSetNumericArray
 */
delta_EStatus MachineSetNumericArray(delta_SState* D);

/* ====================================
 * MachineSetStringArray
 */
delta_EStatus MachineSetStringArray(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineCallNumeric
 */
delta_EStatus MachineCallNumeric(delta_SState* D);

// ------------------------------------------------------------------------- //


/* ====================================
 * FormatNumeric
 */
size_t FormatNumeric(delta_TChar str[], size_t strSize, delta_TNumber number);

/* ====================================
 * PrintTabs
 */
size_t PrintTabs(delta_SState* D, size_t size);

// ------------------------------------------------------------------------- //

/* ====================================
 * TMachineFunction
 */
typedef delta_EStatus(*TMachineFunction)(delta_SState* D);

/* ====================================
 * machine_functions
 */
static const TMachineFunction machine_functions[OPCODE_COUNT] = {
	MachineHalt,
	MachineNextLine,
	MachinePushString,
	MachinePushNumeric,
	MachineConcat,
	MachineAdd,
	MachineSub,
	MachineMul,
	MachineDiv,
	MachineMod,
	MachinePow,
	MachineSetNumeric,
	MachineSetString,
	MachineJump,
	MachinePrintNumeric,
	MachinePrintNumericT,
	MachinePrintString,
	MachinePrintStringT,
	MachinePrintNewLine,
	MachineGetNumeric,
	MachineGetString,
	MachineEqualTo,
	MachineNotEqualTo,
	MachineLessThan,
	MachineGreaterThan,
	MachineLessOrEqualTo,
	MachineGreaterOrEqualTo,
	MachineNeg,
	MachineStop,
	MachineTab,
	MachineGoSub,
	MachineReturn,
	MachineJumpNextLineIfNotZero,
	MachineSetFor,
	MachineSetStepFor,
	MachineNextFor,
	MachineInputNumeric,
	MachineInputString,
	MachineAllocNumericArray,
	MachineAllocStringArray,
	MachineGetNumericArray,
	MachineGetStringArray,
	MachineSetNumericArray,
	MachineSetStringArray,
	MachineCallNumeric,
	OPCODE_CALLS,		// Call String cfunc
	OPCODE_CALLNR,		// Call Numeric cfunc with return
	OPCODE_CALLSR,		// Call String cfunc with return
};

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_ExecuteInstruction
 */
delta_EStatus delta_ExecuteInstruction(delta_SState* D) {
	if (D->currentLine == NULL) {
		delta_FreeStringStack(D);
		return DELTA_END;
	}

	delta_TByte op = D->bytecode[D->ip];
	if (op > OPCODE_LAST)
		return DELTA_MACHINE_UNKNOWN_OPCODE;

	delta_EStatus status = machine_functions[op](D);
	if (status != DELTA_OK)
		D->currentLine = NULL;

	return status;
}

// ------------------------------------------------------------------------- //

#include <stdio.h>

/* ====================================
 * MachineHalt
 */
delta_EStatus MachineHalt(delta_SState* D) {
	D->currentLine = NULL;

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineNextLine
 */
delta_EStatus MachineNextLine(delta_SState* D) {
	D->currentLine = D->currentLine->next;

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePushNumeric
 */
delta_EStatus MachinePushNumeric(delta_SState* D) {
	if (D->numericHead + 1 == DELTABASIC_NUMERIC_STACK_SIZE)
		return DELTA_MACHINE_NUMERIC_STACK_OVERFLOW;

	D->ip += 1;
	const delta_TNumber number = *((delta_TNumber*)(D->bytecode + D->ip));
	D->numericStack[(D->numericHead)++] = number;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineSetNumeric
 */
delta_EStatus MachineSetNumeric(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	var->value = D->numericStack[--(D->numericHead)];

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineGetNumeric
 */
delta_EStatus MachineGetNumeric(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead + 1 == DELTABASIC_NUMERIC_STACK_SIZE)
		return DELTA_MACHINE_NUMERIC_STACK_OVERFLOW;

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	D->numericStack[(D->numericHead)++] = var->value;

	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineConcat
 */
delta_EStatus MachineConcat(delta_SState* D) {
	if (D->stringHead < 2)
		return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

	delta_TChar* strA = D->stringStack[D->stringHead - 2];
	delta_TChar* strB = D->stringStack[D->stringHead - 1];
	const size_t sizeA = delta_Strlen(strA);
	const size_t sizeB = delta_Strlen(strB);
	const size_t size = sizeA + sizeB;

	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (size + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(str, strA, sizeA);
	memcpy(str + sizeA, strB, sizeB);
	str[size] = '\0';

	DELTA_Free(D, strA, sizeof(delta_TChar) * (sizeA + 1));
	DELTA_Free(D, strB, sizeof(delta_TChar) * (sizeB + 1));

	--(D->stringHead);
	D->stringStack[D->stringHead - 1] = str;

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineAdd
 */
delta_EStatus MachineAdd(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] + D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineSub
 */
delta_EStatus MachineSub(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] - D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineMul
 */
delta_EStatus MachineMul(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] * D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineDiv
 */
delta_EStatus MachineDiv(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead); // TODO: by zero check?
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] / D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineMod
 */
delta_EStatus MachineMod(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = fmodf(D->numericStack[D->numericHead - 1], D->numericStack[D->numericHead]);

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePow
 */
delta_EStatus MachinePow(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = powf(D->numericStack[D->numericHead - 1], D->numericStack[D->numericHead]);

	D->ip += 1;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachinePrintNumeric
 */
delta_EStatus MachinePrintNumeric(delta_SState* D) {
	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	delta_TNumber num = D->numericStack[D->numericHead];
	delta_TNumber dec = num - (delta_TNumber)((long)num);
	delta_TChar buffer[32];

	int size;
	if (dec < DELTABASIC_NUMERIC_EPSILON)
		size = snprintf(buffer, 32, "%i", (long)num);
	else
		size = snprintf(buffer, 32, "%f", num);

	D->printFunction(buffer, size);

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePow
 */
delta_EStatus MachinePrintNumericT(delta_SState* D) {
	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	delta_TNumber num = D->numericStack[D->numericHead];
	delta_TNumber dec = num - (delta_TNumber)((long)num);
	delta_TChar buffer[32];

	int size;
	if (dec < DELTABASIC_NUMERIC_EPSILON)
		size = snprintf(buffer, 32, "%i", (long)num);
	else
		size = snprintf(buffer, 32, "%f", num);

	D->printFunction(buffer, size);
	PrintTabs(D, size);

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePrintString
 */
delta_EStatus MachinePrintString(delta_SState* D) {
	if (D->stringHead < 1)
		return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

	--(D->stringHead);
	delta_TChar* str = D->stringStack[D->stringHead];
	size_t size = delta_Strlen(str);

	D->printFunction(str, size);
	DELTA_Free(D, str, sizeof(delta_TChar) * (size + 1));
	
	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePrintStringT
 */
delta_EStatus MachinePrintStringT(delta_SState* D) {
	if (D->stringHead < 1)
		return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

	--(D->stringHead);
	delta_TChar* str = D->stringStack[D->stringHead];
	size_t size = delta_Strlen(str);

	D->printFunction(str, size);
	DELTA_Free(D, str, sizeof(delta_TChar) * (size + 1));

	PrintTabs(D, size);
	
	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachinePrintNewLine
 */
delta_EStatus MachinePrintNewLine(delta_SState* D) {
	delta_TChar buffer[2] = { '\n', '\0' };
	D->printFunction(buffer, 1);
	
	D->ip += 1;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachinePushString
 */
delta_EStatus MachinePushString(delta_SState* D) {
	if (D->stringHead + 1 == DELTABASIC_STRING_STACK_SIZE)
		return DELTA_MACHINE_STRING_STACK_OVERFLOW;

	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];
	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (size + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(str, D->currentLine->str + offset, sizeof(delta_TChar) * size);
	str[size] = '\0';

	D->stringStack[(D->stringHead)++] = str;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineSetString
 */
delta_EStatus MachineSetString(delta_SState* D)  {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->stringHead == 0)
		return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

	delta_SStringVariable* var = delta_FindOrAddStringVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (var->str != NULL) {
		DELTA_Free(D, var->str, (delta_Strlen(var->str) + 1) * sizeof(delta_TChar));
	}

	var->str = D->stringStack[--(D->stringHead)];

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineGetString
 */
delta_EStatus MachineGetString(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->stringHead + 1 == DELTABASIC_STRING_STACK_SIZE)
		return DELTA_MACHINE_STRING_STACK_OVERFLOW;

	delta_SStringVariable* var = delta_FindOrAddStringVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	size_t strSize = 0;
	if (var->str)
		strSize = delta_Strlen(var->str);
	
	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (strSize + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (var->str)
		memcpy(str, var->str, sizeof(delta_TChar) * strSize);
		
	str[strSize] = '\0';

	D->stringStack[(D->stringHead)++] = str;

	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineEqualTo
 */
delta_EStatus MachineEqualTo(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] =
		fabsf(D->numericStack[D->numericHead - 1] - D->numericStack[D->numericHead]) < DELTABASIC_NUMERIC_EPSILON;

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineNotEqualTo
 */
delta_EStatus MachineNotEqualTo(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] =
		fabsf(D->numericStack[D->numericHead - 1] - D->numericStack[D->numericHead]) > DELTABASIC_NUMERIC_EPSILON;

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineLessThan
 */
delta_EStatus MachineLessThan(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] < D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineGreaterThan
 */
delta_EStatus MachineGreaterThan(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] > D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineLessOrEqualTo
 */
delta_EStatus MachineLessOrEqualTo(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] <= D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

/* ====================================
 * MachineGreaterOrEqualTo
 */
delta_EStatus MachineGreaterOrEqualTo(delta_SState* D) {
	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	D->numericStack[D->numericHead - 1] = D->numericStack[D->numericHead - 1] >= D->numericStack[D->numericHead];

	D->ip += 1;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineNeg
 */
delta_EStatus MachineNeg(delta_SState* D) {
	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;
	
	D->numericStack[D->numericHead - 1] = -(D->numericStack[D->numericHead - 1]);

	D->ip += 1;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineStop
 */
delta_EStatus MachineStop(delta_SState* D) {
	D->ip += 1;
	return DELTA_MACHINE_STOP;
}

/* ====================================
 * FindLine
 */
delta_SLine* FindLine(delta_SState* D, delta_TWord number) {
	delta_SLine* line = D->currentLine;
	if ((D->currentLine->prev == NULL) && (D->currentLine->next == NULL)) {
		line = D->head;

		if (line == NULL)
			return NULL;
	}

	if (line->line == number) {
		D->ip = D->ip - 1;
		return line;
	}
	else if (line->line > number) {
		while (line != NULL) {
			if (line->line == number) {
				D->ip = line->offset;
				D->currentLine = line;
				break;
			}

			line = line->prev;
		}
	}
	else { // line->line < number
		while (line != NULL) {
			if (line->line == number) {
				D->ip = line->offset;
				D->currentLine = line;
				break;
			}

			line = line->next;
		}
	}
	
	return line;
}

/* ====================================
 * MachineJump
 */
delta_EStatus MachineJump(delta_SState* D) {
	D->ip += 1;
	const delta_TWord number = *((delta_TWord*)(D->bytecode + D->ip));
	
	if (FindLine(D, number) == NULL)
		return DELTA_OUT_OF_LINES_RANGE;

	//D->ip += 2; // useless
	return DELTA_OK;
}

/* ====================================
 * MachineGoSub
 */
delta_EStatus MachineGoSub(delta_SState* D) {
	if (D->returnHead + 1 == DELTABASIC_RETURN_STACK_SIZE)
		return DELTA_MACHINE_RETURN_STACK_OVERFLOW;

	D->ip += 1;
	const delta_TWord number = *((delta_TWord*)(D->bytecode + D->ip));
	D->ip += 2;
	
	D->returnStack[D->returnHead].ip = D->ip;
	D->returnStack[D->returnHead].line = D->currentLine;
	if (FindLine(D, number) == NULL)
		return DELTA_OUT_OF_LINES_RANGE;

	++(D->returnHead);

	return DELTA_OK;
}

/* ====================================
 * MachineReturn
 */
delta_EStatus MachineReturn(delta_SState* D) {
	D->ip += 1;

	if (D->returnHead < 1)
		return DELTA_MACHINE_RETURN_STACK_UNDERFLOW;

	--(D->returnHead);
	D->ip			= D->returnStack[D->returnHead].ip;
	D->currentLine	= D->returnStack[D->returnHead].line;

	return DELTA_OK;
}

/* ====================================
 * MachineJumpNextLineIfNotZero
 */
delta_EStatus MachineJumpNextLineIfNotZero(delta_SState* D) {
	D->ip += 1;

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	delta_TNumber value = D->numericStack[D->numericHead];
	if (fabsf(value) < DELTABASIC_NUMERIC_EPSILON) {
		D->currentLine = D->currentLine->next;
		if (D->currentLine != NULL)
			D->ip = D->currentLine->offset;
	}

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineSetFor
 */
delta_EStatus MachineSetFor(delta_SState* D) {
	if (D->forHead + 1 == DELTABASIC_FOR_STACK_SIZE)
		return DELTA_MACHINE_FOR_STACK_OVERFLOW;

	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	D->ip += 4;

	delta_SForState* forState = &(D->forStack[D->forHead]);
	++(D->forHead);

	forState->startLine = D->currentLine;
	forState->startIp = D->ip;
	forState->step = 1.0f;

	--(D->numericHead);
	forState->end = D->numericStack[D->numericHead];

	--(D->numericHead);
	var->value = D->numericStack[D->numericHead];
	forState->counter = var;
	
	return DELTA_OK;
}

/* ====================================
 * MachineSetStepFor
 */
delta_EStatus MachineSetStepFor(delta_SState* D) {
	if (D->forHead + 1 == DELTABASIC_FOR_STACK_SIZE)
		return DELTA_MACHINE_FOR_STACK_OVERFLOW;

	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead < 3)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	D->ip += 4;

	delta_SForState* forState = &(D->forStack[D->forHead]);
	++(D->forHead);

	forState->startLine = D->currentLine;
	forState->startIp = D->ip;

	--(D->numericHead);
	forState->step = D->numericStack[D->numericHead];

	--(D->numericHead);
	forState->end = D->numericStack[D->numericHead];

	--(D->numericHead);
	var->value = D->numericStack[D->numericHead];
	forState->counter = var;
	
	return DELTA_OK;
}

/* ====================================
 * MachineNextFor
 */
delta_EStatus MachineNextFor(delta_SState* D) {
	if (D->forHead < 1)
		return DELTA_MACHINE_FOR_STACK_UNDERFLOW;

	D->ip += 1;

	delta_SForState* forState = &(D->forStack[D->forHead - 1]);
	if (forState->counter == NULL) // Just in case
		return DELTA_ALLOCATOR_ERROR;

	forState->counter->value += forState->step;
	const delta_TBool bJump = (forState->step > 0.0f) ?
		(forState->counter->value <= forState->end) : // Increment
		(forState->counter->value >= forState->end); // Decrement

	if (bJump == dtrue) {
		D->currentLine = forState->startLine;
		D->ip = forState->startIp;
	}
	else {
		--(D->forHead);
	}

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineInputNumeric
 */
delta_EStatus MachineInputNumeric(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];
	
	D->printFunction(D->currentLine->str + offset, size);
	D->printFunction("? ", 2);

	delta_TChar buffer[DELTABASIC_INPUT_BUFFER_SIZE];
	size_t inputSize = D->inputFunction(buffer, DELTABASIC_INPUT_BUFFER_SIZE - 1);
	if (inputSize < 1)
		return DELTA_MACHINE_NOT_ENOUGH_INPUT_DATA;

	buffer[inputSize] = '\0';
	delta_TInteger value;
	if (delta_ReadInteger(buffer, &value) == NULL)
		return DELTA_MACHINE_INPUT_PARSE_ERROR;

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	var->value = value;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineInputString
 */
delta_EStatus MachineInputString(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	D->printFunction(D->currentLine->str + offset, size);
	D->printFunction("$? ", 3);

	delta_TChar buffer[DELTABASIC_INPUT_BUFFER_SIZE];
	size_t inputSize = D->inputFunction(buffer, DELTABASIC_INPUT_BUFFER_SIZE - 1);
	if (inputSize < 1)
		return DELTA_MACHINE_NOT_ENOUGH_INPUT_DATA;

	delta_SStringVariable* var = delta_FindOrAddStringVariable(D, D->currentLine->str + offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (var->str != NULL) {
		DELTA_Free(D, var->str, (delta_Strlen(var->str) + 1) * sizeof(delta_TChar));
	}

	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (inputSize + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(str, buffer, sizeof(delta_TChar) * inputSize);
	str[inputSize] = '\0';

	var->str = str;

	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * AllocNumericArray
 */
delta_EStatus AllocNumericArray(delta_SState* D, delta_SNumericArray* array) {
	array->array = (delta_TNumber*)DELTA_Alloc(D, sizeof(delta_TNumber) * (array->size));
	if (array->array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	for (size_t i = 0; i < array->size; ++i)
		array->array[i] = 0.0f;

	return DELTA_OK;
}

/* ====================================
 * AllocStringArray
 */
delta_EStatus AllocStringArray(delta_SState* D, delta_SStringArray* array) {
	array->array = (delta_TChar**)DELTA_Alloc(D, sizeof(delta_TChar*) * (array->size));
	if (array->array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	for (size_t i = 0; i < array->size; ++i) {
		array->array[i] = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * 2);
		if (array->array[i] == NULL)
			return DELTA_ALLOCATOR_ERROR;

		array->array[i][0] = '\0';
	}

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineAllocNumericArray
 */
delta_EStatus MachineAllocNumericArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_SNumericArray* array = delta_FindOrAddNumericArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array != NULL)
		return DELTA_MACHINE_REDIM_ERROR;

	delta_TNumber arrSize = D->numericStack[--(D->numericHead)];
	if (arrSize < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	array->size = DELTABASIC_MAX((size_t)arrSize, DELTABASIC_ARRAY_MIN_SIZE);
	delta_EStatus status = AllocNumericArray(D, array);
	if (status != DELTA_OK)
		return status;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineAllocStringArray
 */
delta_EStatus MachineAllocStringArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_SStringArray* array = delta_FindOrAddStringArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array != NULL)
		return DELTA_MACHINE_REDIM_ERROR;

	delta_TNumber arrSize = D->numericStack[--(D->numericHead)];
	if (arrSize < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	array->size = DELTABASIC_MAX((size_t)arrSize, DELTABASIC_ARRAY_MIN_SIZE);
	delta_EStatus status = AllocStringArray(D, array);
	if (status != DELTA_OK)
		return status;

	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineGetNumericArray
 */
delta_EStatus MachineGetNumericArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_TNumber index = D->numericStack[(D->numericHead) - 1];
	if (index < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	delta_SNumericArray* array = delta_FindOrAddNumericArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array == NULL) {
		array->size = DELTABASIC_ARRAY_MIN_SIZE;
		delta_EStatus status = AllocNumericArray(D, array);
		if (status != DELTA_OK)
			return status;
	}

	if ((size_t)index >= array->size)
		return DELTA_MACHINE_OUT_OF_RANGE;

	D->numericStack[(D->numericHead) - 1] = array->array[(size_t)index];
	
	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineGetStringArray
 */
delta_EStatus MachineGetStringArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead == 0)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;
		
	if (D->stringHead + 1 == DELTABASIC_STRING_STACK_SIZE)
		return DELTA_MACHINE_STRING_STACK_OVERFLOW;
		
	delta_TNumber index = D->numericStack[--(D->numericHead)];
	if (index < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	delta_SStringArray* array = delta_FindOrAddStringArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array == NULL) {
		array->size = DELTABASIC_ARRAY_MIN_SIZE;
		delta_EStatus status = AllocStringArray(D, array);
		if (status != DELTA_OK)
			return status;
	}

	if ((size_t)index >= array->size)
		return DELTA_MACHINE_OUT_OF_RANGE;

	if (array->array[(size_t)index] == NULL) // Just check
		return DELTA_ALLOCATOR_ERROR;

	size_t strSize = delta_Strlen(array->array[(size_t)index]);
	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (strSize + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(str, array->array[(size_t)index], sizeof(delta_TChar) * strSize);
	str[strSize] = '\0';

	D->stringStack[(D->stringHead)++] = str;
	
	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineSetNumericArray
 */
delta_EStatus MachineSetNumericArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead < 2)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	delta_TNumber value = D->numericStack[--(D->numericHead)];
	delta_TNumber index = D->numericStack[--(D->numericHead)];
	if (index < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	delta_SNumericArray* array = delta_FindOrAddNumericArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array == NULL) {
		array->size = DELTABASIC_ARRAY_MIN_SIZE;
		delta_EStatus status = AllocNumericArray(D, array);
		if (status != DELTA_OK)
			return status;
	}

	if ((size_t)index >= array->size)
		return DELTA_MACHINE_OUT_OF_RANGE;

	array->array[(size_t)index] = value;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineSetStringArray
 */
delta_EStatus MachineSetStringArray(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	if (D->stringHead < 1)
		return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

	delta_TNumber index = D->numericStack[--(D->numericHead)];
	if (index < 0.0f)
		return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	delta_SStringArray* array = delta_FindOrAddStringArray(D, D->currentLine->str + offset, size);
	if (array == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (array->array == NULL) {
		array->size = DELTABASIC_ARRAY_MIN_SIZE;
		delta_EStatus status = AllocStringArray(D, array);
		if (status != DELTA_OK)
			return status;
	}

	if ((size_t)index >= array->size)
		return DELTA_MACHINE_OUT_OF_RANGE;

	if (array->array[(size_t)index] != NULL) {
		DELTA_Free(D, array->array[(size_t)index], (delta_Strlen(array->array[(size_t)index]) + 1) * sizeof(delta_TChar));
	}

	array->array[(size_t)index] = D->stringStack[--(D->stringHead)];

	D->ip += 4;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineCallNumeric
 */
delta_EStatus MachineCallNumeric(delta_SState* D) {
	D->ip += 1;
	const delta_TWord index = ((delta_TWord*)(D->bytecode + D->ip))[0];

	D->bIgnoreCFuncReturn = dtrue;

	delta_SCFunction* func = D->cfuncVector.array[index];
	for (delta_TByte i = 0; i < func->argCount; ++i) {
		if (((func->argsMask >> i) & 0x01) == DELTA_CFUNC_ARG_STRING) {
			if (D->stringHead < 1)
				return DELTA_MACHINE_STRING_STACK_UNDERFLOW;

			--(D->stringHead);
			D->cfuncArgs[i].string = D->stringStack[D->stringHead];
		}
		else {
			if (D->numericHead < 1)
				return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

			--(D->numericHead);
			D->cfuncArgs[i].numeric = D->numericStack[D->numericHead];
		}
	}

	D->currentCFunc = func;
	delta_EStatus status = func->func(D);
	D->currentCFunc = NULL;
	for (delta_TByte i = 0; i < func->argCount; ++i) {
		if (((func->argsMask >> i) & 0x01) == DELTA_CFUNC_ARG_STRING) {
			DELTA_Free(D, (delta_TChar*)(D->cfuncArgs[i].string), sizeof(delta_TChar) * (delta_Strlen(D->cfuncArgs[i].string) + 1));
		}
	}

	if (status != DELTA_OK)
		return status;

	D->ip += 2;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * MachineTab
 */
delta_EStatus MachineTab(delta_SState* D) {
	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;
	
	if (D->stringHead + 1 == DELTABASIC_STRING_STACK_SIZE)
		return DELTA_MACHINE_STRING_STACK_OVERFLOW;

	--(D->numericHead);
	long num = (long)D->numericStack[D->numericHead];
	--num;
	
	if (num < 0) // TODO: error?
		num = 0;
		//return DELTA_MACHINE_NEGATIVE_ARGUMENT;

	delta_TChar* str = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (num + 1));
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memset(str, ' ', sizeof(delta_TChar) * num);
	str[num] = '\0';

	D->stringStack[(D->stringHead)++] = str;

	D->ip += 1;
	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * FormatNumeric
 */
size_t FormatNumeric(delta_TChar str[], size_t strSize, delta_TNumber number) {
	delta_TNumber dec = number - floorf(number);

	str[0] = (number < 0.0f) ? '-' : ' ';

	number = fabsf(number);
	size_t digits = (size_t)floorf(log10f(number)) + 1;
	if (number < DELTABASIC_NUMERIC_EPSILON)
		digits = 1;

	size_t size = digits;
	if (digits > strSize) { // TODO: E

	} else {
		int num = (int)number;
		for (size_t i = 0; i < digits; ++i) {
			str[size - i] = '0' + (num % 10);
			num /= 10;
		}

		++size;
		if (dec > DELTABASIC_NUMERIC_EPSILON) {
			str[size++] = '.';
			while (size < strSize) {
				dec *= 10.0f;
				str[size++] = '0' + ((int)dec % 10);
			}

			--size;
			while (str[size - 1] == '0')
				--size;
		}
	}

	return size;
}

/* ====================================
 * PrintTabs
 */
size_t PrintTabs(delta_SState* D, size_t size) {
	size = DELTABASIC_PRINT_TAB_SIZE - size % DELTABASIC_PRINT_TAB_SIZE;

	if (size == 0)
		return 0;

	const delta_TChar tabBuffer[DELTABASIC_PRINT_TAB_SIZE] = { ' ' };
	return D->printFunction(tabBuffer, size);
}
