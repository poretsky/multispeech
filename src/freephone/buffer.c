
/*
 * Buffer operations.
 *
 */

#include "t2s.h"

export char *buffer_text(BUFFER *buffer)
{
  return buffer->text;
}

export void buffer_init(BUFFER *buffer)
{
  buffer->size = 100;
  buffer->fill = 0;
  buffer->text = (char *)malloc(buffer->size + 1);
  buffer->text[0] = 0;
}

static void buffer_require(BUFFER *buffer, int space)
{
  if (space <= buffer->size) return;
  space = ((space*3)/2) + 100;
  buffer->text = (char *)realloc(buffer->text, space+1);
  buffer->size = space;
  buffer->text[space] = 0;
}

export void buffer_add_char(BUFFER *buffer, int c)
{
  int fill = buffer->fill;
  buffer_require(buffer, fill + 1);
  buffer->text[fill] = c;
  buffer->text[fill+1] = 0;
  buffer->fill = fill+1;
}

export void buffer_add_str(BUFFER *buffer, char *str)
{
  int len = strlen(str);
  int fill = buffer->fill;
  buffer_require(buffer, fill+len);
  strcpy(buffer->text + fill, str);
  buffer->fill = fill + len;
}

export char buffer_last_char(BUFFER *buffer)
{
  if (buffer->fill==0) return 0;
  return buffer->text[buffer->fill-1];
}

export void buffer_clear(BUFFER *buffer)
{
  buffer->fill = 0;
  buffer->text[0] = 0;
}

export void buffer_free(BUFFER *buffer) 
{
  free(buffer->text);
}


