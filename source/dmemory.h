//

//	| File:			dmemory.h
//	| Description:	
//	| Created:		3 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_MEMORY_H__
#define __DELTABASIC_MEMORY_H__

#include <stddef.h>

#include "deltabasic.h"

#define DELTA_Alloc(D, size)								((D)->allocFunction(NULL,		0,	(size),		(D)->allocFuncUserData))
#define DELTA_Realloc(D, ptr, size, newsize)				((D)->allocFunction((ptr),	(size),	(newsize),	(D)->allocFuncUserData))
#define DELTA_Free(D, ptr, size)							((D)->allocFunction((ptr),	(size),	0,			(D)->allocFuncUserData))

					//										//										//
/* ====================================
 * delta_Allocator
 */
void*				delta_Allocator(void* ptr, size_t currentSize, size_t newSize, void* userData);

#endif /* !__DELTABASIC_MEMORY_H__ */
