
/*
    Copyright (C) 1996 Alistair Conkie

    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

static int nrules = 31;  /* are you sure??  */

static RULE alloetc[500] = {
  {"",NULL," ","",NULL," "}, /* accelerator  */
  {"s $",NULL,"t","^ [lwry]",NULL,"_t_"},
  {"s $",NULL,"p","^ [lwry]",NULL,"_p_"},
  {"s $",NULL,"k","^ [lwry]",NULL,"_k_"},

  {"",NULL,"k","^ [lwry] [\052\176aeiou]",NULL,"k_"},
  {"",NULL,"p","^ [lwry] [\052\176aeiou]",NULL,"p_"},
  {"",NULL,"s","^ [ktplwry] [\052\176aeiou]",NULL,"s_"},
  {"",NULL,"t","^ [lwry] [\052\176aeiou]",NULL,"t_"},

  {"[kpt] $",NULL,"r","^ [\052\176aeiou]",NULL,"_r"},
  {"[kpst] $",NULL,"w","^ [\052\176aeiou]",NULL,"_w"},
  {"[kpst] $",NULL,"y","^ [\052\176aeiou]",NULL,"_y"},
  {"[kpst] $",NULL,"l","^ [\052\176aeiou]",NULL,"_l"},

  {"s $",NULL,"p","^ [\052\176aeiou]",NULL,"_p"},
  {"s $",NULL,"t","^ [\052\176aeiou]",NULL,"_t"},
  {"s $",NULL,"k","^ [\052\176aeiou]",NULL,"_k"},

  {"",NULL,"p","^ [\055\134\056\053|\043] [\052\176aeiou]",NULL,"p$"},
  {"",NULL,"t","^ [\055\134\056\053|\043] [\052\176aeiou]",NULL,"t$"},
  {"",NULL,"k","^ [\055\134\056\053|\043] [\052\176aeiou]",NULL,"k$"},

  {" $",NULL,"i","^ [\055\134\056\053|\043] [\052\176aeiouwy]",NULL,"ee"},
  {" $",NULL,"i","^ (\043\043|L|M|N)",NULL,"ee"},

  {"",NULL,"oo","^ [\055\056\053|\043] [\052\176aeiou]",NULL,"uu"},
  {"",NULL,"oo","^ (\043\043|L|M|N)",NULL,"uu"},

  {"",NULL,"ai r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"air"},
  {"",NULL,"ee r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"eer"},
  {"",NULL,"uu r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"oor"},
  {"",NULL,"ie r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"ie @"},
  {"",NULL,"oa r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"oa @"},
  {"",NULL,"oi r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"oi @"},
  {"",NULL,"ou r","^( [\055\134\056\053|\043])? [^\052\176aeiou]",NULL,"ou @"},

  {" $",NULL,"r ","^([\055\134\056\053|\043] )?[^\052\176aeiou]",NULL,""},
  {" $",NULL,"r \043\043","",NULL,"\043\043"}};
/* have you counted right??  */

int read_rule_file(char *filename, RULE *rules);


export void load_context_rules(char * filename)
{
  /*
    nrules = read_rule_file(filename,alloetc);
    */
}

/* replace string in situ  */
export void phon_rules_init()
{
  int i;

  for(i=0;i<nrules;i++) {
    alloetc[i].lc = regcomp(alloetc[i].left_context);
    alloetc[i].rc = regcomp(alloetc[i].right_context);
  }
}

export void phon_rules_free()
{
  int i;

  for(i=0;i<nrules;i++) {
    free(alloetc[i].lc);
    free(alloetc[i].rc);
  }
}

export void phon_rules(SENT *sent)
{
  rule_exec(SAME,nrules,alloetc,sent);
}

