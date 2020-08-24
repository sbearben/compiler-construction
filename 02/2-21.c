#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

/* allocate global variables */
FILE *source;
FILE *out;

#define MAXRESERVED 32

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

typedef enum {
   START,
   ENTERING_COMMENT,
   IN_BLOCK_COMMENT,
   IN_SINGLELINE_COMMENT,
   EXITING_BLOCK_COMMENT,
   IN_IDENTIFIER,
   DONE,
} StateType;

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
   int tokenStringIndex = 0;
   int shouldWriteWord = FALSE;
   StateType state = START;

   while ((c = getNextChar(source)) != EOF) {
      shouldWriteWord = FALSE;
      switch (state) {
         case START:
            if (c == '/') {
               state = ENTERING_COMMENT;
            } else if (isalpha(c)) {
               state = IN_IDENTIFIER;
            }
            break;
         case ENTERING_COMMENT:
            if (c == '*') {
               state = IN_BLOCK_COMMENT;
            } else if (c == '/') {
               state = IN_SINGLELINE_COMMENT;
            } else {
               state = START;
            }
            break;
         case IN_BLOCK_COMMENT:
            if (c == '*') {
               state = EXITING_BLOCK_COMMENT;
            }
            break;
         case IN_SINGLELINE_COMMENT:
            if (c == '\n') {
               state = START;
            }
            break;
         case EXITING_BLOCK_COMMENT:
            if (c == '/') {
               state = START;
            } else if (c == '*') {
               break;
            } else {
               state = IN_BLOCK_COMMENT;
            }
            break;
         case IN_IDENTIFIER:
            if (!isalpha(c)) {
               ungetNextChar();
               shouldWriteWord = TRUE;
               state = START;
            }
            break;
         case DONE:
            state = START;
      }
      if (state != IN_IDENTIFIER) {
         if (shouldWriteWord) {
            tokenString[tokenStringIndex] = '\0';
            int isReservedWord = checkIfReservedWord(tokenString);
            for (int i = 0; i < tokenStringIndex; i++)
               putc(isReservedWord ? toupper(tokenString[i]) : tokenString[i], out);
            tokenStringIndex = 0;
         } else {
            putc(c, out);
         }
      } else {
         if (tokenStringIndex <= MAXTOKENLEN) {
            tokenString[tokenStringIndex++] = c;
         }
      }
   }
   fclose(source);
   fclose(out);
   return 0;
}
