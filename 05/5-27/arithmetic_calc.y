%{
#include <stdio.h>
#include <ctype.h>

int yylex();
int yyerror(char *s);

%}
%%

command : exp   { printf("%d\n", $1); }
      ; /* allows printing of the result */

exp   : exp '+' term {$$ = $1 + $3;}
      | exp '-' term {$$ = $1 - $3;}
      | term   {$$ = $1;}
      ;

term  : term '*' factor {$$ = $1 * $3;}
      | factor {$$ = $1;}
      ;

factor   : number      {$$ = $1;}
         | '(' exp ')' {$$ = $2;}
         ;

number  : number digit { $$ = ($1 * 10) + $2; }
        | digit { $$ = $1; }
        ;

digit  : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';

%%

int main() {
  return yyparse();
}

int yylex(void) {
  int c;
  // eliminate blanks
  while ((c = getchar()) == ' ');
  if (isdigit(c)) {
    // found below online, is the idea that it's more memory efficient?
    // *((int*)&yylval) = c - '0';
    yylval = c - '0';
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