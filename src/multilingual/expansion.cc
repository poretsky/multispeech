/*
 * November 2005, Gilles Casse <gcasse@oralux.org>
   Convert number to characters, expand the abbreviations.
   Port Freephone (interface.c, saynum.c,...) from C to C++ 
   for multispeech. 
   Freephone:
    Copyright (C) 1996 Alistair Conkie
 
    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "xmalloc.h"
#include "expansion.h"

//< data

#define MAX_LENGTH 128

// the following arrays concern the English language
// 
static char* Cardinals[] = 
{
  "zero","one","two","three",
  "four","five","six","seventh",
  "eight","nine",		
  "ten","eleven","twelve","thirteen",
  "fourteen","fifteen","sixteen","seventeen",
  "eighteen","nineteen"
};

static char* Twenties[] = 
{
  "twenty","thirty","fourty","fifty",
  "sixty","seventy","eighty","ninety"
};

static char* Ordinals[] = 
{
  "zeroth","first","second","third",
  "fourth","fifth","sixth","seventh",
  "eight","ninth",		
  "tenth","eleventh","twelfth", "thirteenth",
  "fourteenth","fifteenth","sixteenth","seventeenth",
  "eighteenth","nineteenth"
};

static char* Ord_twenties[] = 
{
  "twentieth","thirtieth","fortieth","fiftieth",
  "sixtieth","seventieth","eightieth","ninetieth"
};

//>
//< makeupper

int expansion::makeupper(int character)
{
  if (islower(character))
    return toupper(character);
  else
    return character;
}

//>
//< Xgetc

int expansion::Xgetc()
{
  return( myInput ? (int)myInput[myOffset++] : 0);
}

//>
//< newChar

int expansion::newChar()
{
  /*
    If the cache is full of newline, time to prime the look-ahead
    again.  If an '\0' is found, fill the remainder of the queue with
    '\0''s.
    */
  if (myChar == '\n'  && myChar1 == '\n' && myChar2 == '\n' && myChar3 == '\n')
    {	/* prime the pump again */
      myChar = Xgetc();
      if (myChar == '\0')
	{
	  myChar1 = '\0';
	  myChar2 = '\0';
	  myChar3 = '\0';
	  return myChar;
	}
      if (myChar == '\n')
	return myChar;

      myChar1 = Xgetc();
      if (myChar1 == '\0')
	{
	  myChar2 = '\0';
	  myChar3 = '\0';
	  return myChar;
	}
      if (myChar1 == '\n')
	return myChar;

      myChar2 = Xgetc();
      if (myChar2 == '\0')
	{
	  myChar3 = '\0';
	  return myChar;
	}
      if (myChar2 == '\n')
	return myChar;

      myChar3 = Xgetc();
    }
  else
    {
      /*
	Buffer not full of newline, shuffle the characters and
	either get a new one or propagate a newline or '\0'.
      */
      myChar = myChar1;
      myChar1 = myChar2;
      myChar2 = myChar3;
      if (myChar3 != '\n' && myChar3 != '\0')
	myChar3 = Xgetc();
    }
  return myChar;
}

//>
//< appendString

void expansion::appendString(const char* theNewString)
{
  if (theNewString)
    {
      appendString( theNewString, strlen(theNewString));
    }
}

void expansion::appendString(const char* theNewString, int theLength)
{
  if (!theNewString || !theLength)
    {
      return;
    }

  int aSize = theLength + 1;

  if (myBufferSize <= myBufferIndex + aSize)
    {
      myBufferSize = MIN_BUFFER * (1 + ((myBufferIndex + aSize) / MIN_BUFFER));
      char* aBuffer = (char*)xmalloc( myBufferSize);

      if (myBuffer)
	{
	  memcpy(aBuffer, myBuffer, myBufferIndex);
	  free(myBuffer);
	}

      myBuffer = aBuffer;
    }

  memcpy( myBuffer + myBufferIndex, theNewString, theLength);
  myBufferIndex += theLength; 
  myBuffer[ myBufferIndex] = 0; 
}

//>
//< putCardinal

