//

//	| File:			deltabasic_config.c
//	| Description:	
//	| Created:		1 feb 2024
//	| Author:		Reklov
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __DELTABASIC_CONFIG_H__
#define __DELTABASIC_CONFIG_H__

					//										//										//

#define DELTABASIC_CONFIG_NUMBER							float
#define DELTABASIC_CONFIG_CHAR								char

#define DELTABASIC_COMPILER_INITIAL_BYTECODE_SIZE			64
#define DELTABASIC_COMPILER_MAX_MATH_OPS					32

#define DELTABASIC_EXEC_STRING_SIZE							128
#define DELTABASIC_EXEC_BYTECODE_SIZE						128

#define DELTABASIC_NUMERIC_STACK_SIZE						16
#define DELTABASIC_STRING_STACK_SIZE						16

#define DELTABASIC_PRINT_TAB_SIZE							10
#define DELTABASIC_PRINT_BUFFER_SIZE						(DELTABASIC_PRINT_TAB_SIZE + 2)
#define DELTABASIC_NUMERIC_EPSILON							0.0001f

#endif /* !__DELTABASIC_CONFIG_H__ */