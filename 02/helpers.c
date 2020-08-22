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

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
int getNextChar(FILE* source) {
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