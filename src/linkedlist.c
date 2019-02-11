/*
 * Copyright (c) 2017, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * implementation for generic linked list
 */

#include "linkedlist.h"
#include <stdlib.h>

#define SENTINEL(p) (&(p)->sentinel)
#define FL_INCREMENT 128	/* number of entries to add to free list */

typedef struct llnode {
    struct llnode *next;
    struct llnode *prev;
    void *element;
} LLNode;

typedef struct ll_data {
    long size;
    LLNode *freel;
    LLNode sentinel;
} LlData;

/*
 * local routines for maintaining free list of LLNode's
 */

static void putEntry(LlData *lld, LLNode *p) {
    p->element = NULL;
    p->next = lld->freel;
    lld->freel = p;
}

static LLNode *getEntry(LlData *lld) {
    LLNode *p;

    if ((p = lld->freel) == NULL) {
        long i;
        for (i = 0; i < FL_INCREMENT; i++) {
            p = (LLNode *)malloc(sizeof(LLNode));
            if (p == NULL)
                break;
            putEntry(lld, p);
        }
        p = lld->freel;
    }
    if (p != NULL)
        lld->freel = p->next;
    return p;
}

/*
 * traverses linked list, calling freeFxn on each element and freeing
 * node associated with element
 */
static void purge(LlData *lld, void (*freeFxn)(void *element)) {
    LLNode *cur = lld->sentinel.next;

    while (cur != SENTINEL(lld)) {
        LLNode *next;
        if (freeFxn != NULL)
            (*freeFxn)(cur->element);
        next = cur->next;
        putEntry(lld, cur);
        cur = next;
    }
}

static void ll_destroy(const LinkedList *ll, void (*freeFxn)(void *element)) {
    LlData *lld = (LlData *)ll->self;
    LLNode *p;
    purge(lld, freeFxn);
    p = lld->freel;
    while (p != NULL) {		/* return nodes on free list */
        LLNode *q;
        q = p->next;
        free(p);
        p = q;
    }
    free(lld);
    free((void *)ll);
}

/*
 * link `p' between `before' and `after'
 * must work correctly if `before' and `after' are the same node
 * (i.e. the sentinel)
 */
static void link(LLNode *before, LLNode *p, LLNode *after) {
    p->next = after;
    p->prev = before;
    after->prev = p;
    before->next = p;
}

static int ll_add(const LinkedList *ll, void *element) {
    return ll->addLast(ll, element);
}

static int ll_insert(const LinkedList *ll, long index, void *element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p;

    if (index <= lld->size && (p = getEntry(lld)) != NULL) {
        long n;
        LLNode *b;

        p->element = element;
        status = 1;
        for (n = 0, b = SENTINEL(lld); n < index; n++, b = b->next)
            ;
        link(b, p, b->next);
        lld->size++;
    }
    return status;
}

static int ll_addFirst(const LinkedList *ll, void *element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = getEntry(lld);

    if (p != NULL) {
        p->element = element;
        status = 1;
        link(SENTINEL(lld), p, SENTINEL(lld)->next);
        lld->size++;
    }
    return status;
}

static int ll_addLast(const LinkedList *ll, void *element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = getEntry(lld);

    if (p != NULL) {
        p->element = element;
        status = 1;
        link(SENTINEL(lld)->prev, p, SENTINEL(lld));
        lld->size++;
    }
    return status;
}

static void ll_clear(const LinkedList *ll, void (*freeFxn)(void *element)) {
    LlData *lld = (LlData *)ll->self;
    purge(lld, freeFxn);
    lld->size = 0L;
    lld->sentinel.next = SENTINEL(lld);
    lld->sentinel.prev = SENTINEL(lld);
}

static int ll_get(const LinkedList *ll, long index, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;

    if (index < lld->size) {
        long n;
        LLNode *p;

        status = 1;
        for (n = 0, p = SENTINEL(lld)->next; n < index; n++, p = p->next)
            ;
        *element = p->element;
    }
    return status;
}

