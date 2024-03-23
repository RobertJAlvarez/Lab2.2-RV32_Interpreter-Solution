#include <stdlib.h>  // size_t

#include "helper.h"
#include "my_string.h"

int str_to_int(int32_t *res, const char *str) {
  int i, is_neg;
  unsigned char num;

  i = is_neg = str[0] == '-';

  for (*res = 0; str[i] != '\0'; ++i) {
    if ((num = (unsigned char)(str[i] - '0')) > 9) return 1;
    *res = *res * 10 + (int32_t)num;
  }

  if (is_neg) *res *= -1;

  return 0;
}

int get_reg_number(int32_t *res, char *reg) {
  // Mapping between ABI_name and register number in X# format
  const static struct {
    int32_t num;
    const char *ABI_name;
  } conversion[] = {
      {0, "zero"}, {1, "RA"},  {2, "SP"},  {3, "GP"},   {4, "TP"},   {5, "T0"},
      {6, "T1"},   {7, "T2"},  {8, "FP"},  {8, "S0"},   {9, "S1"},   {10, "A0"},
      {11, "A1"},  {12, "A2"}, {13, "A3"}, {14, "A4"},  {15, "A5"},  {16, "A6"},
      {17, "A7"},  {18, "S2"}, {19, "S3"}, {20, "S4"},  {21, "S5"},  {22, "S6"},
      {23, "S7"},  {24, "S8"}, {25, "S9"}, {26, "S10"}, {27, "S11"}, {28, "T3"},
      {29, "T4"},  {30, "T5"}, {31, "T6"}};

  // If register is already as "X#" return "#" as an int
  if (reg[0] == 'X') {
    // Return -1 if string to int conversion fails
    if (str_to_int(res, &reg[1])) return 1;
    // Otherwise, return the conversion
    return 0;
  }

  // Map ABI name to X# equivalent
  for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++) {
    if (str_cmp(reg, conversion[i].ABI_name) == 0) {
      *res = conversion[i].num;
      return 0;
    }
  }

  return 1;
}

instr_t str_to_enum(char *func3) {
  instr_t instr = {.instr_type = INSTR_ERROR, .instr = {.i = 0}};

  const static struct {
    instr_t instr;
    const char *str;
  } conversion[] = {
      {{INSTR_I, {.i = ADDI}}, "ADDI"},
      {{INSTR_I, {.i = ANDI}}, "ANDI"},
      {{INSTR_I, {.i = JALR}}, "JALR"},
      {{INSTR_I, {.i = LB}}, "LB"},
      {{INSTR_I, {.i = LBU}}, "LBU"},
      {{INSTR_I, {.i = LH}}, "LH"},
      {{INSTR_I, {.i = LHU}}, "LHU"},
      {{INSTR_I, {.i = LW}}, "LW"},
      {{INSTR_I, {.i = ORI}}, "ORI"},
      {{INSTR_I, {.i = SLLI}}, "SLLI"},
      {{INSTR_I, {.i = SLTI}}, "SLTI"},
      {{INSTR_I, {.i = SLTIU}}, "SLTIU"},
      {{INSTR_I, {.i = SRAI}}, "SRAI"},
      {{INSTR_I, {.i = SRLI}}, "SRLI"},
      {{INSTR_I, {.i = XORI}}, "XORI"},
      {{INSTR_PSEUDO, {.pseudo = CALL}}, "CALL"},
      {{INSTR_PSEUDO, {.pseudo = LA}}, "LA"},
      {{INSTR_PSEUDO, {.pseudo = LI}}, "LI"},
      {{INSTR_PSEUDO, {.pseudo = MV}}, "MV"},
      {{INSTR_PSEUDO, {.pseudo = NEG}}, "NEG"},
      {{INSTR_PSEUDO, {.pseudo = NOP}}, "NOP"},
      {{INSTR_PSEUDO, {.pseudo = NOT}}, "NOT"},
      {{INSTR_PSEUDO, {.pseudo = J}}, "J"},
      {{INSTR_PSEUDO, {.pseudo = JR}}, "JR"},
      {{INSTR_PSEUDO, {.pseudo = RET}}, "RET"},
      {{INSTR_R, {.r = ADD}}, "ADD"},
      {{INSTR_R, {.r = AND}}, "AND"},
      {{INSTR_R, {.r = OR}}, "OR"},
      {{INSTR_R, {.r = SLL}}, "SLL"},
      {{INSTR_R, {.r = SLT}}, "SLT"},
      {{INSTR_R, {.r = SLTU}}, "SLTU"},
      {{INSTR_R, {.r = SRA}}, "SRA"},
      {{INSTR_R, {.r = SRL}}, "SRL"},
      {{INSTR_R, {.r = SUB}}, "SUB"},
      {{INSTR_R, {.r = XOR}}, "XOR"},
      {{INSTR_S, {.s = SB}}, "SB"},
      {{INSTR_S, {.s = SH}}, "SH"},
      {{INSTR_S, {.s = SW}}, "SW"},
      {{INSTR_SB, {.sb = BEQ}}, "BEQ"},
      {{INSTR_SB, {.sb = BGE}}, "BGE"},
      {{INSTR_SB, {.sb = BGEU}}, "BGEU"},
      {{INSTR_SB, {.sb = BLT}}, "BLT"},
      {{INSTR_SB, {.sb = BLTU}}, "BLTU"},
      {{INSTR_SB, {.sb = BNE}}, "BNE"},
      {{INSTR_U, {.u = AUIPC}}, "AUIPC"},
      {{INSTR_U, {.u = LUI}}, "LUI"},
      {{INSTR_UJ, {.uj = JAL}}, "JAL"},
  };

  for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++) {
    if (str_cmp(func3, conversion[i].str) == 0) {
      instr = conversion[i].instr;
      break;
    }
  }

  return instr;
}
