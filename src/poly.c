/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych
*/

#include <stdbool.h>
#include <stddef.h>
#include "poly.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void PolyDestroy(Poly *p) {
    if (p->arr != NULL) {
        for (unsigned int i = 0; i < p->size; i++)
            PolyDestroy(&(p->arr[i].p));
        if (p->arr != NULL) {
            free(p->arr);
            p->arr = NULL;
        }
    }
}

Poly PolyClone(const Poly *p) {
    if (p->arr == NULL)
        return PolyFromCoeff(p->coeff);

    Mono * arr = malloc((p->size + 1) * sizeof(Mono));
    if (arr == NULL)
        exit(1);
    Poly q = {.size = p->size, .arr = arr};
    for (unsigned int i = 0; i < p->size; i++)
        q.arr[i] = MonoClone(&p->arr[i]);
    return q;
}

/**
 * Sprawdza, czy wielomian jest rekurencyjnym wielomianem zerowym.
 * Zwraca true jeśli @f$p = rx_i^0@f$ gdzie @f$r@f$ jest rekurencyjnym
 * wielomianem zerowym lub @f$p = 0@f$
 * @param[in] p : wielomian
 * @return Czy wielomian jest rekurencyjnym wielomianem zerowym?
 */
bool PolyIsZeroRec(Poly *p) {
    if (PolyIsZero(p))
        return true;
    if (PolyIsCoeff(p))
        return false;
    if (p->size == 1 && p->arr[0].exp == 0)
        return PolyIsZeroRec(&p->arr[0].p);
    return false;
}

/**
 * Sprawdza, czy wielomian jest rekurencyjnym wielomianem stałym (różnym od 0).
 * Zwraca true jeśli @f$p = rx_i^0@f$ gdzie @f$r@f$ jest rekurencyjnym
 * wielomianem stałym lub @f$p = c@f$ gdzie c to stała
 * @param[in] p : wielomian
 * @return wartość wspołczynnika jeśli wielomian jest stały, 0 wpp.
 */
poly_coeff_t PolyIsCoeffRec(Poly *p) {
    assert(!PolyIsZeroRec(p));
    if (p->size == 1 && p->arr[0].exp == 0) {
        if (PolyIsCoeff(&p->arr[0].p))
            return p->arr[0].p.coeff;
        return PolyIsCoeffRec(&p->arr[0].p);
    }
    return 0;
}

/**
 * Upraszcza wielomian.
 * Jeśli wielomian jest rekurencyjnym wielomianem zerowym
 * lub stałym, sprowadza go do najprostszej postaci
 * @param[in] p : wielomian
 * @return Wielomian w najprostszej możliwej postaci
 */
Poly PolySimplify(Poly *p) {
    if (PolyIsZeroRec(p)) {
        PolyDestroy(p);
        return PolyZero();
    }
    poly_coeff_t c = PolyIsCoeffRec(p);
    if (c != 0) {
        PolyDestroy(p);
        return PolyFromCoeff(c);
    }
    return *p;
}

/**
 * Dodaje wyraz wolny do wielomianu.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] c : współczynnik
 * @return @f$p + c@f$
 */
Poly PolyAddCoeff(const Poly *p, poly_coeff_t c) {
    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff + c);

    Poly q = PolyClone(p);
    if (c == 0)
        return q;

    unsigned long i = q.size - 1;
    poly_exp_t e = MonoGetExp(&q.arr[i]);
    if (e != 0) {
        q.arr[i+1] = (Mono) {.p = PolyFromCoeff(c), .exp = 0};
        q.size++;
    }
    else {
        Poly r = PolyAddCoeff(&q.arr[i].p, c);
        PolyDestroy(&q.arr[i].p);
        if (PolyIsZero(&r))
            q.size--;
        else
            q.arr[i].p = r;
    }
    return q;
}


Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsZero(p))
        return PolyClone(q);
    if (PolyIsZero(q))
        return PolyClone(p);
    if (PolyIsCoeff(p))
        return PolyAddCoeff(q, p->coeff);
    if (PolyIsCoeff(q))
        return PolyAddCoeff(p, q->coeff);

    Mono * arr = malloc((p->size + q->size) * sizeof(Mono));
    if (arr == NULL)
        exit(1);
    Poly res = {.size = p->size + q->size, .arr = arr};
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    int p_exp, q_exp;
    while (i < p->size && j < q->size) {
        p_exp = MonoGetExp(&p->arr[i]);
        q_exp = MonoGetExp(&q->arr[j]);
        if (p_exp > q_exp) {
            res.arr[k] = MonoClone(&p->arr[i]);
            i++;
            k++;
        }
        else if (q_exp > p_exp) {
            res.arr[k] = MonoClone(&q->arr[j]);
            j++;
            k++;
        }
        else {
           Poly sum = PolyAdd(&p->arr[i].p, &q->arr[j].p);
           if (!(PolyIsZero(&sum))) {
                res.arr[k] = (Mono) {.p = sum, .exp = p_exp};
                k++;
            }
                i++;
                j++;
        }
    }

    while (i < p->size) {
        res.arr[k] = MonoClone(&p->arr[i]);
        i++;
        k++;
    }
    while (j < q->size) {
        res.arr[k] = MonoClone(&q->arr[j]);
        j++;
        k++;
    }
    if (k == 0) {
        free(res.arr);
        return PolyZero();
    }
    if (k == 1 && res.arr[0].exp == 0 && PolyIsCoeff(&res.arr[0].p)) {
        Poly r_coeff = PolyFromCoeff(res.arr[0].p.coeff);
        free(res.arr);
        return r_coeff;
    }
    res.size = k;
    res.arr = realloc(res.arr, (k + 1) * sizeof (Mono));
    res = PolySimplify(&res);
    return res;
}

