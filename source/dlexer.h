//

//	| File:			dlexer.h
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_LEXER_H__
#define __DELTABASIC_LEXER_H__

#include "dlimits.h"
#include "deltabasic.h"

					//										//										//
/* ====================================
 * delta_EOp
 */
typedef enum {
	OP_NONE,
	OP_END,
	OP_LET,
	OP_PRINT,
} delta_EOp;

/* ====================================
 * delta_ReadOp
 */
const delta_TChar*	delta_ReadOp(const delta_TChar str[], delta_EOp* op);

/* ====================================
 * delta_GetOpName
 */
const delta_TChar*	delta_GetOpName(delta_EOp op);

/* ====================================
 * delta_ReadInteger
 *
 * Returns a string pointer with offset, on error returns NULL.
 * Skips spaces.
 */
const delta_TChar*	delta_ReadInteger(const delta_TChar str[], delta_TInteger* value);

#endif /* !__DELTABASIC_LEXER_H__ */