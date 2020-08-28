CC = gcc

BASEDIR = ../../tiny-compiler
CFLAGS = -Wall -I$(BASEDIR)

_DEPS = analyze.h cgen.h code.h globals.h parse.h scan.h symtab.h util.h
DEPS = $(patsubst %,$(BASEDIR)/%,$(_DEPS))

_OBJ = main.o util.o parse.o symtab.o analyze.o code.o cgen.o
OBJ = $(patsubst %,$(BASEDIR)/%,$(_OBJ)) scan.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tiny: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(BASEDIR)/parse.o: CFLAGS += -fno-builtin


clean: 
	rm -f $(BASEDIR)/tiny $(BASEDIR)/*.o $(BASEDIR)/lex/lex.yy.c $(BASEDIR)/yacc/tiny.tab.c  

tm: tm.c
	gcc -g tm.c