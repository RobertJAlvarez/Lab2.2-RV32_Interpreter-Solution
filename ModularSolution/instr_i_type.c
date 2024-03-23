#include <stdio.h>

#include "helper.h"
#include "instr_i_type.h"
#include "riscv.h"

typedef enum { SIGNED, UNSIGNED } sign_t;

static int __load(uint32_t rd, uint32_t rs1, int32_t imm, int32_t n_bytes,
                  sign_t sign) {
  uint32_t num = 0;

  // Get one byte of memory at a time and place it in num
  for (int32_t i = 0; i < n_bytes; i++) {
    num |= ((uint32_t)(mem[(int32_t)r[rs1] + imm + i] << (i * 8)));
  }

  // Overwrite destination register with sign extend
  switch (n_bytes) {
    case 1:
      switch (sign) {
        case SIGNED:
          r[rd] =
              (uint32_t)(/*sign extend*/ (int32_t)(/*truncate*/ (uint8_t)num));
          break;
        case UNSIGNED:
          r[rd] = (uint32_t)(/*truncate*/ (uint8_t)num);
          break;
        default:
          fprintf(stderr, "Trying to load using sign = %d\n", sign);
      }
      break;
    case 2:
      switch (sign) {
        case SIGNED:
          r[rd] =
              (uint32_t)(/*sign extend*/ (int32_t)(/*truncate*/ (uint16_t)num));
          break;
        case UNSIGNED:
          r[rd] = (uint32_t)(/*truncate*/ (uint16_t)num);
          break;
        default:
          fprintf(stderr, "Trying to load using sign = %d\n", sign);
      }
      break;
    case 4:
      r[rd] = num;
      break;
    default:
      fprintf(stderr, "Trying to load %d bytes\n", n_bytes);
      return 1;
  }

  return 0;
}

void __jalr(uint32_t rd, uint32_t rs1, int32_t imm) {
  r[rd] = ((uint32_t)(pc + 4));
  pc = ((int32_t)r[rs1]) + imm;
}

void __addi(uint32_t rd, uint32_t rs, int32_t imm) {
  r[rd] = (uint32_t)(((int32_t)r[rs]) + imm);
}

void __xori(uint32_t rd, uint32_t rs, int32_t imm) {
  r[rd] = (uint32_t)(((int32_t)r[rs]) ^ imm);
}

/* Return if the func3, i.e., LW, SB, MV, J, is a memory instruction. */
int __is_i_mem_instr(instr_i_t func3) {
  return ((func3 == LB) || (func3 == LBU) || (func3 == LH) || (func3 == LHU) ||
          (func3 == LW));
}

int exec_i_type(instr_i_t func3, char **tokens) {
  int32_t s_r1, s_r2, imm;
  uint32_t r1, r2;

  if (get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (str_to_int(&imm, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;

  switch (func3) {
    case ADDI:
      __addi(r1, r2, imm);
      break;
    case ANDI:
      r[r1] = (uint32_t)(((int32_t)r[r2]) & imm);
      break;
    case JALR:
      __jalr(r1, r2, imm);
      break;
    case LB:
      __load(r1, r2, imm, 1, SIGNED);
      break;
    case LBU:
      __load(r1, r2, imm, 1, UNSIGNED);
      break;
    case LH:
      __load(r1, r2, imm, 2, SIGNED);
      break;
    case LHU:
      __load(r1, r2, imm, 2, UNSIGNED);
      break;
    case LW:
      __load(r1, r2, imm, 4, UNSIGNED);
      break;
    case ORI:
      r[r1] = (uint32_t)(((int32_t)r[r2]) | imm);
      break;
    case SLLI:
      r[r1] = r[r2] << imm;
      break;
    case SLTI:
      r[r1] = (uint32_t)((int32_t)r[r2] < imm ? 1 : 0);
      break;
    case SLTIU:
      r[r1] = (uint32_t)(r[r2] < (uint32_t)imm ? 1 : 0);
      break;
    case SRAI:
      r[r1] = (uint32_t)(((int32_t)r[r2]) >> imm);
      break;
    case SRLI:
      r[r1] = r[r2] >> imm;
      break;
    case XORI:
      __xori(r1, r2, imm);
      break;
    default:
      fprintf(stderr, "Non supported type i instruction\n");
  }

  return 0;
}
