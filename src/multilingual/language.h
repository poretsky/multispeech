// Language description classes
// This stuff does not depend on language itself

#ifndef LANGUAGE_H
#define LANGUAGE_H 1

#include "regexp.h"
#include "chain.h"

class voice: public chain
{
 protected:
  static char buf[32];

 public:
  static double ra, pi, vo;
  static int fr;

  voice(void);
};

class speech: public chain
{
 private:
  chain params;

  virtual char *perform(char *&);

 public:
  static double pitch, volume, char_factor, caps_factor;
  static int rate, frequency, split_caps, capitalize, space_special_chars;

  speech(chain &, voice &);
};

class language
{
 private:
  regexp *charset;
  chain clean;

 public:
  char *id;
  chain prepare;

  language(char *, char *, chain &, voice &);
  ~language(void);

  int check(char *);
  int sound_access(char *);
  char *say(char *&);
  char *letter(char *&);
};

#endif
