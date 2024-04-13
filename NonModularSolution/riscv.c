#include <errno.h>   //errno
#include <stdint.h>  //uint32_t
#include <stdio.h>   //printf(), fprintf()
#include <stdlib.h>  //malloc(), realloc() & free()
#include <string.h>  //strerror()

#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"

#define N_REGISTERS ((size_t)32)
#define MEM_SIZE ((size_t)(1 << 10))

typedef enum { SIGNED, UNSIGNED } sign_t;

typedef enum {
  ERROR,

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

  SB,  // (S) Save a byte
  SH,  // (S) Save a half a word
  SW,  // (S) Save a word

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

  BEQ,   // (SB) Branch EQual
  BGE,   // (SB) Branch Greater or Equal
  BGEU,  // (SB) Branch Greater or Equal Unsigned
  BLT,   // (SB) Branch Less Than
  BLTU,  // (SB) Branch Less Than Unsigned
  BNE,   // (SB) Branch Not Equal

  JAL,  // (UJ) Jump and Link

  AUIPC,  // (U) Add Upper Immediate to PC
  LUI,    // (U) Add Upper Immediate to PC

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
} instr_t;

uint32_t r[N_REGISTERS];
int32_t pc;
unsigned char mem[MEM_SIZE];

static void __error_no_memory(void) {
  fprintf(stderr, "No more memory available. Error: %s\n", strerror(errno));
}

static void init_memory_elements(void) {
  // Initialize registers
  for (size_t i = 0; i < N_REGISTERS; i++) r[i] = ((uint32_t)0);

  // Initialize Program Counter (PC) to 0
  pc = ((int32_t)0);

  // Initialize all memory to 0
  for (size_t i = 0; i < MEM_SIZE; ++i) mem[i] = ((unsigned char)0);

  // Set sp to be the top part of the memory
  r[2] = MEM_SIZE;
}

static int print_registers(const char *fd_name) {
  FILE *fptr;

  if ((fptr = fopen(fd_name, "w")) == NULL) return 1;

  // Print all registers
  fprintf(fptr, "Registers:\n");
  for (size_t i = 0; i < N_REGISTERS; i++)
    fprintf(fptr, "X[%zu] = %u\n", i, r[i]);

  return 0;
}

/* Given a string representation of a ISA operation, return the instr_t enum
 * value that represent it.
 *
 * For example: Base on instr_t implementation if "LW"
 * is given then LW = 2 is returned.
 */
static instr_t __str_to_enum(const char *func3) {
  const static struct {
    instr_t instr;
    const char *str;
  } conversion[] = {
      {ERROR, "ERROR"}, {LB, "LB"},       {LBU, "LBU"},   {LH, "LH"},
      {LHU, "LHU"},     {LW, "LW"},       {SB, "SB"},     {SH, "SH"},
      {SW, "SW"},       {ADD, "ADD"},     {ADDI, "ADDI"}, {SUB, "SUB"},
      {XOR, "XOR"},     {XORI, "XORI"},   {SLLI, "SLLI"}, {SRAI, "SRAI"},
      {SRLI, "SRLI"},   {MV, "MV"},       {LI, "LI"},     {NEG, "NEG"},
      {NOT, "NOT"},     {JAL, "JAL"},     {J, "J"},       {JALR, "JALR"},
      {JR, "JR"},       {AND, "AND"},     {ANDI, "ANDI"}, {OR, "OR"},
      {ORI, "ORI"},     {SLL, "SLL"},     {SRA, "SRA"},   {SRL, "SRL"},
      {SLT, "SLT"},     {SLTU, "SLTU"},   {SLTI, "SLTI"}, {BEQ, "BEQ"},
      {BGE, "BGE"},     {BGEU, "BGEU"},   {BLT, "BLT"},   {BLTU, "BLTU"},
      {BNE, "BNE"},     {SLTIU, "SLTIU"}, {NOT, "NOT"},   {RET, "RET"},
      {AUIPC, "AUIPC"}, {LUI, "LUI"},     {LA, "LA"},     {CALL, "CALL"},
      {NOP, "NOP"}};

  for (size_t i = 1; i < sizeof(conversion) / sizeof(conversion[0]); i++) {
    if (str_cmp(func3, conversion[i].str) == 0) return conversion[i].instr;
  }

  return ERROR;
}

