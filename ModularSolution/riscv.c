#include <errno.h>   //errno
#include <stdint.h>  //uint32_t
#include <stdio.h>   //printf(), fprintf()
#include <stdlib.h>  //malloc(), realloc() & free()
#include <string.h>  //strerror()

#include "helper.h"
#include "instr_types.h"
#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"

const size_t N_REGISTERS = ((size_t)32);
const size_t MEM_SIZE = ((size_t)(1 << 10));

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

static int print_registers(char *fd_name) {
  FILE *fptr;

  if ((fptr = fopen(fd_name, "w")) == NULL) return 1;

  // Print all registers
  fprintf(fptr, "Registers:\n");
  for (size_t i = 0; i < N_REGISTERS; i++)
    fprintf(fptr, "X[%zu] = %u\n", i, r[i]);

  return 0;
}

static size_t arr_len(char **arr) {
  size_t count = 0;
  for (char **p = arr; *p != NULL; p++) count++;
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
  if ((t1 = tokenize(tokens[2], "(")) == NULL) return NULL;

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
  *str_chr(tokens[2], (int)')') = '\0';
  // Failure from str_chr is not a possible case because we are guarantee a
  // correct format

  return tokens;
}

/* Tokens is guaranteed to be a memory instruction. So, we reorder the
 * parameters.
 *
 * For example:
 *  1) SW RS2,offset(RS1) -> SW RS1,RS2,offset
 *  2) LB RD,offset(RS1) -> LB RD,RS1,offset
 */
static void *__order_mem_instr(instr_type_t instr_type, char **tokens) {
  void *t;

  // If there are 4 tokens then we have the expanded format
  if (arr_len(tokens) == 4) return tokens;

  if ((t = __expand_last(tokens)) == NULL) return NULL;

  tokens = (char **)t;

  // If it is a load instr we already have the token in the wanted order.
  // If it is a save instr, go from [func3, RS2, RS1, offset] to [func3, RS1,
  // RS2, offset] Meaning: from SW RS2,offset(RS1) to SW RS1,RS2,offset
  if (instr_type == INSTR_S) {
    t = (void *)tokens[1];
    tokens[1] = tokens[2];
    tokens[2] = (char *)t;
  }

  return tokens;
}

static int __is_jump_instr(instr_type_t type, instr_union_t func3) {
  return (((type == INSTR_I) && ((func3.i == JALR))) ||
          ((type == INSTR_PSEUDO) &&
           ((func3.pseudo == CALL) || (func3.pseudo == J) ||
            (func3.pseudo == JR))) ||
          ((type == INSTR_SB) &&
           ((func3.sb == BEQ) || (func3.sb == BGE) || (func3.sb == BGEU) ||
            (func3.sb == BLT) || (func3.sb == BLTU) || (func3.sb == BNE))) ||
          ((type == INSTR_UJ) && ((func3.uj == JAL))));
}

/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
int interpret(char *line_instr) {
  char **tokens;
  void *t;
  instr_t instr;
  int status;

  // Parse instr by ' ' and ','
  if ((tokens = tokenize(line_instr, " ,")) == NULL) {
    __error_no_memory();
    return 1;
  }

  instr = str_to_enum(tokens[0]);

  switch (instr.instr_type) {
    case INSTR_I:
      // If instruction is a memory instruction, parameters are reorder
      if (__is_i_mem_instr(instr.instr.i)) {
        if ((t = __order_mem_instr(INSTR_I, tokens)) == NULL) {
          __error_no_memory();
          return 1;
        }
        tokens = t;
      }
      status = exec_i_type(instr.instr.i, tokens);
      break;
    case INSTR_PSEUDO:
      status = exec_pseudo_type(instr.instr.pseudo, tokens);
      break;
    case INSTR_R:
      status = exec_r_type(instr.instr.r, tokens);
      break;
    case INSTR_S:
      // All instructions type s are memory instructions
      if ((t = __order_mem_instr(INSTR_S, tokens)) == NULL) {
        __error_no_memory();
        return 1;
      }
      tokens = t;
      status = exec_s_type(instr.instr.s, tokens);
      break;
    case INSTR_SB:
      status = exec_sb_type(instr.instr.sb, tokens);
      break;
    case INSTR_U:
      status = exec_u_type(instr.instr.u, tokens);
      break;
    case INSTR_UJ:
      status = exec_uj_type(instr.instr.uj, tokens);
      break;
    default:
      fprintf(stderr, "tokens[0]: %s is not a valid instruction.\n", tokens[0]);
      status = 1;
  }

  // If instruction didn't overwrote PC then we add 4 to it
  if (!__is_jump_instr(instr.instr_type, instr.instr)) pc += 4;

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
