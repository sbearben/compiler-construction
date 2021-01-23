// AK: exercise 4.32: Rewrite the calculator so that it returns a syntax tree
//     according to the declarations of Secion 3.3.2 (p. 111)

/* From page 148: Simple intger arithmetic calculator according to EBNF

   <exp> -> <term> { <addop> <term> }
   <addop> -> + | -
   <term> -> <factor> { <mulop> <factor> }
   <mulop> -> *
   <factor> -> ( <exp> ) | Number

   Inputs a line of text from stdin
   Outputs "Error" or the result.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

char token;  // global token variable

typedef enum {
   Plus,
   Minus,
   Times
} OpKind;

typedef enum {
   OpK,
   ConstK
} ExpKind;

typedef struct streenode {
   ExpKind kind;
   OpKind op;
   struct streenode *lchild, *rchild;
   int val;
} STreeNode;

typedef STreeNode *SyntaxTree;

/* function prototypes for recursive calls */
SyntaxTree expression(void);
SyntaxTree term(void);
SyntaxTree factor(void);

SyntaxTree newConstNode(int value);
SyntaxTree newOpNode(OpKind opKind);

int calculateValue(SyntaxTree tree);

void error(void) {
   fprintf(stderr, "Error\n");
   exit(1);
}

void match(char expectedToken) {
   if (token == expectedToken)
      token = getchar();
   else
      error();
}

SyntaxTree expression(void) {
   SyntaxTree temp, next;
   temp = term();
   while ((token == '+') || (token == '-')) {
      next = newOpNode(token == '+' ? Plus : Minus);
      match(token);
      next->lchild = temp;
      next->rchild = term();

      temp = next;
   }
   return temp;
}

SyntaxTree term(void) {
   SyntaxTree temp, next;
   temp = factor();
   while (token == '*') {
      next = newOpNode(Times);
      match('*');
      next->lchild = temp;
      next->rchild = factor();

      temp = next;
   }
   return temp;
}

SyntaxTree factor(void) {
   SyntaxTree temp;
   if (token == '(') {
      match('(');
      temp = expression();
      match(')');
   } else if (isdigit(token)) {
      int value;
      ungetc(token, stdin);
      scanf("%d", &value);
      token = getchar();
      temp = newConstNode(value);
   } else
      error();
   return temp;
}

SyntaxTree newConstNode(int value) {
   STreeNode *t = (STreeNode *)malloc(sizeof(STreeNode));
   if (t == NULL)
      printf("Out of memory error at token %c\n", token);
   else {
      t->lchild = NULL;
      t->rchild = NULL;
      t->kind = ConstK;
      t->val = value;
   }
   return t;
}

SyntaxTree newOpNode(OpKind opKind) {
   STreeNode *t = (STreeNode *)malloc(sizeof(STreeNode));
   if (t == NULL)
      printf("Out of memory error at token %c\n", token);
   else {
      t->lchild = NULL;
      t->rchild = NULL;
      t->kind = OpK;
      t->op = opKind;
   }
   return t;
}

int calculateValue(SyntaxTree node) {
   if (node->kind == OpK) {
      int lhsvalue = calculateValue(node->lchild);
      int rhsValue = calculateValue(node->rchild);
      switch (node->op) {
         case Plus:
            node->val = lhsvalue + rhsValue;
            break;
         case Minus:
            node->val = lhsvalue - rhsValue;
            break;
         case Times:
            node->val = lhsvalue * rhsValue;
            break;
      }
   }
   // constants already have values assigned so we don't handle that case
   return node->val;
};

void printPostFixExp() {
}

int main() {
   SyntaxTree result;
   token = getchar();  // load token with first character for lookahead
   result = expression();
   if (token == '\n') {  // check for end of line
      printf("Result = %d\n", calculateValue(result));
      printPostFixExp();
   } else
      error();  // extraneous chars on line
   return 0;
}