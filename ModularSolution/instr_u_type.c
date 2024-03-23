#include <stdio.h>

#include "helper.h"
#include "instr_u_type.h"
#include "riscv.h"

void __auipc(uint32_t rd, int32_t imm) { r[rd] = (uint32_t)(pc + (imm << 12)); }

static void __lui(uint32_t rd, int32_t imm) { r[rd] = (uint32_t)(imm << 12); }

int exec_u_type(instr_u_t func3, char **tokens) {
  int32_t rd, imm;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (str_to_int(&imm, tokens[2]) != 0) return 1;

  switch (func3) {
    case AUIPC:
      __auipc((uint32_t)rd, imm);
      break;
    case LUI:
      __lui((uint32_t)rd, imm);
      break;
    default:
      fprintf(stderr, "Non supported type u instruction\n");
  }

  return 0;
}
