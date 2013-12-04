CC = g++
CFLAGS = -std=c++0x
FLEX = flex
BISON = bison
RM = rm -rf
OUT = lolcode

HEADER_DEPS = lolcode_utils.h lolcode_value.h lolcode_type.h lolcode_stmt.h
SOURCE_DEPS = lolcode_type.cpp lolcode_stmt.cpp lolcode_utils.cpp

all: $(OUT)

$(OUT): lolcode.cpp lolcode.tab.c lex.yy.c $(HEADER_DEPS) $(SOURCE_DEPS) 
	$(CC) $^ -o $@ $(CFLAGS) 

lex.yy.c: lolcode.l
	$(FLEX) $<

lolcode.tab.c lolcode.tab.h: lolcode.y
	$(BISON) -d $<

clean:
	$(RM) lolcode.tab.h lolcode.tab.c
	$(RM) lex.yy.c
	$(RM) $(OUT)
