#ifndef __STACK_POINTER_H__
#define __STACK_POINTER_H__

#include <stddef.h>     //size_t
#include <sys/types.h>  //ssize_t

typedef struct {
  unsigned char *p;
  size_t size;
} sp_t;

/* Get new stack with initially 0 memory. */
sp_t *sp_init(void);

/* Add some memory to the stack pointer. */
int sp_add_mem(sp_t *, ssize_t);

/* Load from the stack. */
unsigned char sp_get_at(sp_t *, size_t);

/* Save to the stack. */
void sp_set_at(sp_t *, size_t, unsigned char);

#endif
