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

#include "tsiterator.h"
#include <stdlib.h>
#include <pthread.h>

/*
 * implementation for thread-safe generic iterator
 */

typedef struct tsit_data {
    long next;
    long size;
    void **elements;
    pthread_mutex_t *lock;
} TSItData;

static int tsit_hasNext(const TSIterator *it) {
    TSItData *itd = (TSItData *)it->self;

    return (itd->next < itd->size) ? 1 : 0;
}

static int tsit_next(const TSIterator *it, void **element) {
    TSItData *itd = (TSItData *)it->self;
    int status = 0;

    if (itd->next < itd->size) {
        *element = itd->elements[itd->next++];
        status = 1;
    }
    return status;
}

static void tsit_destroy(const TSIterator *it) {
    TSItData *itd = (TSItData *)it->self;

    free(itd->elements);
    pthread_mutex_unlock(itd->lock);
    free(itd);
    free((void *)it);
}

static TSIterator template = {
    NULL, tsit_hasNext, tsit_next, tsit_destroy
};

const TSIterator *TSIterator_create(pthread_mutex_t *lock, long size,
                                    void **elements) {
    TSIterator *it = (TSIterator *)malloc(sizeof(TSIterator));

    if (it != NULL) {
        TSItData *itd = (TSItData *)malloc(sizeof(TSItData));

        if (itd != NULL) {
            itd->next = 0L;
            itd->size = size;
            itd->elements = elements;
            itd->lock = lock;
            *it = template;
            it->self = itd;
        } else {
            free(it);
            it = NULL;
        }
    }
    return it;
}
