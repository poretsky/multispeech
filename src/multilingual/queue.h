// Definition of the queue class

#ifndef QUEUE_H
#define QUEUE_H 1

class queue
{
 private:
  struct member
  {
    struct member *next;
    char *data;
  } *head, *tail;

 public:
  queue(void);
  ~queue(void);

  int empty(void);
  void clear(void);

  void operator << (char *);
  void operator << (queue &);
  void operator >> (char *&);
  void operator >> (queue &);
};

#endif
