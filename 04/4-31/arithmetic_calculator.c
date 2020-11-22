// AK: exercise 4.31: Rewrite the calculator so that it distinguishes between floating-point
//     and integer valus (have to use types now with metadata over primitives)

/* From page 148: Simple intger arithmetic calculator according to EBNF

   <exp> -> <term> { <addop> <term> }
   <addop> -> + | -
   <term> -> <factor> { <mulop> <factor> }
   <mulop> -> *
   <factor> -> ( <exp> ) | Number

   Inputs a line of text from stdin
   Outputs "Error" or the result.
*/

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

char token;  // global token variable

typedef enum {
   Integer,
   Float,
} ValueKind;

typedef struct value {
   ValueKind kind;
   double val;
} Value;

/* function prototypes for recursive calls */
Value expression(void);
Value term(void);
Value factor(void);

// Checks if a float/double has decimal points (or if not can be cast to into without loss)
int isFloat(double value) {
   if (fmod(value, 1.0) == 0.0)
      return 0;
   return 1;
}

// arguments: left-hand side, right-hand side,
ValueKind getKind(Value *lhs, Value *rhs) {
   return rhs->kind == Float ? Float : lhs->kind;
}

void error(void) {
   fprintf(stderr, "Error\n");
   exit(1);
}

void match(char expectedToken) {
   if (token == expectedToken)
      token = getchar();
   else
      error();
}

Value expression(void) {
   Value temp = term();
   while ((token == '+') || (token == '-')) {
      char lastToken = token;  // need to do this or else `match` will cause switch to no-op - alternative is to inline both sequences but then we have a lot of duplication
      match(token);
      Value next = term();
      temp.kind = getKind(&temp, &next);
      switch (lastToken) {
         case '+':
            temp.val += next.val;
            break;
         case '-':
            temp.val -= next.val;
            break;
      }
   }
   return temp;
}

Value term(void) {
   Value temp = factor();
   while (token == '*') {
      match('*');
      Value next = factor();
      temp.kind = getKind(&temp, &next);
      temp.val *= next.val;
   }
   return temp;
}

Value factor(void) {
   Value temp;

   if (token == '(') {
      match('(');
      temp = expression();
      match(')');
   } else if (isdigit(token)) {
      double nextValue;
      ungetc(token, stdin);
      scanf("%lf", &nextValue);
      token = getchar();

      temp.kind = isFloat(nextValue) ? Float : Integer;
      temp.val = nextValue;
   } else
      error();

   return temp;
}

int main() {
   Value result;
   token = getchar();  // load token with first character for lookahead
   result = expression();
   if (token == '\n')  // check for end of line
      if (result.kind == Integer) {
         printf("Result = %ld\n", (long)result.val);
      } else {
         printf("Result = %lf\n", result.val);
      }
   else
      error();  // extraneous chars on line
   return 0;
}