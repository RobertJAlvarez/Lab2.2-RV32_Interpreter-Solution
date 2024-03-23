#ifndef __INSTR_TYPES_H__
#define __INSTR_TYPES_H__

#include "instr_i_type.h"
#include "instr_pseudo_type.h"
#include "instr_r_type.h"
#include "instr_s_type.h"
#include "instr_sb_type.h"
#include "instr_u_type.h"
#include "instr_uj_type.h"

typedef enum {
  INSTR_ERROR,
  INSTR_I,
  INSTR_PSEUDO,
  INSTR_R,
  INSTR_S,
  INSTR_SB,
  INSTR_U,
  INSTR_UJ,
} instr_type_t;

typedef union {
  instr_i_t i;
  instr_pseudo_t pseudo;
  instr_r_t r;
  instr_s_t s;
  instr_sb_t sb;
  instr_u_t u;
  instr_uj_t uj;
} instr_union_t;

typedef struct {
  instr_type_t instr_type;
  instr_union_t instr;
} instr_t;

#endif
