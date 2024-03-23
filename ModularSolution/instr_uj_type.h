#ifndef __INSTR_UJ_TYPES_H__
#define __INSTR_UJ_TYPES_H__

#include <stdint.h>  //uint32_t

#include "instr_u_type.h"

typedef enum {
  JAL,  // (UJ) Jump and Link
} instr_uj_t;

int exec_uj_type(instr_uj_t func3, char **tokens);
void __jal(uint32_t rd, int32_t imm);

#endif
