/** @file
  Interfejs stosu obsługującego elementy typów char, long, Poly oraz Mono.
*/

#ifndef _STACK_H
#define _STACK_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "poly.h"

#define CHAR 0
#define NUMB 1
#define POLY 2
#define MONO 3

/**
 * Struktura przechowująca element stosu.
 * Element jest znakiem, liczbą całkowitą, wielomianem lub jednomianem
 */
typedef struct Element {
    union {
        char c;  ///< znak
        long n;  ///< liczba
        Poly p;  ///< wielomian
        Mono m;  ///< jednomian
    } ;
    int type;  ///< typ elementu (0 - char, 1 - long, 2 - poly, 3 - mono)
} Element;

/**
 * Struktura przechowująca stos.
 * @param[in] elements : tablica elementów na stosie
 * @param[in] capacity : obecna pojemność stosu
 * @param[in] current_size : obecna liczba elementów na stosie
 */
typedef struct Stack {
    struct Element *elements;
    size_t capacity;
    size_t current_size;
} Stack;

/**
 * Funkcja tworząca stos o podanym rozmiarze.
 * @param[in] init_size : pojemność stworzonego stosu
 */
struct Stack* makeStack (int init_size);

/**
 * Funkcja zwracająca rozmiar stosu.
 * @param[in] st : stos
 * @return rozmiar stosu
 */
size_t size (struct Stack *st);

/**
 * Sprawdza, czy stos jest pusty.
 * @param[in] st : stos
 * @return Czy stos jest pusty?
 */
bool isEmpty (struct Stack *st);

/**
 * Sprawdza, czy stos jest pełny.
 * @param[in] st : stos
 * @return Czy stos jest pełny?
 */
bool isFull (struct Stack *st);

/**
 * Wrzuca na stos podany element.
 * @param[in] st : stos
 * @param[in] elem : element
 */
void push (struct Stack *st, struct Element elem);

/**
 * Funckja dająca element z wierzchu stosu.
 * @param[in] st : stos
 * @return Element z wierzchu stosu.
 */
struct Element *top(struct Stack *st);

/**
 * Usuwa element z wierzchołka stosu
 * @param[in] st : stos
 */
void pop(struct Stack *st);

/**
 * Usuwa wszystkie elementy ze stosu.
 * Jeśli element odpowiada za jakąś zaalokowaną przestrzeń to zwalnia ją.
 * @param[in] st : stos
 */
void freeStack(struct Stack *st);

/**
 * Niszczy stos i zwalnia całą zajmowaną przez niego pamięć.
 * W tym pamieć zajmowaną przez elementy ze stosu.
 * @param[in] st : stos
 */
void destroyStack (struct Stack *st);

/**
 * Zwraca element reprezentujący podany wielomian.
 * @param[in] p : wielomian @f$p@f$
 * @return Element reprezentujący podany wielomian.
 */
struct Element elementOfPoly (Poly *p);

/**
 * Zwraca element reprezentujący podany znak.
 * @param[in] c : znak
 * @return Element reprezentujący podany znak.
 */
struct Element elementOfChar (char c);

/**
 * Zwraca element reprezentujący podany jednomian.
 * @param[in] m : jednomian @f$m@f$
 * @return Element reprezentujący podany jednomian.
 */
struct Element elementOfMono (Mono *m);

/**
 * Zwraca element reprezentujący podaną liczbę.
 * @param[in] n : liczba całkowita
 * @return Element reprezentujący podaną liczbę.
 */
struct Element elementOfNumb (long n);

#endif //_STACK_H
