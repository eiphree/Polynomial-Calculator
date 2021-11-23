/** @file
  Implementacja funkcji czytających i wykonujących instrukcje kalkulatora wielomianów wielu zmiennych.
*/

#include "instructions_reader.h"
#include "stack.h"
#include "poly_to_text.h"
#include "poly_from_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

char separators[] = " ";
extern int errno;

/**
 * Czyta i wykonuje podaną instrukcję z parametrem (DEG_BY lub AT).
 * @param[in] str : instrukcja
 * @param[in] st : stos, na którym operuje kalkulator
 * @param[in] line_nr : numer obecnie obsługiwanego wiersza
 */
void takeInstrWithPar(char *str, Stack *st, long line_nr) {
    char *token = strtok(str, separators);
    char *after_number_char;
    if (strcmp(token, "DEG_BY") == 0) {
        char c = *(str + 7);
        if (!isNumberStart(c)) {
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", line_nr);
            return;
        }
        char *str_par = str + 7;

        errno = 0;
        unsigned long par = strtoul(str_par, &after_number_char, 10);
        if (after_number_char == str_par) {
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", line_nr);
            return;
        }

        if (*str_par == '-' && par != 0) { //-0
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", line_nr);
            return;
        }

        if (errno == ERANGE || (strcmp(after_number_char, "\n") != 0 && strcmp(after_number_char, "") != 0)) {
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", line_nr);
            return;
        }

        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        long deg = PolyDegBy(&p, par);
        printf("%ld\n", deg);
        return;
    }
    if (strcmp(str, "AT") == 0) {
        char c = *(str + 3);
        if (!isNumberStart(c)) {
            fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", line_nr);
            return;
        }

        char *str_par = str + 3;
        errno = 0;
        long par = strtol(str_par, &after_number_char, 10);
        if (after_number_char == str_par) {
            fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", line_nr);
            return;
        }

        if (errno == ERANGE || (strcmp(after_number_char, "\n") != 0 && strcmp(after_number_char, "") != 0)) {
            fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", line_nr);
            return;
        }
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        Poly q = PolyAt(&p, par);
        pop(st);
        PolyDestroy(&p);
        push(st, elementOfPoly(&q));
        return;
    }
    else {
        fprintf(stderr, "ERROR %ld WRONG COMMAND\n", line_nr);
    }
}


void takeInstruction(char *str, Stack *st, long line_nr) {
    if (strcmp(str, "ZERO\n") == 0 || strcmp(str, "ZERO") == 0) {
        Poly p = PolyZero();
        Element e = elementOfPoly(&p);
        push(st, e);
        return;
    }
    if (strcmp(str, "IS_COEFF\n") == 0 || strcmp(str, "IS_COEFF") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        bool b = PolyIsCoeff(&p);
        printf("%d\n", b);
        return;
    }
    if (strcmp(str, "IS_ZERO\n") == 0 || strcmp(str, "IS_ZERO") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        bool b = PolyIsZero(&p);
        printf("%d\n", b);
        return;
    }
    if (strcmp(str, "CLONE\n") == 0 || strcmp(str, "CLONE") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        Poly q = PolyClone(&p);
        Element e2 = elementOfPoly(&q);
        push(st, e2);
        return;
    }
    if (strcmp(str, "ADD\n") == 0 || strcmp(str, "ADD") == 0) {
        if (size(st) < 2) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element *e1 = top(st);
        Poly p1 = e1->p;
        pop(st);
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        e1 = top(st);
        Poly p2 = e1->p;
        pop(st);
        Poly p = PolyAdd(&p1, &p2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        push(st, elementOfPoly(&p));
        return;
    }
    if (strcmp(str, "MUL\n") == 0 || strcmp(str, "MUL") == 0) {
        if (size(st) < 2) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element *e1 = top(st);
        Poly p1 = e1->p;
        pop(st);
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        e1 = top(st);
        Poly p2 = e1->p;
        pop(st);
        Poly p = PolyMul(&p1, &p2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        push(st, elementOfPoly(&p));
        return;
    }
    if (strcmp(str, "NEG\n") == 0 || strcmp(str, "NEG") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        Poly q = PolyNeg(&p);
        PolyDestroy(&p);
        pop(st);
        push(st, elementOfPoly(&q));
        return;
    }
    if (strcmp(str, "SUB\n") == 0 || strcmp(str, "SUB") == 0) {
        if (size(st) < 2) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element *e1 = top(st);
        Poly p1 = e1->p;
        pop(st);
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        e1 = top(st);
        Poly p2 = e1->p;
        pop(st);
        Poly p = PolySub(&p1, &p2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        push(st, elementOfPoly(&p));
        return;
    }
    if (strcmp(str, "IS_EQ\n") == 0 || strcmp(str, "IS_EQ") == 0) {
        if (size(st) < 2) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element *e1 = top(st);
        assert(e1->type == POLY);
        Poly p1 = e1->p;
        pop(st);
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element *e2 = top(st);
        Poly p2 = e2->p;
        assert(e2->type == POLY);
        bool b = PolyIsEq(&p1, &p2);
        printf("%d\n", b);
        push(st, elementOfPoly(&p1));
        return;
    }
    if (strcmp(str, "DEG\n") == 0 || strcmp(str, "DEG") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        long deg = PolyDeg(&p);
        printf("%ld\n", deg);
        return;
    }
    if (strcmp(str, "PRINT\n") == 0 || strcmp(str, "PRINT") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        printPoly(&p);
        printf("\n");
        return;
    }
    if (strcmp(str, "POP\n") == 0 || strcmp(str, "POP") == 0) {
        if (isEmpty(st)) {
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", line_nr);
            return;
        }
        Element * e = top(st);
        assert (e->type == POLY);
        Poly p = e->p;
        PolyDestroy(&p);
        pop(st);
        return;
    }
    takeInstrWithPar(str, st, line_nr);
}