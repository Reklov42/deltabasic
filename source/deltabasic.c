/**
 * \file	deltabasic.c
 * \brief	Basic BASIC functions ;)
 * \date	1 feb 2024
 * \author	Reklov
 */
#include "deltabasic.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dlimits.h"
#include "dstate.h"
#include "dstring.h"
#include "dlexer.h"
#include "dmemory.h"
#include "dcompiler.h"
#include "dmachine.h"

#define CreateStateAssert(exp)	if (exp) { delta_ReleaseState(D); return NULL; }

// ******************************************************************************** //

/**
 * delta_Print
 */
int delta_Print(const delta_TChar str[], size_t size);

/**
 * delta_Input
 */
int delta_Input(delta_TChar* buffer, size_t size);

// ******************************************************************************** //

#ifndef __DELTABASIC_LIB__

// ******************************************************************************** //

/**
 * LoadFile
 */
char* LoadFile(const char path[]);

/**
 * main
 */
int main(int argc, char* argv[]) {
	delta_SState* D = delta_CreateState(NULL, NULL);

	if (argc == 2) {
		char* code = LoadFile(argv[1]);
		if (code == NULL)
			return -1;

		if (delta_LoadString(D, code) != DELTA_OK) {
			printf("can't load code. Abort\n");
			delta_ReleaseState(D);
			free(code);

			return -1;
		}
		
		free(code);

		printf("Compiling...\n");
		if (delta_Compile(D) != DELTA_OK) {
			printf("can't compile code (ERROR IN %zu). Abort\n", D->lineNumber);
			delta_ReleaseState(D);

			return -1;
		}

		printf("Interpreting...\n");
		delta_EStatus status = delta_Interpret(D, 0);
		if (status != DELTA_OK) {
			delta_ReleaseState(D);
			if (status == DELTA_END)
				return 0;

			return -1;
		}

		printf("Done.\n");

		return 0;
	}

	char buffer[256];
	while (1) {
		printf("> ");
		fgets(buffer, 256, stdin);

		delta_EStatus status = delta_Execute(D, buffer);
		if (status == DELTA_OK)
			status = delta_Interpret(D, 0);
			
		if (status != DELTA_OK) {
			if (status == DELTA_END) {
				printf("READY\n");
			}
			else {
				size_t line = 0;
				delta_GetLastLine(D, &line);
				printf("ERROR: %u", status);
				if (line != DELTABASIC_EXEC_LINE_NUMBER)
					printf(" IN LINE %lu\n", line);
				else
					printf("\n");

			}
		}
	}

	delta_ReleaseState(D);

	return 0;
}

// ******************************************************************************** //

/* ****************************************
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

// ******************************************************************************** //

#endif

// ******************************************************************************** //

/* ****************************************
 * delta_Print
 */
int delta_Print(const delta_TChar str[], size_t size) {
	size_t n = fwrite(str, sizeof(char), size, stdout);
	fflush(stdout);

	return n;
}

/* ****************************************
 * delta_Input
 */
int delta_Input(delta_TChar* buffer, size_t size) {
	size_t i;
	for (i = 0; i < size; i++) {
		char ch = getchar();
		if ((ch == EOF) || (ch == '\n'))
			return i;

		buffer[i] = ch;
	}

	return i;
}

// ******************************************************************************** //

/* ****************************************
 * delta_CreateState
 */
delta_SState* delta_CreateState(delta_TAllocFunction allocFunc, void* allocFuncUserData) {
	if (allocFunc == NULL)
		allocFunc = delta_Allocator;

	delta_SState* D = (delta_SState*)allocFunc(NULL, 0, sizeof(delta_SState), allocFuncUserData);
	if (D == NULL)
		return NULL;

	memset(D, 0x00, sizeof(delta_SState));

	D->allocFunction			= allocFunc;
	D->allocFuncUserData		= allocFuncUserData;
	D->printFunction			= delta_Print;
	D->inputFunction			= delta_Input;

	D->execLine					= (delta_SLine*)DELTA_Alloc(D, sizeof(delta_SLine) + sizeof(delta_TChar) * DELTABASIC_EXEC_STRING_SIZE);
	CreateStateAssert(D->execLine == NULL);
	D->execLine->str			= (char*)(((delta_TByte*)D->execLine) + sizeof(delta_SLine));

	D->bytecodeSize				= DELTABASIC_EXEC_BYTECODE_SIZE + DELTABASIC_COMPILER_INITIAL_BYTECODE_SIZE;
	D->bytecode					= (delta_TByte*)DELTA_Alloc(D, sizeof(delta_TByte) * D->bytecodeSize);
	CreateStateAssert(D->bytecode == NULL);

	D->cfuncVector.allocated 	= DELTABASIC_CFUNC_VECTOR_START_SIZE;
	D->cfuncVector.size			= 0;
	D->cfuncVector.array		= (delta_SCFunction**)DELTA_Alloc(D, sizeof(delta_SCFunction*) * D->cfuncVector.allocated);
	CreateStateAssert(D->cfuncVector.array == NULL);

	return D;
}

