// German TTS description
// This stuff is language dependent and
// oriented for an Mbrola TTS

#include <stdio.h>
#include <string.h>
#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "German.h"


char *de_puncts[] =
  {
    " Ausrufezeichen ", " Apostroph ", " Fragezeichen ", " Komma ",
    " Punkt ", " Strichpunkt ", " Doppelpunkt ", " Strich ",
    " Kommerzielles At ", " Pfund ", " Dollar ", " Prozent ",
    " Karet ", " Kaufmännisch Und ", " Asterisk ", " Unterstrich ",
    " Runde Klammer auf ", " Runde Klammer zu ", " Pluszeichen ", " Gleichheitszeichen ",
    " Eckige Klammer auf ", " Eckige Klammer zu ", " Geschweifte Klammer auf ", " Geschweifte Klammer zu ",
    " Rückschrägstrich ", " Senkrechter Strich ", " Anführungszeichen ", " Schrägstrich ",
    " Grave-Akzent ", " Tilde-Akzent ", " Kleiner als ", " Größer als "
  };

class de_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      // rate coeff: 207.0 instead of 200.0 (compared to freephone rate). 
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", German->id, 207.0 / ra, pi, vo * 1.10, fr);
      return text;
    }
};

class de_abbreviations: public substitutor
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
  de_abbreviations(char *orig, char *subst): substitutor(orig, subst)
    {
    }
};

 language *German = new language("de", "de_DE.iso885915@euro", "Deutsch", "[a-zA-Z]",
				 *new substitutor("\\.,", " punkt komma ") +=
				 *new substitutor("\\.\\.\\.", " punkt punkt punkt ") +=
				 *new substitutor("\\.\\.", " punkt punkt ") +=
				 *new substitutor("([a-zA-Z])\\.([a-zA-Z])", "\\1 punkt \\2") +=
				 *new substitutor("([0-9])\\.([0-9])", "\\1punkt\\2") +=
				 *new substitutor("[\\+-]?[0-9]+", " & ") +=
				 *new de_abbreviations("((^|[^a-zA-Z])[bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ])"
						       "([bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ]+([^a-zA-Z]|$))", "\\1 \\3") +=
				 *new punctuations(de_puncts) +=
				 *new substitutor("[^-\\.,!?';:0-9a-zA-ZÀ-ÿ]", " "),
				 *new de_tts_params);
