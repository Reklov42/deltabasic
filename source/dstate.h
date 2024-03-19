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

/* ====================================
 * delta_SReturnState
 */
typedef struct delta_SReturnState {
	delta_SLine*	line;
	size_t			ip;
} delta_SReturnState;

/* ====================================
 * delta_SState
 */
struct delta_SState {
	delta_TAllocFunction	allocFunction;
	void*					allocFuncUserData;

	delta_TPrintFunction	printFunction;

	size_t					ip; // Instruction Pointer
	delta_SLine*			currentLine; // If NULL, do nothing (program ENDed)

	delta_SLine*			execLine;
	size_t					lineNumber; // for errors

	delta_SNumericVariable*	numericValiables;
	delta_SStringVariable*	stringVariables;

	size_t					numericHead;
	delta_TNumber			numericStack[DELTABASIC_NUMERIC_STACK_SIZE];

	size_t					stringHead;
	delta_TChar*			stringStack[DELTABASIC_STRING_STACK_SIZE];

	size_t					returnHead;
	delta_SReturnState		returnStack[DELTABASIC_RETURN_STACK_SIZE];

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
 * strSize - size of string with a null-terminal
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
 * strSize - size of string with a null-terminal
 */
delta_SStringVariable* delta_FindOrAddStringVariable(delta_SState* D, const delta_TChar str[], uint16_t size);

/* ====================================
 * delta_FreeStringVariable
 *
 * Only free the variable. Doesn't fix the list
 */
void				delta_FreeStringVariable(delta_SState* D, delta_SStringVariable* variable);

/* ====================================
 * delta_FreeStringStack
 *
 */
void				delta_FreeStringStack(delta_SState* D);

#endif /* !__DELTABASIC_STATE_H__ */