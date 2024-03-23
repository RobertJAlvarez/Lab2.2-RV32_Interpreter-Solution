#ifndef __INSTR_U_TYPES_H__
#define __INSTR_U_TYPES_H__

#include <stdint.h>  //uint32_t

#include "instr_sb_type.h"

typedef enum {
  AUIPC,  // (U) Add Upper Immediate to PC
  LUI,    // (U) Add Upper Immediate to PC
} instr_u_t;

int exec_u_type(instr_u_t func3, char **tokens);
void __auipc(uint32_t rd, int32_t imm);

#endif
