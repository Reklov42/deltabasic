//

//	| File:			dstate.c
//	| Description:	
//	| Created:		3 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "dstate.h"

#include <string.h>

#include "deltabasic_config.h"
#include "dmemory.h"
#include "dstring.h"


// ------------------------------------------------------------------------- //

/* ====================================
 * ReplaceNode
 */
static void ReplaceNode(delta_SLine* oldLine, delta_SLine* newLine);

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_InsertLine
 */
delta_TBool delta_InsertLine(delta_SState* D, size_t lineNumber, const delta_TChar str[], size_t strSize) {
	if ((D == NULL) || (str == NULL))
		return dfalse;

	const size_t blockSize = sizeof(delta_SLine) + sizeof(delta_TChar) * (strSize + 1);
	delta_SLine* line = (delta_SLine*)DELTA_Alloc(D, blockSize);
	if (line == NULL)
		return dfalse;

	memset(line, 0x00, blockSize);
	line->line = lineNumber;
	line->str = ((delta_TByte*)line) + sizeof(delta_SLine);
	memcpy(line->str, str, strSize);

	if (D->head == NULL) { // List is empty
		D->head = line;
		D->tail = line;
		return dtrue;
	}
	else if (D->head->line > lineNumber) { // Less than the first line
		line->next = D->head;
		line->prev = NULL;

		line->next->prev = line;

		D->head = line;

		return dtrue;
	}
	else if (D->tail->line < lineNumber) { // Greater than the last line
		line->next = NULL;
		line->prev = D->tail;

		line->prev->next = line;

		D->tail = line;

		return dtrue;
	}

	delta_SLine* node = D->head;
	while (node != NULL) {
		if (node->line == lineNumber) { // Replace
			line->prev = node->prev;
			line->next = node->next;

			if (line->prev != NULL)
				line->prev->next = line;

			if (line->next != NULL)
				line->next->prev = line;

			delta_FreeNode(D, node);
			if (D->head == node)
				D->head = line;

			if (D->tail == node)
				D->tail = line;
			
			return dtrue;
		}
		else if (node->line > lineNumber) { // Insert
			line->next = node;
			line->prev = node->prev;

			line->prev->next = line;
			line->next->prev = line;

			return dtrue;
		}

		node = node->next;
	}

	delta_FreeNode(D, line);

	return dfalse;
}

/* ====================================
 * delta_RemoveLine
 */
void delta_RemoveLine(delta_SState* D, size_t line) {
	if (D->head == NULL)
		return;
	else if (D->head->line > line)
		return;
	else if (D->tail->line < line)
		return;

	delta_SLine* node = D->head;
	while (node != NULL) {
		if (node->line == line) {
			if (D->head == node)
				D->head = node->next;

			if (D->tail == node)
				D->tail = node->prev;
				
			if (node->prev != NULL)
				node->prev->next = node->next;

			if (node->next != NULL)
				node->next->prev = node->prev;

			delta_FreeNode(D, node);
			return;
		}

		node = node->next;
	}
}

/* ====================================
 * delta_FreeNode
 */
inline void delta_FreeNode(delta_SState* D, delta_SLine* line) {
	DELTA_Free(D, line, sizeof(delta_SLine) + (delta_Strlen(line->str) + 1));
}