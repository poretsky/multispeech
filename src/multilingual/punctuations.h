// Pronouncing of punctuations

#ifndef PUNCTUATIONS_H
#define PUNCTUATIONS_H 1

#include "chain.h"

class punctuations: public chain
{
 private:
  chain first_group;
  chain second_group;

  virtual char *perform(char *&);

 public:
  static int mode;

  punctuations(char **);
};

#endif
