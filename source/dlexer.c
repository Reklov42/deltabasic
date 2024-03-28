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

#define parseHead (L->head)

// ------------------------------------------------------------------------- //
					//										//										//

static const delta_TChar* op_table[] = {
	"DIM",
	"END",
	"FOR",
	"GOSUB",
	"GOTO",
	"IF",
	"LET",
	"NEXT",
	"PRINT",
	"RETURN",
	"STEP",
	"STOP",
//	"TAB",
	"THEN",
	"TO"
};

#define DELTA_OP_TABLE_SIZE									(sizeof(op_table) / sizeof(delta_TChar*))

// ------------------------------------------------------------------------- //

/* ====================================
 * GetNextChar
 */
static delta_TChar GetNextChar(const delta_TChar** str);

// ------------------------------------------------------------------------- //

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
	
	while ((*str == ' ') && (*str == '\0'))
		++str;

	if (isdigit(*str) == 0)
		return NULL;

	if (value != NULL)
		*value = (delta_TInteger)atol(str);

	while (isdigit(*str) != 0)
		++str;

	while (*str == ' ')
		++str;

	return str;
}

/* ====================================
 * delta_ReadWord
 */
const delta_TChar* delta_ReadWord(const delta_TChar str[], const delta_TChar** wordStart, size_t* size) {
	if ((str == NULL))
		return NULL;

	while (((*str == ' ') && (*str == '\0')) && (isalpha(*str) == 0) && (*str != '_'))
			++str;

	if (wordStart != NULL)
		*wordStart = str;

	size_t s = 0;
	while ((isalpha(*str) != 0) || (*str == '_')) {
		++s;
		++str;
	}

	if (size != NULL)
		*size = s;

	return str;
}

/* ====================================
 * CompareStrings
 */
delta_EParseStatus delta_Parse(delta_SLexerState* L) {
	if (L->buffer == NULL)
		return PARSE_STRING_IS_NULL;

	if (L->head == NULL)
		L->head = L->buffer;

	while (*parseHead != '\0') {
		if (isdigit(*parseHead) != 0) { // INTEGER | FLOAT
			const char* pNumberStart = parseHead;

			delta_TBool bFloat = dfalse;
			for (GetNextChar(&parseHead); *parseHead != '\0'; GetNextChar(&parseHead)) {
				if (*parseHead == '.') {
					if (bFloat == dtrue)
						return PARSE_FLOAT_DOT_AGAIN;
					else
						bFloat = dtrue;
				}
				else if (isdigit(*parseHead) == 0)
					break;
			}

			if (bFloat == dtrue) { // FLOAT
				L->type = LEXEM_FLOAT;
				L->floatValue = (float)atof(pNumberStart);
			}
			else { // INTEGER
				L->type = LEXEM_INTEGER;
				L->integerValue = (long)atol(pNumberStart);
			}
			
			return PARSE_OK;
		}
		else if (ispunct(*parseHead) != 0) { // STRING | KEYSYMBOL
			if (*parseHead == '"') { // STRING
				GetNextChar(&parseHead);

				L->type = LEXEM_STRING;
				L->string.offset = L->head - L->buffer;
				L->string.size = 0;

				for (; *parseHead != '\0'; GetNextChar(&parseHead)) {
					if ((*parseHead == '"') && (*(parseHead - 1) != '\\'))
						break;
					else
						++(L->string.size);
				}

				if (*parseHead == '\0')
					return PARSE_UNEXPECTED_NULL_TERMINAL;

				GetNextChar(&parseHead);
			}
			else if (*parseHead == '.') { // FLOAT
				const char* pNumberStart = parseHead;

				for (GetNextChar(&parseHead); *parseHead != '\0'; GetNextChar(&parseHead)) {
					if (*parseHead == '.')
						return PARSE_FLOAT_DOT_AGAIN;
					else if (isdigit(*parseHead) == 0)
						break;
				}

				L->type = LEXEM_FLOAT;
				L->floatValue = (float)atof(pNumberStart);

				return PARSE_OK;
			}
			else { // KEYSYMBOL;
				L->type = LEXEM_SYMBOL;
				L->symbol = *parseHead;
				GetNextChar(&parseHead);
			}
			
			return PARSE_OK;
		}
		else if (isalpha(*parseHead) != 0) { // OP | NAME
			if (delta_Strncmp(parseHead, "REM", 3) == 0) {
				while (GetNextChar(&parseHead) != '\0');

				L->type = LEXEM_EOL;
				return PARSE_OK;
			}

			size_t index;
			for (index = 0; index < DELTA_OP_TABLE_SIZE; ++index) { // OP
				const size_t size = delta_Strlen(op_table[index]);
				const size_t cmp = delta_Strncmp(parseHead, op_table[index], size);
				if (cmp < 0)
					break;

				if (cmp == 0) { // OP
					L->op = index + 1;

					L->type = LEXEM_OP;
					L->head += size;

					return PARSE_OK;
				}
			}

			// NAME
			L->type = LEXEM_NAME;
			L->string.offset = L->head - L->buffer;
			L->string.size = 0;

			for (; *parseHead != '\0'; GetNextChar(&parseHead)) {
				if ((isalpha(*parseHead) == 0) && (*parseHead != '_'))
					break;
				else
					++(L->string.size);
			}
			
			return PARSE_OK;
		}
		else
			GetNextChar(&parseHead);
	}

	L->type = LEXEM_EOL;

	return PARSE_OK;
}

// ------------------------------------------------------------------------- //

/* ====================================
 * GetNextChar
 */
inline delta_TChar GetNextChar(const delta_TChar** str) {
	if (*(*str) != '\0')
		++(*str);

	return *(*str);
}
