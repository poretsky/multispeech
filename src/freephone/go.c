/*
    Copyright (C) 1996 Alistair Conkie
 
    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

static void zero_things(BUFFER *buffer, LING_LIST *ling_list)
{
  int i;

  /* first the buffer  */
  buffer_clear(buffer);

  /* then ling_list  */
  for(i=0;i<ling_list->sz;i++) {
    free(ling_list->text[i]->word);
    free(ling_list->text[i]->transcription);
  }
}

export void go(CONFIG *config, BUFFER *buffer, LING_LIST *ling_list, SENT *sent, SPROSOD_LIST *spl, SPN *ps, ACOUSTIC *as) 
{
  int c, c1;
  
  while (1) {
    c = getc(config->ifd);
    switch(c) {
    case EOF:
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, '.');
      buffer_add_char(buffer, ' ');
      process_sentence(config, buffer, ling_list, sent, spl, ps, as);
      zero_things(buffer, ling_list);
      return;
      
    case '.' : case '?' : case '!':
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, c);
      buffer_add_char(buffer, ' ');
      c1 = getc(config->ifd);
      if(c1==' ' || c1=='\t' || c1=='\n') {
	process_sentence(config, buffer, ling_list, sent, spl, ps, as);
	zero_things(buffer, ling_list);
      } else { ungetc(c1, config->ifd); }
      break;
      
    case '(' : case ')' : case '[' :
    case ']' : case '{' : case '}' :
    case ':' : case ';' : case ',' :
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, c);
      buffer_add_char(buffer, ' ');
      break;
      
    case '\n' :
      buffer_add_char(buffer, ' ');
      c1 = getc(config->ifd);
      if(c1=='\n') {
	process_sentence(config, buffer, ling_list, sent, spl, ps, as);
	zero_things(buffer, ling_list);
      } else if(c1==EOF) {
        buffer_add_char(buffer, ' ');
        buffer_add_char(buffer, '.');
        buffer_add_char(buffer, ' ');
        process_sentence(config, buffer, ling_list, sent, spl, ps, as);
        zero_things(buffer, ling_list);
        return;
      } else { ungetc(c1, config->ifd); }
      break;
      
    case ' ' : case '\t' :
      buffer_add_char(buffer, ' ');
      break;
      
    default :
      buffer_add_char(buffer, c);
      break;
    }
  }
}

export void go3(CONFIG *config, BUFFER *buffer, LING_LIST *ling_list, SENT *sent, SPROSOD_LIST *spl, SPN *ps) 
{
  int c, c1;

  while(1) {
    c = getc(config->ifd);

    switch(c) {
    case EOF:
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, '.');
      buffer_add_char(buffer, ' ');
      produce_spn_file(config, buffer, ling_list, sent, spl, ps);
      zero_things(buffer, ling_list);
      return;

    case '.' : case '?' : case '!' :
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, c);
      buffer_add_char(buffer, ' ');
      c1 = getc(config->ifd);
      if(c1==' ' || c1=='\t' || c1=='\n') {
	produce_spn_file(config, buffer, ling_list, sent, spl, ps);
	zero_things(buffer, ling_list);
      } else { ungetc(c1, config->ifd); }
      break;
	
    case '(' : case ')' : case '[' :
    case ']' : case '{' : case '}' :
    case ':' : case ';' : case ',' :
      buffer_add_char(buffer, ' ');
      buffer_add_char(buffer, c);
      buffer_add_char(buffer, ' ');
      break;
      
    case '\n' :
      buffer_add_char(buffer, ' ');
      c1 = getc(config->ifd);
      if(c1=='\n') {
	produce_spn_file(config, buffer, ling_list, sent, spl, ps);
	zero_things(buffer, ling_list);
      } else if(c1==EOF) {
        buffer_add_char(buffer, ' ');
        buffer_add_char(buffer, '.');
        buffer_add_char(buffer, ' ');
	produce_spn_file(config, buffer, ling_list, sent, spl, ps);
        zero_things(buffer, ling_list);
        return;
      } else { ungetc(c1, config->ifd); }
      break;
      
    case ' ' : case '\t' :
      buffer_add_char(buffer, ' ');
      break;
      
    default :
      buffer_add_char(buffer, c);
      break;
    }
  }
}

export void go2(CONFIG *config, SPN *ps, ACOUSTIC *as) 
{
  process_spn_file(config, ps, as);
}
	
