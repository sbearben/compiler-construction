/*
 * 2-22. Write a Lex input file that will produce a program that capitalizes all comments in a C program
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

%}

%%

"/*"            { char c;
                  int done = FALSE;
                  ECHO;
                  do {
                    while ((c = input()) != '*')
                      fputc(toupper(c), yyout);
                    fputc(c, yyout);
                    c = input();
                    if (c == '/')
                      done = TRUE;
                    fputc(toupper(c), yyout);
                  } while (done == FALSE);
                  
                }
"//"            { char c;
                  ECHO;
                  while ((c = input()) != '\n')
                    fputc(toupper(c), yyout);
                  fputc(c, yyout);;
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

