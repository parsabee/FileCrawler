#ifndef _TSORDEREDSET_H_
#define _TSORDEREDSET_H_

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

#include "tsiterator.h"		/* needed for factory method */

/*
 * interface definition for thread-safe generic ordered set
 *
 * patterned roughly after Java 6 TreeSet generic class
 */

typedef struct tsorderedset TSOrderedSet;	/* forward reference */

/*
 * create an ordered set that is ordered using `cmpFunction' to compare two
 * elements
 *
 * returns a pointer to the set, or NULL if there are malloc() errors
 */
const TSOrderedSet *TSOrderedSet_create(int (*cmpFunction)(void *, void *));

/*
 * now define struct tsorderedset
 */
struct tsorderedset {
/*
 * the private data of the ordered set
 */
    void *self;

/*
 * destroys the ordered set; for each element, if freeFxn != NULL,
 * it is invoked on that element; the storage associated with
 * the ordered set is then returned to the heap
 */
    void (*destroy)(const TSOrderedSet *ts, void (*freeFxn)(void *));

/*
 * obtains the lock for exclusive access
 */
    void (*lock)(const TSOrderedSet *ts);

/*
 * returns the lock
 */
    void (*unlock)(const TSOrderedSet *ts);

/*
 * adds the specified element to the set if it is not already present
 *
 * returns 1 if the element was added, 0 if the element was already present
 */
    int (*add)(const TSOrderedSet *ts, void *element);

/*
 * returns the least element in the set greater than or equal to `element'
 *
 * returns 1 if found, or 0 if no such element
 */
    int (*ceiling)(const TSOrderedSet *ts, void *element, void **ceiling);

/*
 * clears all elements from the ordered set; for each element,
 * if freeFxn != NULL, it is invoked on that element;
 * any storage associated with that element in the ordered set is then
 * returned to the heap
 *
 * upon return, the ordered set will be empty
 */
    void (*clear)(const TSOrderedSet *ts, void (*freeFxn)(void *));

/*
 * returns 1 if the set contains the specified element, 0 if not
 */
    int (*contains)(const TSOrderedSet *ts, void *element);

/*
 * returns the first (lowest) element currently in the set
 *
 * returns 1 if non-empty, 0 if empty
 */
    int (*first)(const TSOrderedSet *ts, void **element);

/*
 * returns the greatest element in the set less than or equal to `element'
 *
 * returns 1 if found, or 0 if no such element
 */
    int (*floor)(const TSOrderedSet *ts, void *element, void **floor);

/*
 * returns the least element in the set strictly greater than `element'
 *
 * returns 1 if found, or 0 if no such element
 */
    int (*higher)(const TSOrderedSet *ts, void *element, void **higher);

/*
 * returns 1 if set is empty, 0 if it is not
 */
    int (*isEmpty)(const TSOrderedSet *ts);

/*
 * returns the last (highest) element currently in the set
 *
 * returns 1 if non-empty, 0 if empty
 */
    int (*last)(const TSOrderedSet *ts, void **element);

/*
 * returns the greatest element in the set strictly less than `element'
 *
 * returns 1 if found, or 0 if no such element
 */
    int (*lower)(const TSOrderedSet *ts, void *element, void **lower);

/*
 * retrieves and removes the first (lowest) element
 *
 * returns 0 if set was empty, 1 otherwise
 */
    int (*pollFirst)(const TSOrderedSet *ts, void **element);

/*
 * retrieves and removes the last (highest) element
 *
 * returns 0 if set was empty, 1 otherwise
 */
    int (*pollLast)(const TSOrderedSet *ts, void **element);

/*
 * removes the specified element from the set if present
 * if freeFxn != NULL, invokes it on the element before removing it
 *
 * returns 1 if successful, 0 if not present
 */
    int (*remove)(const TSOrderedSet *ts, void *element,
                  void (*freeFxn)(void *));

/*
 * returns the number of elements in the ordered set
 */
    long (*size)(const TSOrderedSet *ts);

/*
 * return the elements of the ordered set as an array of void * pointers
 * the order of elements in the array is the as determined by the ordered set's
 * compare function
 *
 * returns pointer to the array or NULL if error
 * returns number of elements in the array in len
 */
    void **(*toArray)(const TSOrderedSet *ts, long *len);

/*
 * create generic iterator to this ordered set
 *
 * returns pointer to the Iterator or NULL if failure
 */
    const TSIterator *(*itCreate)(const TSOrderedSet *ts);
};

#endif /* _TSORDEREDSET_H_ */
