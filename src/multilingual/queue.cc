// Queue manipulation functions

#include <stdlib.h>
#include "queue.h"

queue::queue(void)
{
  head = NULL;
  tail = NULL;
}

queue::~queue(void)
{
  clear();
}

int queue::empty(void)
{
  return !head;
}

void queue::clear(void)
{
  while (head)
    {
      char *tmp;
      *this>>tmp;
      free(tmp);
    }
  tail = NULL;
}

void queue::operator << (char *data)
{
  if (!data) return;
  if (head)
    {
      tail->next = new member;
      tail = tail->next;
    }
  else
    {
      head = new member;
      tail = head;
    }
  tail->data = data;
  tail->next = NULL;
}

void queue::operator << (queue &data)
{
  clear();
  head = data.head;
  tail = data.tail;
  data.head = NULL;
  data.tail = NULL;
}

void queue::operator >> (char *&data)
{
  if (head)
    {
      struct member *tmp;
      data = head->data;
      tmp = head->next;
      delete head;
      head = tmp;
    }
  else data = NULL;
}

void queue::operator >> (queue &data)
{
  data.clear();
  data.head = head;
  data.tail = tail;
  head = NULL;
  tail = NULL;
}
