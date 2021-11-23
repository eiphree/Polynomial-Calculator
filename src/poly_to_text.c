#include <stdio.h>
#include "poly.h"
#include <stddef.h>
#include "poly_to_text.h"

void printPoly(Poly *p) {
    if (PolyIsCoeff(p)) {
        poly_coeff_t c = p->coeff;
        printf("%ld", c);
    }
    else {
        for (size_t i = p->size - 1; i > 0; i--) {
            printMono(&p->arr[i]);
            printf("+");
        }
        printMono(&p->arr[0]);
    }
}

void printMono(Mono *m) {
    printf("(");
    printPoly(&m->p);
    printf(",");
    printf("%d", m->exp);
    printf(")");
}