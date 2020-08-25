/*
 * 2-24. Write a Lex input file that will produce a program that counts
 * characters, words, and lines in a text file and reports the counts.
 */

%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

FILE *source;
FILE *out;

int charno = 0;
int wordno = 0;
int lineno = 0;

%}

digit       [0-9]
letter      [a-zA-Z]
word  ({digit}|{letter})+
newline     \n

%%
{word}          {
                  wordno++;
                  charno += strlen(yytext);
                }
{newline}       { 
                  lineno++; 
                  charno++;
                }
.               { charno++; }

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

  printf("Processing over\n");
  printf("\tCharacter count: %d\n", charno);
  printf("\tWord count: %d\n", wordno);
  printf("\tLine count: %d\n", lineno);

  fclose(source);
  fclose(out);
  return 0;
}

