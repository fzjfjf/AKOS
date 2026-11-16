//
// Created by abdurrahman on 11/16/26.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <kstdlib.h>   /* kmalloc, krealloc, kfree (prilagodi ako je drugdje) */

#define VECTOR_INITIAL_CAP 8
#define VECTOR_NPOS ((size_t)-1)

#define DEFINE_VECTOR(type, name)                                        \
    typedef struct {                                                     \
        type *data;                                                      \
        size_t size;                                                     \
        size_t cap;                                                      \
    } name;                                                              \
                                                                         \
    /* ne alocira ništa, prvi push to odradi */                          \
    static inline void name##_init(name *v) {                            \
        v->data = NULL;                                                  \
        v->size = 0;                                                      \
        v->cap = 0;                                                      \
    }                                                                    \
                                                                         \
    /* oslobodi memoriju, vektor je poslije toga prazan i upotrebljiv */ \
    static inline void name##_free(name *v) {                            \
        if (v->data) kfree(v->data);                                     \
        v->data = NULL;                                                  \
        v->size = 0;                                                      \
        v->cap = 0;                                                      \
    }                                                                    \
                                                                         \
    /* osigura kapacitet za bar n elemenata, false ako alokacija pukne */\
    static inline bool name##_reserve(name *v, size_t n) {               \
        if (n <= v->cap) return true;                                    \
        if (n > ((size_t)-1) / sizeof(type)) return false;               \
        type *p = v->data                                                \
            ? (type *)krealloc(v->data, n * sizeof(type))                \
            : (type *)kmalloc(n * sizeof(type));                         \
        if (!p) return false; /* stari data ostaje valjan */             \
        v->data = p;                                                     \
        v->cap = n;                                                      \
        return true;                                                     \
    }                                                                    \
                                                                         \
    static inline bool name##_is_empty(const name *v) {                  \
        return v->size == 0;                                              \
    }                                                                    \
                                                                         \
    static inline void name##_clear(name *v) {                           \
        v->size = 0;                                                      \
    }                                                                    \
                                                                         \
    static inline bool name##_push(name *v, type x) {                    \
        if (v->size == v->cap) {                                          \
            size_t ncap = v->cap ? v->cap * 2 : VECTOR_INITIAL_CAP;      \
            if (ncap < v->cap) return false; /* overflow */              \
            if (!name##_reserve(v, ncap)) return false;                  \
        }                                                                \
        v->data[v->size++] = x;                                           \
        return true;                                                     \
    }                                                                    \
                                                                         \
    static inline bool name##_pop(name *v, type *out) {                  \
        if (v->size == 0) return false;                                   \
        v->size--;                                                        \
        if (out) *out = v->data[v->size];                                 \
        return true;                                                     \
    }                                                                    \
                                                                         \
    /* pointer na element, NULL ako je indeks van granica */             \
    static inline type *name##_atp(name *v, size_t i) {                  \
        return i < v->size ? &v->data[i] : NULL;                          \
    }                                                                    \
                                                                         \
    /* vrijednost na indeksu, BEZ provjere granica (kao v[i]) */         \
    static inline type name##_get(const name *v, size_t i) {             \
        return v->data[i];                                               \
    }                                                                    \
                                                                         \
    /* indeks prvog elementa jednakog x (==), ili VECTOR_NPOS */         \
    static inline size_t name##_ati(const name *v, type x) {             \
        for (size_t i = 0; i < v->size; i++)                              \
            if (v->data[i] == x) return i;                               \
        return VECTOR_NPOS;                                              \
    }                                                                    \
                                                                         \
    /* isto, s vlastitim komparatorom (0 = jednaki), za structove */     \
    static inline size_t name##_ati_by(const name *v, const type *x,     \
                           int (*eq)(const type *, const type *)) {      \
        for (size_t i = 0; i < v->size; i++)                              \
            if (eq(&v->data[i], x) == 0) return i;                       \
        return VECTOR_NPOS;                                              \
    }                                                                    \
                                                                         \
    static inline bool name##_insert(name *v, size_t idx, type x) {      \
        if (idx > v->size) return false;                                  \
        if (!name##_push(v, x)) return false; /* osigura mjesto */       \
        for (size_t i = v->size - 1; i > idx; i--)                        \
            v->data[i] = v->data[i - 1];                                 \
        v->data[idx] = x;                                                \
        return true;                                                     \
    }                                                                    \
                                                                         \
    static inline bool name##_remove(name *v, size_t idx, type *out) {   \
        if (idx >= v->size) return false;                                 \
        if (out) *out = v->data[idx];                                    \
        for (size_t i = idx; i + 1 < v->size; i++)                        \
            v->data[i] = v->data[i + 1];                                 \
        v->size--;                                                        \
        return true;                                                     \
    }

#endif
