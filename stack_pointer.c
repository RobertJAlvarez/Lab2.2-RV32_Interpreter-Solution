#include <stdlib.h>  //malloc() & free()

#include "stack_pointer.h"

sp_t *init(void) {
  // TODO: Get space for the heap for a sp_t variable

  // TODO: Set field values to a default value

  // TODO: Return element from the heap

  return NULL;
}

int sp_add_mem(sp_t *sp, ssize_t n) {
  // TODO: Add n memory to sp
  // NOTE: The p pointer could be empty

  return 1;
}

unsigned char sp_get_at(sp_t *sp, size_t i) {
  // TODO: Return the i byte in sp
  // NOTE: Doing "return sp->p[i]" is wrong, why?

  return (unsigned char)0;
}

void sp_set_at(sp_t *sp, size_t i, unsigned char c) {
  // TODO: Set the i byte in sp to c
  // NOTE: Doing "sp->p[i] = c" is wrong

  return;
}
