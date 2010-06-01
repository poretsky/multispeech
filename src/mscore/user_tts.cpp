// user_tts.cpp -- User specified speech backend implementation
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>
   This file is part of Multispeech.

   Multispeech is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Multispeech is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Multispeech; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <sysconfig.h>

#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>

#include "user_tts.hpp"

#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;
using namespace boost::assign;


// Instantiation mechanism:
singleton<user_tts> user_tts::instance;


// Object construction:

user_tts::user_tts(void):
  speech_engine(speaker::user_defined, !configuration::user_freq_control()),
  en(make_voice(lang_id::en)),
  ru(make_voice(lang_id::ru))
{
  if (!configuration::user_tts_command().empty())
    command(configuration::user_tts_command());
  else throw configuration::error("no command is specified for user defined backend");
  voices[voice(lang_id::en)] = &en;
  voices[voice(lang_id::ru)] = &ru;
}


// Public methods:

string
user_tts::voice(const string& language)
{
  return string(speaker::user_defined + string("-") + language);
}


// Private methods:

void
user_tts::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(rate);
}

user_tts::voice_attributes
user_tts::make_voice(const char* language)
{
  voice_attributes voice =
    {
      configuration::user_tts_charset(language),
      language,
      configuration::user_tts_dialect(language),
      language,
      configuration::user_sound_format(language),
      configuration::user_sampling(language),
      configuration::user_sound_channels(language)
    };
  return voice;
}

} // namespace multispeech
