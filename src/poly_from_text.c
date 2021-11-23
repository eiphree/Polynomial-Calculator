/** @file
  Implementacje funkcji obsługujących konwersję stringa na wielomian wielu zmiennych,
  stosujących odwrotną notację polską.
*/

#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include "poly.h"
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include "poly_from_text.h"

#define MIN_EXP_VALUE 0
#define MAX_EXP_VALUE 2147483647
#define COEFF 0
#define EXP 1

extern int errno;

bool isNumber (char c) {
    return (c >= '0' && c <= '9');
}

bool isNumberStart (char c) {
    return (isNumber(c) || c == '-');
}

/**
 * Sprawdza, czy znak jest dopuszczalny w zapisie wielomianu.
 * @param[in] c : znak
 * @return Czy znak jest dopuszczalny w zapisie wielomianu?
 */
bool isLegal (char c) {
    return (isNumberStart(c) || c == '(' || c == ')' || c == '+' || c == ',');
}

bool isLetter (char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

long toNumber (char **current_char, int number_type, bool *success) {
    assert(number_type == COEFF || number_type == EXP);
    char *after_number_char;
    long res = strtol(*current_char, &after_number_char, 10);
    if (after_number_char == *current_char) {
        *success = false;
        return -1;
    }

    if (number_type == EXP && (res < MIN_EXP_VALUE || res > MAX_EXP_VALUE)) {
        *success = false;
        return -1;
    }
    *current_char = after_number_char;
    return res;
}

/**
 * Wypisuje błąd parsowania wielomianu i usuwa z niego wszytskie elementy
 * @param[in] st : stos
 * @param[in] line_nr : numer obecnie przetwarzanego wiersza
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą (nie)powodzenie parsowania wielomianu
 */
void reportError(Stack *st, long line_nr, bool *succ) {
    fprintf(stderr, "ERROR %ld WRONG POLY\n", line_nr);
    freeStack(st);
    *succ = false;
}

/**
 * Sprawdza, czy element przechowuje podany znak.
 * @param[in] e : element
 * @param[in] c : znak
 * @return Czy znak element przechowuje podany znak?
 */
bool elementEqChar (Element *e, char c) {
    return (e->type == CHAR && e->c == c);
}

/**
 * Pobiera z wierzchu stosu określony znak.
 * @param[in] st : stos
 * @param[in] c : znak
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą czy udało się pobrać oczekiwany znak
 */
void takeChar (Stack *st, char c, bool *success) {
    Element *e = top(st);
    if (e == NULL || !elementEqChar(e, c)) {
        *success = false;
        return;
    }
    pop(st);
}

/**
 * Pobiera z wierzchu stosu wartości wykładnika wielomianu.
 * @param[in] st : stos
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą czy udało się pobrać wykładnik.
 */
int takeExp (Stack *st, bool *success) {
    Element *e = top(st);
    if (e == NULL || e->type != NUMB) {
        *success = false;
        return -1;
    }
    int exp = e->n;
    pop(st);
    return exp;
}

/**
 * Pobiera z wierzchu stosu jednomian.
 * @param[in] st : stos
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą czy udało się pobrać jednomian.
 */
Mono takeMono (Stack *st, bool *success) {
    Element *e = top(st);
    if (e == NULL || e->type != MONO) {
        *success = false;
        return (Mono) {.exp = -1, .p = PolyZero()};
    }
    Mono m = e->m;
    pop(st);
    return m;
}

/**
 * Sprawdza, czy element przechowuje znak '('.
 * @param[in] e : element
 * @return Czy znak element przechowuje podany znak '('?
 */
bool isOpenBracket(Element *e) {
    return elementEqChar(e, '(');
}

/**
 * Dodaje jednomiany ze stosu aż do napotkania znaku '(' lub błędnego zapisu.
 * W przypadku sukcesu wrzuca jednomian będący sumą tych wielomianów na stos.
 * @param[in] st : stos
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą czy dodawanie zakończyło się powodzeniem.
 */
void addMonosFromStack(Stack *st, bool *success) {
    Mono m = takeMono(st, success);
    Element *e = top(st);

    if (!success || e == NULL) {
        *success = false;
        return;
    }

    Mono *arr = malloc(10 * sizeof (Mono));
    if (arr == NULL)
        exit(1);

    arr[0] = m;
    long current_size = 10;
    long count = 1;

    while (!isOpenBracket(e) && !isEmpty(st)) {
        takeChar(st, '+', success);
        if (!*success) {
            return;
        }
        m = takeMono(st, success);
        if (!*success) {
            return;
        }

        if (count == current_size) {
            arr = realloc(arr, 2 * current_size * sizeof (Mono));
            if (arr == NULL)
                exit(1);
            current_size *=2;
        }
        arr[count] = m;
        count++;

        e = top(st);
        if (e == NULL) {
            *success = false;
            return;
        }
    }
    Poly p = PolyAddMonos(count, arr);
    Element e2 = elementOfPoly(&p);
    push(st, e2);
    free(arr);
}

/**
 * Dodaje jednomiany ze stosu aż do napotkania błędnego zapisu
 * lub obsłużen wszystkich elementóœ stosu.
 * @param[in] st : stos
 * @param[in] succ : wskaźnik na zmienną logiczną
 * oznaczającą czy dodawanie zakończyło się powodzeniem.
 * @return Wielomian będący sumą jednomianów ze stosu.
 */
Poly addMonosFinal(Stack *st, bool *success) {
    Mono m = takeMono(st, success);
    if (!success) {
        return (PolyZero());
    }

    Mono *arr = malloc((size(st) + 1) * sizeof (Mono));
    if (arr == NULL)
        exit(1);
    arr[0] = m;

    long count = 1;
    while (!isEmpty(st)) {
        takeChar(st, '+', success);
        if (!*success) {
            return PolyZero();
        }
        m = takeMono(st, success);
        if (!*success) {
            return PolyZero();
        }
        arr[count] = m;
        count++;
    }

    Poly p = PolyAddMonos(count, arr);
    free(arr);
    return p;
}

Poly stringToPoly(Stack *st, char *current_char, long line_nr, bool *succ) {
    bool success = true;
    int num_type = COEFF;
    while (*current_char != '\n' && *current_char != 0) {
        char c = *current_char;
        if (!isLegal(c)) {
            reportError(st, line_nr, succ);
            return PolyZero();
        }
        if (c == '('  || c == '+') {
            Element e = elementOfChar(c);
            push(st, e);
            num_type = COEFF;
            current_char++;
        }
        else if (c == ',') {
            num_type = EXP;
            if (isEmpty(st)) {
                reportError(st, line_nr, succ);
                return PolyZero();
            }
            Element *e = top(st);
            if (e->type == NUMB) {
                long n = e->n;
                pop(st);
                Poly p = PolyFromCoeff(n);
                Element e2 = elementOfPoly(&p);
                push(st, e2);
            }
            else {
                addMonosFromStack(st, &success);
                if (!success) {
                    reportError(st, line_nr, succ);
                    return PolyZero();
                }
            }
            Element e3 = elementOfChar(',');
            push(st, e3);
            current_char++;
        }
        else if (isNumberStart(c)) {
            long l = toNumber(&current_char, num_type, &success);
            if (errno == ERANGE || !success) {
                reportError(st, line_nr, succ);
                return PolyZero();
            }

            Element e = elementOfNumb(l);
            push(st, e);
        }
        else if (c == ')') {
            num_type = COEFF;
            int exp = takeExp(st, &success);
            takeChar(st, ',', &success);

            Element *e = top(st);
            if (!success || e == NULL) {
                reportError(st, line_nr, succ);
                return PolyZero();
            }

            if (e->type == NUMB) {
                pop(st);
                takeChar(st, '(', &success);

                if (!success) {
                    reportError(st, line_nr, succ);
                    return PolyZero();
                }
                long n = e->n;
                Poly p;
                p = PolyFromCoeff(n);
                Mono m = MonoFromPoly(&p, exp);
                Element e2 = elementOfMono(&m);
                push(st, e2);
            }
            else if (e->type == POLY) {
                Poly p;
                p = e->p;
                pop(st);
                takeChar(st, '(', &success);
                if (!success) {
                    reportError(st, line_nr, succ);
                    return PolyZero();
                }

                if (PolyIsZero(&p)) {
                    exp = 0;
                }
                Mono m = MonoFromPoly(&p, exp);
                Element e2 = elementOfMono(&m);
                push(st, e2);
            }
            else {
                    reportError(st, line_nr, succ);
                    return PolyZero();
            }
            current_char++;
        }
        else {
            reportError(st, line_nr, succ);
            return PolyZero();
        }
    }

    Element *e = top(st);
    if (e == NULL) {
        reportError(st, line_nr, succ);
        return PolyZero();
    }

    if (size(st) == 1 && e->type == NUMB) {
        return PolyFromCoeff(e->n);
    }
    if (size(st) == 1 && e->type == POLY) {
        return e->p;
    }
    if (e->type == MONO) {
        Poly p = addMonosFinal(st, &success);
        if (success)
            return p;
        else {
            reportError(st, line_nr, succ);
            return PolyZero();
        }
    }
    else {
        reportError(st, line_nr, succ);
        return PolyZero();
    }
}