/*
    Copyright (C) 1996 Alistair Conkie

    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

export char **split(char *in) 
{
  char **root;
  char *copy;
  int i = 0;
  int sz = 200;

  root = (char **)malloc(sz*sizeof(char *)); 

  copy = strdup(in);

  if((root[i]=strtok(copy," ")))
    i++;
  else
    return(root);
	

  while((root[i]=strtok(NULL," "))) {
    i++;
    if(i == sz) {
      sz += 200;
      root = (char **)realloc(root,sz*sizeof(char *));
    }
  }
  return(root);
}

export void tidy_split(char **root)
{
  free(root[0]); /* since by the function above it has sz  */
  free(root);
}


/* binary search  */
export KEY *binary(char *word, KEY tab[], int n)
{
  int cond;
  KEY *low = &tab[0];
  KEY *high = &tab[n-1];
  KEY *mid;

  while(low <= high) {
    mid = low + (high-low) / 2;
    if((cond = strcmp(word,mid->keyword)) < 0)
      high = mid-1;
    else if (cond>0)
      low = mid+1;
    else
      return(mid);
  }
  return(NULL);
}

/* more binary search  */
export PKEY *pbinary(char *word, PKEY tab[], int n)
{
  int cond;
  PKEY *low = &tab[0];
  PKEY *high = &tab[n-1];
  PKEY *mid;

  while(low <= high) {
    mid = low + (high-low) / 2;
    if((cond = strcmp(word,mid->keyword)) < 0)
      high = mid-1;
    else if (cond>0)
      low = mid+1;
    else
      return(mid);
  }
  return(NULL);
}

