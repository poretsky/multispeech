// Pronouncing of punctuations

#include <string.h>
#include "punctuations.h"
#include "substitutor.h"

char *punctuations_set = "!'?,.;:-@#$%^&*_()+=[]{}\\|\"/`~<>";

int punctuations::mode = 1;

punctuations::punctuations(char **pronounce)
{
  int i;
  for (i = 0; i < strlen(punctuations_set); i++)
    {
      char pattern[3] = { '\\', punctuations_set[i], 0 };
      if (i < 8) first_group += *new substitutor(pattern, pronounce[i]);
      else second_group += *new substitutor(pattern, pronounce[i]);
    }
}

char *punctuations::perform(char *&s)
{
  if (mode > 1) s = first_group(s);
  if (mode > 0) s = second_group(s);
  return s;
}
