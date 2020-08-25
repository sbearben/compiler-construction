/*
 * 2-26. Add nested C comments to the Lex code of Example 2.23.
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
int blockCommentNestLevel = 0;

%}
%%

[A-Z]           { 
                  fputc(inBlockComment ? yytext[0] : tolower(yytext[0]), yyout); 
                }
"/*"            { 
                  ECHO;
                  blockCommentNestLevel++;
                  inBlockComment = TRUE;
                  
                }
"*/"            { 
                  ECHO;
                  blockCommentNestLevel -= blockCommentNestLevel > 0 ? 1 : 0;
                  inBlockComment = blockCommentNestLevel ? TRUE : FALSE;
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

