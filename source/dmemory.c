//

//	| File:			dmemory.c
//	| Description:	
//	| Created:		3 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dmemory.h"

#include <stdlib.h>

#include "dlimits.h"

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Allocator
 */
void* delta_Allocator(void* ptr, size_t currentSize, size_t newSize, void* userData) {
	DELTABASIC_UNUSED(userData);
	DELTABASIC_UNUSED(currentSize);

	if (newSize == 0) {
		free(ptr);
		return NULL;
	}
	
	if (currentSize == 0)
		return malloc(newSize);
	
	return realloc(ptr, newSize);
}