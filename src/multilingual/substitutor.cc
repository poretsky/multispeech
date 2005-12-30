// Global regular substitution mechanism

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include "chain.h"
#include "substitutor.h"
#include "xmalloc.h"

substitutor::substitutor(char orig, char *subst)
{
  symbol = orig;
  pattern = NULL;
  replacement = subst;
}

substitutor::substitutor(char *orig, char *subst)
{
  symbol = 0;
  pattern = new regex_t;
  if (regcomp(pattern, orig, REG_EXTENDED))
    {
      delete pattern;
      pattern = NULL;
    }
  replacement = subst;
}

substitutor::~substitutor(void)
{
  if (pattern)
    {
      regfree(pattern);
      delete pattern;
    }
}

// Look for the pattern in the given string.
bool substitutor::search(char* text, regmatch_t* hit)
{
  if (!pattern)
    {
      int i = 0;
      char* s = strchr(text, symbol);
      if (s && symbol)
	{
	  hit[0].rm_so = s - text;
	  hit[0].rm_eo = hit[0].rm_so + 1;
	  i++;
	}
      hit[i].rm_so = hit[i].rm_eo = -1;
      return (bool)i;
    }
  else return !regexec(pattern, text, pattern->re_nsub + 1, hit, 0);
}

// searches for the regular expression or symbol in the string
// pointed to by the parameter, and replaces each instance
// using the replacement, which may (in the case of regexp)
// include items like \2, which would be replaced by the 2nd
// subexpression in the pattern.  The string must have
// been allocated from the heap, and the string pointer may be
// modified if the replacement makes the string grow.
char *substitutor::perform(char *&str)
{
  char c, *milestone, *src, *dst, *t, *work, *s = str;
  int n, m, k, len;
  regmatch_t hit[16];
  if (!str) return str;
  n = strlen(s)+32;
  t = (char *)xmalloc(n);
  m = 0;
  while(*s && search(s, hit))
    {
      // found a match for the regular
      // expression.  copy the part before
      // the match
      k = hit[0].rm_so;
      if ((m + k + 1) > n) t = (char *)xrealloc(t, n = m + k + 32);
      memcpy(t + m, s, k);
      milestone = s;
      s += k;
      m += k;
      // make the substitution using work
      // area, then copy it to the result
      work = (char *)xmalloc(hit[0].rm_eo - hit[0].rm_so + 100);
      src = replacement;
      dst = work;
      while ((c = *src++) != '\0')
	{
	  if (c == '&')
	    k = 0;
	  else if (c == '\\' && '0' <= *src && *src <= '9')
	    k = *src++ - '0';
	  else k = -1;
	  if (k < 0)	// Ordinary character.
	    {
	      if (c == '\\' && (*src == '\\' || *src == '&'))
		c = *src++;
	      *dst++ = c;
	    }
	  else if ((hit[k].rm_so >= 0)
		   && (hit[k].rm_eo > hit[k].rm_so))
	    {
	      len = hit[k].rm_eo - hit[k].rm_so;
	      (void)strncpy(dst, milestone + hit[k].rm_so, len);
	      dst += len;
	    }
	}
      *dst++ = '\0';
      k = strlen(work);
      if ((m + k + 1) > n) t = (char *)xrealloc(t, n = m + k + 32);
      memcpy(t + m, work, k);
      free(work);
      s = milestone + hit[0].rm_eo;
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
