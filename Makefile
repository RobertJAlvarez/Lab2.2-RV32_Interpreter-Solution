CC   = cc
OBJS = StackPointer.o my_string.o tokenizer.o

CFLAGS = -O3 -g3 -Wall -Wextra -Werror=format-security -Werror=implicit-function-declaration \
         -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wconversion

all: riscv

riscv: $(OBJS) riscv.o
	$(CC) -o $@ $^

clean:
	rm -f *.o riscv

StackPointer.o: StackPointer.c StackPointer.h
my_string.o: my_string.c my_string.h
tokenizer.o: tokenizer.c tokenizer.h my_string.h
riscv.o: riscv.c my_string.h tokenizer.h StackPointer.h

