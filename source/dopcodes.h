//

//	| File:			dopcodes.h
//	| Description:	
//	| Created:		11 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_OPCODES_H__
#define __DELTABASIC_OPCODES_H__

					//										//										//

// ------------------------------------------------------------------------- //

/* ====================================
 * delta_EOpcodes
 */
typedef enum {
	OPCODE_HLT,		// Halt
	OPCODE_NEXTL,	// Next line
	OPCODE_PUSHS,	// 1 (offset)
	OPCODE_PUSHN,	// 4 (Number)
	OPCODE_CONCAT,
	OPCODE_ADD,
	OPCODE_SUB,
	OPCODE_MUL,
	OPCODE_DIV,
	OPCODE_MOD,
	OPCODE_POW,
	OPCODE_SETN,	// Set Numeric Varialbe 2, 2 (offset, size)
	OPCODE_SETS,	// Set String Varialbe  2, 2 (offset, size)
	OPCODE_JMP,		// 4 (line)
	OPCODE_PRINTN,	// Print Numeric
	OPCODE_PRINTNT, // Print Numeric with Tabs
	OPCODE_PRINTS,
	OPCODE_PRINTST,
	OPCODE_PRINTLN, // Print New Line
	OPCODE_GETN,
	OPCODE_GETS,
	OPCODE_ET,		// Equal To
	OPCODE_NET,		// Not Equal To
	OPCODE_LT,		// Less Than
	OPCODE_GT,		// Greater Than
	OPCODE_LET,		// Less or Equal To
	OPCODE_GET,		// Greater or Equal To
	OPCODE_NEG,		//
	OPCODE_STOP,
	OPCODE_TAB,
	OPCODE_GOSUB,
	OPCODE_RETURN,
	OPCODE_JNLNZ,		// Jump to next line if not zero
	OPCODE_SETFOR,		// for VARNAME=CONST to CONST
	OPCODE_SETSTEPFOR,	// for VARNAME=CONST to CONST step CONST
	OPCODE_NEXTFOR,
	OPCODE_INPUTN,		// Input Numeric
	OPCODE_INPUTS,		// Input String
	OPCODE_ALLOCN,		// Allocate Number Array
	OPCODE_ALLOCS,		// Allocate String Array
	OPCODE_GETIN,		// Get indexed Number
	OPCODE_GETIS,		// Get indexed String
	OPCODE_SETIN,		// Set indexed Number
	OPCODE_SETIS,		// Set indexed String
	OPCODE_CALLN,		// Call Numeric cfunc
	OPCODE_CALLS,		// Call String cfunc
	OPCODE_CALLNR,		// Call Numeric cfunc with return
	OPCODE_CALLSR,		// Call String cfunc with return
	OPCODE_COUNT,
	OPCODE_LAST = OPCODE_CALLSR,
} delta_EOpcodes;

#endif /* !__DELTABASIC_OPCODES_H__ */
