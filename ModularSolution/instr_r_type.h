#ifndef __INSTR_R_TYPES_H__
#define __INSTR_R_TYPES_H__

#include <stdint.h>  //uint32_t

#include "instr_pseudo_type.h"

typedef enum {
  ADD,   // (R) Add
  AND,   // (R) Bitwise AND
  OR,    // (R) Bitwise OR
  SLL,   // (R) Shift Left
  SLT,   // (R) Set Less Than
  SLTU,  // (R) Set Less Than Unsigned
  SRA,   // (R) Shift Right Arithmetic
  SRL,   // (R) Shift Right
  SUB,   // (R) Subtract
  XOR,   // (R) Exclusive OR
} instr_r_t;

int exec_r_type(instr_r_t func3, char **tokens);
void __sub(uint32_t rd, uint32_t rs1, uint32_t rs2);

#endif
