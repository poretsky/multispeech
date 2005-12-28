// Spanish TTS description
// This stuff is language dependent and
// oriented for an Mbrola TTS

#include <stdio.h>
#include <string.h>
#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "Spanish.h"

char *es_puncts[] =
  {
    " Exclamación ", " Apostrofo ", " Interrogación ", " Coma ",
    " Punto ", " Punto y coma ", " Dos puntos ", " Guión-Menos ",
    " Arroba ", " Signo de número ", " Signo de dólar ", " Por ciento ",
    " Acento circunflejo ", " y comercial ", " Asterísco ", " Guión Bajo ",
    " Abrir paréntesis ", " Cerrar paréntesis ", " Más ", " Igual ",
    " Abrir corchete", " Cerrar corchete", " Abrir llave ", " Cerra llave ",
    " barra invertida ", " línea vertical ", " Comillas ", " Barra ",
    " acento grave ", " tilde ", " Menor que  ", " Mayor que "
  };

class es_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      // rate coeff: 184.0 instead of 200.0 (compared to freephone rate). 
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", Spanish->id, 184.0 / ra, pi, vo * 1.40, fr);
      return text;
    }
};

class es_abbreviations: public substitutor
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
  es_abbreviations(char *orig, char *subst): substitutor(orig, subst)
    {
    }
};

language *Spanish = new language("es", "es_ES.iso885915@euro", "Español", "[a-zA-Z]",
				 *new substitutor("\\.,", " punto coma ") +=
				 *new substitutor("\\.\\.\\.", " punto punto punto ") +=
				 *new substitutor("\\.\\.", " punto punto ") +=
				 *new substitutor("([a-zA-Z])\\.([a-zA-Z])", "\\1 punto \\2") +=
				 *new substitutor("([0-9])\\.([0-9])", "\\1punto\\2") +=
				 *new substitutor("[\\+-]?[0-9]+", " & ") +=
 				 *new es_abbreviations("((^|[^a-zA-Z])[bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ])"
 						       "([bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ]+([^a-zA-Z]|$))", "\\1 \\3") +=
				*new punctuations(es_puncts) += 
				*new substitutor("[^-\\.,!?';:0-9a-zA-ZÀ-ÿ]", " "),				
				 *new es_tts_params);
