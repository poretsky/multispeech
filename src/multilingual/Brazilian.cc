// Brazilian TTS description
// This stuff is language dependent and
// oriented for ttpport and Mbrola TTS

#include <cstdio>
#include <cstring>
#include <cctype>

#include <sys/types.h>
#include <regex.h>

#include "chain.h"
#include "substitutor.h"
#include "punctuations.h"
#include "language.h"
#include "Brazilian.h"

//< data

char *br_puncts[] =
  {
    " exclamação ", " apóstrofo ", " interrogação ", " vírgula ",
    " ponto ", " ponto e vírgula ", " dois pontos ", " traço ",
    " arroba ", " sustenido ", " cifrão ", " porcento ",
    " chapéu ", " e comercial ", " asterísco ", " Sublinhado ",
    " abre parênteses ", " fecha parênteses ", " mais ", " igual ",
    " abre colchete ", " fecha colchete ", " abre chave ", " fecha chave ",
    " barra invertida ", " barra vertical ", " áspas ", " barra ",
    " acento ", " til ", " menor que ", " maior que "
  };
// 
static char *Cardinals[] = 
{
  " zero"," um"," dois"," três",
  " quatro"," cinco"," seis"," sete",
  " oito"," nove",		
  " dez"," onze"," doze"," treze",
  " catorze"," quinze"," dezesseis"," dezessete",
  " dezoito"," dezenove"
};

static char *Twenties[] = 
{
  " vinte"," trinta"," quarenta"," cinqüenta",
  " sessenta"," setenta"," oitenta"," noventa"
};

static char *Ordinals[2][20] = 
  {
    {
      " zero", // Not used
      " primeira"," segunda"," terceira",
      " quarta"," quinta"," sexta"," sétima",
      " oitava"," nona",		
      " décima",
      " décima primeira"," décima segunda", " décima terceira",
      " décima quarta"," décima quinta"," décima sexta"," décima sétima",
      " décima oitava"," décima nona"
    },
    {
      " zero", 
      " primeiro"," segundo"," terceiro",
      " quarto"," quinto"," sexto"," sétimo",
      " oitavo"," nono",		
      " décimo",
      " décimo primeiro"," décimo segundo", " décimo terceiro",
      " décimo quarto"," décimo quinto"," décimo sexto"," décimo sétimo",
      " décimo oitavo"," décimo nono"
    }
  };
// suffix 'o or 'a' must be added according to the gender
static char *Ord_twenties[2][9] = 
  {
    {
      " vigésima"," trigésima"," quadragésima"," qüinquagésima",
      " sexagésima"," septuagésima"," octogésima"," nonagésima"
    },
    {
      " vigésimo"," trigésimo"," quadragésimo"," qüinquagésimo",
      " sexagésimo"," septuagésimo"," octogésimo"," nonagésimo"
    }
  };
    
const char* TheBillion=" bilhão";
const char* TheBillions=" bilhões";
const char* TheBillionth[2]={" bilionésima"," bilionésimo"};
const char* TheMinus=" minus";
const char* TheInfinity=" infinity";
const char* TheAnd=" e";
const char* TheMillionth[2]={" milionésima"," milionésimo"};
const char* TheMillion=" Milhão";
const char* TheMillions=" milhões";
const char* TheThousand=" mil";
const char* TheThousandth[2]={" milésima"," milésimo"};

const char* TheHundreds[]={" cento"," duzentos"," trezentos"," quatrocentos"," quinhentos"," seiscentos"," setecentos"," oitocentos"," novecentos"};

const char* TheHundred=" cem";

const char* TheHundredth[2][9]={
  {" centésima","ducentésima","tricentésima","quadringentésima","quingentésima","sexcentésima","septingentésima","octincentésima","nongentésima"},
  {" centésimo","ducentésimo","tricentésimo","quadringentésimo","quingentésimo","sexcentésimo","septingentésimo","octincentésimo","nongentésimo"}};

const char* TheTenth[2]={" décima"," décimo"};

