/**
 * \file	dlimits.h
 * \brief	Limits, basic types, and other "type-based" things
 * \date	1 feb 2024
 * \author	Reklov
 */
#ifndef __DELTABASIC_LIMITS_H__
#define __DELTABASIC_LIMITS_H__

#include <stdint.h>

#define DELTABASIC_CAST(type, exp)							((type)(exp))
#define DELTABASIC_UNUSED(rv)								((void)(rv)) // to avoid warnings

#define DELTABASIC_MIN(a, b)								(((a) > (b)) ? (b) : (a))
#define DELTABASIC_MAX(a, b)								(((a) < (b)) ? (b) : (a))

#define dtrue												1
#define dfalse												0

typedef int													delta_TBool;

typedef uint8_t												delta_TByte;
typedef uint16_t											delta_TWord;
typedef uint32_t											delta_TDWord;

typedef long												delta_TInteger;

typedef delta_TByte											delta_TCFuncArgMask;

#define DELTABASIC_CFUNC_MAX_ARGS							(sizeof(delta_TCFuncArgMask) * 8)

#endif /* !__DELTABASIC_LIMITS_H__ */