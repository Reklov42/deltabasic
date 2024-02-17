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
 * MachineSetNumeric
 */
delta_EStatus MachineSetString(delta_SState* D);

/* ====================================
 * MachineGetString
 */
delta_EStatus MachineGetString(delta_SState* D);

// ------------------------------------------------------------------------- //

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
 * FormatNumeric
 */
size_t FormatNumeric(delta_TChar str[], size_t strSize, delta_TNumber number);

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
	OPCODE_CONCAT,
	MachineAdd,
	MachineSub,
	MachineMul,
	MachineDiv,
	MachineMod,
	MachinePow,
	MachineSetNumeric,
	MachineSetString,
	OPCODE_JMP,
	MachinePrintNumeric,
	MachinePrintNumericT,
	OPCODE_PRINTS,
	OPCODE_PRINTSN,
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
};

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_ExecuteInstruction
 */
delta_EStatus delta_ExecuteInstruction(delta_SState* D) {
	if (D->currentLine == NULL)
		return DELTA_END;

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

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	var->value = D->numericStack[--(D->numericHead)];

	D->ip += 4;
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

/* ====================================
 * MachinePrintNumeric
 */
delta_EStatus MachinePrintNumeric(delta_SState* D) {
	if (D->numericHead < 1)
		return DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW;

	--(D->numericHead);
	delta_TChar buffer[DELTABASIC_PRINT_BUFFER_SIZE];
	size_t size = FormatNumeric(
		buffer,
		DELTABASIC_PRINT_BUFFER_SIZE,
		D->numericStack[D->numericHead]
	);

	if (size < DELTABASIC_PRINT_BUFFER_SIZE)
		buffer[size++] = ' ';

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
	delta_TChar buffer[DELTABASIC_PRINT_BUFFER_SIZE];
	size_t size = FormatNumeric(
		buffer,
		DELTABASIC_PRINT_BUFFER_SIZE,
		D->numericStack[D->numericHead]
	);

	for (size; size < DELTABASIC_PRINT_BUFFER_SIZE; ++size)
		buffer[size] = ' ';

	D->printFunction(buffer, size);

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

/* ====================================
 * MachineGetNumeric
 */
delta_EStatus MachineGetNumeric(delta_SState* D) {
	D->ip += 1;
	const delta_TWord offset = ((delta_TWord*)(D->bytecode + D->ip))[0];
	const delta_TWord size   = ((delta_TWord*)(D->bytecode + D->ip))[1];

	if (D->numericHead + 1 == DELTABASIC_NUMERIC_STACK_SIZE)
		return DELTA_MACHINE_NUMERIC_STACK_OVERFLOW;

	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, offset, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	D->numericStack[(D->numericHead)++] = var->value;

	D->ip += 4;
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
	delta_TChar* str = (delta_TChar)DELTA_Alloc(D, sizeof(delta_TChar) * size + 1);
	if (str == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(str, D->execLine->str + offset, sizeof(delta_TChar) * size);
	str[size] = '\0';

	D->stringStack[(D->stringHead)++] = str;

	D->ip += 4;
	return DELTA_OK;
}

/* ====================================
 * MachineSetNumeric
 */
delta_EStatus MachineSetString(delta_SState* D);

/* ====================================
 * MachineGetString
 */
delta_EStatus MachineGetString(delta_SState* D);

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

			while (str[size - 1] == '0')
				--size;
		}
	}

	return size;
}
