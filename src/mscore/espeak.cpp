// espeak.cpp -- Espeak based speech backends implementation
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

#include <cmath>

#include <boost/lexical_cast.hpp>

#include "espeak.hpp"

#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;


// Objects instantiation mechanism:
singleton<espeak> espeak::instance;
singleton<mbrespeak> mbrespeak::instance;


// Espeak backend.

// Voices definition:
const espeak::voice espeak::en_default = (voice_attributes)
  {
    "", lang_id::en, "none", "default",
    soundfile::autodetect, 22050, 1
  };
const espeak::voice espeak::ru = (voice_attributes)
  {
    "UTF-8", lang_id::ru, "none", "ru",
    soundfile::autodetect, 22050, 1
  };


// Object construction:
espeak::espeak(void):
  speech_engine(speaker::espeak, true)
{
  string cmd(configuration::backend_executable(name));
  if (!cmd.empty())
    {
      cmd += " --stdin --stdout -z -s %rate -p %pitch -v %voice";
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
  voices[en_default] = &en_default;
  voices[ru] = &ru;
}

// Making up voice parameters:
void
espeak::voicify(float rate, float pitch)
{
  format_macros["%pitch"] = lexical_cast<string>((atan((pitch * pitch) - 1) * 50.0 / M_2_PI) + 50.0);
  format_macros["%rate"] = lexical_cast<string>(rate * 170.0);
}


// Constructing voice name:

espeak::voice::voice(const voice_attributes& attribs):
  voice_attributes(attribs)
{
}

espeak::voice::operator string(void) const
{
  return string(speaker::espeak + string("-") + id);
}


// Espeak+Mbrola backend.

// Object construction:
mbrespeak::mbrespeak(void):
  mbrola(speaker::espeak)
{
  string cmd(configuration::backend_executable(speaker::espeak));
  if (!cmd.empty())
    {
      cmd += " --stdin -q -z -v mb-%voice";
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + speaker::espeak);
  voices[en1] = &en1;
  voices[us1] = &us1;
  voices[us2] = &us2;
  voices[us3] = &us3;
}

} // namespace multispeech
