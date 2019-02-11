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

#include "orderedset.h"
#include <stdlib.h>

/*
 * implementation for generic ordered set
 * implemented as an AVL tree
 */

typedef struct tnode {
    struct tnode *link[2];	/* 0 is left, 1 is right */
    void *element;
    int balance;		/* difference between heights of l and r subs */
} TNode;

typedef struct os_data {
    long size;
    TNode *root;
    int (*cmp)(void *,void *);
} OsData;

/*
 * structure needed for recursive population of array of pointers
 */
typedef struct popstruct {
    void **a;
    long len;
} PopStruct;

/*
 * routines used in rotations when rebalancing the tree
 */

/*
 * allocates a new node with the given element and NULL left and right links
 */
static TNode *newNode(void *element) {
    TNode *node = (TNode *)malloc(sizeof(TNode));

    if (node != NULL) {
        node->element = element;
        node->link[0] = node->link[1] = NULL;
        node->balance = 0;
    }
    return node;
}

static TNode *singleRotate(TNode *root, int dir) {
    TNode *save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;
    return save;
}

static TNode *doubleRotate(TNode *root, int dir) {
    TNode *save = root->link[!dir]->link[dir];

    root->link[!dir]->link[dir] = save->link[!dir];
    save->link[!dir] = root->link[!dir];
    root->link[!dir] = save;
    save = root->link[!dir];
    root->link[!dir] = save->link[dir];
    save->link[dir] = root;
    return save;
}

static void adjustBalance(TNode *root, int dir, int bal) {
    TNode *n = root->link[dir];
    TNode *nn = n->link[!dir];

    if (nn->balance == 0)
        root->balance = n->balance = 0;
    else if (nn->balance == bal) {
        root->balance = -bal;
        n->balance = 0;
    } else {	/* nn->balance == -bal */
        root->balance = 0;
        n->balance = bal;
    }
    nn->balance = 0;
}

static TNode *insertBalance(TNode *root, int dir) {
    TNode *n = root->link[dir];
    int bal = (dir == 0) ? -1 : +1;

    if (n->balance == bal) {
        root->balance = n->balance = 0;
        root = singleRotate(root, !dir);
    } else {	/* n->balance == -bal */
        adjustBalance(root, dir, bal);
        root = doubleRotate(root, !dir);
    }
    return root;
}

static TNode *insert(TNode *root, void *element, int *done,
                     int (*cmp)(void*,void*)) {
    if (root == NULL)
        root = newNode(element);
    else {
        int dir = ((*cmp)(root->element, element) < 0);

        root->link[dir] = insert(root->link[dir], element, done, cmp);
        if (! *done) {
            root->balance += (dir == 0) ? -1 : +1;
            if (root->balance == 0)
                *done = 1;
            else if (abs(root->balance) > 1) {
                root = insertBalance(root, dir);
                *done = 1;
            }
        }
    }
    return root;
}

static TNode *removeBalance(TNode *root, int dir, int *done) {
    TNode *n = root->link[!dir];
    int bal = (dir == 0) ? -1 : +1;

    if (n->balance == -bal) {
        root->balance = n->balance = 0;
        root = singleRotate(root, dir);
    } else if (n->balance == bal) {
        adjustBalance(root, !dir, -bal);
        root = doubleRotate(root, dir);
    } else {	/* n->balance == 0 */
        root->balance = -bal;
        n->balance = bal;
        root = singleRotate(root, dir);
        *done = 1;
    }
    return root;
}

static TNode *remove(TNode *root, void *element, int *done,
                     int (*cmp)(void*,void*), void (*uf)(void*)) {
    if (root != NULL) {
        int dir;

        if ((*cmp)(element, root->element) == 0) {
            if (root->link[0] == NULL || root->link[1] == NULL) {
                TNode *save;

                dir = (root->link[0] == NULL);
                save = root->link[dir];
                if (uf != NULL)
                    (*uf)(root->element);
                free(root);
                return save;
            } else {
                TNode *heir = root->link[0];

                while (heir->link[1] != NULL)
                    heir = heir->link[1];
                root->element = heir->element;
                element = heir->element;
            }
        }
        dir = ((*cmp)(root->element, element) < 0);
        root->link[dir] = remove(root->link[dir], element, done, cmp, uf);
        if (! *done) {
            root->balance += (dir != 0) ? -1 : +1;
            if (abs(root->balance) == 1)
                *done = 1;
            else if (abs(root->balance) > 1)
                root = removeBalance(root, dir, done);
        }
    }
    return root;
}

