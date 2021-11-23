/** @file
  Implementacja stosu obsługującego elementy typów char, long, Poly oraz Mono.
*/

#include <stdlib.h>
#include "poly.h"
#include "stack.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Zwraca element 'zerowy' - przechowyjący znak NULL
 * @return element zerowy
 */
struct Element elemZero() {
    struct Element e = {.c = 0, .type = CHAR};
    return e;
}

struct Stack* makeStack (int init_size) {
    struct Stack *st = (struct Stack*)malloc(sizeof(struct Stack));
    if (st == NULL) {
        exit(1);
    }

    st->elements = malloc(init_size * sizeof (struct Element));
    if (st->elements == NULL) {
        exit(1);
    }

    st->capacity = init_size;
    st->current_size = 0;

    return st;
}

size_t size (struct Stack *st) {
    return st->current_size;
}

bool isEmpty (struct Stack *st) {
    return st->current_size == 0;
}

bool isFull (struct Stack *st) {
    return st->current_size == st->capacity;
}

/**
 * Funkcja zwiększająca pojemność stosu.
 * @param[in] st : stos
 */
void enlarge (struct Stack *st) {
    st->elements = realloc(st->elements, 2 * st->capacity * sizeof (struct Element));
    if (st->elements == NULL) {
        exit(1);
    }
    st->capacity *= 2;
}

void push (struct Stack *st, struct Element elem) {
    if (isFull(st)) {
        enlarge(st);
    }
    st->elements[st->current_size] = elem;
    st->current_size++;
}

struct Element *top(struct Stack *st) {
    if (isEmpty(st)) {
        return NULL;
    }
    return &(st->elements[st->current_size - 1]);
}

void pop(struct Stack *st) {
    if (isEmpty(st)) {
        return;
    }
    st->current_size--;
    st->elements[st->current_size] = elemZero();
}

void freeStack(struct Stack *st) {
    struct Element *e;
    while (!isEmpty(st)) {
        e = top(st);
        if (e->type == POLY) {
            PolyDestroy(&e->p);
        }
        else if (e->type == MONO) {
            MonoDestroy(&e->m);
        }
        pop(st);
    }
}

void destroyStack(struct Stack *st) {
    freeStack(st);
    free(st->elements);
    free(st);
}

struct Element elementOfPoly (Poly *p) {
    struct Element e = {.type = POLY, .p = *p};
    return e;
}

struct Element elementOfChar (char c) {
    struct Element e = {.type = CHAR, .c = c};
    return e;
}

struct Element elementOfMono (Mono *m) {
    struct Element e = {.type = MONO, .m = *m};
    return e;
}

struct Element elementOfNumb (long n) {
    struct Element e = {.type = NUMB, .n = n};
    return e;
}