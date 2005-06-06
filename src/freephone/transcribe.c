/*
    Copyright (C) 1996 Alistair Conkie

    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

#include <sys/types.h>
#include <limits.h>

#if defined(FBSD_DATABASE)
#include <db.h>
#elif defined(BERKELEYDB)
#include <db_185.h>
#else
#include <ndbm.h>
#endif
#include <fcntl.h>

static char *lookup_db(char *word, CONFIG *config);
static void ToLower(char *word);

export void transcribe(CONFIG *config, LING_LIST *ling_list)
{
  int i;
  int count = ling_list->sz;
  char *tr;
  char *word;

  for(i=0;i<count;i++) {
    if(ling_list->text[i]->type == PUNCT) {
      ling_list->text[i]->transcription = strdup("");
    } else if((tr = lookup_db(ling_list->text[i]->word,config))) {
      tr = strdup(tr);
      tr = syllabify(tr,config);
      ling_list->text[i]->transcription = tr;
    } else {
      word = strdup(ling_list->text[i]->word);
      ToLower(word);
      if((tr = lookup_db(word,config))) {
	tr = strdup(tr);
	tr = syllabify(tr,config);
	ling_list->text[i]->transcription = tr;
      } else {
	tr = nrl_rules(word);
	tr = strdup(tr);
	tr = syllabify(tr,config);
	tr = stress(tr);
	ling_list->text[i]->transcription = tr;
      }
      free(word);
    }
    /* modify transcriptions..  */
  }
}

#if defined(FBSD_DATABASE) || defined(BERKELEYDB)

static char *lookup_db(char *word, CONFIG *config)
{
  DBT inKey, inVal;
 
  inKey.data = word;
  inKey.size = strlen(word);
 
  inVal.data = NULL;
  inVal.size = 0;
 
 
  if(config->db != NULL) {
    (((DB *)(config->db))->get)((DB *)(config->db),&inKey,&inVal,0);
    return(inVal.data);
  } else 
    return(NULL);
}

#else

static char *lookup_db(char *word, CONFIG *config)
{
  datum Key;

  Key.dptr = word;
  Key.dsize = strlen(word);

  if(config->db != NULL)
    return(dbm_fetch((DBM *)config->db,Key).dptr);
  else
    return(NULL);
}

#endif

static void ToLower(char *word)
{
  while(*word) {
    *word = tolower(*word);
    word++;
  }
}
