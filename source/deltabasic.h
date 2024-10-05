/**
 * \file	deltabasic.h
 * \brief	DeltaBASIC API
 * \date	1 feb 2024
 * \author	Reklov
 */
#ifndef __DELTABASIC_H__
#define __DELTABASIC_H__

#include <stddef.h>

#include "deltabasic_config.h"

typedef DELTABASIC_CONFIG_NUMBER							delta_TNumber;
typedef DELTABASIC_CONFIG_CHAR								delta_TChar;

// ******************************************************************************** //
// State
//

/**
 * DeltaBASIC status values
 */
typedef enum {
	DELTA_OK,
	DELTA_END,
	DELTA_STATE_IS_NULL,
	DELTA_STRING_IS_NULL,
	DELTA_ALLOCATOR_ERROR,
	DELTA_SYNTAX_ERROR,
	DELTA_OUT_OF_LINES_RANGE,
	DELTA_ARG_TYPE_IS_NULL,
	DELTA_ARG_OUT_OF_RANGE,
	DELTA_FUNC_IS_NULL,

	DELTA_MACHINE_UNKNOWN_OPCODE,
	DELTA_MACHINE_NUMERIC_STACK_OVERFLOW,
	DELTA_MACHINE_NUMERIC_STACK_UNDERFLOW,
	DELTA_MACHINE_STRING_STACK_OVERFLOW,
	DELTA_MACHINE_STRING_STACK_UNDERFLOW,
	DELTA_MACHINE_RETURN_STACK_OVERFLOW,
	DELTA_MACHINE_RETURN_STACK_UNDERFLOW,
	DELTA_MACHINE_FOR_STACK_OVERFLOW,
	DELTA_MACHINE_FOR_STACK_UNDERFLOW,
	DELTA_MACHINE_NEGATIVE_ARGUMENT,
	DELTA_MACHINE_NOT_ENOUGH_INPUT_DATA,
	DELTA_MACHINE_INPUT_PARSE_ERROR,
	DELTA_MACHINE_REDIM_ERROR,
	DELTA_MACHINE_OUT_OF_RANGE,
	DELTA_MACHINE_STOP,

	DELTA_CFUNC_WRONG_RETURN_TYPE,
	DELTA_CFUNC_WRONG_ARG_TYPE,
	DELTA_CFUNC_NAME_EXISTS,
	DELTA_FUNC_CALLED_OUTSIDE_CFUNC,
	
	DELTA_MATH_STATUS,
} delta_EStatus;

/**
 * DeltaBASIC state structure
 */
typedef struct delta_SState delta_SState;

/**
 * \brief User allocator
 * 
 * \note When `newSize` is zero, the allocator must return `NULL`.
 * If `currentSize` is not zero and `newSize` is zero, it should free the block pointed to by `ptr`.
 * If `currentSize` is not zero and `newSize` is not zero, it should behave like realloc.
 * The allocator is assumed to never fail if `currentSize >= newSize`.
 * 
 * `ptr` is `NULL` if and only if `currentSize` is zero.
 * If reallocation returns `NULL`, it MUST free `ptr`.
 */
typedef void* (*delta_TAllocFunction)(void* ptr, size_t currentSize, size_t newSize, void* userData);

/**
 * Create new DeltaBASIC state.
 * 
 * If `allocFunc` is `NULL`, `malloc/free` based allocator will be used.
 */
delta_SState*		delta_CreateState(delta_TAllocFunction allocFunc, void* allocFuncUserData);

/**
 * delta_ReleaseState
 */
void				delta_ReleaseState(delta_SState* D);

// ******************************************************************************** //
// C-side variables and commands
//

/**
 * Run code from first line
 */
delta_EStatus		delta_Run(delta_SState* D);

/**
 * Run code from `line`
 */
delta_EStatus		delta_GotoLine(delta_SState* D, size_t line); // TODO: implement

