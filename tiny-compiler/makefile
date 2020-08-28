CC = gcc

BASEDIR = .
CFLAGS = -Wall -I$(BASEDIR)

_DEPS = analyze.h cgen.h code.h globals.h parse.h scan.h symtab.h util.h
DEPS = $(patsubst %,$(BASEDIR)/%,$(_DEPS))

_OBJ = main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o
OBJ = $(patsubst %,$(BASEDIR)/%,$(_OBJ))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tiny: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

parse.o: CFLAGS += -fno-builtin


clean: 
	rm -f $(BASEDIR)/tiny $(BASEDIR)/*.o $(BASEDIR)/lex/lex.yy.c $(BASEDIR)/yacc/tiny.tab.c  

tm: tm.c
	gcc -g tm.c