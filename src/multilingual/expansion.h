// Convert number to characters, expand the abbreviations.

#ifndef EXPANSION_H
#define EXPANSION_H 1

class expansion
{
 public:
  // constructor: 
  // theDecimalPoint: e.g " point" in English
  expansion( char* theNumericPoint, char* theNumericComma);

  ~expansion();

  char* perform(char *&theText);

 protected:
  enum {MIN_BUFFER=1024};
  virtual int isOrdinal( long int aValue, int aLastdigit);
  virtual void putCardinal(long int theValue);
  virtual void putOrdinal(long int theValue);
  void appendString(const char* theNewString);
  void appendString(const char* theNewString, int theLength);
  int makeupper(int character);
  int newChar();
  void abbrev(char *buff);
  int Xgetc();
  void have_number();
  void have_letter();
  void have_special();
  void putAscii( char c);
  virtual void sayChar(char c);

  int myChar, myChar1, myChar2, myChar3;
  int myOffset;
  char *myInput;
  char* myNumericPoint; // " point" in English
  char* myNumericComma; // 
  char* myBuffer;
  int myBufferSize;
  int myBufferIndex; 
};

#endif
