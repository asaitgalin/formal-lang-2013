CC = gcc
CFLAGS = -Wall --std=c99
FLEX = flex
RM = rm -rf
OUTPUT = tacinterp

all: $(OUTPUT)

$(OUTPUT): variable_list.o variable_table.o lex.yy.c
	$(CC) $^ -o $@

lex.yy.c: tacinterp.l
	$(FLEX) tacinterp.l 

variable_list.o: variable_list.h
variable_table.o: variable_list.h variable_table.h

clean:
	$(RM) $(OUTPUT)
	$(RM) lex.yy.c
	$(RM) *.o

