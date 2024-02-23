#ifndef __STACK_POINTER_H__
#define __STACK_POINTER_H__

#include <stddef.h>

typedef struct {
  unsigned char *sp;
  size_t size;
} sp_t;

/* Get new stack with initially 0 memory. */
sp_t *init(void);

/* Add some memory to the stack pointer. */
int sp_add_mem(sp_t *, size_t);

/* Load from the stack. */
unsigned char sp_get_at(sp_t *, size_t);

/* Save to the stack. */
void sp_set_at(sp_t *, size_t, unsigned char);

#endif
