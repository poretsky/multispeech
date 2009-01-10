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

#include <string>
#include <iostream>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <bobcat/syslogstream>

#include "ru_tts.hpp"

#include "server.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace FBB;


// Object construction:

ru_tts::ru_tts(const configuration& conf):
  speech_engine(conf, speaker::ru_tts, "", lang_id::ru, soundfile::s8, 10000, 1, true, "KOI8-R")
{
  if (conf.option_value.count(options::compose(name, option_name::executable)) &&
      !conf.option_value[options::compose(name, option_name::executable)].as<string>().empty())
    {
      string cmd(conf.option_value[options::compose(name, option_name::executable)].as<string>());
      cmd += " -r %rate -p %pitch";
      if (conf.option_value.count(options::compose(name, option_name::lexicon)) &&
          !conf.option_value[options::compose(name, option_name::lexicon)].as<string>().empty())
        {
          path lexicon(conf.option_value[options::compose(name, option_name::lexicon)].as<string>());
          if (exists(lexicon))
            cmd += " -s " + lexicon.file_string();
          else
            {
              server::log << SyslogStream::warning << lexicon.file_string() << " does not exist" << endl;
              if (server::verbose)
                cerr << "Warning: " << lexicon.file_string() << " does not exist" << endl;
            }
        }
      if (conf.option_value.count(options::compose(name, option_name::log)) &&
          !conf.option_value[options::compose(name, option_name::log)].as<string>().empty())
        cmd += " -l " + conf.option_value[options::compose(name, option_name::log)].as<string>();
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
}


// Making up voice parameters:

void
ru_tts::voicify(double rate, double pitch)
{
  double p = pitch * pitch;
  double r = 0.25 / rate - 0.2;
  if (r < 0.0) r = 0.0;
  if (r > 1.0) r = 1.0;
  format_macros["%pitch"] = lexical_cast<string>(atan(p * p / 5.0) * M_2_PI);
  format_macros["%rate"] = lexical_cast<string>(r);
}
