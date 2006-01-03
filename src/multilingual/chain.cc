// Base chain functions

#include <cstdlib>

#include "chain.h"

chain::chain(void)
{
  link = NULL;
}

char *chain::perform(char *&s)
{
  return s;
}

char *chain::operator () (char *&s)
{
  chain *next = this;
  while (next)
    {
      if (!s) break;
      if (*s)
	{
	  s = next->perform(s);
	  next = next->link;
	}
      else
	{
	  free(s);
	  s = NULL;
	  break;
	}
    }
  if (s)
    if (!(*s))
      {
	free(s);
	s = NULL;
      }
  return s;
}

chain &chain::operator += (chain &next)
{
  chain *tail = this;
  while (tail->link) tail = tail->link;
  tail->link = &next;
  return *this;
}
