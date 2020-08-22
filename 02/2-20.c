#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

/* allocate global variables */
FILE *source;
FILE *out;

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

#define MAXRESERVED 32

int lineno = 0;

char lineBuf[BUFLEN]; /* holds the current line */
int linepos = 0;      /* current position in LineBuf */
int bufsize = 0;      /* current size of buffer string */
int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

typedef enum {
   START,
   ENTERING_COMMENT,
   IN_COMMENT,
   EXITING_COMMENT,
   DONE,
} StateType;

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

int nextCommentState(StateType previousState, char c) {
   switch (previousState) {
      case START:
         if (c == '/') {
            return ENTERING_COMMENT;
         }
         break;
      case ENTERING_COMMENT:
         if (c == '*') {
            return IN_COMMENT;
         }
         return START;
      case IN_COMMENT:
         if (c == '*') {
            return EXITING_COMMENT;
         }
         break;
      case EXITING_COMMENT:
         if (c == '/') {
            return DONE;
         } else if (c == '*' || c == '\n') {
            break;
         }
         return IN_COMMENT;
      case DONE:
         return START;
   }
   return previousState;
}

int main(int argc, char *argv[]) {
   char *filename = argv[1]; /* source code file name */
   if (argc != 2) {
      fprintf(stderr, "usage: %s <filename>\n", argv[0]);
      exit(1);
   }

   source = fopen(filename, "r");
   if (source == NULL) {
      fprintf(stderr, "File %s not found\n", filename);
      exit(1);
   }

   out = fopen("./out.c", "w");
   if (out == NULL) {
      fprintf(stderr, "Couldn't open file out.c for writing\n");
      exit(1);
   }

   int c;
   StateType state = START;
   while ((c = getNextChar(source)) != EOF) {
      state = nextCommentState(state, c);
      fputc(state == IN_COMMENT ? toupper(c) : c, out);
   }

   fclose(source);
   fclose(out);
   return 0;
}
