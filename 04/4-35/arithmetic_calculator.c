// AK: exercise 4.35: Device a syntax tree structure for the grammar of the previous exercisse
//     and write a recursive-descent parser for it that returns a syntax tree

/* From page 148: Simple intger arithmetic calculator according to EBNF

   <lexp> -> <number> | ( <op> <lexp-seq> )
   <op> -> + | - | *
   <lexp-seq>  <lexp> { <lexp> }  // EBNF

   // Last production in BNF
   <lexp-seq> <lexp-seq> <lexp> | <lexp>

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
   struct streenode *sibling;
   struct streenode *child;
   int val;
} STreeNode;

typedef STreeNode *SyntaxTree;

/* function prototypes for recursive calls */
SyntaxTree lexp(void);
SyntaxTree lexpSeq();

SyntaxTree newConstNode(int value);
SyntaxTree newOpNode(OpKind opKind);

void error(void) {
   fprintf(stderr, "Error - last token: %c\n", token);
   exit(1);
}

int isWhitespace(int c) {
   // Intentially omitted newline here since that is what we check for in main() to insure input is fine
   return c == ' ' || c == '\t';
}

int getNextChar(void) {
   int nextChar = getchar();
   while (isWhitespace(nextChar)) {
      nextChar = getchar();
   }
   return nextChar;
}

void match(char expectedToken) {
   if (token == expectedToken)
      token = getNextChar();
   else {
      error();
   }
}

OpKind tokenToOpKind(char token) {
   switch (token) {
      case '+':
         return Plus;
      case '-':
         return Minus;
      case '*':
         return Times;
      default:
         return -1;
   }
}

SyntaxTree lexp(void) {
   SyntaxTree temp;
   if (isdigit(token)) {
      int value;
      ungetc(token, stdin);
      scanf("%d", &value);
      token = getNextChar();
      temp = newConstNode(value);
   } else if (token == '(') {
      match('(');
      OpKind kind = tokenToOpKind(token);  // need to do this here because calling with token after match results in call to hit default case (since token is modified by match)
      switch (token) {
         case '+':
         case '-':
         case '*':
            match(token);
            temp = newOpNode(kind);
            break;
         default:
            error();
      }
      temp->child = lexpSeq();
      match(')');
   } else {
      error();
   }

   return temp;
}

SyntaxTree lexpSeq() {
   SyntaxTree temp = lexp();
   SyntaxTree head = temp;
   while (token != ')') {
      SyntaxTree next = lexp();
      head->sibling = next;
      head = next;
   }
   return temp;
}

SyntaxTree newConstNode(int value) {
   STreeNode *t = (STreeNode *)malloc(sizeof(STreeNode));
   if (t == NULL)
      printf("Out of memory error at token %c\n", token);
   else {
      t->child = NULL;
      t->sibling = NULL;
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
      t->child = NULL;
      t->sibling = NULL;
      t->kind = OpK;
      t->op = opKind;
   }
   return t;
}

int calculateValue(SyntaxTree node) {
   if (node->kind == OpK) {
      OpKind opKind = node->op;
      int acc = calculateValue(node->child);
      SyntaxTree temp = node->child->sibling;
      while (temp) {
         int nextValue = calculateValue(temp);
         switch (opKind) {
            case Plus:
               acc += nextValue;
               break;
            case Minus:
               acc -= nextValue;
               break;
            case Times:
               acc *= nextValue;
               break;
         }
         temp = temp->sibling;
      }
      node->val = acc;
   }
   // constants already have values assigned so we don't handle that case
   return node->val;
}

int main() {
   SyntaxTree result;
   token = getNextChar();  // load token with first character for lookahead
   result = lexp();
   if (token == '\n')  // check for end of line
      printf("Result = %d\n", calculateValue(result));
   else
      error();  // extraneous chars on line
   return 0;
}