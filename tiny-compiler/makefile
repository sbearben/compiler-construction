cc = gcc

CFLAGS = -Wall -I.
DEPS = analyze.h cgen.h code.h globals.h parse.h scan.h symtab.h util.h
OBJ = main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tiny: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

parse.o: CFLAGS += -fno-builtin


clean: 
	rm -f tiny main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o lex/lex.yy.c yacc/tiny.tab.c  

tm: tm.c
	gcc -g tm.c