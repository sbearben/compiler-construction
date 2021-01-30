// P-code codegen example with arrary references for the simple arithmetic calculator (p. 422)

#include <stdio.h>

#define CODESIZE 256

#define TRUE 1
#define FALSE 0

typedef enum {
   Plus,
   Assign,
   Subs,
} Optype;

typedef enum {
   OpKind,
   ConstKind,
   IdKind
} NodeKind;

typedef struct streenode {
   NodeKind kind;
   Optype op;  // used with OpKind
   struct streenode *lchild, *rchild;
   int val;       // used with ConstKind
   char *strval;  // used for identifiers and numbers
} STreeNode;

typedef STreeNode *SyntaxTree;

void genCode(SyntaxTree t, int isAddr) {
   // CODESIZE = max length of 1 line of P-code
   char codestr[CODESIZE];
   if (t != NULL) {
      switch (t->kind) {
         case OpKind:
            switch (t->op) {
               case Plus:
                  if (isAddr)
                     emitCode("Error");
                  else {
                     genCode(t->lchild, FALSE);
                     genCode(t->rchild, FALSE);
                     emitCode("adi");  // would be used to generate a single line of P-code, either in a data structure or an output file
                  }
                  break;
               case Assign:
                  genCode(t->lchild, TRUE);
                  genCode(t->rchild, FALSE);
                  emitCode("stn");
                  break;
               case Subs:
                  sprintf(codestr, "%s %s", "lda", t->strval);
                  emitCode(codestr);
                  genCode(t->lchild, FALSE);
                  sprintf(codestr, "%s%s%s", "ixa elem_size(", t->strval, ")");
                  emitCode(codestr);
                  if (!isAddr) emitCode("ind 0");
                  break;
               default:
                  emitCode("Error");
                  break;
            }
         case ConstKind:
            if (isAddr)
               emitCode("Error");
            else {
               sprintf(codestr, "%s %s", "ldc", t->strval);
               emitCode(codestr);
            }
            break;
         case IdKind:
            if (isAddr) {
               sprintf(codestr, "%s %s", "lda", t->strval);
            } else {
               sprintf(codestr, "%s %s", "lod", t->strval);
            }
            emitCode(codestr);
            break;
         default:
            emitCode("Error");
            break;
      }
   }
}