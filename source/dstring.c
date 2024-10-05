/**
 * \file	dstring.c
 * \brief	String specific functions
 * \date	8 feb 2024
 * \author	Reklov
 */
#include "dstring.h"

#include <string.h>

// ******************************************************************************** //

/* ****************************************
 * delta_Strlen
 */
size_t delta_Strlen(const delta_TChar str[]) {
	return strlen(str);
}

/* ****************************************
 * delta_Strncmp
 */
int delta_Strncmp(const delta_TChar strA[], const delta_TChar strB[], size_t n) {
	return strncmp(strA, strB, n);
}
