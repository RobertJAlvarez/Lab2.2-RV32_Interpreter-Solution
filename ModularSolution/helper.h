#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdint.h>  //uint32_t

#include "instr_types.h"

/* If reg have a ABI names ("RA","FP","A0",...) convert it to its X# equivalent.
 *
 * For example, "ra" -> X2.
 */
int get_reg_number(int32_t *res, char *reg);

/*
 *
 */
int str_to_int(int32_t *res, const char *str);

/* Given a string representation of a ISA operation, return the instr_t enum
 * value that represent it.
 *
 * For example: Base on instr_t implementation if "LW"
 * is given then LW = 2 is returned.
 */
instr_t str_to_enum(char *func3);

#endif
