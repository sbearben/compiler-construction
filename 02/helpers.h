#ifndef _HELPERS_H_
#define _HELPERS_H_

#define FALSE 0
#define TRUE 1

#include <stdio.h>

extern int getNextChar(FILE* source);
extern void ungetNextChar(void);

extern int checkIfReservedWord(char* s);

#endif
