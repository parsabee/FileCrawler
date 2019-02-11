#ifndef _TSITERATOR_H_
#define _TSITERATOR_H_

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
 * interface definition for thread safe generic iterator
 */

#include <pthread.h>

typedef struct tsiterator TSIterator;	/* forward reference */

/*
 * creates a thread-safe iterator from the supplied arguments; it is for use
 * by the iterator create methods in thread-safe ADTs
 *
 * at the time TSIterator_create is called, the calling ADT must already hold
 * the lock associated with the ADT instance to guarantee that the array
 * reflects the contents of the ADT instance; this lock is held until the
 * application destroys the thread-safe iterator, at which point the lock
 * is released
 *
 * the iterator assumes responsibility for elements[] if create is succesful
 * i.e. destroy will free the array of pointers
 *
 * returns pointer to iterator if successful, NULL otherwise
 */
const TSIterator *TSIterator_create(pthread_mutex_t *lock, long size,
                                    void **elements);

/*
 * now define struct tsiterator
 */
struct tsiterator {
/*
 * the private data of the thread-safe iterator
 */
    void *self;

/*
 * returns 1/0 if the iterator has a next element
 */
    int (*hasNext)(const TSIterator *it);

/*
 * returns the next element from the iterator in `*element'
 *
 * returns 1 if successful, 0 if unsuccessful (no next element)
 */
    int (*next)(const TSIterator *it, void **element);

/*
 * destroys the iterator
 */
    void (*destroy)(const TSIterator *it);
};

#endif /* _TSITERATOR_H_ */
