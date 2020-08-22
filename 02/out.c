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

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
INT getNextChar(FILE* source) {
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