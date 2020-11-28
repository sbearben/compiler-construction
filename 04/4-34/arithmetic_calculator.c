// AK: exercise 4.34: Write a recursive-descent calculator for the grammar below

/* From page 148: Simple intger arithmetic calculator according to EBNF

   <lexp> -> <number> | ( <op> <lexp-seq> )
   <op> -> + | - | *
   <lexp-seq>  <lexp> { <lexp> }  // EBNF

   // Last production in BNF
   <lexp-seq> <lexp-seq> <lexp> | <lexp>

   Inputs a line of text from stdin
   Outputs "Error" or the result.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

char token;  // global token variable

typedef enum {
   Plus,
   Minus,
   Times
} OpKind;

/* function prototypes for recursive calls */
int lexp(void);
int lexpSeq(OpKind opKind);
int factor(void);

void error(void) {
   fprintf(stderr, "Error - last token: %c\n", token);
   exit(1);
}

int isWhitespace(int c) {
   // Intentially omitted newline here since that is what we check for in main() to insure input is fine
   return c == ' ' || c == '\t';
}

int getNextChar(void) {
   int nextChar = getchar();
   while (isWhitespace(nextChar)) {
      nextChar = getchar();
   }
   return nextChar;
}

void match(char expectedToken) {
   if (token == expectedToken)
      token = getNextChar();
   else {
      error();
   }
}

int lexp(void) {
   int temp;
   if (isdigit(token)) {
      ungetc(token, stdin);
      scanf("%d", &temp);
      token = getNextChar();
   } else if (token == '(') {
      match('(');
      switch (token) {
         case '+':
            match('+');
            temp = lexpSeq(Plus);
            break;
         case '-':
            match('-');
            temp = lexpSeq(Minus);
            break;
         case '*':
            match('*');
            temp = lexpSeq(Times);
            break;
         default:
            error();
      }
      match(')');
   } else {
      error();
   }

   return temp;
}

int lexpSeq(OpKind opKind) {
   int temp = lexp();
   while (token != ')') {
      int next = lexp();
      switch (opKind) {
         case Plus:
            temp += next;
            break;
         case Minus:
            temp -= next;
            break;
         case Times:
            temp *= next;
            break;
      }
   }
   return temp;
}

int main() {
   int result;
   token = getNextChar();  // load token with first character for lookahead
   result = lexp();
   if (token == '\n')  // check for end of line
      printf("Result = %d\n", result);
   else
      error();  // extraneous chars on line
   return 0;
}