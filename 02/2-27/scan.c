/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "scan.h"

#include "../../tiny-compiler/globals.h"
#include "../../tiny-compiler/util.h"

/* states in scanner DFA */
typedef enum { START,
               INASSIGN,
               INCOMMENT,
               INNUM,
               INID,
               DONE } StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void) {
   if (!(linepos < bufsize)) {
      lineno++;
      if (fgets(lineBuf, BUFLEN - 1, source)) {
         if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
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
static void ungetNextChar(void) {
   if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct ReservedWord {
   char* str;
   TokenType tok;
};

// These need to be in alphabetical order for the binary search implementation
// part of exercise 2-27a
static struct ReservedWord reservedWords[MAXRESERVED] = {
    {"else", ELSE},
    {"end", END},
    {"if", IF},
    {"read", READ},
    {"repeat", REPEAT},
    {"then", THEN},
    {"until", UNTIL},
    {"write", WRITE},
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char* s) {
   int i;
   for (i = 0; i < MAXRESERVED; i++)
      if (!strcmp(s, reservedWords[i].str))
         return reservedWords[i].tok;
   return ID;
}

/* lookup an identifier to see if it is a reserved word */
/* uses binary search (exercise 2-27a) */
static TokenType reservedLookupBinary(char* s) {
   int min = 0;
   int max = MAXRESERVED - 1;
   int compareValue;

   while (max >= min) {
      int mid = (min + max) / 2;
      compareValue = strcmp(s, reservedWords[mid].str);
      if (!compareValue)
         return reservedWords[mid].tok;
      else if (compareValue < 0)
         max = mid - 1;
      else
         min = mid + 1;
   }
   return ID;
}

// Part of 2-27b
static struct {
   int key;
   struct ReservedWord word;
} reservedWordHashItems[MAXRESERVED];

static int wordTableInitialized = FALSE;

static void initializeReservedWordTable() {
   for (int i = 0; i < MAXRESERVED; i++) {
      reservedWordHashItems[i].key = computeHash(reservedWords[i].str);
      reservedWordHashItems[i].word = reservedWords[i];
   }
}

/* 2-27b - lookup an identifier to see if it is a reserved word using the hashtable */
static TokenType reservedHashLookup(char* s) {
   int identifierHash = computeHash(s);
   for (int i = 0; i < MAXRESERVED; i++)
      if (identifierHash == reservedWordHashItems[i].key)
         return reservedWordHashItems[i].word.tok;
   return ID;
}

// 2-27b - Hash value for a string of all lowercase alpha characters
static long long computeHash(char* s) {
   const int p = 31;
   const int m = 1e9 + 9;
   long long hash_value = 0;
   long long p_pow = 1;

   while (*s) {
      hash_value = (hash_value + (*s++ - 'a' + 1) * p_pow) % m;
      p_pow = (p_pow * p) % m;
   }
   return hash_value;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType
getToken(void) { /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;

   // 2-27b
   if (!wordTableInitialized) {
      initializeReservedWordTable();
      wordTableInitialized = TRUE;
   }

   while (state != DONE) {
      int c = getNextChar();
      save = TRUE;
      switch (state) {
         case START:
            if (isdigit(c))
               state = INNUM;
            else if (isalpha(c))
               state = INID;
            else if (c == ':')
               state = INASSIGN;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
               save = FALSE;
            else if (c == '{') {
               save = FALSE;
               state = INCOMMENT;
            } else {
               state = DONE;
               switch (c) {
                  case EOF:
                     save = FALSE;
                     currentToken = ENDFILE;
                     break;
                  case '=':
                     currentToken = EQ;
                     break;
                  case '<':
                     currentToken = LT;
                     break;
                  case '+':
                     currentToken = PLUS;
                     break;
                  case '-':
                     currentToken = MINUS;
                     break;
                  case '*':
                     currentToken = TIMES;
                     break;
                  case '/':
                     currentToken = OVER;
                     break;
                  case '(':
                     currentToken = LPAREN;
                     break;
                  case ')':
                     currentToken = RPAREN;
                     break;
                  case ';':
                     currentToken = SEMI;
                     break;
                  default:
                     currentToken = ERROR;
                     break;
               }
            }
            break;
         case INCOMMENT:
            save = FALSE;
            if (c == EOF) {
               state = DONE;
               currentToken = ENDFILE;
            } else if (c == '}')
               state = START;
            break;
         case INASSIGN:
            state = DONE;
            if (c == '=')
               currentToken = ASSIGN;
            else { /* backup in the input */
               ungetNextChar();
               save = FALSE;
               currentToken = ERROR;
            }
            break;
         case INNUM:
            if (!isdigit(c)) { /* backup in the input */
               ungetNextChar();
               save = FALSE;
               state = DONE;
               currentToken = NUM;
            }
            break;
         case INID:
            if (!isalpha(c)) { /* backup in the input */
               ungetNextChar();
               save = FALSE;
               state = DONE;
               currentToken = ID;
            }
            break;
         case DONE:
         default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR;
            break;
      }
      if ((save) && (tokenStringIndex <= MAXTOKENLEN))
         tokenString[tokenStringIndex++] = (char)c;
      if (state == DONE) {
         tokenString[tokenStringIndex] = '\0';
         if (currentToken == ID)
            currentToken = reservedLookup(tokenString);
      }
   }
   if (TraceScan) {
      fprintf(listing, "\t%d: ", lineno);
      printToken(currentToken, tokenString);
   }
   return currentToken;
} /* end getToken */
