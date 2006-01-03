// Global regular substitution mechanism

#ifndef SUBSTITUTOR_H
#define SUBSTITUTOR_H 1

class substitutor: public chain
{
 private:
  char symbol;
  regex_t *pattern;
  char *replacement;

  bool search(char*, regmatch_t*);

 protected:
  virtual char *perform(char *&);

 public:
  substitutor(char, char *);
  substitutor(char *, char *);
  virtual ~substitutor(void);
};

#endif
