#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* BUFLEN = LENGTH OF THE INPUT BUFFER FOR
   SOURCE CODE LINES */
#define BUFLEN 256

static int lineno = 0;

static char lineBuf[BUFLEN]; /* HOLDS THE CURRENT LINE */
static int linepos = 0;      /* CURRENT POSITION IN LINEBUF */
static int bufsize = 0;      /* CURRENT SIZE OF BUFFER STRING */
static int EOF_flag = FALSE; /* CORRECTS UNGETNEXTCHAR BEHAVIOR ON EOF */

/* GETNEXTCHAR FETCHES THE NEXT NON-BLANK CHARACTER
   FROM LINEBUF, READING IN A NEW LINE IF LINEBUF IS
   EXHAUSTED */
int getNextChar(FILE* source) {
   if (!(linepos < bufsize)) {
      lineno++;
      if (fgets(lineBuf, BUFLEN - 1, source)) {
         // FPRINTF(STDOUT, "%4D: %S", LINENO, LINEBUF);
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

/* UNGETNEXTCHAR BACKTRACKS ONE CHARACTER
   IN LINEBUF */
void ungetNextChar(void) {
   if (!EOF_flag) linepos--;
}