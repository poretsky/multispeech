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

#include "config.hpp"
#include "speech_server.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace FBB;


// Static data:
const string ru_tts::name("ru_tts");
const string ru_tts::lexicon_default_path(complete("freespeech/rulex.db", package::datadir).generic_string());
string ru_tts::executable(ru_tts::name);
string ru_tts::lexicon(ru_tts::lexicon_default_path);
string ru_tts::log_file;
double ru_tts::expressiveness = 1.0;
bool ru_tts::female_voice = false;
bool ru_tts::decimal_point = true;
bool ru_tts::decimal_comma = true;
double ru_tts::interclause_gap_factor = 1.0;
double ru_tts::comma_gap_factor = 1.0;
double ru_tts::dot_gap_factor = 1.0;
double ru_tts::semicolon_gap_factor = 1.0;
double ru_tts::colon_gap_factor = 1.0;
double ru_tts::question_gap_factor = 1.0;
double ru_tts::exclamation_gap_factor = 1.0;
double ru_tts::intonational_gap_factor = 1.0;


// Object construction:

ru_tts::ru_tts(void):
  speech_engine(name, "", lang_id::ru, soundfile::s8, 10000, 1, true, "KOI8-R")
{
  if (!executable.empty())
    {
      string cmd(executable);
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
      if (!lexicon.empty())
        {
          path lexicon(ru_tts::lexicon);
          if (exists(lexicon))
            cmd += " -s " + lexicon.generic_string();
          else
            {
              speech_server::log << SyslogStream::warning << lexicon.generic_string() << " does not exist" << endl;
              if (speech_server::verbose)
                cerr << "Warning: " << lexicon.generic_string() << " does not exist" << endl;
            }
        }
      if (!log_file.empty())
        cmd += " -l " + log_file;
      if (female_voice)
        cmd += " -a";
      if (version >= 6.0)
        {
          int flags = 0;
          if (!decimal_point)
            flags |= 1;
          if (!decimal_comma)
            flags |= 2;
          switch (flags)
            {
            case 1:
              cmd += " -d,";
              break;
            case 2:
              cmd += " -d.";
              break;
            case 3:
              cmd += " -d-";
              break;
            default:
              break;
            }
          if (expressiveness != 1.0)
            cmd += " -e " + lexical_cast<string>(expressiveness);
          if (interclause_gap_factor != 1.0)
            cmd += " -g " + lexical_cast<string>(interclause_gap_factor);
          if (comma_gap_factor != 1.0)
            cmd += " -g ," + lexical_cast<string>(comma_gap_factor);
          if (dot_gap_factor != 1.0)
            cmd += " -g ." + lexical_cast<string>(dot_gap_factor);
          if (semicolon_gap_factor != 1.0)
            cmd += " -g ;" + lexical_cast<string>(semicolon_gap_factor);
          if (colon_gap_factor != 1.0)
            cmd += " -g :" + lexical_cast<string>(colon_gap_factor);
          if (question_gap_factor != 1.0)
            cmd += " -g ?" + lexical_cast<string>(question_gap_factor);
          if (exclamation_gap_factor != 1.0)
            cmd += " -g !" + lexical_cast<string>(exclamation_gap_factor);
          if (intonational_gap_factor != 1.0)
            cmd += " -g -" + lexical_cast<string>(intonational_gap_factor);
        }
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
  else speech_engine::voicify(rate, pitch);
}
