// AK: exercise 4.29

/* From page 148: Simple intger arithmetic calculator according to EBNF

   <exp> -> <term> { <addop> <term> }
   <addop> -> + | -
   <term> -> <factor> { <mulop> <factor> }
   <mulop> -> *
   <factor> -> ( <exp> ) | Number

   ** AK: from Part c) and d) of the exercise, the grammar becomes:

   <exp> -> <term> { <addop> <term> }
   <addop> -> + | -
   <term> -> <unary-minus> { <mulop> <unary-minus> }
   <mulop> -> *
   <unary-minus> -> <-> <unary-minus> | <exponent>   // AK: this is from part d)
   <exponent> -> factor <powop> <exponent> | factor  // AK: right recursive so don't use EBNF (same with unary-minus)
   <powop> -> ^
   <factor> -> ( <exp> ) | number

   Inputs a line of text from stdin
   Outputs "Error" or the result.
*/

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

char token;  // global token variable

// AK: NOTE - had to rename `exp` to `expression` because of namespace clashing with math.h (it also has an `exp` method)

/* function prototypes for recursive calls */
long expression(void);
long term(void);
long unaryMinus(void);
long exponent(void);
long factor(void);

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

long expression(void) {
   long temp = term();
   while ((token == '+') || (token == '-')) {
      switch (token) {
         case '+':
            match('+');
            temp += term();
            break;
         case '-':
            match('-');
            temp -= term();
            break;
      }
   }
   return temp;
}

long term(void) {
   long temp = unaryMinus();
   // Part a) and b) - add division and modulo
   while ((token == '*') || (token == '/')) {
      switch (token) {
         case '*':
            match('*');
            temp *= unaryMinus();
            break;
         case '/':
            match('/');
            temp /= unaryMinus();
            break;
         case '%':
            match('%');
            temp %= unaryMinus();
            break;
      }
   }
   return temp;
}

long unaryMinus(void) {
   if (token == '-') {
      match('-');
      return -1 * unaryMinus();
   }
   return exponent();
}

long exponent(void) {
   long temp = factor();
   if (token == '^') {
      match('^');
      temp = pow(temp, exponent());
   }
   return temp;
}

long factor(void) {
   long temp;
   if (token == '(') {
      match('(');
      temp = expression();
      match(')');
   } else if (isdigit(token)) {
      ungetc(token, stdin);
      scanf("%ld", &temp);
      token = getchar();
   } else
      error();
   return temp;
}

int main() {
   long result;
   token = getchar();  // load token with first character for lookahead
   result = expression();
   if (token == '\n')  // check for end of line
      printf("Result = %ld\n", result);
   else
      error();  // extraneous chars on line
   return 0;
}