/*
 * finds element in the set; returns null if it cannot be found
 */
static TNode *find(void *element, TNode *tree, int (*cmp)(void*,void*)) {
    int result;

    if (tree == NULL)
        return NULL;
    result = (*cmp)(element, tree->element);
    if (result < 0)
        return find(element, tree->link[0], cmp);
    else if (result > 0)
        return find(element, tree->link[1], cmp);
    else
        return tree;
}

/*
 * infix traversal to populate array of pointers
 */
static void populate(PopStruct *ps, TNode *node) {
    if (node != NULL) {
        populate(ps, node->link[0]);
        (ps->a)[ps->len++] = node->element;
        populate(ps, node->link[1]);
    }
}

/*
 * postorder traversal, invoking freeFxn and then freeing node
 */
static void postpurge(TNode *leaf, void (*freeFxn)(void *element)) {
    if (leaf != NULL) {
        postpurge(leaf->link[0], freeFxn);
        postpurge(leaf->link[1], freeFxn);
        if (freeFxn != NULL)
            (*freeFxn)(leaf->element);
        free(leaf);
    }
}

static void os_destroy(const OrderedSet *os, void (*freeFxn)(void *element)) {
    OsData *osd = (OsData *)os->self;
    postpurge(osd->root, freeFxn);
    free(osd);
    free((void *)os);
}

static int os_add(const OrderedSet *os, void *element) {
    OsData *osd = (OsData *)os->self;
    int done = 0;

    if (find(element, osd->root, osd->cmp) != NULL)
        return 0;
    osd->root = insert(osd->root, element, &done, osd->cmp);
    osd->size++;
    return 1;
}

static TNode *Min(TNode *n1, TNode *n2, int (*cmp)(void*,void*)) {
    TNode *ans = n1;
    if (n1 == NULL)
        return n2;
    if (n2 == NULL)
        return n1;
    if ((*cmp)(n1->element, n2->element) > 0)
        ans = n2;
    return ans;
}

static TNode *Max(TNode *n1, TNode *n2, int (*cmp)(void*,void*)) {
    TNode *ans = n1;
    if (n1 == NULL)
        return n2;
    if (n2 == NULL)
        return n1;
    if ((*cmp)(n1->element, n2->element) < 0)
        ans = n2;
    return ans;
}

static int os_ceiling(const OrderedSet *os, void *element, void **ceiling) {
    OsData *osd = (OsData *)os->self;
    TNode *t = osd->root;
    TNode *current = NULL;

    while (t != NULL) {
        int cmp = (*osd->cmp)(element, t->element);
        if (cmp == 0) {
            current = t;
            break;
        } else if (cmp < 0) {
            current = Min(t, current, osd->cmp);
            t = t->link[0];
        } else {
            t = t->link[1];
        }
    }
    if (current == NULL)
        return 0;
    *ceiling = current->element;
    return 1;
}

static void os_clear(const OrderedSet *os, void (*freeFxn)(void *element)) {
    OsData *osd = (OsData *)os->self;
    postpurge(osd->root, freeFxn);
    osd->root = NULL;
    osd->size = 0L;
}

static int os_contains(const OrderedSet *os, void *element) {
    OsData *osd = (OsData *)os->self;
    return (find(element, osd->root, osd->cmp) != NULL);
}

/*
 * find node with minimum value in subtree
 */
TNode *findMin(TNode *tree) {
    if (tree != NULL)
        while (tree->link[0] != NULL)
            tree = tree->link[0];
    return tree;
}

static int os_first(const OrderedSet *os, void **element) {
    OsData *osd = (OsData *)os->self;
    TNode *current = findMin(osd->root);

    if (current == NULL)
        return 0;
    *element = current->element;
    return 1;
}

static int os_floor(const OrderedSet *os, void *element, void **floor) {
    OsData *osd = (OsData *)os->self;
    TNode *t = osd->root;
    TNode *current = NULL;

    while (t != NULL) {
        int cmp = (*osd->cmp)(element, t->element);
        if (cmp == 0) {
            current = t;
            break;
        } else if (cmp > 0) {
            current = Max(t, current, osd->cmp);
            t = t->link[1];
        } else {
            t = t->link[0];
        }
    }
    if (current == NULL)
        return 0;
    *floor = current->element;
    return 1;
}

