#include <stdio.h>

#include "helper.h"
#include "instr_i_type.h"
#include "instr_s_type.h"
#include "riscv.h"

static void __save(uint32_t rs1, uint32_t rs2, int32_t imm, int32_t n_bytes) {
  // Save n_bytes from rs2 into mem using little endian
  for (int32_t i = 0; i < n_bytes; i++) {
    // Extract the i group of 8 bits from rs2 and place it in mem
    mem[(int32_t)r[rs1] + imm + i] = (unsigned char)((r[rs2] >> i * 8) & 0xFF);
  }
}

int exec_s_type(instr_s_t func3, char **tokens) {
  int32_t s_r1, s_r2, imm;
  uint32_t r1, r2;

  if (get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (str_to_int(&imm, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;

  switch (func3) {
    case SB:
      __save(r1, r2, imm, 1);
      break;
    case SH:
      __save(r1, r2, imm, 2);
      break;
    case SW:
      __save(r1, r2, imm, 4);
      break;
    default:
      fprintf(stderr, "Non supported type s instruction\n");
  }

  return 0;
}
