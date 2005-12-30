// Language description classes
// This stuff does not depend on language itself

#ifndef LANGUAGE_H
#define LANGUAGE_H 1

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
  speech(voice &);
};

class language
{
 protected:
  virtual char* post_prepare (char *&);
  chain prepare_chain;

 private:
  regex_t *charset;
  chain clean;

 public:
  char *id;
  char* name;
  char* locale;

  language(char *, char *, char*, char *, chain &, voice &);
  language(char *, char *, char*, voice &);
  ~language(void);

  int check(char *);
  int sound_access(char *);
  char *say(char *&);
  char *letter(char *&);
  char *prepare (char *&);

};

#endif
