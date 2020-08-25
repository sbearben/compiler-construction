#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

STATIC INT lineno = 0;

STATIC CHAR lineBuf[BUFLEN]; /* holds the current line */
STATIC INT linepos = 0;      /* current position in LineBuf */
STATIC INT bufsize = 0;      /* current size of buffer string */
STATIC INT EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

#define MAXRESERVED 32

TYPEDEF ENUM {
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

/* lookup table of reserved words */
STATIC STRUCT {
   CHAR *str;
   ReservedWordType type;
} reservedWords[MAXRESERVED] = {
    {"AUTO", AUTO},
    {"BREAK", BREAK},
    {"CASE", CASE},
    {"CHAR", CHAR},
    {"CONST", CONST},
    {"CONTINUE", CONTINUE},
    {"DEFAULT", DEFAULT},
    {"DO", DO},
    {"DOUBLE", DOUBLE},
    {"ELSE", ELSE},
    {"ENUM", ENUM},
    {"EXTERN", EXTERN},
    {"FLOAT", FLOAT},
    {"FOR", FOR},
    {"GOTO", GOTO},
    {"IF", IF},
    {"INT", INT},
    {"LONG", LONG},
    {"REGISTER", REGISTER},
    {"RETURN", RETURN},
    {"SHORT", SHORT},
    {"SIGNED", SIGNED},
    {"SIZEOF", SIZEOF},
    {"STATIC", STATIC},
    {"STRUCT", STRUCT},
    {"SWITCH", SWITCH},
    {"TYPEDEF", TYPEDEF},
    {"UNION", UNION},
    {"UNSIGNED", UNSIGNED},
    {"VOID", VOID},
    {"VOLATILE", VOLATILE},
    {"WHILE", WHILE},
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
INT checkIfReservedWord(CHAR *s) {
   INT i;
   FOR (i = 0; i < MAXRESERVED; i++)
      IF (!strcmp(s, reservedWords[i].str))
         RETURN TRUE;
   RETURN FALSE;
}

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
INT getNextChar(FILE *source) {
   IF (!(linepos < bufsize)) {
      lineno++;
      IF (fgets(lineBuf, BUFLEN - 1, source)) {
         // fprintf(stdout, "%4d: %s", lineno, lineBuf);
         bufsize = strlen(lineBuf);
         linepos = 0;
         RETURN lineBuf[linepos++];
      } ELSE {
         EOF_flag = TRUE;
         RETURN EOF;
      }
   } ELSE
      RETURN lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
VOID ungetNextChar(VOID) {
   IF (!EOF_flag) linepos--;
}