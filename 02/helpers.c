#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static int lineno = 0;

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

#define MAXRESERVED 32

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

/* lookup table of reserved words */
static struct {
   char *str;
   ReservedWordType type;
} reservedWords[MAXRESERVED] = {
    {"auto", AUTO},
    {"break", BREAK},
    {"case", CASE},
    {"char", CHAR},
    {"const", CONST},
    {"continue", CONTINUE},
    {"default", DEFAULT},
    {"do", DO},
    {"double", DOUBLE},
    {"else", ELSE},
    {"enum", ENUM},
    {"extern", EXTERN},
    {"float", FLOAT},
    {"for", FOR},
    {"goto", GOTO},
    {"if", IF},
    {"int", INT},
    {"long", LONG},
    {"register", REGISTER},
    {"return", RETURN},
    {"short", SHORT},
    {"signed", SIGNED},
    {"sizeof", SIZEOF},
    {"static", STATIC},
    {"struct", STRUCT},
    {"switch", SWITCH},
    {"typedef", TYPEDEF},
    {"union", UNION},
    {"unsigned", UNSIGNED},
    {"void", VOID},
    {"volatile", VOLATILE},
    {"while", WHILE},
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
int checkIfReservedWord(char *s) {
   int i;
   for (i = 0; i < MAXRESERVED; i++)
      if (!strcmp(s, reservedWords[i].str))
         return TRUE;
   return FALSE;
}

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
int getNextChar(FILE *source) {
   if (!(linepos < bufsize)) {
      lineno++;
      if (fgets(lineBuf, BUFLEN - 1, source)) {
         // fprintf(stdout, "%4d: %s", lineno, lineBuf);
         bufsize = strlen(lineBuf);
         linepos = 0;
         return lineBuf[linepos++];
      } else {
         EOF_flag = TRUE;
         return EOF;
      }
   } else
      return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
void ungetNextChar(void) {
   if (!EOF_flag) linepos--;
}