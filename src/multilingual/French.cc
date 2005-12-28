// French TTS description
// This stuff is language dependent and
// oriented for an Mbrola TTS

#include <stdio.h>
#include <string.h>
#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "French.h"

char *fr_puncts[] =
  {
    " point d'exclamation ", " apostrophe ", " point d'interrogation ", " virgule ",
    " point ", " point virgule ", " deux points ", " tiret ",
    " arobas ", " dièse ", " dollar ", " pourcent ",
    " chapeau ", " et commercial ", " astérisque ", " souligné ",
    " parenthèse ouvrante ", " parenthèse fermante ", " plus ", " égal ",
    " crochet ouvrant", " crochet fermant ", " accolade ouvrante ", " accolade fermante ",
    " anti slash ", " barre verticale ", " guillemets ", " slash ",
    " accent grave ", " tilde ", " inférieur à  ", " supérieur à "
  };

class fr_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      // rate coeff: 232.0 instead of 200.0 (compared to freephone rate). 
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", French->id, 232.0 / ra, pi, vo * 0.35, fr);
      return text;
    }
};

class fr_abbreviations: public substitutor
{
private:
  virtual char *perform(char *&t)
    {
      int l = 0;
      while (l < strlen(t))
	{
	  l = strlen(t);
	  t = substitutor::perform(t);
	}
      return t;
    }

public:
  fr_abbreviations(char *orig, char *subst): substitutor(orig, subst)
    {
    }
};

language *French = new language("fr", "fr_FR.iso885915@euro", "Français", "[a-zA-Z]",
				 *new substitutor("\\.,", " point virgule ") +=
				 *new substitutor("\\.\\.\\.", " point point point ") +=
				 *new substitutor("\\.\\.", " point point ") +=
				 *new substitutor("([a-zA-Z])\\.([a-zA-Z])", "\\1 point \\2") +=
				 *new substitutor("([0-9])\\.([0-9])", "\\1point\\2") +=
				 *new substitutor("[\\+-]?[0-9]+", " & ") +=
 				 *new fr_abbreviations("((^|[^a-zA-Z])[bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ])"
 						       "([bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ]+([^a-zA-Z]|$))", "\\1 \\3") +=
				*new punctuations(fr_puncts) += 
				*new substitutor("[^-\\.,!?';:0-9a-zA-ZÀ-ÿ]", " "),				
				 *new fr_tts_params);
