// p. 440

#include <stdio.h>

#define CODESIZE 256

#define TRUE 1
#define FALSE 0

typedef enum {
   PrgK,
   FnK,
   ParamK,
   PlusK,
   CallK,
   ConstK,
   IdK,
} NodeKind;

typedef struct streenode {
   NodeKind kind;
   struct streenode *lchild, *rchild, *sibling;
   char *name;  // used with FnK, ParamK, CallK, IdK
   int val;     // used with ConstK
} STreeNode;

typedef STreeNode *SyntaxTree;

void genCode(SyntaxTree t) {
   // CODESIZE = max length of 1 line of P-code
   char codestr[CODESIZE];
   SyntaxTree p;
   if (t != NULL) {
      switch (t->kind) {
         case PrgK:
            p = t->lchild;
            while (p != NULL) {
               genCode(p);
               p = p->sibling;
            }
            genCode(t->rchild);
            break;
         case FnK:
            sprintf(codestr, "%s %s", "ent", t->name);
            emitCode(codestr);
            genCode(t->rchild);
            emitCode("ret");
            break;
         case ParamK:  // no actions
            break;
         case ConstK:
            sprintf(codestr, "%s %d", "ldc", t->val);
            emitCode(codestr);
            break;
         case PlusK:
            genCode(t->lchild);
            genCode(t->rchild);
            emitCode("adi");
            break;
         case IdK:
            sprintf(codestr, "%s %s", "lod", t->name);
            emitCode(codestr);
            break;
         case CallK:
            emitCode("mst");
            p = t->rchild;
            while (p != NULL) {
               genCode(p);
               p = p->sibling;
            }
            sprintf(codestr, "%s %s", "cup", t->name);
            emitCode(codestr);
         default:
            emitCode("Error");
            break;
      }
   }
}