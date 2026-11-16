//
// Created by abdurrahman on 11/15/26.
//
#pragma once

#include <stddef.h>

#define DEFINE_RING_BUFFER(type, name)                                   \
    typedef struct {                                                     \
        type *start;  /* početak bufera */                               \
        type *end;    /* jedan iza zadnjeg elementa */                   \
        type *head;   /* odakle čitamo (pop) */                          \
        type *tail;   /* gdje ide sljedeći append */                     \
        size_t count;                                                    \
    } name;                                                              \
                                                                         \
    static inline void name##_init(name *rb, type *buf, size_t cap) {    \
        rb->start = rb->head = rb->tail = buf;                           \
        rb->end = buf + cap;                                             \
        rb->count = 0;                                                   \
    }                                                                    \
                                                                         \
    static inline bool name##_append(name *rb, type element) {           \
        if (rb->count == (size_t)(rb->end - rb->start)) return false;    \
        *rb->tail++ = element;                                           \
        if (rb->tail == rb->end) rb->tail = rb->start;                   \
        rb->count++;                                                     \
        return true;                                                     \
    }                                                                    \
                                                                         \
    static inline bool name##_pop(name *rb, type *out) {                 \
        if (rb->count == 0) return false;                                \
        *out = *rb->head++;                                              \
        if (rb->head == rb->end) rb->head = rb->start;                   \
        rb->count--;                                                     \
        return true;                                                     \
    }                                                                    \
    static inline void name##_append_overwrite(name *rb, type element) { \
    bool full = rb->count == (size_t)(rb->end - rb->start);              \
    *rb->tail++ = element;                                               \
    if (rb->tail == rb->end) rb->tail = rb->start;                       \
    if (full)                                                            \
        rb->head = rb->tail; /* najstariji element je pregažen */        \
    else                                                                 \
        rb->count++;                                                     \
    }
