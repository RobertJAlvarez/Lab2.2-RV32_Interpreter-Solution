#include <errno.h>   //errno
#include <stdint.h>  //int32_t
#include <stdio.h>   //printf() fprintf()
#include <stdlib.h>  //malloc() & free()

#include "my_string.h"
#include "process_file.h"
#include "stack_pointer.h"
#include "tokenizer.h"

#define N_REGISTERS ((size_t)32)

int32_t *r;

static void error_no_memory(void) {
  fprintf(stderr, "No more memory available. Errorno: %d\n", errno);
  return;
}

static int init_regs(void) {
  if ((r = (int32_t)malloc(N_REGISTERS * sizeof(int32_t))) == NULL) return 1;

  for (size_t i = 0; i < N_REGISTERS; i++) r[i] = ((int32_t)0);

  return 0;
}

static void end_regs(void) { free(r); }

/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
int interpret(char *instr) {
  // TODO:

  return 1;
}

int main(int argc, char **argv) {
  FILE *file;
  char *buffer;

  if (argc != 2) {
    fprintf(stderr, "Only two parameters must be passed.\n");
    return 1;
  }

  if (process_file(argv[1])) return 1;
  if (open_file()) return 1;

  if (init_regs()) {
    close_file();
    return 1;
  }

  buffer = (char *)malloc((LINE_SIZE + 1) * sizeof(char));

  if (buffer == NULL) {
    close_file();
    end_regs();
    return 1;
  }

  /* --- Your code starts here. --- */

  // For each line in the file, interpret it.
  // NOTE: Use get_line() function in process_file.h

  /* --- Your code ends here. --- */

  close_file();
  end_regs();
  free(buffer);

  return 0;
}
