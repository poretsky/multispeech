/*
    Copyright (C) 1996 Alistair Conkie

    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

KEY broad_cats[] = {
  {        "*", STRESS, },     /* because it is always beside a vowel	*/
  {        "+", BOUND, },      /* morph boundary */
  {        "-", BOUND, },
  {        ".", BOUND, },      /* to force boundary			*/
  {        "a", VOWEL, },
  {        "aa", VOWEL, },
  {        "ai", VOWEL, },
  {        "air", VOWEL, },
  {        "ar", VOWEL, },
  {        "aw", VOWEL, },
  {        "b", CONST, },
  {        "ch", CONST, },
  {        "d", CONST, },
  {        "dh", CONST, },
  {        "e", VOWEL, },
  {        "ee", VOWEL, },
  {        "eer", VOWEL, },
  {        "er", VOWEL, },
  {        "f", CONST, },
  {        "g", CONST, },
  {        "h", CONST, },
  {        "i", VOWEL, },
  {        "ie", VOWEL, },
  {        "j", CONST, },
  {        "k", CONST, },
  {        "l", CONST, },
  {        "m", CONST, },
  {        "n", CONST, },
  {        "ng", CONST, },
  {        "o", VOWEL, },
  {        "oa", VOWEL, },
  {        "oi", VOWEL, },
  {        "oo", VOWEL, },
  {        "oor", VOWEL, },
  {        "ou", VOWEL, },
  {        "p", CONST, },
  {        "r", CONST, },
  {        "s", CONST, },
  {        "sh", CONST, },
  {        "t", CONST, },
  {        "th", CONST, },
  {        "u", VOWEL, },
  {        "uu", VOWEL, },
  {        "v", CONST, },
  {        "w", CONST, },
  {        "y", CONST, },
  {        "z", CONST, },
  {        "zh", CONST, },
  {        "|", BOUND, },         /* word boundary */
  {        "~", STRESS, },        /* because it is always beside a vowel	*/
  {	"", 0, },
};

