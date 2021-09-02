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

#include <cmath>
#include <map>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include "espeak.hpp"

#include "config.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;


// Espeak backend.

// Static data:
const char* const espeak::name = ESPEAK;
string espeak::executable(espeak::name);
string espeak::en(lang_id::en);
string espeak::de(lang_id::de);
string espeak::it(lang_id::it);
string espeak::fr(lang_id::fr);
string espeak::es(lang_id::es);
string espeak::pt(lang_id::pt);
string espeak::ru(lang_id::ru);

static const map<const char*, const string*> espeak_voices = map_list_of
  (lang_id::en, &espeak::en)
  (lang_id::de, &espeak::de)
  (lang_id::it, &espeak::it)
  (lang_id::fr, &espeak::fr)
  (lang_id::es, &espeak::es)
  (lang_id::pt, &espeak::pt)
  (lang_id::ru, &espeak::ru)
  .convert_to_container< map<const char*, const string*> >();

static const map<const char*, const string*> mbrola_voices = map_list_of
  (lang_id::en, &mbrola::en)
  (lang_id::de, &mbrola::de)
  (lang_id::it, &mbrola::it)
  (lang_id::fr, &mbrola::fr)
  (lang_id::es, &mbrola::es)
  (lang_id::pt, &mbrola::pt)
  .convert_to_container< map<const char*, const string*> >();

// Choose voice for language from provided map:
static const string&
getvoiceid(const char* lang, const map<const char*, const string*>& voices)
{
  return voices.count(lang) ? *voices.at(lang) : speech_engine::novoice;
}

// Object construction:
espeak::espeak(const char* lang):
  speech_engine(name, getvoiceid(lang, espeak_voices), lang, soundfile::autodetect, 22050, 1, true, "UTF-8")
{
  if (voice.empty())
    throw configuration::error(string(lang) + " voice for " + name + " is not specified");
  if (!executable.empty())
    {
      string cmd(executable);
      cmd += " --stdin --stdout -z -s %rate -p %pitch -v " + voice;
      command(cmd);
    }
  else throw configuration::error("no path to " + string(name));
}

// Making up voice parameters:
void
espeak::voicify(double rate, double pitch)
{
  format_macros[pitch_macro] = lexical_cast<string>((atan((pitch * pitch) - 1) * 50.0 / M_2_PI) + 50.0);
  format_macros[rate_macro] = lexical_cast<string>(rate * 170.0);
}


// Espeak+Mbrola backend.

// Object construction:
mbrespeak::mbrespeak(const char* lang):
  mbrola(COMPOSE(ESPEAK, MBROLA), getvoiceid(lang, mbrola_voices), lang)
{
  if (!espeak::executable.empty())
    {
      string cmd(espeak::executable);
      cmd += " --stdin -q --pho -z -v mb-" + voice;
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + espeak::name);
}
