/****************************************************/
/* File: analyze.h                                  */
/* Semantic analyzer interface for TINY compiler    */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "globals.h"

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 * - calls symbol table `st_insert` procedure as it encounters
 *   varaible identifiers in the tree
 * - after it is complete, then calls `printSymTab` to print out
 *   the stored information to the listing file 
 */
void buildSymtab(TreeNode *);

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 * - inserts data types into tree nodes as they are computed
 *   and reports any type checking errors to the listing file
 */
void typeCheck(TreeNode *);

#endif