static int os_higher(const OrderedSet *os, void *element, void **higher) {
    OsData *osd = (OsData *)os->self;
    TNode *t = osd->root;
    TNode *current = NULL;

    while (t != NULL) {
        int cmp = (*osd->cmp)(element, t->element);
        if (cmp < 0) {
            current = Min(t, current, osd->cmp);
            t = t->link[0];
        } else {
            t = t->link[1];
        }
    }
    if (current == NULL)
        return 0;
    *higher = current->element;
    return 1;
}

static int os_isEmpty(const OrderedSet *os) {
    OsData *osd = (OsData *)os->self;
    return (osd->size == 0L);
}

/*
 * find node with maximum value in subtree
 */
TNode *findMax(TNode *tree) {
    if (tree != NULL)
        while (tree->link[1] != NULL)
            tree = tree->link[1];
    return tree;
}

static int os_last(const OrderedSet *os, void **element) {
    OsData *osd = (OsData *)os->self;
    TNode *current = findMax(osd->root);

    if (current == NULL)
        return 0;
    *element = current->element;
    return 1;
}

static int os_lower(const OrderedSet *os, void *element, void **lower) {
    OsData *osd = (OsData *)os->self;
    TNode *t = osd->root;
    TNode *current = NULL;

    while (t != NULL) {
        int cmp = (*osd->cmp)(element, t->element);
        if (cmp > 0) {
            current = Max(t, current, osd->cmp);
            t = t->link[1];
        } else {
            t = t->link[0];
        }
    }
    if (current == NULL)
        return 0;
    *lower = current->element;
    return 1;
}

static int os_pollFirst(const OrderedSet *os, void **element) {
    OsData *osd = (OsData *)os->self;
    TNode *node = findMin(osd->root);
    int done = 0;

    if (node == NULL)
        return 0;
    *element = node->element;
    osd->root = remove(osd->root, node->element, &done, osd->cmp, NULL);
    return 1;
}

static int os_pollLast(const OrderedSet *os, void **element) {
    OsData *osd = (OsData *)os->self;
    TNode *node = findMax(osd->root);
    int done = 0;

    if (node == NULL)
        return 0;
    *element = node->element;
    osd->root = remove(osd->root, node->element, &done, osd->cmp, NULL);
    return 1;
}

static int os_remove(const OrderedSet *os, void *element, void (*freeFxn)(void *element)) {
    OsData *osd = (OsData *)os->self;
    int done = 0;

    if (find(element, osd->root, osd->cmp) == NULL)
        return 0;
    osd->root = remove(osd->root, element, &done, osd->cmp, freeFxn);
    osd->size--;
    return 1;
}

static long os_size(const OrderedSet *os) {
    OsData *osd = (OsData *)os->self;
    return osd->size;
}

/*
 * generates an array of void * pointers on the heap and copies
 * tree elements into the array
 *
 * returns pointer to array or NULL if malloc failure
 */
static void **genArray(OsData *osd) {
    void **tmp = NULL;
    PopStruct ps;
    if (osd->size > 0L) {
        size_t nbytes = osd->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            ps.a = tmp;
            ps.len = 0;
            populate(&ps, osd->root);
        }
    }
    return tmp;
}

static void **os_toArray(const OrderedSet *os, long *len) {
    OsData *osd = (OsData *)os->self;
    void **array = genArray(osd);

    if (array != NULL)
        *len = osd->size;
    return array;
}

static const Iterator *os_itCreate(const OrderedSet *os) {
    OsData *osd = (OsData *)os->self;
    const Iterator *it = NULL;
    void **tmp = genArray(osd);

    if (tmp != NULL) {
        it = Iterator_create(osd->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static OrderedSet template = {
    NULL, os_destroy, os_add, os_ceiling, os_clear, os_contains,
    os_first, os_floor, os_higher, os_isEmpty, os_last, os_lower,
    os_pollFirst, os_pollLast, os_remove, os_size, os_toArray, os_itCreate
};

const OrderedSet *OrderedSet_create(int (*cmpFunction)(void *, void *)) {
    OrderedSet *os = (OrderedSet *)malloc(sizeof(OrderedSet));

    if (os != NULL) {
        OsData *osd = (OsData *)malloc(sizeof(OsData));
        if (osd != NULL) {
            osd->size = 0L;
            osd->root = NULL;
            osd->cmp = cmpFunction;
            *os = template;
            os->self = osd;
        } else {
            free(os);
            os = NULL;
        }
    }
    return os;
}
