#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef enum {
   BasedNum,
   // Not including BaseChar since in practice seems this could be part of the BasedNum node
   // BaseChar,
   Num,
   Digit,
} NodeKind;

typedef enum {
   Decimal = 10,
   Octal = 8,
} Base;

typedef struct streenode {
   NodeKind kind;
   struct streenode *lchild, *rchild;
   Base base;
   int val;
   int error;
} TreeNode;

typedef TreeNode *SyntaxTree;

static void evalWithBase(SyntaxTree node) {
   switch (node->kind) {
      case BasedNum: {
         node->lchild->base = node->base;
         evalWithBase(node->lchild);
         node->val = node->lchild->val;
         break;
      }
      case Num: {
         node->lchild->base = node->base;
         evalWithBase(node->lchild);
         if (node->rchild != NULL) {
            node->rchild->base = node->base;
            evalWithBase(node->rchild);
            if (node->lchild->error == FALSE && node->rchild->error == FALSE) {
               node->val = (node->base * node->lchild->val) + node->rchild->val;
            } else {
               node->error = TRUE;
            }
         } else {
            node->val = node->lchild->val;
         }
         break;
      }
      case Digit: {
         if (node->base == Octal && (node->val == 8 || node->val == 9)) {
            node->error = TRUE;
         } else {
            // Do nothing since seems like val is fine in this case (textbook expects actual digit token to be a child of this node)
         }
      }
   }
}