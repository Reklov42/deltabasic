/**
 * \file	dcompiler.h
 * \brief	Bytecode compiler
 * \date	1 feb 2024
 * \author	Reklov
 */
#ifndef __DELTABASIC_COMPILER_H__
#define __DELTABASIC_COMPILER_H__

#include "deltabasic.h"
#include "dstate.h"
#include "dlimits.h"

// ******************************************************************************** //

/**
 * delta_SBytecode
 */
typedef struct delta_SBytecode {
	size_t			bytecodeSize;
	size_t			index;
	delta_TByte*	bytecode;
	delta_TBool		bCanResize;
} delta_SBytecode;

/**
 * delta_Compile
 */
delta_EStatus		delta_Compile(delta_SState* D);

/**
 * delta_CompileLine
 */
delta_EStatus		delta_CompileLine(delta_SState* D, delta_SLine* Line, delta_TChar* str, delta_SBytecode* bytecode);

#endif /* !__DELTABASIC_COMPILER_H__ */