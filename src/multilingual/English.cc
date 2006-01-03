// English TTS description
// This stuff is language dependent and
// oriented for Freephone and Mbrola TTS

#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <regex.h>

#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "English.h"

char *en_puncts[] =
  {
    " exclamation ", " apostrophe ", " questionmark ", " comma ",
    " period ", " semicolon ", " colon ", " dash ",
    " at ", " pound ", " dollar ", " percent ",
    " caret ", " ampersand ", " star ", " underscore ",
    " left paren ", " right paren ", " plus ", " equals ",
    " left bracket ", " right bracket ", " left brace ", " right brace ",
    " backslash ", " pipe ", " quotes ", " slash ",
    " backquote ", " tilde ", " less than ", " greater than "
  };

class en_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", English->id, 200.0 / ra, pi, vo, fr);
      return text;
    }
};

class en_abbreviations: public substitutor
{
private:
  virtual char *perform(char *&t)
    {
      unsigned int l = 0;
      while (l < strlen(t))
	{
	  l = strlen(t);
	  t = substitutor::perform(t);
	}
      return t;
    }

public:
  en_abbreviations(char *orig, char *subst): substitutor(orig, subst)
    {
    }
};

language *English = new language("en", "en_US.iso885915", "English", "[a-zA-Z]",
				 *new substitutor("\\.,", " dot comma ") +=
				 *new substitutor("\\.\\.\\.", " dot dot dot ") +=
				 *new substitutor("\\.\\.", " dot dot ") +=
				 *new substitutor("([a-zA-Z])\\.([a-zA-Z])", "\\1 dot \\2") +=
				 *new substitutor("([0-9])\\.([0-9])", "\\1period\\2") +=
				 *new substitutor("[\\+-]?[0-9]+", " & ") +=
				 *new en_abbreviations("((^|[^a-zA-Z])[bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ])"
						       "([bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ]+([^a-zA-Z]|$))", "\\1 \\3") +=
				 *new punctuations(en_puncts) +=
				 *new substitutor("[^-\\.,!?';:0-9a-zA-Z]", " "),
				 *new en_tts_params);
