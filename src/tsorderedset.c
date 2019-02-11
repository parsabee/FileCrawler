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

#include "tsorderedset.h"
#include "orderedset.h"
#include <stdlib.h>
#include <pthread.h>

/*
 * implementation for generic thread-safe treeset implementation
 */

#define LOCK(ts) &((ts)->lock)

typedef struct tsos_data {
    const OrderedSet *os;
    pthread_mutex_t lock;
} TSOsData;

static void tsos_destroy(const TSOrderedSet *os,
                         void (*freeFxn)(void *element)) {
    TSOsData *osd = (TSOsData *)os->self;

    pthread_mutex_lock(LOCK(osd));
    osd->os->destroy(osd->os, freeFxn);
    pthread_mutex_unlock(LOCK(osd));
    pthread_mutex_destroy(LOCK(osd));
    free(osd);
    free((void *)os);
}

static void tsos_lock(const TSOrderedSet *os) {
    TSOsData *osd = (TSOsData *)os->self;

    pthread_mutex_lock(LOCK(osd));
}

static void tsos_unlock(const TSOrderedSet *os) {
    TSOsData *osd = (TSOsData *)os->self;

    pthread_mutex_unlock(LOCK(osd));
}

static int tsos_add(const TSOrderedSet *os, void *element) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->add(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_ceiling(const TSOrderedSet *os, void *element,
                        void **ceiling) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->ceiling(osd->os, element, ceiling);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static void tsos_clear(const TSOrderedSet *os,
                       void (*freeFxn)(void *element)) {
    TSOsData *osd = (TSOsData *)os->self;

    pthread_mutex_lock(LOCK(osd));
    osd->os->clear(osd->os, freeFxn);
    pthread_mutex_unlock(LOCK(osd));
}

static int tsos_contains(const TSOrderedSet *os, void *element) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->contains(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_first(const TSOrderedSet *os, void **element) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->first(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_floor(const TSOrderedSet *os, void *element, void **floor) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->floor(osd->os, element, floor);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_higher(const TSOrderedSet *os, void *element, void **higher) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->higher(osd->os, element, higher);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_isEmpty(const TSOrderedSet *os) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->isEmpty(osd->os);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_last(const TSOrderedSet *os, void **element) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->last(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_lower(const TSOrderedSet *os, void *element, void **lower) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->lower(osd->os, element, lower);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_pollFirst(const TSOrderedSet *os, void **element) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->pollFirst(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_pollLast(const TSOrderedSet *os, void **element) {
    TSOsData *osd = (TSOsData *)os->self;

    int result;
    pthread_mutex_lock(LOCK(osd));
    result = osd->os->pollLast(osd->os, element);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static int tsos_remove(const TSOrderedSet *os, void *element,
                       void (*freeFxn)(void *element)) {
    TSOsData *osd = (TSOsData *)os->self;
    int result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->remove(osd->os, element, freeFxn);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static long tsos_size(const TSOrderedSet *os) {
    TSOsData *osd = (TSOsData *)os->self;
    long result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->size(osd->os);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static void **tsos_toArray(const TSOrderedSet *os, long *len) {
    TSOsData *osd = (TSOsData *)os->self;
    void **result;

    pthread_mutex_lock(LOCK(osd));
    result = osd->os->toArray(osd->os, len);
    pthread_mutex_unlock(LOCK(osd));
    return result;
}

static const TSIterator *tsos_itCreate(const TSOrderedSet *os) {
    TSOsData *osd = (TSOsData *)os->self;
    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(osd));
    tmp = osd->os->toArray(osd->os, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(osd), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(osd));
    return it;
}

static TSOrderedSet template = {
    NULL, tsos_destroy, tsos_lock, tsos_unlock, tsos_add, tsos_ceiling,
    tsos_clear, tsos_contains, tsos_first, tsos_floor, tsos_higher,
    tsos_isEmpty, tsos_last, tsos_lower, tsos_pollFirst, tsos_pollLast,
    tsos_remove, tsos_size, tsos_toArray, tsos_itCreate
};

const TSOrderedSet *TSOrderedSet_create(int (*cmpFunction)(void *, void *)) {
    TSOrderedSet *tsos = (TSOrderedSet *)malloc(sizeof(TSOrderedSet));

    if (tsos != NULL) {
        TSOsData *osd = (TSOsData *)malloc(sizeof(TSOsData));

        if (osd != NULL) {
            osd->os = OrderedSet_create(cmpFunction);
            if (osd->os != NULL) {
                pthread_mutexattr_t ma;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(osd), &ma);
                pthread_mutexattr_destroy(&ma);
                *tsos = template;
                tsos->self = osd;
            } else {
                free(osd);
                free(tsos);
                tsos = NULL;
            }
        } else {
            free(tsos);
            tsos = NULL;
        }
    }
    return tsos;
}
