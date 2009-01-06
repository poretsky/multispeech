// mbrola.cpp -- Mbrola based speech engine implementation
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

#include <string>
#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>

#include "mbrola.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;


// Object construction:

mbrola::mbrola(const configuration& conf,
               const string& backend,
               const string& voice_id,
               language_description* lang,
               unsigned int sampling):
  speech_engine(conf, backend, voice_id, lang, soundfile::s16, sampling, 1, false)
{
  if (voice.empty())
    throw configuration::error(language->id + " voice for " + name + " is not specified");
  if (conf.option_value.count(options::compose(speaker::mbrola, option_name::executable)) &&
      !conf.option_value[options::compose(speaker::mbrola, option_name::executable)].as<string>().empty())
    {
      string cmd(conf.option_value[options::compose(speaker::mbrola, option_name::executable)].as<string>());
      cmd += " -t %rate -f %pitch -l %freq -v 3.0 -e ";
      if (conf.option_value.count(options::compose(speaker::mbrola, option_name::voices)))
        {
          path voice_file(complete(voice,
                                   conf.option_value[options::compose(speaker::mbrola, option_name::voices)].as<string>()));
          if (exists(voice_file))
            cmd += voice_file.file_string();
          else throw configuration::error(voice_file.file_string() + " does not exist");
        }
      else throw configuration::error(string("no path to ") + speaker::mbrola + " voices");
      cmd += " - -A";
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + speaker::mbrola);
}


// Making up voice parameters:

void
mbrola::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(1.0 / rate);
}
