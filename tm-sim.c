#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/******* const *******/
#define IADDR_SIZE 1024 /* increase for large programs */
#define DADDR_SIZE 1024 /* increase for large programs */
#define NO_REGS 8
#define PC_REG 7

#define LINESIZE 121
#define WORDSIZE 20

/******* type *******/

typedef enum {
   opclRR, /* reg operands r,s,t */
   opclRM, /* reg r, mem d+s */
   opclRA  /* reg r, int d+s */
} OPCLASS;

typedef enum {
   /* RR instructions */
   opHALT,  /* RR   halt, operands must be zero */
   opIN,    /* RR   read into reg(r); s and t are ignored */
   opOUT,   /* RR   write from reg(r), s and t are ignored */
   opADD,   /* RR   reg(r) = reg(s) + reg(t) */
   opSUB,   /* RR   reg(r) = reg(s) - reg(t) */
   opMUL,   /* RR   reg(r) = reg(s) * reg(t) */
   opDIV,   /* RR   reg(r) = reg(s) / reg(t) */
   opRRLim, /* limit of RR opcodes */

   /* RM instructions */
   opLD,    /* RM    reg(r) = mem(d+reg(s)) */
   opST,    /* RM    mem(d+reg(s)) = reg(r) */
   opRMLim, /* Limit of RM opcodes */

   /* RA instructions */
   opLDA,   /* RA    reg(r) = d+reg(s) */
   opLDC,   /* RA    reg(r) = d; reg(s) is ignored */
   opJLT,   /* RA    if reg(r)<0 thewn reg(7) = d+reg(s) */
   opJLE,   /* RA    if reg(r)<=0 then reg(7) = d+reg(s) */
   opJGT,   /* RA    if reg(r)>0 then reg(7) = d+reg(s) */
   opJGE,   /* RA    if reg(r)>=0 then reg(7) = d+reg(s) */
   opJEQ,   /* RA    if reg(r)==0 then reg(7) = d+reg(s) */
   opJNE,   /* RA    if reg(r)!=0 then reg(7) = d+reg(s) */
   opRALLim /* Limit of RA opcodes */
} OPCODE;

typedef enum {
   srOKAY,
   srHALT,
   srIMEM_ERR,
   srDMEM_ERR,
   srZERODIVIDE
} STEPRESULT;

typedef struct {
   int iop;
   int iarg1;
   int iarg2;
   int iarg3;
} INSTRUCTION;

/******* vars *******/
int iloc = 0;
int dloc = 0;
int traceflag = FALSE;
int icountflag = FALSE;

INSTRUCTION iMem[IADDR_SIZE];
int dMem[DADDR_SIZE];
int reg[NO_REGS];

char *opCodeTab[] = {
    "HALT", "IN", "OUT", "ADD", "SUB", "MUL", "DIV", "????",
    /* RR opcodes */
    "LD", "ST", "????",
    /* RM opcodes */
    "LDA", "LDC", "JLT", "JLE", "JGT", "JGE", "JEQ", "JME", "????",
    /* RA opcodes */
};

char *stepResultTab[] = {
    "OK",
    "Halted",
    "Instruction Memory Fault",
    "Data Memory Fault",
    "Division by 0",
};

char pgmName[20];
FILE *pgm;

char in_Line[LINESIZE];
int lineLen;
int inCol;
int num;
char word[WORDSIZE];
char ch;
int done;

/**************************************************/
int opClass(int c) {
   if (c <= opRRLim)
      return opclRR;
   else if (c <= opRMLim)
      return opclRM;
   else
      return opclRA;
}

/**************************************************/
void writeInstructions(int loc) {
   printf("%5d: ", loc);

   if ((loc >= 0) && (loc < IADDR_SIZE)) {
      printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);

      switch (opClass(iMem[loc].iop)) {
         case opclRR:
            printf("%1d,%1d", iMem[loc].iarg2, iMem[loc].iarg3);
            break;
         case opclRM:
         case opclRA:
            printf("%3d(%1d)", iMem[loc].iarg2, iMem[loc].iarg3);
            break;
      }
   }
   printf("\n");
}

/**************************************************/
void getCh(void) {
   if (++inCol < lineLen)
      ch = in_Line[inCol];
   else
      ch = ' ';
}

