//

//	| File:			DeltaBasic.h
//	| Description:	Header to include as a library
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_H__
#define __DELTABASIC_H__

#include <stddef.h>

#include "deltabasic_config.h"

#define DELTABASIC_TYPE_NUMBER								0x01
#define DELTABASIC_TYPE_STRING								0x02

					//										//										//
typedef DELTABASIC_CONFIG_NUMBER							delta_TNumber;
typedef DELTABASIC_CONFIG_CHAR								delta_TChar;

// ------------------------------------------------------------------------- //
//
//

/* ====================================
 * delta_EStatus
 */
typedef enum {
	DELTA_OK,
	DELTA_END,
	DELTA_STATE_IS_NULL,
	DELTA_STRING_IS_NULL,
	DELTA_ALLOCATOR_ERROR,
	DELTA_SYNTAX_ERROR,

	DELTA_MACHINE_UNKNOWN_OPCODE,
	DELTA_MACHINE_NUMERIC_STACK_OVERFLOW,
	DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW,
	DELTA_MACHINE_STRING_STACK_OVERFLOW,
	DELTA_MACHINE_STRING_STACK_UNDERFLOW,
	DELTA_MACHINE_NEGATIVE_ARGUMENT,
	DELTA_MACHINE_STOP,
	
	DELTA_MATH_STATUS,
} delta_EStatus;

/* ====================================
 * delta_SState
 */
typedef struct delta_SState delta_SState;

/* ====================================
 * delta_TAllocFunction
 *
 * When newSize is zero, the allocator must return NULL.
 * If currentSize is not zero and newSize is zero, it should free the block pointed to by ptr
 * If currentSize is not zero and newSize is not zero, it should behave like realloc.
 * The allocator is assumed to never fail if currentSize >= newSize.
 * 
 * ptr is NULL if and only if currentSize is zero.
 * If reallocation returns NULL, it MUST free ptr.
 */
typedef void* (*delta_TAllocFunction)(void* ptr, size_t currentSize, size_t newSize, void* userData);

/* ====================================
 * delta_CreateState
 */
delta_SState*		delta_CreateState(delta_TAllocFunction allocFunc, void* allocFuncUserData);

/* ====================================
 * delta_ReleaseState
 */
void				delta_ReleaseState(delta_SState* D);

// ------------------------------------------------------------------------- //
//
//

/* ====================================
 * delta_GotoLine
 */
delta_EStatus		delta_GotoLine(delta_SState* D, size_t line);

/* ====================================
 * delta_New
 *
 * Delete all lines, variables and clear bytecode buffer
 */
delta_EStatus		delta_New(delta_SState* D);

/* ====================================
 * delta_Break
 */
delta_EStatus		delta_Break(delta_SState* D);

/* ====================================
 * delta_SetNumeric
 */
delta_EStatus		delta_SetNumeric(delta_SState* D, const char str[], delta_TNumber value);

/* ====================================
 * delta_GetNumeric
 */
delta_EStatus		delta_GetNumeric(delta_SState* D, const char str[], delta_TNumber* value);

/* ====================================
 * delta_SetString
 */
delta_EStatus		delta_SetString(delta_SState* D, const char str[], const char value[]);

/* ====================================
 * delta_GetString
 *
 * DO NOT save a pointer, just copy the data
 */
delta_EStatus		delta_GetString(delta_SState* D, const char str[], const char* value[]);

// ------------------------------------------------------------------------- //
//
//

/* ====================================
 * delta_Execute
 */
delta_EStatus		delta_Execute(delta_SState* D, const char str[]);

/* ====================================
 * delta_Interpret
 *
 * Interpret nInstructions VM's instructions
 * If N set to zero, interpret all code
 */
delta_EStatus		delta_Interpret(delta_SState* D, size_t nInstructions);

// ------------------------------------------------------------------------- //
//
//

/* ====================================
 * TReadFunction
 */
typedef size_t (*delta_TReadFunction)(void* pData, size_t size, size_t count);

/* ====================================
 * delta_Load
 */
delta_EStatus		delta_Load(delta_SState* D, delta_TReadFunction readFunc);

/* ====================================
 * delta_LoadString
 */
delta_EStatus		delta_LoadString(delta_SState* D, const delta_TChar str[]);

// ------------------------------------------------------------------------- //
//
//

/* ====================================
 * delta_TPrintFunction
 */
typedef int (*delta_TPrintFunction)(const delta_TChar str[], size_t size);

/* ====================================
 * delta_SetPrintFunction
 */
delta_EStatus		delta_SetPrintFunction(delta_SState* D, delta_TPrintFunction func);

#endif /* !__DELTABASIC_H__ */