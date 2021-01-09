%{
#include <stdio.h>
#include <ctype.h>

typedef enum {
   Plus,
   Minus,
   Times
} OpKind;

int yylex();
int yyerror(char *s);

char opStack[50];
char stackPos = 0;

%}

%token NUMBER

%%

command : lexp   { printf("%d\n", $1); }
      ; /* allows printing of the result */

lexp  : NUMBER {$$ = $1;}
      | '(' '+' { *(opStack + stackPos++) = '+'; } lexpSeq ')' {$$ = $4;}
      | '(' '-' { *(opStack + stackPos++) = '-'; } lexpSeq ')' {$$ = $4;}
      | '(' '*' { *(opStack + stackPos++) = '*'; } lexpSeq ')' {$$ = $4;}
      ;

lexpSeq  : lexpSeq lexp {
             char token = opStack[stackPos];
             switch(token) {
               case '+':
                 $$ = $1 + $2;
                 break;
               case '-':
                 $$ = $1 - $2;
                 break;
               case '*':
                 $$ = $1 * $2;
                 break;
             }
           }
         | lexp {
             char token = opStack[stackPos--];
             if (token == '-') {
               $$ = -$1;
             } else {
               $$ = $1;
             }
           }
      ;

%%

int main() {
  return yyparse();
}

int yylex(void) {
  int c;
  // eliminate blanks
  while ((c = getchar()) == ' ');
  if (isdigit(c)) {
    ungetc(c, stdin);
    scanf("%d", &yylval);
    return NUMBER;
  }
  // makes the parse stop
  if (c == '\n') return 0;
  return c;
}

// allows for printing of an error message
int yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
  return 0;
}