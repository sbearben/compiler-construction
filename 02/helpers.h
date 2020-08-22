#ifndef _HELPERS_H_
#define _HELPERS_H_

#define FALSE 0
#define TRUE 1

#include <stdio.h>

extern int getNextChar(FILE *source);
extern void ungetNextChar(void);

typedef enum {
   START,
   ENTERING_COMMENT,
   IN_COMMENT,
   EXITING_COMMENT,
   IN_IDENTIFIER,
   DONE,
} StateType;

extern int nextCommentState(StateType previousState, char c);

typedef enum {
   AUTO,
   BREAK,
   CASE,
   CHAR,
   CONST,
   CONTINUE,
   DEFAULT,
   DO,
   DOUBLE,
   ELSE,
   ENUM,
   EXTERN,
   FLOAT,
   FOR,
   GOTO,
   IF,
   INT,
   LONG,
   REGISTER,
   RETURN,
   SHORT,
   SIGNED,
   SIZEOF,
   STATIC,
   STRUCT,
   SWITCH,
   TYPEDEF,
   UNION,
   UNSIGNED,
   VOID,
   VOLATILE,
   WHILE
} ReservedWordType;

#endif
