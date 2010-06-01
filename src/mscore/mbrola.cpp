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

#include <sysconfig.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "mbrola.hpp"

#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;


// Known voices definition:
const mbrola::voice mbrola::en1 = (voice_attributes)
  {
    "", lang_id::en, "uk", "en1",
    soundfile::s16, 16000, 1
  };
const mbrola::voice mbrola::us1 = (voice_attributes)
  {
    "", lang_id::en, "us", "us1",
    soundfile::s16, 16000, 1
  };
const mbrola::voice mbrola::us2 = (voice_attributes)
  {
    "", lang_id::en, "us", "us2",
    soundfile::s16, 16000, 1
  };
const mbrola::voice mbrola::us3 = (voice_attributes)
  {
    "", lang_id::en, "us", "us3",
    soundfile::s16, 16000, 1
  };


// Object construction:

mbrola::mbrola(const string& backend):
  speech_engine(backend, false)
{
  string cmd(configuration::backend_executable(speaker::mbrola));
  if (!cmd.empty())
    {
      cmd += " -t %rate -f %pitch -l %freq -v 3.0 -e %mbrola_voice - -A";
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + speaker::mbrola);
}


// Checking voice availability:

bool
mbrola::check_voice(const string& voice_name)
{
  return exists(complete(voices[voice_name]->id, configuration::mbrola_voices()));
}


// Making up voice parameters:

void
mbrola::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(1.0 / rate);
  format_macros["%mbrola_voice"] = complete(current_voice->id, configuration::mbrola_voices()).file_string();
}


// Constructing voice name:

mbrola::voice::voice(const voice_attributes& attribs):
  voice_attributes(attribs)
{
}

mbrola::voice::operator string(void) const
{
  return string(speaker::mbrola + string("-") + id);
}

} // namespace multispeech
