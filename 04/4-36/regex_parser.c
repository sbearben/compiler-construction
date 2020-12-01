/* AK: exercise 4.36 -> trying to parse regex into an NFA ?

   <regexp> -> <concat> { "|" <concat> }
   <concat> -> <repetition> { <repetition> }
   <repetition> -> <factor> [ * ]
   <factor> ( <regexp> ) | Letter

   Inputs a line of text from stdin
   Outputs "Error" or the result.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

char token;  // global token variable

typedef enum {
   Choice,
   Concat,
   Repetition,
   Literal,
} NodeKind;

typedef struct streenode {
   NodeKind kind;
   struct streenode *child;   // For `choice` and `concat` this would be "lchild" - "repetition" just uses `child`
   struct streenode *rchild;  // Only needed for `choice` and `concat`
   char val;                  // Only used by `literal` (alphanumeric)
} STreeNode;

// Does a concat node more-so represent an object with a value and a `next`, as opposed to 2 child nodes?
// Or does every node have a `next`? Because we need to know if input is valid for next state given current state?

/**
 *  K, should probably be:
 *    - Choice: lchild, rchild
 *    - Concat: child, next (or sibling)
 *    - Repetition: child
 *    - Literal: value
 * 
 * Looking at this figure from Wikipedia: https://upload.wikimedia.org/wikipedia/commons/0/00/Thompson%27s_construction_algorithm_applied_to_regular_expression_for_binary_multiples_of_3.gif
 * - looks like I got the syntax tree construction right (the tree in top right of figures)
 * - EXCEPT, concatition sequences should maybe be 1 node with a bunch of siblings (not sure if this makes a difference though, besides minimizing depth and prob perf of the tree)
 */

typedef STreeNode *SyntaxTree;

/* function prototypes for recursive calls */
SyntaxTree regexp(void);
SyntaxTree concat(void);
SyntaxTree repetition(void);
SyntaxTree factor(void);

SyntaxTree newChoiceNode(void);
SyntaxTree newConcatNode(void);
SyntaxTree newRepetitionNode(void);
SyntaxTree newLiteralNode(char);
SyntaxTree newNode(NodeKind, char);

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

SyntaxTree regexp(void) {
   SyntaxTree temp, next;
   temp = concat();
   while (token == '|') {
      match('|');
      next = newChoiceNode();
      next->child = temp;
      next->rchild = concat();
      temp = next;
   }
   return temp;
}

SyntaxTree concat(void) {
   SyntaxTree temp, next;
   temp = repetition();
   while (isalnum(token) || token == '(') {
      // We don't match here since current token is part of first set of `factor`, so if we matched the conditions in factor will fail
      next = newConcatNode();
      next->child = temp;
      next->rchild = repetition();
      temp = next;
   }
   return temp;
}

SyntaxTree repetition(void) {
   SyntaxTree temp, next;
   temp = factor();
   if (token == '*') {
      match('*');
      next = newRepetitionNode();
      next->child = temp;
      temp = next;
   }
   return temp;
}

SyntaxTree factor(void) {
   SyntaxTree temp;
   if (token == '(') {
      match('(');
      temp = regexp();
      match(')');
   } else if (isalnum(token)) {
      char value = token;
      match(token);
      temp = newLiteralNode(value);
   } else {
      error();
   }
   return temp;
}

SyntaxTree newChoiceNode() {
   return newNode(Choice, -1);
}

SyntaxTree newConcatNode() {
   return newNode(Concat, -1);
}

SyntaxTree newRepetitionNode() {
   return newNode(Repetition, -1);
}

SyntaxTree newLiteralNode(char value) {
   return newNode(Literal, value);
}

SyntaxTree newNode(NodeKind kind, char value) {
   STreeNode *t = (STreeNode *)malloc(sizeof(STreeNode));
   if (t == NULL)
      printf("Out of memory error at token %c\n", token);
   else {
      t->child = NULL;
      t->rchild = NULL;
      t->kind = kind;
      t->val = value;
   }
   return t;
}

// Given node and input, can we advance to next state? (is input valid)
int canConsumeInput(char input, SyntaxTree node) {
   switch (node->kind) {
      case Choice:
         return canConsumeInput(input, node->child) || canConsumeInput(input, node->rchild);
      case Concat:
         return canConsumeInput(input, node->child);
      case Repetition:
         return canConsumeInput(input, node->child);
      case Literal:
         return input == node->val;
   }
}

int matchesWord(char *word, SyntaxTree node) {
   char *p = word;
   SyntaxTree head = node;
   int advance = 1;
   while (*p && advance && head) {
      advance = canConsumeInput(*p, head);
      head = head->rchild;
      p++;
   }
   return head == NULL && advance;
}

static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void) {
   int i;
   for (i = 0; i < indentno; i++)
      printf(" ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(SyntaxTree tree) {
   int i;
   INDENT;
   while (tree != NULL) {
      printSpaces();
      switch (tree->kind) {
         case Choice:
            printf("Choice\n");
            printTree(tree->child);
            printTree(tree->rchild);
            tree = NULL;
            break;
         case Concat:
            printf("Concat\n");
            printTree(tree->child);
            tree = tree->rchild;
            break;
         case Repetition:
            printf("Repetition\n");
            printTree(tree->child);
            tree = NULL;
            break;
         case Literal:
            printf("Literal: %c\n", tree->val);
            tree = NULL;
            break;
         default:
            printf("Unknown node kind\n");
            break;
      }
      UNINDENT;
   }
}

int main() {
   SyntaxTree result;
   token = getchar();  // load token with first character for lookahead
   result = regexp();
   if (token == '\n') {  // check for end of line
      // printf("Result = %d\n", result);
      // printf("We done\n");
      char input[256];
      printf("Enter string to match: ");
      fgets(input, 256, stdin);

      int match = matchesWord(input, result);
      if (match) {
         printf("Regex match\n");
      } else {
         printf("No match\n");
      }
   } else {
      error();  // extraneous chars on line
   }

   return 0;
}