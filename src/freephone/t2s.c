/*
    Copyright (C) 1996 Alistair Conkie

    You may distribute under the terms of the GNU General Public
    Licence as specified in the README file.
*/

#include "t2s.h"

export void process_sentence(CONFIG *config, BUFFER *buffer, LING_LIST *ling_list, SENT *sent, SPROSOD_LIST *spl, SPN *ps, ACOUSTIC *as)
{
  high_level(config,buffer,ling_list,sent,spl,ps);

  low_level(config,ps,as);

  fflush(config->ofd);
}

export void produce_spn_file(CONFIG *config, BUFFER *buffer, LING_LIST *ling_list, SENT *sent, SPROSOD_LIST *spl, SPN *ps)
{
  high_level(config,buffer,ling_list,sent,spl,ps);
  
  if(config->type == _SPN_) {
    put_spn_data(config,ps);
  } else if(config->type == _MBROLA_) {
    put_mbrola_data(config,ps);
  }
  
  fflush(config->ofd);
}

export void process_spn_file(CONFIG *config, SPN *ps, ACOUSTIC *as)
{
  get_spn_data(config,ps);

  low_level(config,ps,as);

  fflush(config->ofd);
}

export void high_level(CONFIG *config, BUFFER *buffer, LING_LIST *ling_list, SENT *sent, SPROSOD_LIST *spl, SPN *ps)
{
  tags(config,buffer,ling_list); 

  grammar(ling_list);

  transcribe(config,ling_list);

  conv1(config,ling_list,sent);
  phon_rules(sent);
  conv2(config,sent,spl);

  durpros(config,spl);

  prosody(spl,ps);
}

export void low_level(CONFIG *config, SPN *ps, ACOUSTIC *as)
{
  phonstoframes(ps,as);

  durations(config,ps,as);       

  calc_pitch(config,ps,as);      
}
