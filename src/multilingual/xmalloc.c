/* Memory allocation routines */

#include <stdlib.h>
#include <stdio.h>

void *xrealloc(void *p, unsigned int n)
{
  void *u = realloc(p, n);
  if (u) return u;
  else
    {
      perror("Memory allocation error");
      exit(EXIT_FAILURE);
    }
}

void *xmalloc(unsigned int n)
{
  void *u = NULL;
  return xrealloc(u, n);
}
