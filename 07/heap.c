#include <stdio.h>

#define NULL 0
#define MEMSIZE 8096 /* change for different sizes */

typedef double Align;

typedef union header {
   struct {
      union header *next;
      unsigned usedsize;
      unsigned freesize;
   } s;
   Align a;
} Header;

static Header mem[MEMSIZE];
/**
 * always points to a block that has some free space
 * (usually the last space to be allocated or freed)
 */
static Header *memptr = NULL;

/**
 * The initial header that is allocated on the first call to malloc will never
 * be freed (p. 378). After first call there is one block in the list, and the
 * remainder of the code of malloc searches the list and returns a new block
 * from the first block that has enough free space (this is the `first fit`
 * algorithm)
 */
void *malloc(unsigned nbytes) {
   Header *p, *newp;
   unsigned nunits;
   nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
   if (memptr == NULL) {
      memptr->s.next = memptr = mem;
      memptr->s.usedsize = 1;
      memptr->s.freesize = MEMSIZE - 1;
   }
   for (p = memptr; (p->s.next != memptr) && (p->s.freesize < nunits); p = p->s.next)
      ;
   /* no block big enough */
   if (p->s.freesize < nunits) return NULL;

   newp = p + p->s.usedsize;
   newp->s.usedsize = nunits;
   newp->s.freesize = p->s.freesize - nunits;
   newp->s.next = p->s.next;
   p->s.freesize = 0;
   p->s.next = newp;
   memptr = newp;

   return (void *)(newp + 1);
}

/**
 * `free` first decrements the pointer passed by the user to find the header of
 * the block. It then searches the list for a pointer that is idential to this
 * one, thus protecting the list from becoming corrupted, and also allowing the
 * pointer to the previous block to be computed. When found the block is removed
 * from the list, and both its used and free space are added to the free space
 * of the previous block, thus automatically coalescing the free space. Note
 * that `memptr` is also set to point to the block containing the memory just freed
 */
void free(void *ap) {
   Header *bp, *p, *prev;
   bp = (Header *)ap - 1;
   for (prev = memptr, p = memptr->s.next; (p != bp) && (p != memptr); prev = p, p = p->s.next)
      ;
   /* corrupted list, do nothing */
   if (p != bp) return;

   prev->s.freesize += p->s.usedsize + p->s.freesize;
   prev->s.next = p->s.next;
   memptr = prev;
}