/* Return if the func3, i.e., LW, SB, MV, J, is a memory instruction. */
static inline int __is_mem_instr(const instr_t func3) {
  return ((func3 == LB) || (func3 == LBU) || (func3 == LH) || (func3 == LHU) ||
          (func3 == LW) || (func3 == SB) || (func3 == SH) || (func3 == SW));
}

static size_t arr_len(const char **arr) {
  size_t count = 0;
  for (const char **p = arr; *p != NULL; p++) count++;
  return count;
}

/* Expand last token into two, e.g., offset(RS1) -> RS1,offset
 *
 * For example [func3, RD, offset(RS1)] -> [func3, RD, RS1, offset]
 */
static void *__expand_last(char **tokens) {
  char **t1;
  void *t2;

  // Split the last token into 2
  if ((t2 = tokenize(tokens[2], "(")) == NULL) return NULL;
  t1 = (char **)t2;

  // Expand tokens from 3 to 4 to store the tokens
  if ((t2 = realloc(tokens, 5 * sizeof(char *))) == NULL) {
    free(t1);
    return NULL;
  }
  tokens = (char **)t2;

  // Save tokens[2] with RS1 and tokens[3] with offset
  tokens[2] = t1[1];
  tokens[3] = t1[0];
  tokens[4] = NULL;

  free(t1);

  // Overwrite ')' with '\0'
  if ((t2 = str_chr(tokens[2], (int)')')) == NULL) {
    return NULL;
  }
  *((char *)t2) = '\0';

  return tokens;
}

/* Tokens is guaranteed to be a memory instruction. So, we reorder the
 * parameters.
 *
 * For example:
 *  1) SW RS2,offset(RS1) -> SW RS1,RS2,offset
 *  2) LB RD,offset(RS1) -> LB RD,RS1,offset
 */
static void *__order_mem_instr(const instr_t func3, char **tokens) {
  void *t;

  // If there are 4 tokens then we have the expanded format
  if (arr_len((const char **)tokens) == 4) return tokens;

  if ((t = __expand_last(tokens)) == NULL) return NULL;

  tokens = (char **)t;

  // If it is a load instr we already have the token in the wanted order.
  // If it is a save instr, go from [func3, RS2, RS1, offset] to [func3, RS1,
  // RS2, offset] Meaning: from SW RS2,offset(RS1) to SW RS1,RS2,offset
  if ((func3 == SB) || (func3 == SH) || (func3 == SW)) {
    t = (void *)tokens[1];
    tokens[1] = tokens[2];
    tokens[2] = (char *)t;
  }

  return tokens;
}

/*
 *
 */
