// Pronouncing of punctuations

#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include "chain.h"
#include "punctuations.h"
#include "substitutor.h"

int punctuations::mode = 1;

const char *punctuations::punctuations_set = "!'?,.;:-@#$%^&*_()+=[]{}\\|\"/`~<>";

punctuations::punctuations(char **pronounce)
{
  unsigned int i;
  for (i = 0; i < strlen(punctuations_set); i++)
    {
      if (i < 8)
	first_group += *new substitutor(punctuations_set[i], pronounce[i]);
      else second_group += *new substitutor(punctuations_set[i], pronounce[i]);
    }
}

char *punctuations::perform(char *&s)
{
  if (mode > 1) s = first_group(s);
  if (mode > 0) s = second_group(s);
  return s;
}
