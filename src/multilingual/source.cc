// Input commands from client

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "xmalloc.h"
#include "source.h"

char *parser::params(char *buf)
{
  char *s, *t;

  // eliminate leading whitespace and surrounding braces
  s = strrchr(buf, '}');
  if (s) *s = 0;
  s = buf + strlen(buf);
  while (s > buf)
    {
      if (*--s == ' ') *s = 0;
      else break;
    }
  s = buf;
  while ((*s == ' ') || (*s == '\t')) s++;
  t = strchr(s, '{');
  if (t) *t = ' ';

  // parse the command
  cmd = s;
  s += strcspn(s, " \t");
  if (*s) *s++ = 0;
  while (*s == ' ') s++;
  return s;
}

int parser::operator == (char *s)
{
  return !strcmp(cmd, s);
}

source::source(void)
{
  buffer = NULL;
  bufsize = 0;
}

source::~source(void)
{
  free(buffer);
}

int source::read(void)
  // read one line from stdin, and parse it.
  // Enlarge buffer if needed.
  // Returns nonzero if success.
{
  char *s, *sprev;
  unsigned int i;

  if (!bufsize) buffer = (char *)xmalloc(bufsize = 64);
  s = buffer;
  for (i = 0; i < bufsize; i++) s[i] = 0;
  while(errno = 0, (s = fgets(sprev=s, buffer + bufsize - s, stdin)) || (errno == EINTR))
    {
      if (s)
	{
	  for (s = buffer + bufsize - 1; s >= sprev; s--) if (*s) break;
	  if (s >= sprev)
	    {
	      char *t;
	      for (t = sprev; t < s; t++) if(!(*t)) *t = ' ';
	    }
	  if (*sprev)
	    {
	      if (*s == '\n')
		{
		  int count = 0, c;
		  *s = ' ';	// treat newline as whitespace
		  for (s = buffer; (c = *s); s++)
		    {
		      if ((c < ' ') && (c >= 0)) *s =' ';
		      switch(c)
			{
			  case '{':
			    count++;
			    break;
			  case '}':
			    count--;
			    break;
			  default:
			    break;
			}
		    }
		  if (!count)
		    {
		      data = command.params(buffer);
		      return 1;
		    }
		}
	      else buffer = (char *)xrealloc(buffer, bufsize *= 2);
	      s = buffer + strlen(buffer);
	      for (i = s - buffer; i < bufsize; i++) buffer[i] = 0;
	    }
	  else s = sprev;
	}
      else s = sprev;
    }
  return 0;
}
