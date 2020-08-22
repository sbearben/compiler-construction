#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

/* allocate global variables */
FILE *source;
FILE *out;

typedef enum {
   START,
   ENTERING_COMMENT,
   IN_COMMENT,
   EXITING_COMMENT,
   DONE,
} StateType;

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
