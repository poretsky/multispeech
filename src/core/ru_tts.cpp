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
#include <cstdio>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <bobcat/syslogstream>

#include "ru_tts.hpp"

#include "speech_server.hpp"

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
      ostringstream info;
      info << cmd << " -v 2>&1";
      FILE* backend = popen(info.str().c_str(), "r");
      version = 0.0;
      if (backend)
        {
          regex version_format("[Vv]ersion +(\\d+\\.\\d+)");
          smatch versioninfo;
          char s[100];
          info.str("");
          info.clear();
          while (fgets(s, 80, backend))
            info << s;
          pclose(backend);
          string response(info.str());
          if (regex_search(response, versioninfo, version_format)
              && versioninfo[1].matched)
            version = lexical_cast<double>(string(versioninfo[1].first, versioninfo[1].second));
        }
      cmd += " -r %rate -p %pitch";
      if (conf.option_value.count(options::compose(name, option_name::lexicon)) &&
          !conf.option_value[options::compose(name, option_name::lexicon)].as<string>().empty())
        {
          path lexicon(conf.option_value[options::compose(name, option_name::lexicon)].as<string>());
          if (exists(lexicon))
            cmd += " -s " + lexicon.generic_string();
          else
            {
              speech_server::log << SyslogStream::warning << lexicon.generic_string() << " does not exist" << endl;
              if (speech_server::verbose)
                cerr << "Warning: " << lexicon.generic_string() << " does not exist" << endl;
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
  if (version < 6.0)
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
  else
    {
      format_macros["%pitch"] = lexical_cast<string>(pitch);
      format_macros["%rate"] = lexical_cast<string>(rate);
    }
}
