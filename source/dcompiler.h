//

//	| File:			dcompiler.c
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_COMPILER_H__
#define __DELTABASIC_COMPILER_H__

#include "deltabasic.h"
#include "dstate.h"

					//										//										//

typedef struct delta_SBytecode {
	size_t			bytecodeSize;
	size_t			index;
	delta_TByte*	bytecode;
} delta_SBytecode;

/* ====================================
 * delta_Compile
 */
delta_EStatus		delta_Compile(delta_SState* D);

/* ====================================
 * delta_CompileLine
 */
delta_EStatus		delta_CompileLine(delta_SState* D, delta_SLine* Line, delta_SBytecode* bytecode);

#endif /* !__DELTABASIC_COMPILER_H__ */