// Translate a number to string and append it to myBuffer.  
// This version is for CARDINAL numbers.
// 	 Note: this is recursive.
void expansion::putCardinal(long int theValue)
{
  if (theValue < 0)
    {
      appendString(" minus");
      theValue = (-theValue);
      if (theValue < 0)	/* Overflow!  -32768 */
	{
	  appendString(" infinity");
	  return;
	}
    }

  if (theValue >= 1000000000L)	/* Billions */
    {
      putCardinal(theValue/1000000000L);
      appendString(" billion");
      theValue = theValue % 1000000000;
      if (theValue == 0)
	return;		/* Even billion */
      if (theValue < 100)	/* as in THREE BILLION AND FIVE */
	appendString(" and");
    }

  if (theValue >= 1000000L)	/* Millions */
    {
      putCardinal(theValue/1000000L);
      appendString(" million");
      theValue = theValue % 1000000L;
      if (theValue == 0)
	return;		/* Even million */
      if (theValue < 100)	/* as in THREE MILLION AND FIVE */
	appendString(" and");
    }

  /* Thousands 1000..1099 2000..99999 */
  /* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */
  if ((theValue >= 1000L && theValue <= 1099L) || theValue >= 2000L)
    {
      putCardinal(theValue/1000L);
      appendString(" thousand");
      theValue = theValue % 1000L;
      if (theValue == 0)
	return;		/* Even thousand */
      if (theValue < 100)	/* as in THREE THOUSAND AND FIVE */
	appendString(" and");
    }

  if (theValue >= 100L)
    {
      appendString(Cardinals[theValue/100]);
      appendString(" hundred");
      theValue = theValue % 100;
      if (theValue == 0)
	return;		/* Even hundred */
    }

  if (theValue >= 20)
    {
      appendString(Twenties[(theValue-20)/ 10]);
      theValue = theValue % 10;
      if (theValue == 0)
	return;		/* Even ten */
    }

  appendString(Cardinals[theValue]);
  return;
} 
//>
//< putOrdinal

// Translate a number to string and append it to myBuffer.  
// This version is for ORDINAL numbers.
// 	 Note: this is recursive.
void expansion::putOrdinal(long int theValue)
{
  if (theValue < 0)
    {
      appendString(" minus");
      theValue = (-theValue);
      if (theValue < 0)	/* Overflow!  -32768 */
	{
	  appendString(" infinity");
	  return;
	}
    }

  if (theValue >= 1000000000L)	/* Billions */
    {
      putCardinal(theValue/1000000000L);
      theValue = theValue % 1000000000;
      if (theValue == 0)
	{
	  appendString(" billionth");
	  return;		/* Even billion */
	}
      appendString(" billion");
      if (theValue < 100)	/* as in THREE BILLION AND FIVE */
	appendString(" and");
    }

  if (theValue >= 1000000L)	/* Millions */
    {
      putCardinal(theValue/1000000L);
      theValue = theValue % 1000000L;
      if (theValue == 0)
	{
	  appendString(" millionth");
	  return;		/* Even million */
	}
      appendString(" million");
      if (theValue < 100)	/* as in THREE MILLION AND FIVE */
	appendString(" and");
    }

  /* Thousands 1000..1099 2000..99999 */
  /* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */
  if ((theValue >= 1000L && theValue <= 1099L) || theValue >= 2000L)
    {
      putCardinal(theValue/1000L);
      theValue = theValue % 1000L;
      if (theValue == 0)
	{
	  appendString(" thousandth");
	  return;		/* Even thousand */
	}
      appendString(" thousand");
      if (theValue < 100)	/* as in THREE THOUSAND AND FIVE */
	appendString(" and");
    }

  if (theValue >= 100L)
    {
      appendString(Cardinals[theValue/100]);
      theValue = theValue % 100;
      if (theValue == 0)
	{
	  appendString(" hundredth");
	  return;		/* Even hundred */
	}
      appendString(" hundred");
    }

  if (theValue >= 20)
    {
      if ((theValue%10) == 0)
	{
	  appendString(Ord_twenties[(theValue-20)/ 10]);
	  return;		/* Even ten */
	}
      appendString(Twenties[(theValue-20)/ 10]);
      theValue = theValue % 10;
    }

  appendString(Ordinals[theValue]);
  return;
} 

//>
//< isOrdinal

int expansion::isOrdinal( long int theValue, int theLastdigit)
{
  int aResult=0;
  switch (theLastdigit)
    {
    case '1':	/* ST */
      if (makeupper(myChar) == 'S' && makeupper(myChar1) == 'T' &&
	  !isalpha(myChar2) && !isdigit(myChar2))
	{
	  putOrdinal(theValue);
	  newChar();	/* Used myChar */
	  newChar();	/* Used myChar1 */
	  aResult=1;
	}
      break;

    case '2':	/* ND */
      if (makeupper(myChar) == 'N' && makeupper(myChar1) == 'D' &&
	  !isalpha(myChar2) && !isdigit(myChar2))
	{
	  putOrdinal(theValue);
	  newChar();	/* Used myChar */
	  newChar();	/* Used myChar1 */
	  aResult=1;
	}
      break;

    case '3':	/* RD */
      if (makeupper(myChar) == 'R' && makeupper(myChar1) == 'D' &&
	  !isalpha(myChar2) && !isdigit(myChar2))
	{
	  putOrdinal(theValue);
	  newChar();	/* Used myChar */
	  newChar();	/* Used myChar1 */
	  aResult=1;
	}
      break;

    case '0':	/* TH */
    case '4':	/* TH */
    case '5':	/* TH */
    case '6':	/* TH */
    case '7':	/* TH */
    case '8':	/* TH */
    case '9':	/* TH */
      if (makeupper(myChar) == 'T' && makeupper(myChar1) == 'H' &&
	  !isalpha(myChar2) && !isdigit(myChar2))
	{
	  putOrdinal(theValue);
	  newChar();	/* Used myChar */
	  newChar();	/* Used myChar1 */
	  aResult=1;
	}
      break;
    }
  return aResult;
}

