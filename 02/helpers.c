#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

#define MAXRESERVED 32

static int lineno = 0;

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* lookup table of reserved words */
static struct {
   char* str;
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
      case 4:
         return START;
   }
   return previousState;
}

int nextIdentifierState(StateType previousState, char* tokenString, char c) {
   switch (previousState) {
      case START:
         if (isalpha(c)) {
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
      case 4:
         return START;
   }
   return previousState;
}
