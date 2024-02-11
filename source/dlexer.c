//

//	| File:			dlexer.c
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dlexer.h"

#include <ctype.h>
#include <stdlib.h>

#include "dstring.h"

// ------------------------------------------------------------------------- //
					//										//										//

static const delta_TChar* op_table[] = {
	"END",
	"LET",
	"PRINT"
};

#define DELTA_OP_TABLE_SIZE									(sizeof(op_table) / sizeof(delta_TChar*))

// ------------------------------------------------------------------------- //

/* ====================================
 * CompareStrings
 *
 * strB must end with a null-terminal
 */
static delta_TBool CompareStrings(const delta_TChar strA[], const delta_TChar strB[]);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_ReadOp
 */
const delta_TChar* delta_ReadOp(const delta_TChar str[], delta_EOp* op) {
	if (str == NULL)
		return NULL;
	
	while (*str == ' ')
		++str;

	if (isalpha(*str) == 0)
		return NULL;

	for (size_t i = 0; i < DELTA_OP_TABLE_SIZE; ++i) {
		if (CompareStrings(str, op_table[i]) == dtrue) {
			if (op != NULL)
				*op = i + 1;

			return str + delta_Strlen(op_table[i]);
		}
	}

	return NULL;
}

/* ====================================
 * delta_GetOpName
 */
const delta_TChar* delta_GetOpName(delta_EOp op) {
	if ((op < 1) || (op > DELTA_OP_TABLE_SIZE))
		return NULL;

	return op_table[op - 1];
}

/* ====================================
 * delta_ReadInteger
 */
const delta_TChar* delta_ReadInteger(const delta_TChar str[], delta_TInteger* value) {
	if (str == NULL)
		return NULL;
	
	while (*str == ' ')
		++str;

	if (isdigit(*str) == 0)
		return NULL;

	if (value != NULL)
		*value = (delta_TInteger)atol(str);

	while (isdigit(*str) != 0)
		++str;

	return str;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * CompareStrings
 */
inline delta_TBool CompareStrings(const delta_TChar strA[], const delta_TChar strB[]) {
	while ((*strA != '\0') && (*strB != '\0')) {
		if (*strA != *strB)
			return dfalse;

		++strA;
		++strB;
	}

	if (*strB == '\0')
		return dtrue;
	
	return dfalse;
}
