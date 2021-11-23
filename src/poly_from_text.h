/** @file
  Interfejs funkcji obsługujących konwersję stringa na wielomian wielu zmiennych,
  stosujący odwrotną notację polską.
*/

#ifndef _POLY_FROM_TEXT_H
#define _POLY_FROM_TEXT_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "poly.h"
#include "stack.h"
#include <errno.h>

#define MIN_EXP_VALUE 0
#define MAX_EXP_VALUE 2147483647
#define COEFF 0
#define EXP 1

/**
 * Sprawdza, czy znak jest cyfrą.
 * @param[in] c : znak
 * @return Czy znak jest cyfrą?
 */
bool isNumber (char c);

/**
 * Sprawdza, czy znak jest możliwym początkiem liczby.
 * @param[in] c : znak
 * @return Czy znak jest cyfrą lub znakiem @f$-@f$?
 */
bool isNumberStart (char c);

/**
 * Sprawdza, czy znak jest literą alfabetu angielskiego.
 * @param[in] c : znak
 * @return Czy znak jest literą alfabetu angielskiego?
 */
bool isLetter (char c);

/**
 * Konwertuje podany string na liczbę całkowitą.
 * @param[in] current_char : wskaźnik do stringa
 * @param[in] number_type : typ konwertowanej liczby - współczynnik lub wykładnik
 * @param[in] success : wskaźnik na zmienną logiczną
 * ustawianą w zależności od tego, czy parsowanie powiodło się
 * @return Sparsowana liczba lub w razie sukcesu, -1 wpp.
 */
long toNumber (char **current_char, int number_type, bool *success);

/**
 * Konwertuje podany string na wielomian zgodnie z opisanymi założeniami kalkulatora.
 * @param[in] st : stos
 * @param[in] current_char : wskaźnik na znak, od którego rozpoczynamy konwersję
 * @param[in] line_nr : numer obecnie przetwarzanego wiersza
 * @param[in] succ : wskaźnik na zmienną logiczną
 * ustawianą w zależności od tego, czy parsowanie powiodło się
 * @return Wielomian zapisany w wierszu, jeśli zawiera on poprawny zapis wielomianu, wielomian zerowy wpp.
 */
Poly stringToPoly(Stack *st, char *current_char, long line_nr, bool *succ);

#endif //_POLY_FROM_TEXT_H