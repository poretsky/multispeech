// Global regular substitution mechanism

#ifndef SUBSTITUTOR_H
#define SUBSTITUTOR_H 1

#include "regexp.h"
#include "chain.h"

class substitutor: public chain
{
 private:
  regexp *pattern;
  char *replacement;

 protected:
  virtual char *perform(char *&);

 public:
  substitutor(char *, char *);
  ~substitutor(void);
};

#endif
