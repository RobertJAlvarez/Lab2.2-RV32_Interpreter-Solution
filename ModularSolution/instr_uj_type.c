#include <stdio.h>

#include "helper.h"
#include "instr_uj_type.h"
#include "riscv.h"

void __jal(uint32_t rd, int32_t imm) {
  r[rd] = ((uint32_t)pc) + 4;
  pc += imm;
}

int exec_uj_type(instr_uj_t func3, char **tokens) {
  int32_t s_r1, imm;
  uint32_t r1;

  if (get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (str_to_int(&imm, tokens[2]) != 0) return 1;

  r1 = (uint32_t)s_r1;

  switch (func3) {
    case JAL:
      __jal(r1, imm);
      break;
    default:
      fprintf(stderr, "Non supported type uj instruction\n");
  }

  return 0;
}
