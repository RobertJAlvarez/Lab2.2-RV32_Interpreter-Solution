#ifndef __INSTR_SB_TYPES_H__
#define __INSTR_SB_TYPES_H__

#include "instr_s_type.h"

typedef enum {
  BEQ,   // (SB) Branch EQual
  BGE,   // (SB) Branch Greater or Equal
  BGEU,  // (SB) Branch Greater or Equal Unsigned
  BLT,   // (SB) Branch Less Than
  BLTU,  // (SB) Branch Less Than Unsigned
  BNE,   // (SB) Branch Not Equal
} instr_sb_t;

int exec_sb_type(instr_sb_t func3, char **tokens);

#endif
