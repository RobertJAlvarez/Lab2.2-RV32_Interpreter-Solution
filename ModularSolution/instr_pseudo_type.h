#ifndef __INSTR_PSEUDO_TYPES_H__
#define __INSTR_PSEUDO_TYPES_H__

#include "instr_i_type.h"

typedef enum {
  CALL,  // (P->U/I) CALL
  LA,    // (P->U/I) Load Address
  LI,    // (P->I) Load Immediate
  MV,    // (P->I) Move
  NEG,   // (P->R) Negate
  NOP,   // (P->I) No OPeration
  NOT,   // (P->I) Ones Complement
  J,     // (P->IJ) Jump
  JR,    // (P->I) Jump Register
  RET,   // (P->I) RETurn
} instr_pseudo_t;

int exec_pseudo_type(instr_pseudo_t instr, char **tokens);

#endif
