CC   = cc
OBJS = my_string.o helper.o tokenizer.o process_file.o instr_i_type.o instr_pseudo_type.o \
        instr_r_type.o instr_s_type.o instr_sb_type.o instr_u_type.o instr_uj_type.o

CFLAGS = -O3 -g3 -Wall -Wextra -Werror=format-security -Werror=implicit-function-declaration \
          -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wconversion

# This files are dummy files to test the basics of the interpreter
INSTRUCTIONS := LI_SP_ADD MV_X0 NEG_NOT J_JR

B_SUF = _BUFFER.txt
A_SUF = _ASSEMBLY.txt

all: riscv $(foreach instr,$(INSTRUCTIONS),$(instr))

riscv: $(OBJS) riscv.o
	$(CC) -o $@ $^

$(INSTRUCTIONS):
	cp ../assembly_files/$@$(A_SUF) .
	touch $@$(B_SUF)  # Create buffer file
	# Interpret $@$(A_SUF) using ./riscv and which populates $@$(B_SUF) with final registers status
	./riscv $@$(A_SUF) $@$(B_SUF)

clean:
	rm -f *.o riscv $(addsuffix $(A_SUF),$(INSTRUCTIONS)) $(addsuffix $(B_SUF),$(INSTRUCTIONS)) seekable_file.txt

helper.o: helper.c helper.h instr_types.h my_string.h
instr_i_type.o: instr_i_type.c helper.h instr_i_type.h riscv.h
instr_pseudo_type.o: instr_pseudo_type.c helper.h instr_i_type.h instr_pseudo_type.h \
                      instr_r_type.h instr_u_type.h instr_uj_type.h riscv.h
instr_r_type.o: instr_r_type.c helper.h instr_r_type.h riscv.h
instr_s_type.o: instr_s_type.c helper.h instr_i_type.h instr_s_type.h riscv.h
instr_sb_type.o: instr_sb_type.c helper.h instr_i_type.h instr_sb_type.h riscv.h
instr_u_type.o: instr_u_type.c helper.h instr_u_type.h riscv.h
instr_uj_type.o: instr_uj_type.c helper.h instr_uj_type.h riscv.h
my_string.o: my_string.c my_string.h
tokenizer.o: tokenizer.c tokenizer.h my_string.h
process_file.o: process_file.c process_file.h
riscv.o: riscv.c my_string.h tokenizer.h process_file.h instr_types.h

