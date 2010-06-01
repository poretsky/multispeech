// ru_tts.cpp -- Ru_tts speech backend implementation
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

#include <string>
#include <iostream>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <bobcat/syslogstream>

#include "ru_tts.hpp"

#include "server.hpp"
#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace FBB;


// Supported voices:
ru_tts::voice_attributes ru_tts::native =
  {
    "KOI8-R", lang_id::ru, "none", "",
    soundfile::s8, 10000, 1
  };


// Instantiation mechanism:
singleton<ru_tts> ru_tts::instance;


// Object construction:

ru_tts::ru_tts(void):
  speech_engine(speaker::ru_tts, true)
{
  string cmd(configuration::backend_executable(name));
  if (!cmd.empty())
    {
      cmd += " -r %rate -p %pitch";
      if (!configuration::backend_lexicon(name).empty())
        {
          path lexicon(configuration::backend_lexicon(name));
          if (exists(lexicon))
            cmd += " -s " + lexicon.file_string();
          else
            {
              server::log << SyslogStream::warning << lexicon.file_string() << " does not exist" << endl;
              if (server::verbose)
                cerr << "Warning: " << lexicon.file_string() << " does not exist" << endl;
            }
        }
      if (!configuration::backend_log(name).empty())
        cmd += " -l " + configuration::backend_log(name);
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
  voices[speaker::ru_tts] = &native;
  voice_setup(speaker::ru_tts);
}


// Making up voice parameters:

void
ru_tts::voicify(double rate, double pitch)
{
  double p = pitch * pitch;
  double r = 0.5;
  if (rate <= 0.0)
    r = 1.0;
  else if (rate >= 2.0)
    r = 0.0;
  else if (rate < 1.0)
    r = 1.0 / (rate + 1.0);
  else r = 1.0 / rate - 0.5;
  format_macros["%pitch"] = lexical_cast<string>(atan(p * p / 5.0) * M_2_PI);
  format_macros["%rate"] = lexical_cast<string>(r);
}

} // namespace multispeech
