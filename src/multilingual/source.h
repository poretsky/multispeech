// Source of input
// Usually, parent process

#ifndef SOURCE_H
#define SOURCE_H 1

class parser
{
 private:
  char *cmd;

 public:
  char *params(char *);
  int operator == (char *);
};

class source
{
 private:
  char *buffer;
  unsigned int bufsize;

 public:
  char *data;
  parser command;

  source(void);
  ~source(void);

  int read(void);
};

#endif
