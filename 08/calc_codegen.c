// P-code codegen example for the simple arithmetic calculator (p. 410)

#include <stdio.h>

#define CODESIZE 256

typedef enum {
   Plus,
   Assign
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

void genCode(SyntaxTree t) {
   // CODESIZE = max length of 1 line of P-code
   char codestr[CODESIZE];
   if (t != NULL) {
      switch (t->kind) {
         case OpKind:
            switch (t->op) {
               case Plus:
                  genCode(t->lchild);
                  genCode(t->rchild);
                  emitCode("adi");  // would be used to generate a single line of P-code, either in a data structure or an output file
                  break;
               case Assign:
                  sprintf(codestr, "%s %s", "lda", t->strval);
                  emitCode(codestr);
                  genCode(t->lchild);
                  emitCode("stn");
                  break;
               default:
                  emitCode("Error");
                  break;
            }
         case ConstKind:
            sprintf(codestr, "%s %s", "ldc", t->strval);
            emitCode(codestr);
            break;
         case IdKind:
            sprintf(codestr, "%s %s", "lod", t->strval);
            emitCode(codestr);
            break;
         default:
            emitCode("Error");
            break;
      }
   }
}