/**
 * Porównuje jednomiany pod względem wykładnika.
 * @param[in] mm : jednomian
 * @param[in] nn : jednomian
 * @return liczba @f$k@f$: @f$k < 0@f$ jeśli deg @f$mm@f$ > deg @f$nn@f$,
 * @f$k > 0@f$ jeśli deg @f$mm@f$ < deg @f$nn@f$,
 * @f$k = 0@f$ jeśli deg @f$mm@f$ = deg @f$nn@f$.
 */
static int MonosCompare(const void * mm, const void * nn) {
    Mono m = *(Mono*)mm;
    Mono n = *(Mono*)nn;
    return n.exp - m.exp;
}

/**
 * Sortuje tablicę jednomianów pod względem wykładników.
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 */
void MonosSort(Mono* monos, size_t count) {
    qsort(monos, count, sizeof(Mono), MonosCompare);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyZero();
    if (count == 1) {
        Mono * arr = malloc(2 * sizeof(Mono));
        if (arr == NULL)
            exit(1);
        arr[0] = monos[0];
        Poly p = (Poly) {.size = 1, .arr = arr};
        p = PolySimplify(&p);
        return p;
    }
    else {
        Mono * monos_cpy = malloc((count + 1) * sizeof(Mono));
        if (monos_cpy == NULL)
            exit(1);
        for (unsigned int i = 0; i < count; i++) {
            monos_cpy[i] = monos[i];
        }
        MonosSort(monos_cpy, count);
        monos_cpy[count] = (Mono) {.p = PolyZero(), .exp = -1};

        Mono * arr = malloc((count + 1) * sizeof(Mono));
        if (arr == NULL)
            exit(1);
        Poly res = (Poly) {.size = count, .arr = arr};
        int current_exp = monos_cpy[0].exp;
        Poly current_poly = monos_cpy[0].p;
        bool should_destroy = false;
        unsigned int i = 1;
        unsigned int k = 0;

        while (i <= count) {
            if (monos_cpy[i].exp != current_exp) {
                Poly q = PolyClone(&current_poly);
                if (should_destroy)
                    PolyDestroy(&current_poly);
                if (!(PolyIsZero(&q))) {
                    res.arr[k] = (Mono) {.p = q, .exp = current_exp};
                    k++;
                }
                current_poly = monos_cpy[i].p;
                current_exp = monos_cpy[i].exp;
                should_destroy = false;
                i++;
            }
            else {
                Poly q = PolyAdd(&current_poly, &monos_cpy[i].p);
                if (should_destroy)
                    PolyDestroy(&current_poly);
                should_destroy = true;
                current_poly = q;
                i++;
            }
        }

        for (unsigned int j = 0; j <= count; j++)
            PolyDestroy(&monos_cpy[j].p);
        free(monos_cpy);

        if (k == 0) {
            free(res.arr);
            return PolyZero();
        }

        if (k == 1 && res.arr[0].exp == 0 && PolyIsCoeff(&res.arr[0].p)) {
            Poly r_coeff = PolyFromCoeff(res.arr[0].p.coeff);
            free(res.arr);
            return r_coeff;
        }

        res.size = k;
        res.arr = realloc(res.arr, (k + 1) * sizeof (Mono));
        res = PolySimplify(&res);
        return res;
    }
}

/**
 * Mnoży wielomian przez współczynnik.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] c : współczynnik
 * @return @f$p * c@f$
 */
