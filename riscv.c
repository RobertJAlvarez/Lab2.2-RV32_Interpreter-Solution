#include <errno.h>   //errno
#include <stdint.h>  // int32_t
#include <stdio.h>   // printf()
#include <stdlib.h>  // malloc() & free()

#include "StackPointer.h"
#include "my_string.h"
#include "tokenizer.h"

#define N_REGISTERS ((size_t)32)

int32_t *r;

static int error_no_memory(void) {
  fprintf(stderr, "No more memory available. Errorno: %d\n", errno);
  return 1;
}

/* Do not alter this function! */
int init_regs(void) {
  r = malloc(N_REGISTERS * sizeof(int32_t));

  if (r == NULL) return error_no_memory();
  for (size_t i = 0; i < N_REGISTERS; i++) r[i] = ((int32_t)0);

  return 0;
}

/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
int interpret(char *instr) {
  /* Your code starts here. */

  return 1;
}

int main(void) {
  if (init_regs()) return 1;

  /* Your code starts here. */

  return 0;
}
