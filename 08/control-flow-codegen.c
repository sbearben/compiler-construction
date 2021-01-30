// Sample code generation techniques for if- and while-statements (p. 433)
// - for simplicity the grammar uses the token 'other' to represent statements
//   not included in the grammar.
// - assumes a `genLabel` procedure that returns label names in the sequence
//   "L1", "L2", "L3", and so on

#include <stdio.h>

#define CODESIZE 256

#define TRUE 1
#define FALSE 0

typedef enum {
   ExpKind,
   IfKind,
   WhileKind,
   BreakKind,
   OtherKind,
} NodeKind;

typedef struct streenode {
   NodeKind kind;
   struct streenode *child[3];
   int val;  // used with ExpKind
} STreeNode;

typedef STreeNode *SyntaxTree;

void genCode(SyntaxTree t, char *label) {
   // CODESIZE = max length of 1 line of P-code
   char codestr[CODESIZE];
   // Used to save label names for which there are jump and/or definitions still pending
   char *lab1, *lab2;
   if (t != NULL) {
      switch (t->kind) {
         case ExpKind:
            if (t->val == 0)
               emitCode("ldc false");
            else
               emitCode("ldc true");
            break;
         case IfKind:
            genCode(t->child[0], label);
            lab1 = genLabel();
            sprintf(codestr, "%s %s", "fjp", lab1);
            emitCode(codestr);
            genCode(t->child[1], label);
            if (t->child[2] != NULL) {
               lab2 = genLabel();
               sprintf(codestr, "%s %s", "ujp", lab2);
               emitCode(codestr);
            }
            sprintf(codestr, "%s %s", "lab", lab1);
            emitCode(codestr);
            if (t->child[2] != NULL) {
               genCode(t->child[2], label);
               sprintf(codestr, "%s %s", "lab", lab2);
               emitCode(codestr);
            }
            break;
         case WhileKind:
            lab1 = genLabel();
            sprintf(codestr, "%s %s", "lab", lab1);
            emitCode(codestr);
            genCode(t->child[0], label);
            lab2 = genLabel();
            sprintf(codestr, "%s %s", "fjp", lab2);
            emitCode(codestr);
            genCode(t->child[1], lab2);
            sprintf(codestr, "%s %s", "ujp", lab1);
            emitCode(codestr);
            sprintf(codestr, "%s %s", "lab", lab2);
            emitCode(codestr);
            break;
         case BreakKind:
            // Will always cause a jump out of the most closely nested while-statement.
            // Any break-statement encountered outside a while-statement will
            // generate a jump label to an empty label, which is an error.
            sprintf(codestr, "%s %s", "ujp", label);
            emitCode(codestr);
            break;
         case OtherKind:
            emitCode("Other");
            break;
         default:
            emitCode("Error");
            break;
      }
   }
}