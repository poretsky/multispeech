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

#include <boost/lexical_cast.hpp>

#include "espeak.hpp"

using namespace std;
using namespace boost;


// Espeak backend.

// Object construction:
espeak::espeak(const configuration& conf, const string& lang):
  speech_engine(conf, speaker::espeak, novoice, lang, soundfile::autodetect, 22050, 1, true, "UTF-8")
{
  if (voice.empty())
    throw configuration::error(lang + " voice for " + name + " is not specified");
  if (conf.option_value.count(options::espeak::executable) &&
      !conf.option_value[options::espeak::executable].as<string>().empty())
    {
      string cmd(conf.option_value[options::espeak::executable].as<string>());
      cmd += " --stdin --stdout -z -s %rate -p %pitch -v " + voice;
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
}

// Making up voice parameters:
void
espeak::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>((atan((pitch * pitch) - 1) * 50.0 / M_2_PI) + 50.0);
  format_macros["%rate"] = lexical_cast<string>(rate * 170.0);
}


// Espeak+Mbrola backend.

// Object construction:
mbrespeak::mbrespeak(const configuration& conf, const string& lang):
  mbrola(conf, options::compose(speaker::espeak, speaker::mbrola), novoice, lang)
{
  if (conf.option_value.count(options::espeak::executable) &&
      !conf.option_value[options::espeak::executable].as<string>().empty())
    {
      string cmd(conf.option_value[options::espeak::executable].as<string>());
      cmd += " --stdin -q --pho -z -v mb-" + voice;
      command(cmd);
    }
  else throw configuration::error(string("no path to ") + speaker::espeak);
}