static int ll_getFirst(const LinkedList *ll, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = SENTINEL(lld)->next;

    if (p != SENTINEL(lld)) {
        status = 1;
        *element = p->element;
    }
    return status;
}

static int ll_getLast(const LinkedList *ll, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = SENTINEL(lld)->prev;

    if (p != SENTINEL(lld)) {
        status = 1;
        *element = p->element;
    }
    return status;
}

/*
 * unlinks the LLNode from the doubly-linked list
 */
static void unlink(LLNode *p) {
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

static int ll_remove(const LinkedList *ll, long index, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;

    if (index < lld->size) {
        long n;
        LLNode *p;

        status = 1;
        for (n = 0, p = SENTINEL(lld)->next; n < index; n++, p = p->next)
            ;
        *element = p->element;
        unlink(p);
        putEntry(lld, p);
        lld->size--;
    }
    return status;
}

static int ll_removeFirst(const LinkedList *ll, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = SENTINEL(lld)->next;

    if (p != SENTINEL(lld)) {
        status = 1;
        *element = p->element;
        unlink(p);
        putEntry(lld, p);
        lld->size--;
    }
    return status;
}

static int ll_removeLast(const LinkedList *ll, void **element) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;
    LLNode *p = SENTINEL(lld)->prev;

    if (p != SENTINEL(lld)) {
        status = 1;
        *element = p->element;
        unlink(p);
        putEntry(lld, p);
        lld->size--;
    }
    return status;
}

static int ll_set(const LinkedList *ll, long index, void *element, void **previous) {
    LlData *lld = (LlData *)ll->self;
    int status = 0;

    if (index < lld->size) {
        long n;
        LLNode *p;

        status = 1;
        for (n = 0, p = SENTINEL(lld)->next; n < index; n++, p = p->next)
            ;
        *previous = p->element;
        p->element = element;
    }
    return status;
}

static long ll_size(const LinkedList *ll) {
    LlData *lld = (LlData *)ll->self;
    return lld->size;
}

static int ll_isEmpty(const LinkedList *ll) {
    LlData *lld = (LlData *)ll->self;
    return (lld->size == 0L);
}

/*
 * local function to generate array of element values on the heap
 *
 * returns pointer to array or NULL if malloc failure
 */
static void **genArray(LlData *lld) {
    void **tmp = NULL;
    if (lld->size > 0L) {
        size_t nbytes = lld->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i;
            LLNode *p;
            for (i = 0, p = SENTINEL(lld)->next; i < lld->size; i++, p = p->next)
                tmp[i] = p->element;
        }
    }
    return tmp;
}

static void **ll_toArray(const LinkedList *ll, long *len) {
    LlData *lld = (LlData *)ll->self;
    void **tmp = genArray(lld);

    if (tmp != NULL)
        *len = lld->size;
    return tmp;
}

static const Iterator *ll_itCreate(const LinkedList *ll) {
    LlData *lld = (LlData *)ll->self;
    const Iterator *it = NULL;
    void **tmp = genArray(lld);

    if (tmp != NULL) {
        it = Iterator_create(lld->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static LinkedList template = {
    NULL, ll_destroy, ll_add, ll_insert, ll_addFirst, ll_addLast, ll_clear,
    ll_get, ll_getFirst, ll_getLast, ll_remove, ll_removeFirst, ll_removeLast,
    ll_set, ll_size, ll_isEmpty, ll_toArray, ll_itCreate
};

const LinkedList *LinkedList_create(void) {
    LinkedList *ll = (LinkedList *)malloc(sizeof(LinkedList));

    if (ll != NULL) {
        LlData *lld = (LlData *)malloc(sizeof(LlData));
        if (lld != NULL) {
            lld->size = 0l;
            lld->freel = NULL;
            lld->sentinel.next = SENTINEL(lld);
            lld->sentinel.prev = SENTINEL(lld);
            *ll = template;
            ll->self = lld;
        } else {
            free(ll);
            ll = NULL;
        }
    }
    return ll;
}
