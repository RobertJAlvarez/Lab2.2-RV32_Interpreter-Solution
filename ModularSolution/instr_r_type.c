#include <stdio.h>

#include "helper.h"
#include "instr_r_type.h"
#include "riscv.h"

void __sub(uint32_t rd, uint32_t rs1, uint32_t rs2) { r[rd] = r[rs1] - r[rs2]; }

int exec_r_type(instr_r_t func3, char **tokens) {
  int32_t s_r1, s_r2, s_r3;
  uint32_t r1, r2, r3;

  if (get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (get_reg_number(&s_r3, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;
  r3 = (uint32_t)s_r3;

  switch (func3) {
    case ADD:
      r[r1] = r[r2] + r[r3];
      break;
    case AND:
      r[r1] = r[r2] & r[r3];
      break;
    case OR:
      r[r1] = r[r2] | r[r3];
      break;
    case SLL:
      r[r1] = r[r2] << r[r3];
      break;
    case SLT:
      r[r1] = (uint32_t)((int32_t)r[r2] < (int32_t)r[r3] ? 1 : 0);
      break;
    case SLTU:
      r[r1] = (uint32_t)(r[r2] < r[r3] ? 1 : 0);
      break;
    case SRA:
      r[r1] = (uint32_t)(((int32_t)r[r2]) >> r[r3]);
      break;
    case SRL:
      r[r1] = r[r2] >> r[r3];
      break;
    case SUB:
      __sub(r1, r2, r3);
      break;
    case XOR:
      r[r1] = r[r2] ^ r[r3];
      break;
    default:
      fprintf(stderr, "Non supported type r instruction\n");
  }

  return 0;
}