/**
 * delta_New
 *
 * Delete all lines, variables and clear bytecode buffer
 */
delta_EStatus		delta_New(delta_SState* D); // TODO: implement

/**
 * \param[out] line last line number
 */
delta_EStatus		delta_GetLastLine(delta_SState* D, size_t* line);

/**
 * Set numeric variable
 */
delta_EStatus		delta_SetNumeric(delta_SState* D, const char name[], delta_TNumber value);

/**
 * Get numeric variable
 */
delta_EStatus		delta_GetNumeric(delta_SState* D, const char name[], delta_TNumber* value);

/**
 * Set string variable
 */
delta_EStatus		delta_SetString(delta_SState* D, const char name[], const char value[]);

/**
 * Get string variable
 *
 * \warning DO NOT save a pointer, just copy the data
 */
delta_EStatus		delta_GetString(delta_SState* D, const char name[], const char* value[]);

// ******************************************************************************** //
// C-side Function
//

/**
 * delta_ECFuncArgType
 *
 * Used in bit field
 */
typedef enum {
	DELTA_CFUNC_ARG_NUMERIC = 0,
	DELTA_CFUNC_ARG_STRING = 1
} delta_ECFuncArgType;

/**
 * delta_GetArgType
 */
//delta_EStatus		delta_GetArgType(delta_SState* D, size_t index, delta_ECFuncArgType* type);

/**
 * delta_SetNumeric
 */
delta_EStatus		delta_GetArgNumeric(delta_SState* D, size_t index, delta_TNumber* value);

/**
 * delta_GetArgString
 *
 * \warning DO NOT save a pointer, just copy the data
 */
delta_EStatus		delta_GetArgString(delta_SState* D, size_t index, const char* value[]);

/**
 * delta_ReturnNumeric
 */
delta_EStatus		delta_ReturnNumeric(delta_SState* D, delta_TNumber value);

/**
 * delta_ReturnString
 */
delta_EStatus		delta_ReturnString(delta_SState* D, const char value[]);

/**
 * delta_TCFunction
 */
typedef delta_EStatus (*delta_TCFunction)(delta_SState* D);

/**
 * delta_RegisterCFunction
 */
delta_EStatus		delta_RegisterCFunction(delta_SState* D, const char name[], delta_ECFuncArgType argsType[], size_t argCount, delta_ECFuncArgType returnType, delta_TCFunction func);

// ******************************************************************************** //
// Execution
//

/**
 * delta_Execute
 */
delta_EStatus		delta_Execute(delta_SState* D, const char str[]);

/**
 * Interpret `nInstructions` VM's instructions
 * If `nInstructions` set to zero, interpret all code
 */
delta_EStatus		delta_Interpret(delta_SState* D, size_t nInstructions);

// ******************************************************************************** //
// File IO
//

/**
 * delta_TReadFunction
 */
typedef size_t (*delta_TReadFunction)(void* pData, size_t size, size_t count);

/**
 * delta_Load
 */
delta_EStatus		delta_Load(delta_SState* D, delta_TReadFunction readFunc); // TODO: implement

/**
 * delta_LoadString
 */
delta_EStatus		delta_LoadString(delta_SState* D, const delta_TChar str[]);

// ******************************************************************************** //
// Terminal IO
//

/**
 * User function for `PRINT` command
 */
typedef int (*delta_TPrintFunction)(const delta_TChar str[], size_t size);

/**
 * delta_SetPrintFunction
 */
delta_EStatus		delta_SetPrintFunction(delta_SState* D, delta_TPrintFunction func);

/**
 * User function for `INPUT` command
 *
 * \returns filled size or -1 on error
 */
typedef int (*delta_TInputFunction)(delta_TChar* buffer, size_t size);

/**
 * delta_SetInputFunction
 */
delta_EStatus		delta_SetInputFunction(delta_SState* D, delta_TInputFunction func);

#endif /* !__DELTABASIC_H__ */