/**************************************************/
int nonBlank(void) {
   while ((inCol < lineLen) && (in_Line[inCol] == ' '))
      inCol++;

   if (inCol < lineLen) {
      ch = in_Line[inCol];
      return TRUE;
   } else {
      ch = ' ';
      return FALSE;
   }
}

/**************************************************/
int getNum(void) {
   int sign;
   int term;
   int temp = FALSE;
   num = 0;
   do {
      sign = 1;
      while (nonBlank() && ((ch == '+') || (ch == '-'))) {
         temp = FALSE;
         if (ch == '-') sign = -sign;
         getCh();
      }
      term = 0;
      nonBlank();
      while (isdigit(ch)) {
         temp = TRUE;
         term = term * 10 + (ch - '0');
         getCh();
      }
      num = num + (term * sign);
   } while ((nonBlank()) && ((ch == '+') || (ch == '-')));
   return temp;
}

/**************************************************/
int getWord(void) {
   int temp = FALSE;
   int length = 0;
   if (nonBlank()) {
      while (isalnum(ch)) {
         if (length < WORDSIZE - 1) word[length++] = ch;
         getCh();
      }
      word[length] = '\0';
      temp = (length != 0);
   }
   return temp;
}

/**************************************************/
int skipCh(char c) {
   int temp = FALSE;
   if (nonBlank() && (ch == c)) {
      getCh();
      temp = TRUE;
   }
   return temp;
}

/**************************************************/
int atEOL(void) {
   return (!nonBlank());
}

/**************************************************/
int error(char *msg, int lineNo, int instNo) {
   printf("Line %d", lineNo);
   if (instNo >= 0) printf(" (Instruction %d)", instNo);
   printf("    %s\n", msg);
   return FALSE;
}

/**************************************************/
int readInstructions(void) {
   OPCODE op;
   int arg1, arg2, arg3;
   int loc, regNo, lineNo;

   for (regNo = 0; regNo < NO_REGS; regNo++)
      reg[regNo] = 0;

   dMem[0] = DADDR_SIZE - 1;

   for (loc = 1; loc < DADDR_SIZE; loc++)
      dMem[loc] = 0;
   for (loc = 0; loc < IADDR_SIZE; loc++) {
      iMem[loc].iop = opHALT;
      iMem[loc].iarg1 = 0;
      iMem[loc].iarg2 = 0;
      iMem[loc].iarg3 = 0;
   }
   lineNo = 0;

   while (!feof(pgm)) {
      fgets(in_Line, LINESIZE - 2, pgm);
      inCol = 0;
      lineNo++;
      lineLen = strlen(in_Line) - 1;

      if (in_Line[lineLen] == '\n')
         in_Line[lineLen] = '\0';
      else
         in_Line[++lineLen] = '\0';

      if ((nonBlank()) && (in_Line[inCol] != '*')) {
         if (!getNum())
            return error("Bad location", lineNo, -1);
         loc = num;
         if (loc > IADDR_SIZE)
            return error("Location too large", lineNo, loc);
         if (!skipCh(':'))
            return error("Missing colon", lineNo, loc);
         if (!getWord())
            return error("Missing opcode", lineNo, loc);

         op = opHALT;
         while ((op < opRALLim) && (strncmp(opCodeTab[op], word, 4) != 0))
            op++;
         if (strncmp(opCodeTab[op], word, 4) != 0)
            return error("Illegal opcode", lineNo, loc);

         switch (opClass(op)) {
            case opclRR:
               /****************************************/
               if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                  return error("Bad first register", lineNo, loc);
               arg1 = num;
               if (!skipCh(','))
                  return error("Mising comma", lineNo, loc);
               arg2 = num;
               if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                  return error("Bad third register", lineNo, loc);
               arg3 = num;
               break;

            case opclRM:
            case opclRA:
               /****************************************/
               if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                  return error("Bad first register", lineNo, loc);
               arg1 = num;
               if (!skipCh(','))
                  return error("Mising comma", lineNo, loc);
               if (!getNum())
                  return error("Bad displacement", lineNo, loc);
               arg2 = num;
               if (!skipCh('(') && !skipCh(','))
                  return error("Missing LParen", lineNo, loc);
               if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                  return error("Bad second register", lineNo, loc);
               arg3 = num;
               break;
         }
         iMem[loc].iop = op;
         iMem[loc].iarg1 = arg1;
         iMem[loc].iarg2 = arg2;
         iMem[loc].iarg3 = arg3;
      }
   }
   return TRUE;
}