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
 * implementation for thread-safe generic linked list
 */

#include "tslinkedlist.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(ll) &((ll)->lock)

typedef struct tsll_data {
    const LinkedList *ll;
    pthread_mutex_t lock;	/* this is a recursive lock */
} TSLlData;

static void tsll_destroy(const TSLinkedList *tsll,
                         void (*freeFxn)(void *element)) {
    TSLlData *lld = (TSLlData *)tsll->self;

    pthread_mutex_lock(LOCK(lld));
    lld->ll->destroy(lld->ll, freeFxn);
    pthread_mutex_unlock(LOCK(lld));
    pthread_mutex_destroy(LOCK(lld));
    free(lld);
    free((void *)tsll);
}

static void tsll_lock(const TSLinkedList *tsll) {
    TSLlData *lld = (TSLlData *)tsll->self;

    pthread_mutex_lock(LOCK(lld));
}

static void tsll_unlock(const TSLinkedList *tsll) {
    TSLlData *lld = (TSLlData *)tsll->self;

    pthread_mutex_unlock(LOCK(lld));
}

static int tsll_add(const TSLinkedList *tsll, void *element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->add(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_insert(const TSLinkedList *tsll, long index, void *element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->insert(lld->ll, index, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_addFirst(const TSLinkedList *tsll, void *element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->addFirst(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_addLast(const TSLinkedList *tsll, void *element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->addLast(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static void tsll_clear(const TSLinkedList *tsll,
                       void (*freeFxn)(void *element)) {
    TSLlData *lld = (TSLlData *)tsll->self;

    pthread_mutex_lock(LOCK(lld));
    lld->ll->clear(lld->ll, freeFxn);
    pthread_mutex_unlock(LOCK(lld));
}

static int tsll_get(const TSLinkedList *tsll, long index, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->get(lld->ll, index, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_getFirst(const TSLinkedList *tsll, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->getFirst(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_getLast(const TSLinkedList *tsll, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->getLast(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_remove(const TSLinkedList *tsll, long index, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->remove(lld->ll, index, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_removeFirst(const TSLinkedList *tsll, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->removeFirst(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_removeLast(const TSLinkedList *tsll, void **element) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->removeLast(lld->ll, element);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static int tsll_set(const TSLinkedList *tsll, long index,
                    void *element, void **previous) {
    TSLlData *lld = (TSLlData *)tsll->self;
    int result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->set(lld->ll, index, element, previous);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static long tsll_size(const TSLinkedList *tsll) {
    TSLlData *lld = (TSLlData *)tsll->self;
    long result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->size(lld->ll);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static void **tsll_toArray(const TSLinkedList *tsll, long *len) {
    TSLlData *lld = (TSLlData *)tsll->self;
    void **result;

    pthread_mutex_lock(LOCK(lld));
    result = lld->ll->toArray(lld->ll, len);
    pthread_mutex_unlock(LOCK(lld));
    return result;
}

static const TSIterator *tsll_itCreate(const TSLinkedList *tsll) {
    TSLlData *lld = (TSLlData *)tsll->self;
    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(lld));
    tmp = lld->ll->toArray(lld->ll, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(lld), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(lld));
    return it;
}

static TSLinkedList template = {
    NULL, tsll_destroy, tsll_lock, tsll_unlock, tsll_add, tsll_insert,
    tsll_addFirst, tsll_addLast, tsll_clear, tsll_get, tsll_getFirst,
    tsll_getLast, tsll_remove, tsll_removeFirst, tsll_removeLast,
    tsll_set, tsll_size, tsll_toArray, tsll_itCreate
};

const TSLinkedList *TSLinkedList_create(void) {
    TSLinkedList *tsll = (TSLinkedList *)malloc(sizeof(TSLinkedList));

    if (tsll != NULL) {
        TSLlData *lld = (TSLlData *)malloc(sizeof(TSLlData));

        if (lld != NULL) {
            lld->ll = LinkedList_create();
            if (lld->ll != NULL) {
                pthread_mutexattr_t ma;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(lld), &ma);
                pthread_mutexattr_destroy(&ma);
                *tsll = template;
                tsll->self = lld;
            } else {
                free(lld);
                free(tsll);
                tsll = NULL;
            }
        } else {
            free(tsll);
            tsll = NULL;
        }
    }
    return tsll;
}
