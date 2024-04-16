//

//	| File:			dstate.h
//	| Description:	
//	| Created:		2 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_STATE_H__
#define __DELTABASIC_STATE_H__

#include <stddef.h>

#include "deltabasic.h"
#include "dlimits.h"

					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SLine
 */
typedef struct delta_SLine {
	size_t			line; // Line number
	char*			str; // Allocated at the end of the struct

	size_t			offset; // In bytecode

	struct delta_SLine* prev;
	struct delta_SLine* next;
} delta_SLine;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SNumericVariable
 */
typedef struct delta_SNumericVariable {
	delta_TChar*	name; // Allocated at the end of the struct
	delta_TNumber	value;

	struct delta_SNumericVariable* next;
} delta_SNumericVariable;

/* ====================================
 * delta_SStringVariable
 */
typedef struct delta_SStringVariable {
	delta_TChar*	name; // Allocated at the end of the struct
	delta_TChar*	str;

	struct delta_SStringVariable* next;
} delta_SStringVariable;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SNumericArray
 */
typedef struct delta_SNumericArray {
	delta_TChar*	name; // Allocated at the end of the struct
	delta_TNumber*	array;
	size_t			size;

	struct delta_SNumericArray* next;
} delta_SNumericArray;

/* ====================================
 * delta_SStringArray
 */
typedef struct delta_SStringArray {
	delta_TChar*	name; // Allocated at the end of the struct
	delta_TChar**	array;
	size_t			size;

	struct delta_SStringArray* next;
} delta_SStringArray;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SReturnState
 */
typedef struct delta_SReturnState {
	delta_SLine*	line;
	size_t			ip;
} delta_SReturnState;

/* ====================================
 * delta_SForState
 */
typedef struct delta_SForState {
	delta_SLine*	startLine;
	size_t			startIp;

	delta_TNumber	end;
	delta_TNumber	step;

	delta_SNumericVariable* counter;
} delta_SForState;

// ------------------------------------------------------------------------- //

typedef union {
	delta_TNumber		numeric;
	const delta_TChar*	string;
} delta_UCFuncValue;

/* ====================================
 * delta_SCFunction
 */
typedef struct delta_SCFunction {
	delta_TChar*		name; // Allocated at the end of the struct
	delta_TCFunction	func;
	delta_TCFuncArgMask	argsMask;
	delta_TByte			argCount;
	delta_ECFuncArgType	retType;
} delta_SCFunction;

/* ====================================
 * delta_SCFuncVector
 */
typedef struct delta_SCFuncVector {
	delta_SCFunction**	array;
	size_t				size;
	size_t				allocated;
} delta_SCFuncVector;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SState
 */
struct delta_SState {
	delta_TAllocFunction	allocFunction;
	void*					allocFuncUserData;

	delta_TPrintFunction	printFunction;
	delta_TInputFunction	inputFunction;

	size_t					ip; // Instruction Pointer
	delta_SLine*			currentLine; // If NULL, do nothing (program ENDed)

	delta_SLine*			execLine;
	size_t					lineNumber; // for errors

	delta_SNumericVariable*	numericValiables;
	delta_SStringVariable*	stringVariables;

	delta_SNumericArray*	numericArrays;
	delta_SStringArray*		stringArrays;

	size_t					numericHead;
	delta_TNumber			numericStack[DELTABASIC_NUMERIC_STACK_SIZE];

	size_t					stringHead;
	delta_TChar*			stringStack[DELTABASIC_STRING_STACK_SIZE];

	size_t					returnHead;
	delta_SReturnState		returnStack[DELTABASIC_RETURN_STACK_SIZE];

	size_t					forHead;
	delta_SForState			forStack[DELTABASIC_FOR_STACK_SIZE];

	delta_SCFuncVector		cfuncVector;
	delta_SCFunction*		currentCFunc;
	delta_UCFuncValue		cfuncArgs[DELTABASIC_CFUNC_MAX_ARGS];
	delta_UCFuncValue		cfuncReturn;
	delta_TBool				bIgnoreCFuncReturn;

	size_t					bytecodeSize;
	delta_TByte*			bytecode;

	delta_TBool				bCompiled;

	delta_SLine*			head;
	delta_SLine*			tail;
};

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_InsertLine
 * 
 * strSize - size of string with a null-terminal
 */
delta_TBool			delta_InsertLine(delta_SState* D, size_t lineNumber, const delta_TChar str[], size_t strSize);

/* ====================================
 * delta_RemoveLine
 */
void				delta_RemoveLine(delta_SState* D, size_t line);

/* ====================================
 * delta_FreeNode
 *
 * Only free the node. Doesn't fix the list
 */
void				delta_FreeNode(delta_SState* D, delta_SLine* line);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FindOrAddNumericVariable
 * 
 * size - size of name in string with a null-terminal
 */
delta_SNumericVariable* delta_FindOrAddNumericVariable(delta_SState* D, const delta_TChar str[], uint16_t size);

/* ====================================
 * delta_FreeNumericVariable
 *
 * Only free the variable. Doesn't fix the list
 */
void				delta_FreeNumericVariable(delta_SState* D, delta_SNumericVariable* variable);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FindOrAddStringVariable
 * 
 * size - size of name in string with a null-terminal
 */
delta_SStringVariable* delta_FindOrAddStringVariable(delta_SState* D, const delta_TChar str[], uint16_t size);

/* ====================================
 * delta_FreeStringVariable
 *
 * Only free the variable. Doesn't fix the list
 */
void				delta_FreeStringVariable(delta_SState* D, delta_SStringVariable* variable);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FindOrAddNumericArray
 * 
 * size - size of name in string with a null-terminal
 */
delta_SNumericArray* delta_FindOrAddNumericArray(delta_SState* D, const delta_TChar str[], uint16_t size);

/* ====================================
 * delta_FreeNumericArray
 *
 * Only free the array. Doesn't fix the list
 */
void				delta_FreeNumericArray(delta_SState* D, delta_SNumericArray* array);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FindOrAddStringArray
 * 
 * size - size of name in string with a null-terminal
 */
delta_SStringArray* delta_FindOrAddStringArray(delta_SState* D, const delta_TChar str[], uint16_t size);

/* ====================================
 * delta_FreeStringArray
 *
 * Only free the array. Doesn't fix the list
 */
void				delta_FreeStringArray(delta_SState* D, delta_SStringArray* array);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FindCFunction
 */
delta_TBool			delta_FindCFunction(delta_SState* D, const delta_TChar name[], uint16_t size, size_t* index);

/* ====================================
 * delta_FreeCFunction
 */
void				delta_FreeCFunction(delta_SState* D, delta_SCFunction* function);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_FreeStringStack
 *
 */
void				delta_FreeStringStack(delta_SState* D);

#endif /* !__DELTABASIC_STATE_H__ */