/* ****************************************
 * delta_ReleaseState
 */
void delta_ReleaseState(delta_SState* D) {
	if (D == NULL)
		return;

	delta_TAllocFunction allocFunc	= D->allocFunction;
	void* userData					= D->allocFuncUserData;

	DELTA_Free(D, D->execLine, sizeof(delta_SLine) + sizeof(delta_TChar) * DELTABASIC_EXEC_STRING_SIZE);
	DELTA_Free(D, D->bytecode, sizeof(delta_TByte) * D->bytecodeSize);

	if (D->cfuncVector.array != NULL) {
		for (size_t i = 0; i < D->cfuncVector.size; ++i)
			delta_FreeCFunction(D, D->cfuncVector.array[i]);

		DELTA_Free(D, D->cfuncVector.array, sizeof(delta_SCFunction*) * D->cfuncVector.allocated);
	}

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

			nvar = next; 
		}
	}
	
	{
		delta_SStringVariable* nvar = D->stringVariables;
		while (nvar != NULL) {
			delta_SStringVariable* next = nvar->next;

			delta_FreeStringVariable(D, nvar);

			nvar = next; 
		}
	}


	{
		delta_SNumericArray* narr = D->numericArrays;
		while (narr != NULL) {
			delta_SNumericArray* next = narr->next;

			delta_FreeNumericArray(D, narr);

			narr = next; 
		}
	}

	{
		delta_SStringArray* narr = D->stringArrays;
		while (narr != NULL) {
			delta_SStringArray* next = narr->next;

			delta_FreeStringArray(D, narr);

			narr = next; 
		}
	}
	
	allocFunc(D, sizeof(delta_SState), 0, userData);
}

// ******************************************************************************** //

/* ****************************************
 * delta_Run
 */
delta_EStatus delta_Run(delta_SState* D) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (D->bCompiled == dtrue) {
		D->ip			= DELTABASIC_EXEC_BYTECODE_SIZE;
		D->currentLine	= D->head;

		return DELTA_OK;
	}

	return delta_Compile(D);
}

/* ****************************************
 * delta_GetLastLine
 */
delta_EStatus delta_GetLastLine(delta_SState* D, size_t* line) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (line != NULL)
		*line = D->lineNumber;

	return DELTA_OK;
}

// ******************************************************************************** //

/* ****************************************
 * delta_SetNumeric
 */
delta_EStatus delta_SetNumeric(delta_SState * D, const char name[], delta_TNumber value) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (name == NULL)
		return DELTA_STRING_IS_NULL;

	size_t size = strlen(name);
	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, name, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;


	var->value = value;

	return DELTA_OK;
}

/* ****************************************
 * delta_GetNumeric
 */
delta_EStatus delta_GetNumeric(delta_SState* D, const char name[], delta_TNumber* value) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (name == NULL)
		return DELTA_STRING_IS_NULL;

	size_t size = strlen(name);
	delta_SNumericVariable* var = delta_FindOrAddNumericVariable(D, name, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (value != NULL)
		*value = var->value;

	return DELTA_OK;
}

/* ****************************************
 * delta_SetString
 */
