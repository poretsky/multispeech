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
    " ��������c������� ���a� ", " ���a����� ",
    " �������c������� ���a� ", " ������a� ",
    " ��a��� ", " ��c��� � ������a� ",
    " ������a��� ", " ����a ",
    " ����a�� ", " ����a��� ",
    " ��a���� ", " �����a�� ",
    " ��a��� ", " �������a�� ",
    " ������a ", " �����a�������� ",
    " ���a���� ���c��� �����a�� ", " ���a���� ���c��� �����a�� ",
    " ���� ", " �����a ",
    " ������a���� ���c��� �����a�� ", " ������a���� ���c��� �����a�� ",
    " ����a���� ���c��� �����a�� ", " ����a���� ���c��� �����a�� ",
    " ������a� ", " �������c����� �����a ",
    " ����a��� ", " ���� ",
    " �����c���� ����a��� ", " ��a���� ",
    " ��a���� ", " ��a���� "
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
      mark += *new substitutor("(^|[^a-z\243�-�])([bvhjqwxyz�����������������������]+)([^a-z\243�-�]|$)", "\\1 \\2m\\3");
      mark += *new substitutor(" ?( [���])m([, ]|$)", "\\1\\2");
      mark += *new substitutor("([\243�-�]+[��������\243]) +([��])m", "\\1\\2");
      mark += *new substitutor("([\243�-�]*�) +([��])m", "\\1\\2");
      mark += *new substitutor("(^|[\\(\\[{<\"'`,-]) ([���])m([-, ]|$)", "\\1\\2\\3");
      unmark += *new substitutor(" w ", " �������a ");
      unmark += *new substitutor(" y ", " �a���� ");
      unmark += *new substitutor(" � ", " ����a���� ");
      unmark += *new substitutor(" � ", " ��c���� ���a� ");
      unmark += *new substitutor(" � ", " ��\243c���� ���a� ");
      unmark += *new substitutor(" m", "");
      preparator += *new substitutor("( [bv���])([a-z�-�]*m|\\.)", "\\1� \\2");
      preparator += *new substitutor("( �)([a-z�-�]*m)", "\\1� \\2");
      preparator += *new substitutor(" (�)([a-z�-�]*m)", " �\\1 \\2");
      preparator += *new substitutor("( h)([a-z�-�]*m)", " �� \\2");
      preparator += *new substitutor("( j)([a-z�-�]*m)", " ��� \\2");
      preparator += *new substitutor("( q)([a-z�-�]*m)", " �� \\2");
      preparator += *new substitutor("( x)([a-z�-�]*m)", " ��� \\2");
      preparator += *new substitutor("( z)([a-z�-�]*m)", " ��� \\2");
      preparator += *new substitutor("( [wy������������������])([a-z�-�]*m)", "\\1  \\2");
    }
};

language *Russian = new language("ru", "[�-�\243\263]",
				 *new ru_recoder("�b�����h�j������q����vwxyz") +=
				 *new substitutor("[\\+-]?[0-9]+(\\.[0-9]+)?", " & ") +=
				 *new substitutor("-([0-9])", " ����� \\1") +=
				 *new ru_abbreviations +=
				 *new punctuations(ru_puncts) +=
				 *new substitutor("[^-\\.,!?;:0-9abchjqvwxyz\243�-�]", " ") +=
				 *new ru_recoder("+�=��������������������x��") +=
				 *new substitutor("x", "��") +=
				 *new substitutor("([0-9])( +[0-9])", "\\1,\\2") +=
				 *new substitutor(" ([�-�\243\\+=]+[-\\.,?!:; ]*[0-9])", "\n\\1"),
				 *new ru_tts_params);
