/**
 * \file	dstate.c
 * \brief	DeltaBASIC state
 * \date	3 feb 2024
 * \author	Reklov
 */
#include "dstate.h"

#include <string.h>

#include "deltabasic_config.h"
#include "dmemory.h"
#include "dstring.h"


// ******************************************************************************** //

/* ****************************************
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
	line->str = (char*)(((delta_TByte*)line) + sizeof(delta_SLine));
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

/* ****************************************
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

/* ****************************************
 * delta_FreeNode
 */
inline void delta_FreeNode(delta_SState* D, delta_SLine* line) {
	DELTA_Free(D, line, sizeof(delta_SLine) + (delta_Strlen(line->str) + 1) * sizeof(delta_TChar));
}

// ******************************************************************************** //

/* ****************************************
 * delta_FindOrAddNumericVariable
 */
delta_SNumericVariable* delta_FindOrAddNumericVariable(delta_SState* D, const delta_TChar str[], uint16_t size) {
	if (D == NULL)
		return NULL;

	delta_SNumericVariable* var = D->numericValiables;
	while (var != NULL) {
		if (delta_Strncmp(var->name, str, size) == 0)
			return var;

		var = var->next;
	}

	const size_t blockSize = sizeof(delta_SNumericVariable) + sizeof(delta_TChar) * (size + 1);
	var = (delta_SNumericVariable*)DELTA_Alloc(D, blockSize);
	if (var == NULL)
		return NULL;

	memset(var, 0x00, blockSize);
	var->name = (delta_TChar*)(((delta_TByte*)var) + sizeof(delta_SNumericVariable));
	memcpy(var->name, str, size);

	var->next = D->numericValiables;
	D->numericValiables = var;

	return var;
}

/* ****************************************
 * delta_FreeNumericVariable
 */
void delta_FreeNumericVariable(delta_SState* D, delta_SNumericVariable* variable) {
	DELTA_Free(D, variable, sizeof(delta_SNumericVariable) + (delta_Strlen(variable->name) + 1) * sizeof(delta_TChar));
}

// ******************************************************************************** //

/* ****************************************
 * delta_FindOrAddStringVariable
 */
delta_SStringVariable* delta_FindOrAddStringVariable(delta_SState* D, const delta_TChar str[], uint16_t size) {
	if (D == NULL)
		return NULL;

	delta_SStringVariable* var = D->stringVariables;
	while (var != NULL) {
		if (delta_Strncmp(var->name, str, size) == 0)
			return var;

		var = var->next;
	}

	const size_t blockSize = sizeof(delta_SStringVariable) + sizeof(delta_TChar) * (size + 1);
	var = (delta_SStringVariable*)DELTA_Alloc(D, blockSize);
	if (var == NULL)
		return NULL;

	memset(var, 0x00, blockSize);
	var->name = (delta_TChar*)(((delta_TByte*)var) + sizeof(delta_SStringVariable));
	memcpy(var->name, str, size);

	var->next = D->stringVariables;
	D->stringVariables = var;

	return var;
}

/* ****************************************
 * delta_FreeStringVariable
 */
void delta_FreeStringVariable(delta_SState* D, delta_SStringVariable* variable) {
	if (variable->str != NULL) {
		DELTA_Free(D, variable->str, (delta_Strlen(variable->str) + 1) * sizeof(delta_TChar));
	}

	DELTA_Free(D, variable, sizeof(delta_SStringVariable) + (delta_Strlen(variable->name) + 1) * sizeof(delta_TChar));
}

// ******************************************************************************** //

/* ****************************************
 * delta_FindOrAddNumericArray
 */
delta_SNumericArray* delta_FindOrAddNumericArray(delta_SState* D, const delta_TChar str[], uint16_t size) {
	if (D == NULL)
		return NULL;

	delta_SNumericArray* var = D->numericArrays;
	while (var != NULL) {
		if (delta_Strncmp(var->name, str, size) == 0)
			return var;

		var = var->next;
	}

	const size_t blockSize = sizeof(delta_SNumericArray) + sizeof(delta_TChar) * (size + 1);
	var = (delta_SNumericArray*)DELTA_Alloc(D, blockSize);
	if (var == NULL)
		return NULL;

	memset(var, 0x00, blockSize);
	var->name = (delta_TChar*)(((delta_TByte*)var) + sizeof(delta_SNumericArray));
	memcpy(var->name, str, size);

	var->next = D->numericArrays;
	D->numericArrays = var;

	return var;
}

/* ****************************************
 * delta_FreeNumericArray
 */
void delta_FreeNumericArray(delta_SState* D, delta_SNumericArray* array) {
	if (array->array != NULL) {
		DELTA_Free(D, array->array, sizeof(delta_TNumber) * (array->size));
	}

	DELTA_Free(D, array, sizeof(delta_SNumericArray) + (delta_Strlen(array->name) + 1) * sizeof(delta_TChar));
}

// ******************************************************************************** //

/* ****************************************
 * delta_FindOrAddStringArray
 */
delta_SStringArray* delta_FindOrAddStringArray(delta_SState* D, const delta_TChar str[], uint16_t size) {
	if (D == NULL)
		return NULL;

	delta_SStringArray* var = D->stringArrays;
	while (var != NULL) {
		if (delta_Strncmp(var->name, str, size) == 0)
			return var;

		var = var->next;
	}

	const size_t blockSize = sizeof(delta_SStringArray) + sizeof(delta_TChar) * (size + 1);
	var = (delta_SStringArray*)DELTA_Alloc(D, blockSize);
	if (var == NULL)
		return NULL;

	memset(var, 0x00, blockSize);
	var->name = (delta_TChar*)(((delta_TByte*)var) + sizeof(delta_SStringArray));
	memcpy(var->name, str, size);

	var->next = D->stringArrays;
	D->stringArrays = var;

	return var;
}

/* ****************************************
 * delta_FreeStringArray
 */
void delta_FreeStringArray(delta_SState* D, delta_SStringArray* array) {
	if (array->array != NULL) {
		for (size_t i = 0; i < array->size; ++i) {
			if (array->array[i] != NULL) {
				DELTA_Free(D, array->array[i], (delta_Strlen(array->array[i]) + 1) * sizeof(delta_TChar));
			}
		}

		DELTA_Free(D, array->array, sizeof(delta_TChar*) * (array->size));
	}

	DELTA_Free(D, array, sizeof(delta_SStringArray) + (delta_Strlen(array->name) + 1) * sizeof(delta_TChar));
}

// ******************************************************************************** //

/* ****************************************
 * delta_FreeStringVariable
 */
void delta_FreeStringStack(delta_SState* D) {
	for (size_t i = 0; i < D->stringHead; ++i) {
		DELTA_Free(D, D->stringStack[i], (delta_Strlen(D->stringStack[i]) + 1) * sizeof(delta_TChar));
	}

	D->stringHead = 0;
}

// ******************************************************************************** //

/* ****************************************
 * delta_FindCFunction
 */
delta_TBool delta_FindCFunction(delta_SState* D, const delta_TChar name[], uint16_t size, size_t* index) {
	for (size_t i = 0; i < D->cfuncVector.size; ++i) {
		if (delta_Strncmp(name, D->cfuncVector.array[i]->name, size) == 0) {
			if (index != NULL)
				*index = i;

			return dtrue;
		}
	}

	return dfalse;
}

/* ****************************************
 * delta_FreeCFunction
 */
void delta_FreeCFunction(delta_SState* D, delta_SCFunction* function) {
	DELTA_Free(D, function, sizeof(delta_SCFunction) + (delta_Strlen(function->name) + 1) * sizeof(delta_TChar));
}