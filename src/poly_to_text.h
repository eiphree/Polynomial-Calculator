/** @file
  Interfejs funkcji obsługujących wypisanie wielomianu wielu zmiennych
  zgodnie z odwrotną notacją polską.
*/

#ifndef _POLY_TO_TEXT_H
#define _POLY_TO_TEXT_H

#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include "poly.h"
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>

/**
 * Wypisuje w odwrotnej notacji polskiej wielomian.
 * @param[in] p : wielomian @f$p@f$
 */
void printPoly(Poly *p);

/**
 * Wypisuje w odwrotnej notacji polskiej jednomian.
 * @param[in] m : jednomian @f$m@f$
 */
void printMono(Mono *m);

#endif //_POLY_TO_TEXT_H
