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
	OPCODE_SETN,	// Set Numeric Varialbe 1, 1 (offset, size)
	OPCODE_SETS,	// Set String Varialbe  1, 1 (offset, size)
	OPCODE_JMP,		// 4 (line)
} delta_EOpcodes;

#endif /* !__DELTABASIC_OPCODES_H__ */