//>
//< putAscii
void expansion::putAscii( char c)
{
  char s[3];
  s[0]=' ';
  s[1]=c;
  s[2]=0;
  appendString( s);
}

//>
//< have_number

void expansion::have_number()
{
  long int aValue;
  int aLastdigit;

  aValue = myChar - '0';
  aLastdigit = myChar;

  if(aValue)
    {
      for (newChar() ; isdigit(myChar) ; newChar())
	{
	  aValue = 10 * aValue + (myChar - '0');
	  aLastdigit = myChar;
	}
    }
  else
    { // each zero will be spelled (1.000)
      newChar();
    }
  
  /* Recognize ordinals based on last digit of number */
  if (!isOrdinal( aValue, aLastdigit))
    {
      putCardinal(aValue);

      /* Recognize decimal points */
      if (isdigit(myChar1))
	{
	  if ((myChar == '.') || (myChar == ','))
	    {
	      sayChar(myChar);
	    }
	}

      /* Spell out trailing abbreviations */
      if (isalpha(myChar))
	{
	  while (isalpha(myChar))
	    {
	      putAscii( myChar);
	      newChar();
	    }
	}
    }
  return;
}

//>
//< have_letter

void expansion::have_letter()
{
  char buff[MAX_LENGTH];
  int count;

  count = 0;
  buff[count++] = ' ';	/* Required initial blank */

  //  buff[count++] = makeupper(myChar);
  buff[count++] = myChar;

  for (newChar() ; isalpha(myChar) || myChar == '\'' ; newChar())
    {
      //      buff[count++] = makeupper(myChar);
      buff[count++] = myChar;
      if (count > MAX_LENGTH-2)
	{
	  buff[count++] = ' ';
	  buff[count++] = '\0';
	  appendString(buff);
	  count = 1;
	}
    }

  buff[count++] = ' ';	/* Required terminating blank */
  buff[count++] = '\0';

  /* myCheck for AAANNN type abbreviations */
  if (isdigit(myChar))
    {
      appendString(buff);
      return;
    }
  else if (strlen(buff) == 3)	 /* one character, two spaces */
      putAscii( buff[1]);
  else if (myChar == '.')		/* Possible abbreviation */
    abbrev(buff);
  else
    appendString(buff);

  if (myChar == '-' && isalpha(myChar1))
    newChar();	/* Skip hyphens */
}

//>
//< expansion: constructor, destructor

expansion::expansion( char* theNumericPoint, char* theNumericComma)
  : myBuffer(NULL)
  , myBufferSize(0)
  , myBufferIndex(0)
{
  myNumericPoint = strdup( theNumericPoint);
  myNumericComma = strdup( theNumericComma);
}

expansion::~expansion()
{
  if(myBuffer)
    {
      free( myBuffer);
    }
  free( myNumericComma);
  free( myNumericPoint);
}

//>
//< have_special

void expansion::have_special()
{
  if ((myChar == '\n') || !isspace(myChar))
    {
      putAscii( myChar);
    }
  
  newChar();
}

//>
//< perform

char* expansion::perform(char *&theText)
{
  if (!theText)
    {
      return theText;
    }

  // copy the parameters field
  myOffset = 0;
  myInput = strchr(theText, '\n');

  if(!myInput || myBuffer || myBufferSize)
    {
      return theText;
    }
  myInput++;

  appendString( theText, myInput - theText);

  // process data
  myChar = '\n';
  myChar1 = '\n';
  myChar2 = '\n';
  myChar3 = '\n';
  newChar();	/* Fill myChar, myChar1, myChar2 and myChar3 */

  while (myChar != '\0')	/* All of the words in the file */
    {
      if (isdigit(myChar))
	have_number();
      else
	if (isalpha(myChar) || myChar == '\'')
	  have_letter();
	else
	  have_special();
    }

  free(theText);
  theText = myBuffer;
  myBuffer = NULL;
  myBufferSize = 0;
  myBufferIndex = 0;

  return theText;
}

//>
//< abbrev

/* Handle abbreviations.  Text in buff was followed by '.' */
void expansion::abbrev(char *buff)
{
  if (strcmp(buff, " SR ") == 0)
    {
      appendString(" señor ");
      newChar();
    }
  else if (strcmp(buff, " DR ") == 0)
    {
      appendString(" doctor ");
      newChar();
    }
  else
    appendString(buff);
}

//>
//< sayChar
void expansion::sayChar(char c)
{
  char s[3];
  sprintf(s, " %c", c);
  appendString( s);
}
//>
