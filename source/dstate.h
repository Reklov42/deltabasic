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
 * delta_SState
 */
struct delta_SState {
	delta_TAllocFunction	allocFunction;
	void*					allocFuncUserData;

	delta_TPrintFunction	printFunction;

	size_t					ip; // Instruction Pointer
	delta_SLine*			currentLine; // If NULL, do nothing (program ENDed)

	delta_SLine*			execLine;

	delta_SNumericVariable*	numericValiables;

	size_t					numericHead;
	delta_TNumber			numericStack[DELTABASIC_NUMERIC_STACK_SIZE];

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
delta_SNumericVariable* delta_FindOrAddNumericVariable(delta_SState* D, uint16_t offset, uint16_t size);

/* ====================================
 * delta_FreeNumericVariable
 *
 * Only free the variable. Doesn't fix the list
 */
void				delta_FreeNumericVariable(delta_SState* D, delta_SNumericVariable* variable);

#endif /* !__DELTABASIC_STATE_H__ */