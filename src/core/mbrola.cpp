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

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "mbrola.hpp"
#include "espeak.hpp"

#include "config.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::filesystem;


// Static data:
const char* const mbrola::name = MBROLA;
const string mbrola::voices_default_path(complete("mbrola", DATA_DIR).generic_string());
string mbrola::executable(mbrola::name);
string mbrola::voices_path(mbrola::voices_default_path);
string mbrola::en("en1");
string mbrola::de("de6");
string mbrola::it("it3");
string mbrola::fr("fr4");
string mbrola::es("es1");
string mbrola::pt("br3");

static const map<const char*, const string*> voices = map_list_of
  (lang_id::en, &mbrola::en)
  (lang_id::de, &mbrola::de)
  (lang_id::it, &mbrola::it)
  (lang_id::fr, &mbrola::fr)
  (lang_id::es, &mbrola::es)
  (lang_id::pt, &mbrola::pt)
  .convert_to_container< map<const char*, const string*> >();


// Object construction:

mbrola::mbrola(const char* lang):
  mbrola(MBROLA, getvoiceid(lang, voices), lang)
{
  if (!espeak::executable.empty())
    {
      string cmd(espeak::executable);
      cmd += " --stdin -q --pho -z -v mb-" + voice;
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + espeak::name);
}

mbrola::mbrola(const string& backend,
               const string& voice_id,
               const char* lang):
  speech_engine(backend, voice_id, lang, soundfile::s16, 16000, 1, false)
{
  if (voice.empty())
    throw configuration::error(string(lang) + " voice for " + name + " is not specified");
  if (!executable.empty())
    {
      string cmd(executable);
      cmd += " -t %rate -f %pitch -l %freq -v ";
      // The en1 voice is especially quiet.
      cmd += (voice == "en1") ? "3.0" : "1.0";
      cmd += " -e ";
      if (!voices_path.empty())
        {
          path voice_path(complete(voice, voices_path));
          path voice_file(complete(voice, voice_path));
          if (exists(voice_file))
            cmd += voice_file.generic_string();
          else if (exists(voice_path))
            cmd += voice_path.generic_string();
          else throw configuration::error(voice_path.generic_string() + " does not exist");
        }
      else throw configuration::error(string("no path to ") + name + " voices");
      cmd += " - -A";
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + name);
  // Take in account some especial voices sampling rate
  if ((voice == "de5") || (voice == "de6") ||
      (voice == "de7") || (voice == "es2"))
    sampling(22050);
}


// Making up voice parameters:

void
mbrola::voicify(double rate, double pitch)
{
  format_macros[pitch_macro] = lexical_cast<string>(pitch);
  format_macros[rate_macro] = lexical_cast<string>(1.0 / rate);
}
