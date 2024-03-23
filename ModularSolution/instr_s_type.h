#ifndef __INSTR_S_TYPES_H__
#define __INSTR_S_TYPES_H__

#include "instr_r_type.h"

typedef enum {
  SB,  // (S) Save a byte
  SH,  // (S) Save a half a word
  SW,  // (S) Save a word
} instr_s_t;

int exec_s_type(instr_s_t func3, char **tokens);

#endif
