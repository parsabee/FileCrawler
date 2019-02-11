#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

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
 * interface definition for generic linked list
 *
 * patterned roughly after Java 6 LinkedList generic class, with many
 * duplicate methods removed
 */

#include "iterator.h"			/* needed for factory method */

typedef struct linkedlist LinkedList;	/* forward reference */

/*
 * create a linked list
 *
 * returns a pointer to the linked list, or NULL if there are malloc() errors
 */
const LinkedList *LinkedList_create(void);

/*
 * now define struct linkedlist
 */
struct linkedlist {
/*
 * the private data of the linked list
 */
    void *self;

/*
 * destroys the linked list; for each element, if freeFxn != NULL, invokes
 * freeFxn on the element; then returns any list structure associated with
 * the element; finally, deletes any remaining structures associated with the
 * list
 */
    void (*destroy)(const LinkedList *ll, void (*freeFxn)(void *element));

/*
 * appends `element' to the end of the list
 *
 * returns 1 if successful, 0 if unsuccesful (malloc errors)
 */
    int (*add)(const LinkedList *ll, void *element);

/*
 * inserts `element' at the specified position in the list;
 * all elements from `index' upwards are shifted one position;
 * if current size is N, 0 <= index <= N must be true
 *
 * returns 1 if successful, 0 if unsuccessful (malloc errors)
 */
    int (*insert)(const LinkedList *ll, long i, void *element);

/*
 * inserts `element' at the beginning of the list
 * equivalent to insert(ll, 0, element);
 */
    int (*addFirst)(const LinkedList *ll, void *element);

/*
 * appends `element' at the end of the list
 * equivalent to add(ll, element);
 */
    int (*addLast)(const LinkedList *ll, void *element);

/*
 * clears the linked list; for each element, if freeFxn != NULL, invokes
 * freeFxn on the element; then returns any list structure associated with
 * the element
 *
 * upon return, the list is empty
 */
    void (*clear)(const LinkedList *ll, void (*freeFxn)(void *element));

/*
 * Retrieves, but does not remove, the element at the specified index
 *
 * return 1 if successful, 0 if not
 */
    int (*get)(const LinkedList *ll, long index, void **element);

/*
 * Retrieves, but does not remove, the first element
 *
 * return 1 if successful, 0 if not
 */
    int (*getFirst)(const LinkedList *ll, void **element);

/*
 * Retrieves, but does not remove, the last element
 *
 * return 1 if successful, 0 if not
 */
    int (*getLast)(const LinkedList *ll, void **element);

/*
 * Retrieves, and removes, the element at the specified index
 *
 * return 1 if successful, 0 if not
 */
    int (*remove)(const LinkedList *ll, long index, void **element);

/*
 * Retrieves, and removes, the first element
 *
 * return 1 if successful, 0 if not
 */
    int (*removeFirst)(const LinkedList *ll, void **element);

/*
 * Retrieves, and removes, the last element
 *
 * return 1 if successful, 0 if not
 */
    int (*removeLast)(const LinkedList *ll, void **element);

/*
 * Replaces the element at the specified index; the previous element
 * is returned in `*previous'
 *
 * return 1 if successful, 0 if not
 */
    int (*set)(const LinkedList *ll, long index, void *element, void **previous);

/*
 * returns the number of elements in the linked list
 */
    long (*size)(const LinkedList *ll);

/*
 * returns true if the linked list is empty, false if not
 */
    int (*isEmpty)(const LinkedList *ll);

/*
 * returns an array containing all of the elements of the linked list in
 * proper sequence (from first to last element); returns the length of the
 * list in `len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 */
    void **(*toArray)(const LinkedList *ll, long *len);

/*
 * creates an iterator for running through the linked list
 *
 * returns pointer to the Iterator or NULL
 */
    const Iterator *(*itCreate)(const LinkedList *ll);
};

#endif /* _LINKEDLIST_H_ */
