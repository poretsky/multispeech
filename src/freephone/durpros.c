/*
    Copyright (C) 1996 Alistair Conkie
 
    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

/* assignment of intrinsic durations  */
/* note that the section dealing with diacritics is rather poorly done  */

export void durpros(CONFIG *config, SPROSOD_LIST *spl)
{
  int i;
  SPROSOD *splp;
  char tph[5];
  int syll_ref = -1;
  KEY *tmp;

  for(i=spl->sz-1;i>=0;i--) {
    splp = spl->phoneme[i];
    if(strcmp(splp->phoneme,"#")) {
      if(splp->phoneme[0]!='_') {
	sscanf(splp->phoneme,"%[a-z]",tph);
      } else {
	sscanf(&(splp->phoneme[1]),"%[a-z]",tph);
      }
      if((tmp=binary(tph,dur0,config->dur0_num))!=NULL) {
	splp->dur = tmp->keycount; 
      } else {
	splp->dur = 100;
	fprintf(stderr,"Unrecognized phoneme: %s\n",tph);
      }
      if((syll_ref-splp->syll)==1) {
	splp->dur *=1.3;  /* phrase final lengthening  */
      }
    } else {
      syll_ref = splp->syll;
    }
  }
}

