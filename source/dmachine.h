/**
 * \file	dmachine.h
 * \brief	DeltaBASIC VM
 * \date	14 feb 2024
 * \author	Reklov
 */
#ifndef __DELTABASIC_MACHINE_H__
#define __DELTABASIC_MACHINE_H__

#include "deltabasic.h"
#include "dlimits.h"

// ******************************************************************************** //

/**
 * delta_ExecuteInstruction
 */
delta_EStatus		delta_ExecuteInstruction(delta_SState* D);

#endif /* !__DELTABASIC_MACHINE_H__ */