delta_EStatus delta_SetString(delta_SState* D, const char name[], const char value[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (name == NULL)
		return DELTA_STRING_IS_NULL;

	size_t valueSize = strlen(value);
	delta_TChar* buffer = (delta_TChar*)DELTA_Alloc(D, sizeof(delta_TChar) * (valueSize + 1));
	if (name == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memcpy(buffer, value, sizeof(delta_TChar) * valueSize);
	buffer[valueSize] = '\0';

	size_t size = strlen(name);
	delta_SStringVariable* var = delta_FindOrAddStringVariable(D, name, size);
	if (var == NULL) {
		DELTA_Free(D, buffer, sizeof(delta_TChar) * (valueSize + 1));
		return DELTA_ALLOCATOR_ERROR;
	}

	if (var->str != NULL) {
		DELTA_Free(D, var->str, (strlen(var->str) + 1) * sizeof(delta_TChar));
	}

	var->str = buffer;

	return DELTA_OK;
}

/* ****************************************
 * delta_GetString
 */
delta_EStatus delta_GetString(delta_SState* D, const char name[], const char* value[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (name == NULL)
		return DELTA_STRING_IS_NULL;

	size_t size = strlen(name);
	delta_SStringVariable* var = delta_FindOrAddStringVariable(D, name, size);
	if (var == NULL)
		return DELTA_ALLOCATOR_ERROR;

	if (value != NULL)
		*value = var->str;

	return DELTA_OK;
}

// ******************************************************************************** //

/* ****************************************
 * delta_GetArgNumeric
 */
delta_EStatus delta_GetArgNumeric(delta_SState* D, size_t index, delta_TNumber* value) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (D->currentCFunc == NULL)
		return DELTA_FUNC_CALLED_OUTSIDE_CFUNC;
	
	if (index > D->currentCFunc->argCount)
		return 	DELTA_ARG_OUT_OF_RANGE;

	if (((D->currentCFunc->argsMask >> index) & 0x01) != DELTA_CFUNC_ARG_NUMERIC)
		return DELTA_CFUNC_WRONG_ARG_TYPE;

	if (value != NULL)
		*value = D->cfuncArgs[index].numeric;

	return DELTA_OK;
}

/* ****************************************
 * delta_GetArgString
 */
delta_EStatus delta_GetArgString(delta_SState* D, size_t index, const char* value[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (D->currentCFunc == NULL)
		return DELTA_FUNC_CALLED_OUTSIDE_CFUNC;
	
	if (index > D->currentCFunc->argCount)
		return 	DELTA_ARG_OUT_OF_RANGE;

	if (((D->currentCFunc->argsMask >> index) & 0x01) != DELTA_CFUNC_ARG_STRING)
		return DELTA_CFUNC_WRONG_ARG_TYPE;

	if (value != NULL)
		*value = D->cfuncArgs[index].string;

	return DELTA_OK;
}

/* ****************************************
 * delta_ReturnNumeric
 */
delta_EStatus delta_ReturnNumeric(delta_SState* D, delta_TNumber value) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (D->currentCFunc == NULL)
		return DELTA_FUNC_CALLED_OUTSIDE_CFUNC;

	if (D->currentCFunc->retType != DELTA_CFUNC_ARG_NUMERIC)
		return DELTA_CFUNC_WRONG_RETURN_TYPE;

	D->cfuncReturn.numeric = value;

	return DELTA_OK;
}

/* ****************************************
 * delta_ReturnString
 */
delta_EStatus delta_ReturnString(delta_SState* D, const char value[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (D->currentCFunc == NULL)
		return DELTA_FUNC_CALLED_OUTSIDE_CFUNC;

	if (D->currentCFunc->retType != DELTA_CFUNC_ARG_STRING)
		return DELTA_CFUNC_WRONG_RETURN_TYPE;

	if (D->bIgnoreCFuncReturn == dfalse) {
		if (D->cfuncReturn.string != NULL) {
			DELTA_Free(D, (delta_TChar*)(D->cfuncReturn.string), sizeof(delta_TChar) * (delta_Strlen(D->cfuncReturn.string) + 1));
			D->cfuncReturn.string = NULL;
		}

		size_t size = delta_Strlen(value);
		delta_TChar* string = DELTA_Alloc(D, sizeof(delta_TChar) * (size + 1));
		if (string == NULL)
			return DELTA_ALLOCATOR_ERROR;

		memcpy(string, value, sizeof(delta_TChar) * size);
		string[size] = '\0';
		D->cfuncReturn.string = string;
	}

	return DELTA_OK;
}

/* ****************************************
 * delta_RegisterCFunction
 */
delta_EStatus delta_RegisterCFunction(delta_SState* D, const char name[], delta_ECFuncArgType argsType[], size_t argCount, delta_ECFuncArgType returnType, delta_TCFunction func) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (name == NULL)
		return DELTA_STRING_IS_NULL;

	if ((argCount != 0) && (argsType == NULL))
		return DELTA_ARG_TYPE_IS_NULL;

	if (argCount > DELTABASIC_CFUNC_MAX_ARGS)
		return 	DELTA_ARG_OUT_OF_RANGE;

	if (func == NULL)
		return DELTA_FUNC_IS_NULL;

	size_t size = strlen(name);
	if (delta_FindCFunction(D, name, size, NULL) == dtrue)
		return DELTA_CFUNC_NAME_EXISTS;

	const size_t blockSize = sizeof(delta_SCFunction) + sizeof(delta_TChar) * (size + 1);
	delta_SCFunction* funcData = (delta_SCFunction*)DELTA_Alloc(D, blockSize);
	if (funcData == NULL)
		return DELTA_ALLOCATOR_ERROR;

	memset(funcData, 0x00, blockSize);
	funcData->name = (delta_TChar*)(((delta_TByte*)funcData) + sizeof(delta_SCFunction));
	memcpy(funcData->name, name, size);

	funcData->func = func;
	funcData->argCount = argCount;
	funcData->retType = returnType;
	for (size_t i = 0; i < argCount; ++i) {
		if (argsType[i] == DELTA_CFUNC_ARG_STRING)
			funcData->argsMask |= 1 << i;
	}

	if (D->cfuncVector.size + 1 >= D->cfuncVector.allocated) {
		const size_t newSize = D->cfuncVector.allocated * 2;

		D->cfuncVector.array = DELTA_Realloc(
			D,
			D->cfuncVector.array,
			sizeof(delta_SCFunction*) * D->cfuncVector.allocated,
			sizeof(delta_SCFunction*) * newSize
		);

		if (D->cfuncVector.array == NULL) {
			delta_FreeCFunction(D, funcData);
			return DELTA_ALLOCATOR_ERROR;
		}

		D->cfuncVector.allocated = newSize;
	}

	D->cfuncVector.array[D->cfuncVector.size] = funcData;
	++(D->cfuncVector.size);

	return DELTA_OK;
}

// ******************************************************************************** //

/* ****************************************
 * delta_Execute
 */
delta_EStatus delta_Execute(delta_SState* D, const char execStr[]) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (execStr == NULL)
		return DELTA_STRING_IS_NULL;

	while (*execStr == ' ')
		++execStr;

	const delta_TChar* end = execStr + strlen(execStr) - 1;
	if (*end == '\n')
		--end;

	while ((*end == ' ') && (execStr < end))
		--end;
	
	size_t size = (end - execStr) + 1;

	delta_TInteger lineNumber = 0;
	const char* str = delta_ReadInteger(execStr, &lineNumber);
	if (str == NULL) {
		delta_SBytecode bc;
		bc.bytecodeSize	= DELTABASIC_EXEC_BYTECODE_SIZE;
		bc.index		= 0;
		bc.bytecode		= D->bytecode;
		bc.bCanResize	= dfalse;

		memset(bc.bytecode, 0x00, DELTABASIC_EXEC_BYTECODE_SIZE);

		size = DELTABASIC_MIN(size, DELTABASIC_EXEC_STRING_SIZE - 1);
		memcpy(D->execLine->str, execStr, size);
		D->execLine->str[size] = '\0';
		D->execLine->next = NULL;
		D->execLine->prev = NULL;
		D->execLine->line = SIZE_MAX;

		delta_EStatus status = delta_CompileLine(D, D->execLine, NULL, &bc);
		if (status != DELTA_OK)
			return status;

		D->currentLine = D->execLine;
		D->ip = 0;

		//return delta_Interpret(D, 0);
	}
	else {
		while ((*str == ' ') && (str < end))
			++str;

		size = (end - str) + 1;

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

/* ****************************************
 * delta_Interpret
 */
delta_EStatus delta_Interpret(delta_SState* D, size_t nInstructions) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	/*if (D->bCompiled == dfalse) {
		delta_EStatus status = delta_Compile(D);
		if (status != DELTA_OK)
			return status;
	}*/

	if (nInstructions == 0)
		nInstructions = SIZE_MAX;
	
	for (size_t i = 0; i < nInstructions; ++i) {
		delta_EStatus status = delta_ExecuteInstruction(D);
		if (status != DELTA_OK)
			return status;
	}

	return DELTA_OK;
}

// ******************************************************************************** //

/* ****************************************
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
			start = delta_ReadInteger(start, &lineNumber);
			if (start != NULL) { // TODO: if (start == NULL)
				const size_t size = end - start;

				if (size != 0) {
					if (delta_InsertLine(D, lineNumber, start, size) == dfalse)
						return DELTA_ALLOCATOR_ERROR;
				}
			}

			start = str + 1;
		}

		++str;
	}

	return DELTA_OK;
}

// ******************************************************************************** //

/* ****************************************
 * delta_SetPrintFunction
 */
delta_EStatus delta_SetPrintFunction(delta_SState* D, delta_TPrintFunction func) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;

	if (func == NULL)
		return DELTA_FUNC_IS_NULL;

	D->printFunction = func;
	return DELTA_OK;
}

/* ****************************************
 * delta_SetInputFunction
 */
delta_EStatus delta_SetInputFunction(delta_SState* D, delta_TInputFunction func) {
	if (D == NULL)
		return DELTA_STATE_IS_NULL;
		
	if (func == NULL)
		return DELTA_FUNC_IS_NULL;

	D->inputFunction = func;
	return DELTA_OK;
}