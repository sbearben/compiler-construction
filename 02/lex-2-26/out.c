#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* BUFLEN = length of the input buffer for
   source code lines */
#define buflen 256

static int lineno = 0;

static char linebuf[buflen]; /* holds the current line /* A NESTED COOMMENT */
more stuff MORE */
    static int linepos = 0;  /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int eof_flag = false; /* corrects ungetNextChar behavior on EOF */

#define maxreserved 32

typedef enum {
   auto,
   break,
   case,
   char,
   const,
   continue,
   default,
   do,
   double,
   else,
   enum,
   extern,
   float,
   for,
   goto,
   if,
   int,
   long,
   register,
   return,
   short,
   signed,
   sizeof,
   static,
   struct,
   switch,
   typedef,
   union,
   unsigned,
   void,
   volatile,
   while
} reservedwordtype;

/* lookup table of reserved words */
static struct {
   char *str;
   reservedwordtype type;
} reservedwords[maxreserved] = {
    {"auto", auto},
    {"break", break},
    {"case", case},
    {"char", char},
    {"const", const},
    {"continue", continue},
    {"default", default},
    {"do", do},
    {"double", double},
    {"else", else},
    {"enum", enum},
    {"extern", extern},
    {"float", float},
    {"for", for},
    {"goto", goto},
    {"if", if},
    {"int", int},
    {"long", long},
    {"register", register},
    {"return", return},
    {"short", short},
    {"signed", signed},
    {"sizeof", sizeof},
    {"static", static},
    {"struct", struct},
    {"switch", switch},
    {"typedef", typedef},
    {"union", union},
    {"unsigned", unsigned},
    {"void", void},
    {"volatile", volatile},
    {"while", while},
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
int checkifreservedword(char *s) {
   int i;
   for (i = 0; i < maxreserved; i++)
      if (!strcmp(s, reservedwords[i].str))
         return true;
   return false;
}

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
int getnextchar(file *source) {
   if (!(linepos < bufsize)) {
      lineno++;
      if (fgets(linebuf, buflen - 1, source)) {
         // fprintf(stdout, "%4d: %s", lineno, lineBuf);
         bufsize = strlen(linebuf);
         linepos = 0;
         return linebuf[linepos++];
      } else {
         eof_flag = true;
         return eof;
      }
   } else
      return linebuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
void ungetnextchar(void) {
   if (!eof_flag) linepos--;
}