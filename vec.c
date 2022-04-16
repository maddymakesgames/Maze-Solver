#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// we define this as a macro
// so that users can redifine it if wanted
#ifndef VEC_REALLOC_BY
#define VEC_REALLOC_BY 2
#endif

#define __VEC_IMPL

typedef struct VecInternal {
    void** data;
    int len;
    int capacity;
    int (*cmp)(void*, void*);
} VecInternal;

typedef VecInternal* Vec;

#include "vec.h"

Vec vec_new(int (*cmp)(void*, void*)) {
    Vec v = malloc(sizeof(VecInternal));

    v->data = malloc(sizeof(void*));
    v->capacity = 1;
    v->len = 0;
    v->cmp = cmp;

    return v;
}

void vec_free(Vec vec) {
    free(vec->data);
    free(vec);
}

void vec_insert(Vec vec, void* data) {
    if (vec->len + 1 >= vec->capacity) {
        vec->capacity *= VEC_REALLOC_BY;

        void* tmp = realloc(vec->data, sizeof(void*) * vec->capacity);
        if (tmp == NULL) {
            // Hopefully this should never realistically be taken
            // We log to make it easier to know whats going on
            printf("Failed to realloc Vec, exiting");
            exit(EXIT_FAILURE);
        }

        vec->data = tmp;
    }

    if (vec->cmp == NULL) {
        vec->data[vec->len] = data;
    } else {
        // find the index we have to insert the element at
        int i;
        for (i = 0; i < vec->len; i++) {
            int relationship = vec->cmp(data, vec->data[i]);
            if (relationship <= 0) {
                break;
            }
        }

        // shift every element back one
        for (int e = vec->len; e > i; e--) {
            // Indexing on e+1 is fine because the capacity check ensures this
            // is inbounds
            vec->data[e] = vec->data[e - 1];
        }

        // insert the element
        vec->data[i] = data;
    }

    ++vec->len;
}

bool vec_empty(Vec vec) { return vec->len == 0; }

int vec_len(Vec vec) { return vec->len; }

void* vec_remove(Vec vec) {
    assert(!vec_empty(vec));
    void* item = vec->data[0];

    // Shift all items forward and set the last item to null
    for (int i = 0; i < vec->len - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }
    vec->data[vec->len - 1] = NULL;

    vec->len--;

    // we shrink the allocation of the vec
    // we don't let capacity go below 4 so we don't have issues when reallocing
    if (vec->len < vec->capacity / VEC_REALLOC_BY && vec->capacity > 4) {
        vec->capacity /= 2;
        void* tmp = realloc(vec->data, sizeof(void*) * vec->capacity);
        if (tmp == NULL) {
            // Hopefully this should never realistically be taken
            // We log to make it easier to know whats going on
            printf("Failed to realloc Vec, exiting");
            exit(EXIT_FAILURE);
        }

        vec->data = tmp;
    }
    return item;
}