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

#include <boost/lexical_cast.hpp>

#include "user_tts.hpp"

using namespace std;
using namespace boost;


// Static data:

string user_tts::command;
string user_tts::format;
unsigned int user_tts::sampling = 22050;
bool user_tts::stereo = false;
bool user_tts::freq_control = false;
string user_tts::charset;


// Extract sound format specification from the configuration:

static soundfile::format
sound_format(const string& fmt)
{
  soundfile::format result = soundfile::autodetect;
  if (!fmt.empty())
    {
      if ("s8" == fmt)
        result = soundfile::s8;
      else if ("u8" == fmt)
        result = soundfile::u8;
      else if ("s16" == fmt)
        result = soundfile::s16;
      else throw configuration::error("unknown sound format specification \"" + fmt + '\"');
    }
  return result;
}


// Object construction:

user_tts::user_tts(const configuration& conf, const string& lang):
  speech_engine(conf, speaker::user, "", lang, sound_format(format), sampling, stereo ? 2 : 1, !freq_control, charset)
{
  if (!command.empty())
    speech_engine::command(user_tts::command);
  else throw configuration::error("no command is specified for user defined backend");
}


// Private methods:

void
user_tts::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(rate);
}
