// Language description procedures
// Update: 
// * 2005 Gilles Casse <gcasse@oralux.org>
// prepare : method updated. Call the post_prepare method before 
//           calling the chain () operator. 
//           Helpful for some basic TTS where e.g. numbers are not allowed. 
//           See e.g. the Brazilian class.
//           
// By default this method has no action.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
#include "xmalloc.h"
#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"

extern char *letters_lib;

double voice::ra;
double voice::pi;
double voice::vo;
int voice::fr;

char voice::buf[32];

double speech::pitch = 1.0;
double speech::volume = 2.0;
double speech::char_factor = 1.2;
double speech::caps_factor = 1.2;
int speech::rate = 225;
int speech::frequency = 16000;
int speech::split_caps = 1;
int speech::capitalize = 0;
int speech::space_special_chars = 1;

voice::voice(void)
{
  *this += *new substitutor("^ *(.*)", buf);
}

speech::speech(chain &text_preparations, voice &command_insertion)
{
  params += *new substitutor("\\[:np \\]", "");
  params += *new substitutor("^(\\[_:(.*)\\])?(.*)", "\\2");
  *this += *new substitutor("\\[[^]]*\\]", " ");
  *this += text_preparations;
  *this += *new substitutor(" +", " ");
  *this += *new substitutor("^ +", "");
  *this += *new substitutor(" +$", "");
  *this += *new substitutor("^[-\\.,!?;:' ]+$", "");
  *this += *new substitutor("([-\\.,!?;:'])+", "\\1");
  *this += command_insertion;
}
speech::speech(voice &command_insertion)
{
  *this += command_insertion;
}

char *speech::perform(char *&text)
{
  char *t = strdup(text);
  t = params(t);
  voice::ra = (double)rate;
  voice::pi = pitch;
  voice::vo = volume;
  voice::fr = frequency;
  if (!t) return text;
  if (*t)
    {
      char *tptr=t;
      char *option;
      while ((option = strsep(&tptr, " ")))
	{
	  double p;
	  int q;
	  char *s = strsep(&option, ":");
	  switch (*s++)
	    {
	      case 'r':
		if (*s == 'a')
		  if ((p = atof(option))) voice::ra = p;
		break;
	      case 'p':
		if (*s == 'i')
		  if ((p = atof(option))) voice::pi = p;
		break;
	      case 'v':
		if (*s == 'o')
		  if ((p = atof(option))) voice::vo = p;
		break;
	      case 'f':
		if (*s == 'r')
		  if ((q = atoi(option))) voice::fr = q;
		break;
	    }
	}
    }
  free(t);
  return text;
}

char *language::post_prepare (char *&text)
{
  return text;
}

char *language::prepare (char *&text)
{
  prepare_chain( text);
  char* s=post_prepare( text);
  return s;
}

language::language(char *lang, char *theLocale, char* label, char *chars, chain &tpreps, voice &pars)
{
  id = lang;
  name = label;
  locale = theLocale;
  charset = new regex_t;
  if (regcomp(charset, chars, REG_EXTENDED | REG_NEWLINE))
    {
      delete charset;
      charset = NULL;
    }
  prepare_chain += *new speech(tpreps, pars);
  clean += *new substitutor("\\[[^]]*\\]", "");
  clean += *new substitutor(" +", "");
}

language::language(char *lang, char *theLocale, char* label, voice &pars)
{
  id = lang;
  name = label;
  locale = theLocale;
  charset = NULL;
  prepare_chain += *new speech(pars);
}

language::~language(void)
{
  if (charset)
    {
      regfree(charset);
      delete charset;
    }
}

int language::check(char *text)
{
  char *t = strdup(text);
  if (clean(t))
    {
      int status;
      if (charset)
	status = !regexec(charset, t, 0, NULL, 0);
      else status = 1;
      free(t);
      return status;
    }
  else return 0;
}

int language::sound_access(char *text)
{
  int stat;
  char *s = (char *)xmalloc(strlen(letters_lib) + strlen(id) + strlen(text) + 10);
  sprintf(s, "%s/%s/%s.raw", letters_lib, id, text);
  stat = !access(clean(s), R_OK);
  free(s);
  return stat;
}

char *language::say(char *&text)
{
  if (sound_access(text))
    {
      char *s = (char *)xmalloc(strlen(letters_lib) + (strlen(id) * 2) + strlen(text) + 10);
      sprintf(s, "l%s %s/%s/%s.raw", id, letters_lib, id, clean(text));
      if (text) free(text);
      text = s;
    }
  else text = prepare(text);
  return text;
}

char *language::letter(char *&text)
{
  int saved_mode = punctuations::mode;
  int saved_rate = speech::rate;
  double saved_pitch = speech::pitch;
  speech::rate = (int)((double)speech::rate * speech::char_factor);
  punctuations::mode = 2;
  if (isupper(*text)) speech::pitch *= speech::caps_factor;
  text = say(text);
  speech::rate = saved_rate;
  speech::pitch = saved_pitch;
  punctuations::mode = saved_mode;

  return text;
}

