//

//	| File:			dstring.h
//	| Description:	
//	| Created:		8 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_STRING_H__
#define __DELTABASIC_STRING_H__

#include <stddef.h>

#include "deltabasic.h"

					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Strlen
 */
size_t delta_Strlen(const delta_TChar str[]);

/* ====================================
 * delta_Strncmp
 */
int delta_Strncmp(const delta_TChar strA[], const delta_TChar strB[], size_t n);

#endif /* !__DELTABASIC_STRING_H__ */