Poly PolyMulByCoeff(const Poly *p, poly_coeff_t c) {
    if (c == 0)
        return PolyZero();
    if (c == 1)
        return PolyClone(p);
    if (PolyIsCoeff(p))
        return PolyFromCoeff((p->coeff * c));

    Mono * arr = malloc((p->size + 1) * sizeof(Mono));
    if (arr == NULL)
        exit(1);
    Poly res = (Poly) {.size = p->size, .arr = arr};
    int k = 0;
    for (unsigned int i = 0; i < p->size; i++) {
        Poly q = PolyMulByCoeff(&p->arr[i].p, c);
        if (!PolyIsZero(&q)) {
            res.arr[k].p = q;
            res.arr[k].exp = p->arr[i].exp;
            k++;
        }
    }

    if (k == 0) {
        free(res.arr);
        return PolyZero();
    }
    if (k == 1 && res.arr[0].exp == 0 && PolyIsCoeff(&res.arr[0].p)) {
        Poly r_coeff = PolyFromCoeff(res.arr[0].p.coeff);
        free(res.arr);
        return r_coeff;
    }
    res.size = k;
    res.arr = realloc(res.arr, (k + 1) * sizeof (Mono));
    return res;
}


Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q))
        return PolyZero();
    if (PolyIsCoeff(p))
        return PolyMulByCoeff(q, p->coeff);
    if (PolyIsCoeff(q))
        return PolyMulByCoeff(p, q->coeff);

    Mono * arr = malloc((p->size * q->size + 1) * sizeof(Mono));
    if (arr == NULL)
        exit(1);
    int k = 0;

    for (unsigned int i = 0; i < p->size; i++) {
        for (unsigned int j = 0; j < q->size; j++) {
            Poly r = PolyMul(&p->arr[i].p, &q->arr[j].p);
            if (!PolyIsZero(&r)) {
                arr[k].p = r;
                arr[k].exp = p->arr[i].exp + q->arr[j].exp;
                k++;
            }
        }
    }
    Poly res = PolyAddMonos(k, arr);
    free(arr);
    res = PolySimplify(&res);
    return res;
}

Poly PolyNeg(const Poly *p) {
    return PolyMulByCoeff(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly neg_q = PolyNeg(q);
    Poly res = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return res;
}

/**
 * Zwraca maksimum dwóch współczynników.
 * @param[in] a : wartość współczynnika
 * @param[in] b : wartość współczynnika
 * @return @f$max(a, b)@f$
 */
poly_exp_t max(poly_exp_t a, poly_exp_t b) {
    if (a < b)
        return b;
    return a;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p))
        return -1;
    if (var_idx == 0) {
        if (PolyIsCoeff(p))
            return 0;
        return p->arr[0].exp;
    }
    if (PolyIsCoeff(p))
        return 0; //

    poly_exp_t deg = -1;
    for (unsigned int i = 0; i < p->size; i++)
        deg = max(deg, PolyDegBy(&p->arr[i].p, var_idx - 1));
    return deg;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;

    poly_exp_t deg = 0;
    for (unsigned int i = 0; i < p->size; i++) {
        deg = max(deg, PolyDeg(&p->arr[i].p) + p->arr[i].exp);
    }
    return deg;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) != PolyIsCoeff(q))
        return false;
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return p->coeff == q->coeff;
    if (p->size != q->size)
        return false;

    for (unsigned int i = 0; i < p->size; i++) {
        if (!PolyIsEq(&p->arr[i].p, &q->arr[i].p) || p->arr[i].exp != q->arr[i].exp)
            return false;
    }
    return true;
}

/**
 * Zwraca potęgę danej liczby.
 * @param[in] x : podstawa potęgi
 * @param[in] n : wykładnik potęgi
 * @return @f$x^n@f$
 */
poly_coeff_t Expo(poly_coeff_t x, poly_exp_t n) {
    if (n == 0)
        return 1;
    poly_coeff_t t = Expo(x * x, n / 2);
    if (n % 2 == 1)
        t *= x;
    return t;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        Poly q = PolyClone(p);
        return q;
    }
    poly_coeff_t c = Expo(x, p->arr[0].exp);
    Poly q = PolyMulByCoeff(&p->arr[0].p, c);
    Poly s;
    Poly r;
    for (unsigned int i = 1; i < p->size; i++) {
        c = Expo(x, p->arr[i].exp);
        r = PolyMulByCoeff(&p->arr[i].p, c);
        s = PolyAdd(&q, &r);
        PolyDestroy(&q);
        PolyDestroy(&r);
        q = s;
    }
    return q;
}

void PolyToString(Poly *p, int ind) {
    if (PolyIsZero(p))
        printf("0");
    else if (PolyIsCoeff(p))
        printf("%ld", p->coeff);
    else {
        unsigned long i = p->size;
        printf("( ");
        for (unsigned int j=0; j < i; j++) {
            Mono m = p->arr[j];
            PolyToString(&m.p, ind + 1);
            printf(" * x_%d^%d ", ind + 1, m.exp);
            if (j < i - 1)
                printf("+ ");
        }
        printf(")");
    }
}

void MonoToString(Mono *m, int ind) {
    printf(" ( ");
    PolyToString(&m->p, ind);
    printf(") * x_%d^%d ", ind, m->exp);
}