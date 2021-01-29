/**
 * Possible data-structure for quadruples (representation of three-address code)
 * See page 399/402
 */

#include <stdio.h>

typedef enum {
   rd,
   gt,
   if_f,
   asn,
   lab,
   mul,
   sub,
   eq,
   wri,
   hald,
   // others
} OpKind;

typedef enum {
   Empty,
   InstConst,
   String,
} AddrKind;

typedef struct {
   AddrKind kind;
   union {
      int val;
      char *name;
   } contents;
} Address;

typedef struct {
   OpKind op;
   Address addr1, addr2, addr3;
} Quad;
