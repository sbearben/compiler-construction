/*
 * 2-23. Write a Lex input file that will produce a program that capitalizes all reserved words outside of comments in a C program
 */

%option noyywrap

%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../helpers.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

FILE *source;
FILE *out;

%}

letter      [a-zA-Z]
identifier  {letter}+

%%

"/*"            { char c;
                  int done = FALSE;
                  ECHO;
                  do {
                    while ((c = input()) != '*')
                      fputc(c, yyout);
                    fputc(c, yyout);
                    c = input();
                    if (c == '/')
                      done = TRUE;
                    fputc(c, yyout);
                  } while (done == FALSE);
                  
                }
"//"            { char c;
                  ECHO;
                  while ((c = input()) != '\n')
                    fputc(c, yyout);
                  fputc(c, yyout);
                }
{identifier}    {
                  if (checkIfReservedWord(yytext)) {
                    for (int i = 0; i < strlen(yytext); i++)
                      putc(toupper(yytext[i]), yyout);
                  } else {
                    ECHO;
                  }
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

