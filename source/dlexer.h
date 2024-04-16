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
 * delta_ReadInteger
 *
 * Returns a string pointer with offset, on error returns NULL.
 * Skips spaces.
 */
const delta_TChar*	delta_ReadInteger(const delta_TChar str[], delta_TInteger* value);
  
// ------------------------------------------------------------------------- //

/* ====================================
 * delta_EOp
 */
typedef enum {
	OP_NONE,
	OP_DIM,
	OP_END,
	OP_FOR,
	OP_GOSUB,
	OP_GOTO,
	OP_IF,
	OP_INPUT,
	OP_LET,
	OP_NEXT,
	OP_PRINT,
	OP_RETURN,
	OP_STEP,
	OP_STOP,
//	OP_TAB,
	OP_THEN,
	OP_TO,
} delta_EOp;

/* ====================================
 * delta_GetOpName
 */
const delta_TChar*	delta_GetOpName(delta_EOp op);
// ------------------------------------------------------------------------- //

/* ====================================
 * delta_ESym
 */
typedef enum {
	SYM_ADD,
	SYM_SUB,
	SYM_MUL,
	SYM_DIV,
} delta_ESym;

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_ELexemType
 */
typedef enum delta_ELexemType {
	LEXEM_FLOAT,
	LEXEM_INTEGER,
	LEXEM_STRING,
	LEXEM_NAME,
	LEXEM_OP,
	LEXEM_SYMBOL,
	LEXEM_EOL
} delta_ELexemType;

/* ====================================
 * delta_SLexemString
 */
typedef struct {
	uint32_t offset;
	uint16_t size;
} delta_SLexemString;

/* ====================================
 * delta_SLexerState
 */
typedef struct delta_SLexerState {
	delta_ELexemType type;
	union {
		long		integerValue;
		float		floatValue;
		delta_SLexemString string;
		delta_EOp	op;
		delta_TChar	symbol;
	};

	const delta_TChar* buffer;
	const delta_TChar* head;
} delta_SLexerState;

/* ====================================
 * delta_EParseStatus
 */
typedef enum delta_EParseStatus {
	PARSE_OK,
	PARSE_STRING_IS_NULL,
	PARSE_FLOAT_DOT_AGAIN,
	PARSE_UNEXPECTED_NULL_TERMINAL,
} delta_EParseStatus;

/* ====================================
 * delta_EParseStatus
 */
delta_EParseStatus	delta_Parse(delta_SLexerState* L);

#endif /* !__DELTABASIC_LEXER_H__ */