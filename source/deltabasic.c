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
#include "dmachine.h"

#define CreateStateAssert(exp)	if (exp) { delta_ReleaseState(D); return NULL; }

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
	if (argc == 2) {
		char* code = LoadFile(argv[1]);
		if (code == NULL) {
			printf("can't open file: \"%s\". Abort\n", argv[1]);
			return -1;
		}

		delta_SState* D = delta_CreateState(Allocator, NULL);
		if (delta_LoadString(D, code) != DELTA_OK) {
			printf("can't load code. Abort\n");
			delta_ReleaseState(D);
			free(code);

			return -1;
		}
		
		free(code);

		if (delta_Compile(D) != DELTA_OK) {
			printf("can't compile code. Abort\n");
			delta_ReleaseState(D);

			return -1;
		}

		delta_EStatus status = delta_Interpret(D, 0);
		if (status != DELTA_OK) {
			delta_ReleaseState(D);
			if (status == DELTA_END)
				return 0;

			if (status == DELTA_ALLOCATOR_ERROR)
				printf("DELTA_ALLOCATOR_ERROR\n");

			if (status == DELTA_SYNTAX_ERROR)
				printf("DELTA_SYNTAX_ERROR\n");

			return -1;
		}

		return 0;
	}

	char* code = LoadFile("test.bas");
	if (code == NULL)
		return -1;

	printf("%s\n", code);

	delta_SState* D = delta_CreateState(Allocator, NULL);
	PrintUsedMemory();

	//delta_CompileSource(D, code);
	PrintUsedMemory();

	printf("%.2i|\n", 123);
	printf("%+-11f|\n", 0.123);

	char buffer[256];
	while (1) {
		printf("> ");
		fgets(buffer, 256, stdin);

		delta_EStatus status = delta_Execute(D, buffer);
		/*
		if (delta_Compile(D) == DELTA_OK) {
			for (size_t i = 0; i < D->bytecodeSize; ++i) {
				printf("%2X ", D->bytecode[i]);
				if (i % 15 == 0)
					printf("\n");
			}
		}*/
		if (status != DELTA_OK) {
			if (status == DELTA_ALLOCATOR_ERROR)
				printf("DELTA_ALLOCATOR_ERROR\n");

			if (status == DELTA_SYNTAX_ERROR)
				printf("DELTA_SYNTAX_ERROR\n");
		}

		/*
		for (size_t i = 0; i < DELTABASIC_EXEC_BYTECODE_SIZE; ++i) {
			printf("%02X ", D->bytecode[i]);
			if (i % 16 == 15)
				printf("\n");
		}*/
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

	D->execLine				= (delta_SLine*)DELTA_Alloc(D, sizeof(delta_SLine) + sizeof(delta_TChar) * DELTABASIC_EXEC_STRING_SIZE);
	CreateStateAssert(D->execLine == NULL);
	D->execLine->str		= ((delta_TByte*)D->execLine) + sizeof(delta_SLine);

	D->bytecodeSize			= DELTABASIC_EXEC_BYTECODE_SIZE + DELTABASIC_COMPILER_INITIAL_BYTECODE_SIZE;
	D->bytecode				= (delta_TByte*)DELTA_Alloc(D, sizeof(delta_TByte) * D->bytecodeSize);
	CreateStateAssert(D->bytecode == NULL);

	return D;
}

/* ====================================
 * delta_ReleaseState
 */
void delta_ReleaseState(delta_SState* D) {
	if (D == NULL)
		return;

	delta_TAllocFunction allocFunc	= D->allocFunction;
	void* userData					= D->allocFuncUserData;

	DELTA_Free(D, D->execLine, sizeof(delta_SLine) + sizeof(delta_TChar) * DELTABASIC_EXEC_STRING_SIZE);
	DELTA_Free(D, D->bytecode, sizeof(delta_TByte) * D->bytecodeSize);

	{
		delta_SLine* line = D->head;
		while (line != NULL) {
			delta_SLine* next = line->next;

			delta_FreeNode(D, line);

			line = next; 
		}
	}

	{
		delta_SNumericVariable* nvar = D->numericValiables;
		while (nvar != NULL) {
			delta_SNumericVariable* next = nvar->next;

			delta_FreeNumericVariable(D, nvar);

			nvar = nvar; 
		}
	}
	
	{
		delta_SStringVariable* nvar = D->stringVariables;
		while (nvar != NULL) {
			delta_SStringVariable* next = nvar->next;

			delta_FreeStringVariable(D, nvar);

			nvar = nvar; 
		}
	}
	
	allocFunc(D, sizeof(delta_SState), 0, userData);
}

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_Execute
 */
delta_EStatus delta_Execute(delta_SState* D, const char execStr[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (execStr == NULL)
		return DELTA_STRING_IS_NULL;

	while (*execStr == ' ')
		++execStr;

	const delta_TChar* end = execStr + strlen(execStr);
	if (*end == '\n')
		--end;

	while ((*end == ' ') && (execStr < end))
		--end;
	
	size_t size = end - execStr;

	delta_TInteger lineNumber = 0;
	const char* str = delta_ReadInteger(execStr, &lineNumber);
	if (str == NULL) {
		delta_SBytecode bc;
		bc.bytecodeSize	= DELTABASIC_EXEC_BYTECODE_SIZE;
		bc.index		= 0;
		bc.bytecode		= D->bytecode;
		bc.bCanResize	= dfalse;

		memset(bc.bytecode, 0x00, DELTABASIC_EXEC_BYTECODE_SIZE);

		memcpy(D->execLine->str, execStr, DELTABASIC_MIN(size, DELTABASIC_EXEC_STRING_SIZE));

		delta_EStatus status = delta_CompileLine(D, D->execLine, &bc);
		if (status != DELTA_OK)
			return status;

		D->currentLine = D->execLine;
		D->ip = 0;

		while(delta_ExecuteInstruction(D) == DELTA_OK);
	}
	else {
		size = end - str;

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
 * delta_Interpret
 */
delta_EStatus delta_Interpret(delta_SState* D, size_t nInstructions) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (nInstructions == 0)
		nInstructions = SIZE_MAX;
	
	for (size_t i = 0; i < nInstructions; ++i) {
		delta_EStatus status = delta_ExecuteInstruction(D);
		if (status != DELTA_OK)
			return status;
	}

	return DELTA_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_SetPrintFunction
 */
delta_EStatus delta_LoadString(delta_SState* D, const delta_TChar str[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (str == NULL)
		return DELTA_STRING_IS_NULL;

	const delta_TChar* start = str;
	while (*str != '\0') {
		if (*str == '\n') {
			while (*start == ' ')
				++start;

			const delta_TChar* end = str;
			while ((*end == ' ') && (start < end))
				--end;

			delta_TInteger lineNumber = 0;
			const char* start = delta_ReadInteger(start, &lineNumber);
			if (start != NULL) { // TODO: if (start == NULL)
				const size_t size = end - start;

				if (size == 0)
					delta_RemoveLine(D, lineNumber);
				else {
					if (delta_InsertLine(D, lineNumber, str, size) == dfalse)
						return DELTA_ALLOCATOR_ERROR;
				}
			}

			start = str + 1;
		}

		++str;
	}

	return DELTA_OK;
}

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
