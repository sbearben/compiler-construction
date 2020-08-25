/*
 * 2-25. The Lex code of Example 2.23 (Section 2.62) can be shortened by using a global flag
 * inComment to distinguish behaviour inside comments from behaviour elsewhere. Rewrite the
 * code of the example to do this.
 */

%option noyywrap

%{
#include <stdio.h>
#include <ctype.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

FILE *source;
FILE *out;

int inBlockComment = FALSE;

%}
%%

[A-Z]           { 
                  fputc(inBlockComment ? yytext[0] : tolower(yytext[0]), yyout); 
                }
"/*"            { 
                  ECHO;
                  inBlockComment = TRUE;
                }
"*/"            { 
                  ECHO;
                  inBlockComment = FALSE;
                }
"//"            { char c;
                  ECHO;
                  while ((c = input()) != '\n')
                    fputc(c, yyout);
                  fputc(c, yyout);
                }
%%

int main(int argc, char *argv[])
{
  char *filename = argv[1]; /* source code file name */
   if (argc != 2) {
      fprintf(stderr, "usage: %s <filename>\n", argv[0]);
      exit(1);
   }

   FILE *source = fopen(filename, "r");
   if (source == NULL) {
      fprintf(stderr, "File %s not found\n", filename);
      exit(1);
   }

   FILE *out = fopen("./out.c", "w");
   if (out == NULL) {
      fprintf(stderr, "Couldn't open file out.c for writing\n");
      exit(1);
   }

  yyin = source;
  yyout = out;

  yylex();

  fclose(source);
  fclose(out);
  return 0;
}

