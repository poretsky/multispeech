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


// Extract sound format specification from the configuration:

static soundfile::format
sound_format(const configuration& conf)
{
  soundfile::format result = soundfile::autodetect;
  if (conf.option_value.count(options::compose(speaker::user, option_name::format)))
    {
      string fmt(conf.option_value[options::compose(speaker::user, option_name::format)].as<string>());
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
    }
  return result;
}


// Object construction:

user_tts::user_tts(const configuration& conf, const string& lang):
  speech_engine(conf, speaker::user, "", lang, sound_format(conf),
                conf.option_value[options::compose(speaker::user, option_name::sampling)].as<unsigned int>(),
                conf.option_value[options::compose(speaker::user, option_name::stereo)].as<bool>() ? 2 : 1,
                !conf.option_value[options::compose(speaker::user, option_name::freq_control)].as<bool>(),
                conf.option_value[options::compose(speaker::user, option_name::charset)].as<string>())
{
  if (conf.option_value.count(options::compose(name, option_name::command)) &&
      !conf.option_value[options::compose(name, option_name::command)].as<string>().empty())
    command(conf.option_value[options::compose(name, option_name::command)].as<string>());
  else throw configuration::error("no command is specified for user defined backend");
}


// Private methods:

void
user_tts::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(rate);
}
