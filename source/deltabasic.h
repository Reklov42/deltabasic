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

					//										//										//
typedef DELTABASIC_CONFIG_NUMBER							delta_TNumber;
typedef DELTABASIC_CONFIG_CHAR								delta_TChar;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_EStatus
 */
typedef enum {
	DELTA_OK,
	DELTA_STATE_IS_NULL,
	DELTA_STRING_IS_NULL,
	DELTA_ALLOCATOR_ERROR
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

/* ====================================
 * delta_GotoLine
 */
int					delta_GotoLine(delta_SState* D, size_t line);

/* ====================================
 * delta_New
 */
void				delta_New(delta_SState* D);

/* ====================================
 * delta_Break
 */
void				delta_Break(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Execute
 */
delta_EStatus		delta_Execute(delta_SState* D, const char str[]);

// ------------------------------------------------------------------------- //

/* ====================================
 * TReadFunction
 */
typedef size_t (*delta_TReadFunction)(void* pData, size_t size, size_t count);

/* ====================================
 * delta_Load
 */
delta_EStatus		delta_Load(delta_SState* D, delta_TReadFunction readFunc);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_TPrintFunction
 */
typedef int (*delta_TPrintFunction)(const delta_TChar str[], size_t size);

/* ====================================
 * delta_SetPrintFunction
 */
int					delta_SetPrintFunction(delta_SState* D, delta_TPrintFunction func);

#endif /* !__DELTABASIC_H__ */