// Russian TTS description
// This stuff is language dependent and
// oriented for Ru_tts

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "Russian.h"

char *ru_puncts[] =
  {
    " восклицаcтельный знаaк ", " апоaстроф ",
    " вопросиcтельный знаaк ", " запятаaя ",
    " тоaчка ", " тоcчка с запятоaй ",
    " двоетоaчие ", " тирэa ",
    " собаaка ", " решоaтка ",
    " доaллар ", " процеaнт ",
    " доaмик ", " амперсаaнд ",
    " звездаa ", " подчоaркивание ",
    " круaглая скоcбка открыaть ", " круaглая скоcбка закрыaть ",
    " плюс ", " равноa ",
    " квадраaтная скоcбка открыaть ", " квадраaтная скоcбка закрыaть ",
    " фигуaрная скоcбка открыaть ", " фигуaрная скоcбка закрыaть ",
    " бэкслэaш ", " вертикаcльная чертаa ",
    " кавыaчка ", " слэш ",
    " одинаcрная кавыaчка ", " тиaльда ",
    " меaньше ", " боaльше "
  };

class ru_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      double r = 50.0 / ra - 0.2;
      double p = pi * pi;
      if (r < 0.0) r = 0.0;
      if (r > 1.0) r = 1.0;
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", Russian->id, r,
	      atan(p * p / 5.0) * M_2_PI, atan(vo) * M_2_PI, fr * 5 / 8);
      return text;
    }
};

class ru_recoder: public chain
{
private:
  unsigned char *table;

  virtual char *perform(char *&text)
    {
      unsigned char *t;
      if (text) for (t = (unsigned char *)text; *t; t++) *t = table[*t];
      return text;
    }

public:
  ru_recoder(char *t)
    {
      unsigned int i;
      table = new unsigned char[256];
      for (i = 0; i < 33; i++) table[i] = ' ';
      for (i = 33; i < 127; i++) table[i] = i;
      for (i = 127; i < 192; i++) table[i] = ' ';
      for (i = 192; i < 224; i++) table[i] = i;
      for (i = 224; i < 256; i++) table[i] = i - 32;
      for (i = 0; t[i]; i++)
	{
	  table[i + 65] = t[i];
	  table[i + 97] = t[i];
	}
      table[163] = 163;
      table[179] = 163;
    }

  ~ru_recoder(void)
    {
      delete table;
    }
};

class ru_abbreviations: public chain
{
private:
  chain mark, unmark, preparator;

  virtual char *perform(char *&t)
    {
      int l = 0;
      t = mark(t);
      while (l < strlen(t))
	{
	  l = strlen(t);
	  t = preparator(t);
	}
      t = unmark(t);
      return t;
    }

public:
  ru_abbreviations(void)
    {
      mark += *new substitutor("(^|[^a-z\243ю-ъ])([bvhjqwxyzбвгджзйклмнпрстфхцчшщъь]+)([^a-z\243ю-ъ]|$)", "\\1 \\2m\\3");
      mark += *new substitutor(" ?( [вкс])m([, ]|$)", "\\1\\2");
      mark += *new substitutor("([\243ю-ъ]+[аеиоуыэю\243]) +([бж])m", "\\1\\2");
      mark += *new substitutor("([\243ю-ъ]*я) +([бж])m", "\\1\\2");
      mark += *new substitutor("(^|[\\(\\[{<\"'`,-]) ([вкс])m([-, ]|$)", "\\1\\2\\3");
      unmark += *new substitutor(" w ", " дубльвэa ");
      unmark += *new substitutor(" y ", " иaгрек ");
      unmark += *new substitutor(" й ", " икраaткое ");
      unmark += *new substitutor(" ь ", " мяcгкий знаaк ");
      unmark += *new substitutor(" ъ ", " тв\243cрдый знаaк ");
      unmark += *new substitutor(" m", "");
      preparator += *new substitutor("( [bvбвж])([a-zю-ъ]*m|\\.)", "\\1э \\2");
      preparator += *new substitutor("( к)([a-zю-ъ]*m)", "\\1а \\2");
      preparator += *new substitutor(" (с)([a-zю-ъ]*m)", " э\\1 \\2");
      preparator += *new substitutor("( h)([a-zю-ъ]*m)", " аш \\2");
      preparator += *new substitutor("( j)([a-zю-ъ]*m)", " йот \\2");
      preparator += *new substitutor("( q)([a-zю-ъ]*m)", " ку \\2");
      preparator += *new substitutor("( x)([a-zю-ъ]*m)", " икс \\2");
      preparator += *new substitutor("( z)([a-zю-ъ]*m)", " зэт \\2");
      preparator += *new substitutor("( [wyгдзйлмнпртфхцчшщъь])([a-zю-ъ]*m)", "\\1  \\2");
    }
};

language *Russian = new language("ru", "[ю-Ъ\243\263]",
				 *new ru_recoder("аbцдефгhиjклмнопqрстуvwxyz") +=
				 *new substitutor("[\\+-]?[0-9]+(\\.[0-9]+)?", " & ") +=
				 *new substitutor("-([0-9])", " минус \\1") +=
				 *new ru_abbreviations +=
				 *new punctuations(ru_puncts) +=
				 *new substitutor("[^-\\.,!?;:0-9abchjqvwxyz\243ю-ъ]", " ") +=
				 *new ru_recoder("+б=дефгхийклмнопкрстуввxиз") +=
				 *new substitutor("x", "кс") +=
				 *new substitutor("([0-9])( +[0-9])", "\\1,\\2") +=
				 *new substitutor(" ([ю-ъ\243\\+=]+[-\\.,?!:; ]*[0-9])", "\n\\1"),
				 *new ru_tts_params);