int __str_to_int(int32_t *res, const char *str) {
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

/* If reg have a ABI names ("RA","FP","A0",...) convert it to its X# equivalent.
 *
 * For example, "ra" -> X2.
 */
static int __get_reg_number(int32_t *res, const char *reg) {
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
    if (__str_to_int(res, &reg[1])) return 1;
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

static void __sub(const uint32_t rd, const uint32_t rs1, const uint32_t rs2) {
  r[rd] = r[rs1] - r[rs2];
}

static int __exec_r_type(const instr_t func3, const char **tokens) {
  int32_t s_r1, s_r2, s_r3;
  uint32_t r1, r2, r3;

  if (__get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (__get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (__get_reg_number(&s_r3, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;
  r3 = (uint32_t)s_r3;

  if (func3 == ADD) {
    r[r1] = r[r2] + r[r3];
  } else if (func3 == AND) {
    r[r1] = r[r2] & r[r3];
  } else if (func3 == OR) {
    r[r1] = r[r2] | r[r3];
  } else if (func3 == SLL) {
    r[r1] = r[r2] << r[r3];
  } else if (func3 == SLT) {
    r[r1] = (uint32_t)((int32_t)r[r2] < (int32_t)r[r3] ? 1 : 0);
  } else if (func3 == SLTU) {
    r[r1] = (uint32_t)(r[r2] < r[r3] ? 1 : 0);
  } else if (func3 == SRA) {
    r[r1] = (uint32_t)(((int32_t)r[r2]) >> r[r3]);
  } else if (func3 == SRL) {
    r[r1] = r[r2] >> r[r3];
  } else if (func3 == SUB) {
    __sub(r1, r2, r3);
  } else if (func3 == XOR) {
    r[r1] = r[r2] ^ r[r3];
  }

  return 0;
}

static int __save(const uint32_t rs1, const uint32_t rs2, const int32_t imm,
                  const int32_t n_bytes) {
  // Save n_bytes from rs2 into mem using little endian
  for (int32_t i = 0; i < n_bytes; i++) {
    // Extract the i group of 8 bits from rs2 and place it in mem
    mem[(int32_t)r[rs1] + imm + i] = (unsigned char)((r[rs2] >> i * 8) & 0xFF);
  }

  return 0;
}

int __load(const uint32_t rd, const uint32_t rs1, const int32_t imm,
           const int32_t n_bytes, const sign_t sign) {
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

static void __jalr(const uint32_t rd, const uint32_t rs1, const int32_t imm) {
  r[rd] = ((uint32_t)(pc + 4));
  pc = ((int32_t)r[rs1]) + imm;
}

static void __addi(const uint32_t rd, const uint32_t rs, const int32_t imm) {
  r[rd] = (uint32_t)(((int32_t)r[rs]) + imm);
}

static void __xori(const uint32_t rd, const uint32_t rs, const int32_t imm) {
  r[rd] = r[rs] ^ ((uint32_t)imm);
}

static int __exec_i_type(const instr_t func3, const char **tokens) {
  int32_t s_r1, s_r2, imm;
  uint32_t r1, r2;

  if (__get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (__get_reg_number(&s_r2, tokens[2]) != 0) return 1;
  if (__str_to_int(&imm, tokens[3]) != 0) return 1;

  r1 = (uint32_t)s_r1;
  r2 = (uint32_t)s_r2;

  if (func3 == SB) {
    __save(r1, r2, imm, 1);
  } else if (func3 == SH) {
    __save(r1, r2, imm, 2);
  } else if (func3 == SW) {
    __save(r1, r2, imm, 4);
  } else if (func3 == ADDI) {
    __addi(r1, r2, imm);
  } else if (func3 == ANDI) {
    r[r1] = r[r2] & ((uint32_t)imm);
  } else if (func3 == JALR) {
    __jalr(r1, r2, imm);
  } else if (func3 == LB) {
    __load(r1, r2, imm, 1, SIGNED);
  } else if (func3 == LBU) {
    __load(r1, r2, imm, 1, UNSIGNED);
  } else if (func3 == LH) {
    __load(r1, r2, imm, 2, SIGNED);
  } else if (func3 == LHU) {
    __load(r1, r2, imm, 2, UNSIGNED);
  } else if (func3 == LW) {
    __load(r1, r2, imm, 4, UNSIGNED);
  } else if (func3 == OR) {
    r[r1] = r[r2] | ((uint32_t)imm);
  } else if (func3 == SLLI) {
    r[r1] = r[r2] << imm;
  } else if (func3 == SLTI) {
    r[r1] = (uint32_t)((int32_t)r[r2] < imm ? 1 : 0);
  } else if (func3 == SLTIU) {
    r[r1] = (uint32_t)(r[r2] < (uint32_t)imm ? 1 : 0);
  } else if (func3 == SRAI) {
    r[r1] = ((uint32_t)(((int32_t)r[r2]) >> imm));
  } else if (func3 == SRLI) {
    r[r1] = r[r2] >> imm;
  } else if (func3 == XORI) {
    __xori(r1, r2, imm);
  } else if (func3 == BEQ) {
    if (r[r1] == r[r2])
      pc += imm;
    else
      pc += 4;
  } else if (func3 == BGE) {
    if ((int32_t)r[r1] >= (int32_t)r[r2])
      pc += imm;
    else
      pc += 4;
  } else if (func3 == BGEU) {
    if (r[r1] >= r[r2])
      pc += imm;
    else
      pc += 4;
  } else if (func3 == BLTU) {
    if (r[r1] <= r[r2])
      pc += imm;
    else
      pc += 4;
  } else if (func3 == BLT) {
    if ((int32_t)r[r1] <= (int32_t)r[r2])
      pc += imm;
    else
      pc += 4;
  } else if (func3 == BNE) {
    if (r[r1] != r[r2])
      pc += imm;
    else
      pc += 4;
  } else {
    fprintf(stderr, "Non supported type i instruction\n");
  }

  return 0;
}

static int __exec_s_type(const instr_t func3, const char **tokens) {
  return __exec_i_type(func3, tokens);
}

static void __jal(const uint32_t rd, const int32_t imm) {
  r[rd] = ((uint32_t)(pc + ((int32_t)4)));
  pc += imm;
}

static int __exec_sb_type(const instr_t func3, const char **tokens) {
  return __exec_i_type(func3, tokens);
}

static int __exec_uj_type(const instr_t func3, const char **tokens) {
  int32_t s_r1, imm;
  uint32_t r1;

  if (__get_reg_number(&s_r1, tokens[1]) != 0) return 1;
  if (__str_to_int(&imm, tokens[2]) != 0) return 1;

  r1 = (uint32_t)s_r1;

  if (func3 == JAL) {
    __jal(r1, imm);
  } else {
    return 1;
  }

  return 0;
}

static void __auipc(const uint32_t rd, const int32_t imm) {
  r[rd] = (uint32_t)(pc + (imm << 12));
}

static void __lui(const uint32_t rd, const int32_t imm) {
  r[rd] = (uint32_t)(imm << 12);
}

static int __exec_u_type(const instr_t func3, const char **tokens) {
  int32_t rd, imm;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__str_to_int(&imm, tokens[2]) != 0) return 1;

  if (func3 == AUIPC) {
    __auipc((uint32_t)rd, imm);
  } else if (func3 == LUI) {
    __lui((uint32_t)rd, imm);
  } else {
    return 1;
  }

  return 0;
}

static int __call(const char **tokens) {
  int32_t imm;

  if (__str_to_int(&imm, tokens[1]) != 0) return 1;

  __auipc((uint32_t)1, (imm & ((int32_t)0xFFFFF000)) >> 12);
  __jalr((uint32_t)1, (uint32_t)1, imm & 0xFFF);

  return 0;
}

static int __la(const char **tokens) {
  int32_t rd, imm;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__str_to_int(&imm, tokens[2]) != 0) return 1;

  __auipc((uint32_t)rd, (imm & ((int32_t)0xFFFFF000)) >> 12);

  r[0] = (uint32_t)0;  // Overwrite x0 to 0

  __addi((uint32_t)rd, (uint32_t)rd, imm & 0xFFF);

  return 0;
}

static int __li(const char **tokens) {
  int32_t rd, imm;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__str_to_int(&imm, tokens[2]) != 0) return 1;

  __addi((uint32_t)rd, (uint32_t)0, imm);

  return 0;
}

static int __mv(const char **tokens) {
  int32_t rd, rs;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__get_reg_number(&rs, tokens[2]) != 0) return 1;

  __addi((uint32_t)rd, (uint32_t)rs, (int32_t)0);

  return 0;
}

static int __neg(const char **tokens) {
  int32_t rd, rs;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__get_reg_number(&rs, tokens[2]) != 0) return 1;

  __sub((uint32_t)rd, (uint32_t)0, (uint32_t)rs);

  return 0;
}

static int __not(const char **tokens) {
  int32_t rd, rs;

  if (__get_reg_number(&rd, tokens[1]) != 0) return 1;
  if (__get_reg_number(&rs, tokens[2]) != 0) return 1;

  __xori((uint32_t)rd, (uint32_t)rs, (int32_t)-1);

  return 0;
}

static int __ret(void) {
  __jalr(0, 1, 0);

  return 0;
}

static int __j(const char **tokens) {
  int32_t imm;

  if (__str_to_int(&imm, tokens[1]) != 0) return 1;

  __jal((uint32_t)0, imm);

  return 0;
}

static int __jr(const char **tokens) {
  int32_t rs;

  if (__get_reg_number(&rs, tokens[1]) != 0) return 1;

  __jalr((uint32_t)0, (uint32_t)rs, 0);

  return 0;
}

static int __is_jump_instr(const instr_t func3) {
  return ((func3 == JALR) || (func3 == BEQ) || (func3 == BGE) ||
          (func3 == BGEU) || (func3 == BLT) || (func3 == BLTU) ||
          (func3 == BNE) || (func3 == JAL) || (func3 == CALL) || (func3 == J) ||
          (func3 == JR) || (func3 == RET));
}

/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
int interpret(char *instr) {
  char **tokens;
  void *t;
  instr_t func3;
  int status;

  // Parse instr by ' ' and ','
  if ((tokens = tokenize(instr, " ,")) == NULL) {
    __error_no_memory();
    return 1;
  }

  func3 = __str_to_enum(tokens[0]);

  // If func3 is a memory instruction, parameters are reorder
  if (__is_mem_instr(func3)) {
    if ((t = __order_mem_instr(func3, tokens)) == NULL) {
      __error_no_memory();
      return 1;
    }
    tokens = t;
  }

  switch (func3) {
    case ADDI:
    case ANDI:
    case JALR:
    case LB:
    case LBU:
    case LH:
    case LHU:
    case LW:
    case ORI:
    case SLLI:
    case SLTI:
    case SLTIU:
    case SRAI:
    case SRLI:
    case XORI:
      status = __exec_i_type(func3, (const char **)tokens);
      break;
    case SB:
    case SH:
    case SW:
      status = __exec_s_type(func3, (const char **)tokens);
      break;
    case ADD:
    case AND:
    case OR:
    case SLL:
    case SLT:
    case SLTU:
    case SRA:
    case SRL:
    case SUB:
    case XOR:
      status = __exec_r_type(func3, (const char **)tokens);
      break;
    case BEQ:
    case BGE:
    case BGEU:
    case BLT:
    case BLTU:
    case BNE:
      status = __exec_sb_type(func3, (const char **)tokens);
      break;
    case JAL:
      status = __exec_uj_type(func3, (const char **)tokens);
      break;
    case AUIPC:
    case LUI:
      status = __exec_u_type(func3, (const char **)tokens);
      break;
    case CALL:
      status = __call((const char **)tokens);
      break;
    case LA:
      status = __la((const char **)tokens);
      break;
    case LI:
      status = __li((const char **)tokens);
      break;
    case MV:
      status = __mv((const char **)tokens);
      break;
    case NEG:
      status = __neg((const char **)tokens);
      break;
    case NOP:
      status = 0;
      break;
    case NOT:
      status = __not((const char **)tokens);
      break;
    case J:
      status = __j((const char **)tokens);
      break;
    case JR:
      status = __jr((const char **)tokens);
      break;
    case RET:
      status = __ret();
      break;
    default:
      fprintf(stderr, "tokens[0]: %s is not a valid instruction.\n", tokens[0]);
      status = 1;
  }

  // If instruction didn't overwrote PC then we add 4 to it
  if (!__is_jump_instr(func3)) pc += 4;

  // Keep X0 = 0
  r[0] = (uint32_t)0;

  free(tokens);

  return status;
}

int main(int argc, char **argv) {
  char *buffer;

  if (argc != 3) {
    fprintf(stderr,
            "Only three parameters must be passed where argv[1] = filename "
            "with the assembly instructions and argv[2] = filename to save the "
            "registers status.\n");
    return 1;
  }

  if (process_file(argv[1])) return 1;
  if (open_file()) return 1;

  // Initialize PC, registers and memory
  init_memory_elements();

  buffer = (char *)malloc((LINE_SIZE + 1) * sizeof(char));

  if (buffer == NULL) {
    close_file();
    return 1;
  }

  while (pc != -1) {
    get_line(buffer, ((size_t)(pc >> 2)));
    if (interpret(buffer)) {
      close_file();
      free(buffer);
      return 1;
    }
  }

  close_file();
  free(buffer);

  return print_registers(argv[2]);
}
