// Base chain class

#ifndef CHAIN_H
#define CHAIN_H 1

class chain
{
 private:
  chain *link;

 protected:
  virtual char *perform(char *&);

 public:
  chain(void);

  char *operator () (char *&);
  chain &operator += (chain &);
};

#endif
