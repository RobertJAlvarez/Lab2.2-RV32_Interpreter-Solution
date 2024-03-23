#include <stdio.h>

#include "helper.h"
#include "instr_i_type.h"
#include "instr_sb_type.h"
#include "riscv.h"

int exec_sb_type(instr_sb_t func3, char **tokens) {
  int32_t s_r1, s_r2, imm;
  uint32_t r1, r2;

  if (get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (str_to_int(&imm, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;

  switch (func3) {
    case BEQ:
      if (r[r1] == r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    case BGE:
      if ((int32_t)r[r1] >= (int32_t)r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    case BGEU:
      if (r[r1] >= r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    case BLT:
      if ((int32_t)r[r1] <= (int32_t)r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    case BLTU:
      if (r[r1] <= r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    case BNE:
      if (r[r1] != r[r2])
        pc += imm;
      else
        pc += 4;
      break;
    default:
      fprintf(stderr, "Non supported type sb instruction\n");
  }

  return 0;
}
