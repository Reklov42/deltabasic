//

//	| File:			dcompiler.h
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dcompiler.h"

#include <string.h>

#include "dlexer.h"
#include "dmemory.h"

					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * PushBytecodeByte
 */
static delta_TBool	PushBytecodeByte(delta_SState* D, delta_TByte byte);

/* ====================================
 * PushBytecodeWord
 */
static delta_TBool	PushBytecodeWord(delta_SState* D, delta_TWord word);

/* ====================================
 * ExpandBytecodeBuffer
 */
static delta_TBool	ExpandBytecodeBuffer(delta_SState* D);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Compile
 */
delta_EStatus delta_Compile(delta_SState* D) {
	if (D->head == NULL)
		return DELTA_OK;

	if (D->bytecodeSize != 0)
		DELTA_Free(D, D->bytecode, sizeof(delta_TByte) * D->bytecodeSize);

	D->bytecodeSize = 0;
	D->bytecode = DELTA_Alloc(D, sizeof(delta_TByte) * DELTABASIC_COMPILER_INITIAL_BYTECODE_SIZE);
	if (D->bytecode == NULL)
		return DELTA_ALLOCATOR_ERROR;

	D->bytecodeSize = DELTABASIC_COMPILER_INITIAL_BYTECODE_SIZE;

	for (delta_SLine* node = D->head; node != NULL; node = node->next) {

	}

	return DELTA_OK;
}

/* ====================================
 * delta_CompileLine
 */
delta_EStatus delta_CompileLine(delta_SState* D, delta_SLine* Line, delta_SBytecode* bytecode) {

}

// ------------------------------------------------------------------------- //

/* ====================================
 * PushBytecodeByte
 *
delta_TBool PushBytecodeByte(delta_SCompilerState* C, delta_TByte byte) {
	if (C->iBytecode + 1 >= C->bytecodeBufferSize) {
		if (ExpandBytecodeBuffer(C) == dfalse)
			return dfalse;
	}

	C->bytecode[C->iBytecode] = byte;
	C->iBytecode += 1;

	return dtrue;
}

/* ====================================
 * PushBytecodeWord
 *
delta_TBool PushBytecodeWord(delta_SState* C, delta_TWord word) {
	if (C->iBytecode + 2 >= C->bytecodeBufferSize) {
		if (ExpandBytecodeBuffer(C) == dfalse)
			return dfalse;
	}

	*((delta_TWord*)(C->bytecode + C->iBytecode)) = word;
	C->iBytecode += 2;

	return dtrue;
}
*/
/* ====================================
 * PushBytecodeByte
 */
delta_TBool ExpandBytecodeBuffer(delta_SState* D) {
	const size_t newSize = D->bytecodeSize * 2;

	D->bytecode = DELTA_Realloc(
		D,
		D->bytecode,
		sizeof(delta_TByte) * D->bytecodeSize,
		sizeof(delta_TByte) * newSize
	);

	if (D->bytecode == NULL)
		return dfalse;

	D->bytecodeSize = newSize;

	return dtrue;
}
