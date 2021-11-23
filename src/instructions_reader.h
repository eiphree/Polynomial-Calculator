/** @file
  Interfejs funkcji czytających i wykonujących instrukcje kalkulatora wielomianów wielu zmiennych.
*/

#ifndef _INSTRUCTIONS_READER_H
#define _INSTRUCTIONS_READER_H
#include "stack.h"

/**
 * Czyta i wykonuje podaną instrukcję.
 * @param[in] str : instrukcja
 * @param[in] st : stos, na którym operuje kalkulator
 * @param[in] line_nr : numer obecnie obsługiwanego wiersza
 */
void takeInstruction(char *str, Stack *st, long line_nr);

#endif //_INSTRUCTIONS_READER_H