//>
//< br_tts_params

class br_tts_params: public voice
{
private:
  virtual char *perform(char *&text)
    {
      // rate coeff: 163.0 instead of 200.0 (compared to freephone rate). 
      sprintf(buf, "s%s %.2f %.2f %.2f %d\n\\1\n", Brazilian->id, 163.0 / ra, pi, vo * 0.4, fr);
      return text;
    }
};

//>
//< br_abbreviations

class br_abbreviations: public substitutor
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
  br_abbreviations(char *orig, char *subst): substitutor(orig, subst)
  {}
};

//>
//< br_expansion

class br_expansion: public expansion
{
public:
  br_expansion();
protected:
  virtual void sayChar(char c);
  virtual int isOrdinal( long int aValue, int aLastdigit);
  virtual void putCardinal(long int theValue);

  enum gender {FEMININE, MASCULINE};
  virtual void putOrdinal(long int theValue, gender theGender);
};

br_expansion::br_expansion()
  : expansion(" ponto", " vírgula")
{
}

void br_expansion::sayChar(char c)
{
  unsigned int i;

  for (i=0; i < strlen( punctuations::punctuations_set); i++)
  {
    if (c == punctuations::punctuations_set[i])
      {
	appendString( br_puncts[i]);
	return;
      }
  }
  expansion::sayChar( c);
}

//< putCardinal

// Translate a number to string and append it to myBuffer.  
// This version is for CARDINAL numbers.
// 	 Note: this is recursive.

void br_expansion::putCardinal(long int theValue)
{
 if (theValue < 0)
    {
      appendString(TheMinus);
      theValue = (-theValue);
      if (theValue < 0)	/* Overflow!  -32768 */
      {
	  appendString(TheInfinity);
	  return;
	}
    }

  if (theValue >= 1000000000L)	/* Billions */
    {
      long int x=theValue/1000000000L;
      putCardinal(x);
      if (x==1)
	{
	  appendString(TheBillion);
	}
      else
	{
	  appendString(TheBillions);
	}
      theValue = theValue % 1000000000;
      if (theValue == 0)
	return;		/* Even billion */
      if (theValue < 100)	/* as in "três bilhões e cinco" */
	appendString(TheAnd);
    }

  if (theValue >= 1000000L)	/* Millions */
    {
      long int x=theValue/1000000L;
      putCardinal(x);
      if (x==1)
	{
	  appendString(TheMillion);
	}
      else
	{
	  appendString(TheMillions);
	}
      theValue = theValue % 1000000L;
      if (theValue == 0)
	return;		/* Even million */
      if (theValue < 100)	/* as in "três bilhões e cinco" */
	appendString(TheAnd);
    }

  /* Thousands 1000..99999 */
  if (theValue >= 1000L)
    {
      long int x=theValue/1000L;
      if (x>1)
	{
	  putCardinal(x);
	}

      appendString(TheThousand);
      theValue = theValue % 1000L;
      if (theValue == 0)
	return;		/* Even thousand */
      if (theValue < 100)    
 	appendString(TheAnd);
    }

  if (theValue >= 100L)
    {
      long int x=theValue/100L - 1;

      theValue = theValue % 100;
      if ((x==0)&&(theValue==0))
	{
	  appendString(TheHundred);
	  return;		/* Even hundred */
	}
      else
	{
	  appendString(TheHundreds[x]);
	}
      if (theValue == 0)
	{
	  return;		/* Even hundred */
	}
      appendString(TheAnd);
    }

  if (theValue >= 20)
    {
      appendString(Twenties[(theValue-20)/ 10]);
      theValue = theValue % 10;
      if (theValue == 0)
	return;		/* Even ten */
      appendString(TheAnd);
    }

  appendString(Cardinals[theValue]);
  return;
} 
//>
//< putOrdinal


