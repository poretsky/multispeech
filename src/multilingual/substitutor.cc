// Global regular substitution mechanism

#include <stdlib.h>
#include <string.h>
#include "substitutor.h"
#include "xmalloc.h"

substitutor::substitutor(char *orig, char *subst)
{
  pattern = regcomp(orig);
  replacement = subst;
}

substitutor::~substitutor(void)
{
  free(pattern);
}

// searches for the regular expression in the string pointed to by the
// parameter, and replaces each instance using the replacement, which may
// include items like \2, which would be replaced by the 2nd
// subexpression in the pattern - see regsub(3).  The string must have
// been allocated from the heap, and the string pointer may be
// modified if the replacement makes the string grow.
char *substitutor::perform(char *&str)
{
  char *t, *work, *s = str;
  int n, m, k;
  if (!str) return str;
  n = strlen(s)+32;
  t = (char *)xmalloc(n);
  m = 0;
  while(*s && regexec(pattern, s))
    {
      // found a match for the regular
      // expression.  copy the part before
      // the match
      k = pattern->startp[0]-s;
      if ((m + k + 1) > n) t = (char *)xrealloc(t, n = m + k + 32);
      memcpy(t + m, s, k);
      s += k;
      m += k;
      // make the substitution using work
      // area, then copy it to the result
      work = (char *)xmalloc(pattern->endp[0] - pattern->startp[0] + 100);
      regsub(pattern, replacement, work);
      k = strlen(work);
      if ((m + k + 1) > n) t = (char *)xrealloc(t, n = m + k + 32);
      memcpy(t + m, work, k);
      free(work);
      s = pattern->endp[0];
      m += k;
    }
  if (*s)
    {
      // copy the part after the last match
      k = strlen(s);
      if ((m + k + 1) > n) t = (char *)xrealloc(t, n = m + k + 32);
      memcpy(t + m, s, k);
      m += k;
    }
  t[m] = 0;
  free(str);
  str = t;
  return str;
}
