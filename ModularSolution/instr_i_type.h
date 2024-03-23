#ifndef __INSTR_I_TYPES_H__
#define __INSTR_I_TYPES_H__

#include <stdint.h>  //uint32_t

typedef enum {
  ADDI,   // (I) ADD Immediate
  ANDI,   // (I) Bitwise And Immediate
  JALR,   // (I) Jump and Link Register
  LB,     // (I) Load a Byte
  LBU,    // (I) Load a Byte Unsigned
  LH,     // (I) Load a Half a word
  LHU,    // (I) Load a Half a word
  LW,     // (I) Load a Word
  ORI,    // (I) Bitwise OR Immediate
  SLLI,   // (I) Shift Left Immediate
  SLTI,   // (I) Set Less Than Immediate
  SLTIU,  // (I) Set Less Than Immediate Unsigned
  SRAI,   // (I) Shift Right Arithmetic Immediate
  SRLI,   // (I) Shift Right Immediate
  XORI,   // (I) Exclusive OR Immediate
} instr_i_t;

int __is_i_mem_instr(instr_i_t func3);
int exec_i_type(instr_i_t func3, char **tokens);
void __jalr(uint32_t rd, uint32_t rs1, int32_t imm);
void __addi(uint32_t rd, uint32_t rs, int32_t imm);
void __xori(uint32_t rd, uint32_t rs, int32_t imm);

#endif