// Translate a number to string and append it to myBuffer.  
// This version is for ORDINAL numbers.
// 	 Note: this is recursive.
void br_expansion::putOrdinal(long int theValue, gender theGender)
{
  if (theValue < 0)
    {
      appendString(TheMinus); 
      theValue = (-theValue);
      if (theValue < 0)	/* Overflow!  -32768 */
	{
	  appendString(TheInfinity);
	  return;
	}
    }

  if (theValue >= 1000000000L)	/* Billions */
    {
      putCardinal(theValue/1000000000L);
      theValue = theValue % 1000000000;
      if (theValue == 0)
	{
	  appendString(TheBillionth[theGender]); 
	  return;		/* Even billion */
	}
      appendString(TheBillions); 
      if (theValue < 100)	/* as in "três bilhões e cinco" */
	appendString(TheAnd);
    }

  if (theValue >= 1000000L)	/* Millions */
    {
      putCardinal(theValue/1000000L);
      theValue = theValue % 1000000L;
      if (theValue == 0)
	{
	  appendString(TheMillionth[theGender]);
	  return;		/* Even million */
	}
      appendString(TheMillion);
      if (theValue < 100)
	appendString(TheAnd);
    }

  /* Thousands 1000..99999 */
  if (theValue >= 1000L)
    {
      putCardinal(theValue/1000L);
      theValue = theValue % 1000L;
      if (theValue == 0)
	{
	  appendString(TheThousandth[theGender]);
	  return;		/* Even thousand */
	}
      appendString(TheThousand);
      if (theValue < 100)
	appendString(TheAnd);
    }

  if (theValue >= 100L)
    {
      long int x = theValue/100L - 1;
      theValue = theValue % 100;

      if (theValue == 0)
	{
	  appendString(TheHundredth[theGender][x]);
	  return;		/* Even hundred */
	}
      else
	{
	  appendString(TheHundreds[x]);
	}
      appendString(TheAnd);
    }

  if (theValue >= 20)
    {
      appendString(Ord_twenties[theGender][(theValue-20)/ 10]);
      if ((theValue%10) == 0)
	{
	  return;		/* Even ten */
	}
      theValue = theValue % 10;
    }

  appendString(Ordinals[theGender][theValue]);
  return;
} 

//>
//< isOrdinal

int br_expansion::isOrdinal( long int theValue, int theLastdigit)
{
  int aResult=0;
  switch(myChar)
    {
    case 'º':
      putOrdinal(theValue, MASCULINE);
      newChar(); /* Used myChar */
      aResult=1;
      break;
    case 'ª':
      putOrdinal(theValue, FEMININE);
      newChar();	
      aResult=1;
      break;
    default:
      break;
    }

  return aResult;
}

//>

//>
//< br_language

br_language::br_language()
  :language("br", "pt_BR.iso88591", "Brasileiro", "[a-zA-Z]", // not used (no automatic charset detection)
	    *new substitutor("\\.,", " ponto vírgula ") +=
	    *new substitutor("\\.\\.\\.", " ponto ponto ponto ") +=
	    *new substitutor("\\.\\.", " ponto ponto ") +=
	    *new substitutor("([a-zA-Z])\\.([a-zA-Z])", "\\1 ponto \\2") +=
	    *new substitutor("([0-9])\\.([0-9])", "\\1ponto\\2") +=
	    *new substitutor("[\\+-]?[0-9]+", " & ") +=
	    // TODO GC: at the moment, commented out ("tres" which would be said "t res")
	    //	    *new br_abbreviations("((^|[^a-zA-Z])[bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ])"
	    //				  "([bcdfghj-np-tvwxzBCDFGHJ-NP-TVWXZ]+([^a-zA-Z]|$))", "\\1 \\3") +=
	    *new punctuations(br_puncts) +=
	    *new substitutor("[^-\\.,!?';:0-9a-zA-ZÀ-ÿ]", " "),
	    *new br_tts_params)
   ,my_expansion(new br_expansion)
{
}

br_language::~br_language()
{
  delete(my_expansion);
}

char *br_language::post_prepare (char *&text)
{
  return my_expansion->perform( text);
}

//>

br_language *Brazilian = new br_language();
