%{
#include <stdio.h>
#include <ctype.h>

int yylex();
int yyerror(char *s);

#define YYSTYPE double
%}

%token NUMBER

%%

command : exp   { printf("%lf\n", $1); }
      ; /* allows printing of the result */

exp   : exp '+' term {$$ = $1 + $3;}
      | exp '-' term {$$ = $1 - $3;}
      | term   {$$ = $1;}
      ;

term  : term '*' factor {$$ = $1 * $3;}
      | factor {$$ = $1;}
      ;

factor   : NUMBER      {$$ = $1;}
         | '(' exp ')' {$$ = $2;}
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
    scanf("%lf", &yylval);
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