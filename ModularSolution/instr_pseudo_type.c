#include <stdio.h>

#include "helper.h"
#include "instr_i_type.h"
#include "instr_pseudo_type.h"
#include "instr_r_type.h"
#include "instr_u_type.h"
#include "instr_uj_type.h"
#include "riscv.h"

static int __call(char **tokens) {
  int32_t imm;

  if (str_to_int(&imm, tokens[1]) != 0) return 1;

  __auipc((uint32_t)1, (imm & ((int32_t)0xFFFFF000)) >> 12);
  __jalr((uint32_t)1, (uint32_t)1, imm & 0xFFF);

  return 0;
}

static int __la(char **tokens) {
  int32_t rd, imm;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (str_to_int(&imm, tokens[2]) != 0) return 1;

  __auipc((uint32_t)rd, (imm & ((int32_t)0xFFFFF000)) >> 12);

  r[0] = (uint32_t)0;  // Overwrite x0 to 0

  __addi((uint32_t)rd, (uint32_t)rd, imm & 0xFFF);

  return 0;
}

static int __li(char **tokens) {
  int32_t rd, imm;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (str_to_int(&imm, tokens[2]) != 0) return 1;

  __addi((uint32_t)rd, (uint32_t)0, imm);

  return 0;
}

static int __mv(char **tokens) {
  int32_t rd, rs;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (get_reg_number(&rs, tokens[2]) != 0) return 1;

  __addi((uint32_t)rd, (uint32_t)rs, (int32_t)0);

  return 0;
}

static int __neg(char **tokens) {
  int32_t rd, rs;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (get_reg_number(&rs, tokens[2]) != 0) return 1;

  __sub((uint32_t)rd, (uint32_t)0, (uint32_t)rs);

  return 0;
}

static int __not(char **tokens) {
  int32_t rd, rs;

  if (get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (get_reg_number(&rs, tokens[2]) != 0) return 1;

  __xori((uint32_t)rd, (uint32_t)rs, (int32_t)-1);

  return 0;
}

static int __ret(void) {
  __jalr(0, 1, 0);

  return 0;
}

static int __j(char **tokens) {
  int32_t imm;

  if (str_to_int(&imm, tokens[1]) != 0) return 1;

  __jal((uint32_t)0, imm);

  return 0;
}

static int __jr(char **tokens) {
  int32_t rs;

  if (get_reg_number(&rs, tokens[1]) != 0) return 1;

  __jalr((uint32_t)0, (uint32_t)rs, 0);

  return 0;
}

int exec_pseudo_type(instr_pseudo_t instr, char **tokens) {
  int status = 0;

  switch (instr) {
    case CALL:
      status = __call(tokens);
      break;
    case LA:
      status = __la(tokens);
      break;
    case LI:
      status = __li(tokens);
      break;
    case MV:
      status = __mv(tokens);
      break;
    case NEG:
      status = __neg(tokens);
      break;
    case NOP:
      status = 0;
      break;
    case NOT:
      status = __not(tokens);
      break;
    case J:
      status = __j(tokens);
      break;
    case JR:
      status = __jr(tokens);
      break;
    case RET:
      status = __ret();
      break;
    default:
      fprintf(stderr, "Non supported type pseudo instruction\n");
  }

  return status;
}
