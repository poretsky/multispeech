// Commands execution by running appropriate scripts

#ifndef SUBPROCESS_H
#define SUBPROCESS_H 1

#include "queue.h"

class subprocess
{
 private:
  static int pid, fd, flag;
  char *dsp_device;

  void run(char *, char *, char **);

 public:
  int busy(void);
  void abort(void);

  void operator << (char *);
  void operator << (queue &);
};

#endif
