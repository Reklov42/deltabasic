//

//	| File:			DeltaBasic.c
//	| Description:	Header to include as a library
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#include "deltabasic.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dlimits.h"
#include "dstate.h"
#include "dlexer.h"
#include "dmemory.h"
#include "dcompiler.h"

// ------------------------------------------------------------------------- //

/* ====================================
 * LoadFile
 */
char* LoadFile(const char path[]);

static size_t usedMemory = 0;
void PrintUsedMemory() {
	printf("Used: %zu (%.2f)\n", usedMemory, ((float)usedMemory) / 1024.0f);
}

void* Allocator(void* ptr, size_t currentSize, size_t newSize, void* userData) {
	DELTABASIC_UNUSED(userData);
	DELTABASIC_UNUSED(currentSize);

	if (newSize == 0) {
		usedMemory -= currentSize;
		free(ptr);
		return NULL;
	}
	
	if (currentSize == 0) {
		usedMemory += newSize;
		return malloc(newSize);
	}
	else {
		if (currentSize > newSize)
			usedMemory -= currentSize - newSize;
		else
			usedMemory += newSize - currentSize;

		return realloc(ptr, newSize);
	}
}

/* ====================================
 * delta_Print
 */
int delta_Print(const delta_TChar str[], size_t size);

// ------------------------------------------------------------------------- //

/* ====================================
 * main
 */
int main(int argc, char* argv[]) {
	char* code = LoadFile("test.bas");
	if (code == NULL)
		return -1;

	printf("%s\n", code);

	delta_SState* D = delta_CreateState(Allocator, NULL);
	PrintUsedMemory();

	//delta_CompileSource(D, code);
	PrintUsedMemory();


	char buffer[256];
	while (1) {
		printf("> ");
		fgets(buffer, 256, stdin);

		delta_Execute(D, buffer);
		if (delta_Compile(D) == DELTA_OK) {
			for (size_t i = 0; i < D->bytecodeSize; ++i) {
				printf("%2X ", D->bytecode[i]);
				if (i % 15 == 0)
					printf("\n");
			}
		}
	}

	delta_ReleaseState(D);
	PrintUsedMemory();

	return 0;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * LoadFile
 */
char* LoadFile(const char path[]) {
	FILE* pFile = fopen(path, "rt");
	if (pFile == NULL) {
		printf("Can't open file: \"%s\"\n", path);
		return NULL;
	}

	fseek(pFile, 0, SEEK_END);
	size_t size = (size_t)ftell(pFile);
	//fseek(file, 0L, SEEK_SET);
	rewind(pFile);

	char* buffer = (char*)malloc(sizeof(char) * (size + 2));
	if (buffer == NULL) {
		fclose(pFile);
		return NULL;
	}
	
	fread(buffer, sizeof(char), size, pFile);
	fclose(pFile);
	buffer[size] = '\n';
	buffer[size + 1] = '\0';

	return buffer;
}

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Print
 */
int delta_Print(const delta_TChar str[], size_t size) {
	size_t n = fwrite(str, sizeof(char), size, stdout);
	fflush(stdout);

	return n;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_CreateState
 */
delta_SState* delta_CreateState(delta_TAllocFunction allocFunc, void* allocFuncUserData) {
	if (allocFunc == NULL)
		allocFunc = delta_Allocator;

	delta_SState* D = (delta_SState*)allocFunc(NULL, 0, sizeof(delta_SState), allocFuncUserData);
	if (D == NULL)
		return NULL;

	memset(D, 0x00, sizeof(delta_SState));

	D->allocFunction		= allocFunc;
	D->allocFuncUserData	= allocFuncUserData;
	D->printFunction		= delta_Print;

	return D;
}

/* ====================================
 * delta_ReleaseState
 */
void delta_ReleaseState(delta_SState* D) {
	if (D == NULL)
		return;

	delta_TAllocFunction allocFunc = D->allocFunction;
	void* userData = D->allocFuncUserData;

	{
		delta_SLine* line = D->head;
		while (line != NULL) {
			delta_SLine* next = line->next;

			delta_FreeNode(D, line);

			line = next; 
		}
	}
	
	allocFunc(D, sizeof(delta_SState), 0, userData);
}

/* ====================================
 * delta_Execute
 */
delta_EStatus delta_Execute(delta_SState* D, const char str[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (str == NULL)
		return DELTA_STRING_IS_NULL;

	delta_TInteger lineNumber = 0;
	str = delta_ReadInteger(str, &lineNumber);
	if (str == NULL) {

	}
	else {
		while (*str == ' ')
			++str;

		const delta_TChar* end = str + strlen(str);
		if (*end == '\n')
			--end;

		while ((*end == ' ') && (str < end))
			--end;
		
		const size_t size = str - end;

		D->bCompiled = dfalse;
		if (size == 0)
			delta_RemoveLine(D, lineNumber);
		else {
			if (delta_InsertLine(D, lineNumber, str, size) == dfalse)
				return DELTA_ALLOCATOR_ERROR;
		}
	}

	return DELTA_OK;
}

/* ====================================
 * delta_InterpretString
 */
delta_EStatus delta_InterpretString(delta_SState* D, const char str[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //


// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SetPrintFunction
 */
int delta_SetPrintFunction(delta_SState* D, delta_TPrintFunction func) {
	if (func == NULL)
		return 0;

	D->printFunction = func;
	return dtrue;
}
