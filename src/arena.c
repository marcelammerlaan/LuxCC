/* based on: http://www.cs.rice.edu/~javaplt/312/2007/Readings/fastalloc.pdf */
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define MEMINCR     10
#define MULTIPLE    4

typedef struct Block Block;
struct Block {
    char *limit;
    char *avail;
    Block *next;
};

struct Arena {
    Block *first;
    Block *last;
};

static void *allocate(Arena *a, unsigned n);

/* create a new `size' bytes sized arena */
Arena *arena_new(unsigned size)
{
    Arena *a;
    unsigned n;

    a = malloc(sizeof(Arena));
    n = sizeof(Block)+size;
    a->first = malloc(n);
    a->first->limit = (char *)a->first+n;
    a->first->avail = (char *)a->first+sizeof(Block);
    a->first->next = NULL;
    a->last = a->first;

    return a;
}

/* return `n' bytes of storage */
void *arena_alloc(Arena *a, unsigned n)
{
    void *p;

    p = a->last->avail;
    if ((a->last->avail+=n) > a->last->limit)
        p = allocate(a, n);

    return p;
}

/* allocate `n' bytes from `a'; create a new block if necessary */
void *allocate(Arena *a, unsigned n)
{
    Block *ap;

    for (ap = a->last; ap->avail+n > ap->limit; a->last = ap) {
        if (ap->next != NULL) { /* move to next block */
            ap = ap->next;
            ap->avail = (char *)ap+sizeof(Block); /* reset */
        } else { /* allocate a new block */
            unsigned m;

            m = round_up(n, MULTIPLE)+MEMINCR*1024+sizeof(Block);
            if ((ap->next=malloc(m)) == NULL)
                return NULL;
            ap = ap->next;
            ap->limit = (char *)ap+m;
            ap->avail = (char *)ap+sizeof(Block);
            ap->next = NULL;
        }
    }
    ap->avail += n;
    return ap->avail-n;
}

void arena_reset(Arena *a)
{
    a->last = a->first;
    a->last->avail = (char *)a->last+sizeof(Block);
}

void arena_destroy(Arena *a)
{
    Block *p, *q;

    p = a->first;
    while (p != NULL) {
        q = p;
        p = p->next;
        free(q);
    }
    free(a);
}
