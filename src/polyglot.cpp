// polyglot.cpp -- General text-to-speech transfer control implementation
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

#include <iostream>

#include <bobcat/syslogstream>

#include <boost/assign.hpp>

#include "polyglot.hpp"

#include "server.hpp"
#include "freephone.hpp"
#include "ru_tts.hpp"
#include "espeak.hpp"

using namespace std;
using namespace FBB;
using namespace boost;
using namespace boost::assign;


// Supported languages:
const vector<string> polyglot::langs = list_of
  // Follow detection order
  (lang_id::ru)
  (lang_id::en);


// Construct the object:

polyglot::polyglot(const configuration& conf):
  talker(langs.size()),
  lang(langs.size()),
  autolanguage(false)
{
  bool initialized = false;
  for (int i = 0; i < langs.size(); i++)
    if (conf.option_value.count(options::compose(langs[i], option_name::engine)))
      {
        talker[i].reset(speech_backend(conf.option_value[options::compose(langs[i], option_name::engine)].as<string>(),
                                       langs[i], conf));
        initialized = true;
      }
  if (!initialized)
    throw configuration::error("no speech backends are defined");
  if (conf.option_value.count(options::speech::language))
    language(conf.option_value[options::speech::language].as<string>());
  else language(lang_id::autodetect);
  if (autolanguage)
    {
      if (conf.option_value.count(options::speech::language) &&
          (conf.option_value[options::speech::language].as<string>() != lang_id::autodetect))
        throw configuration::error("unsupported language " +
                                  conf.option_value[options::speech::language].as<string>());
      for (int i = 0; i < langs.size(); i++)
        {
          if ((lang < langs.size()) && talker[lang].get())
            break;
          lang = i;
        }
    }
  if ((lang >= langs.size()) || !talker[lang].get())
    throw configuration::error("no speech backend for chosen language");
}


// Public methods:

speech_task
polyglot::text_task(const wstring& s, bool use_translation)
{
  if (autolanguage)
    detect_language(s, use_translation);
  if (talker[lang].get())
    return talker[lang]->text_task(s, use_translation);
  return speech_task();
}

speech_task
polyglot::text_task(const wstring& s,
                double volume, double rate,
                double pitch, double deviation,
                bool use_translation)
{
  if (autolanguage)
    detect_language(s, use_translation);
  if (talker[lang].get())
    return talker[lang]->text_task(s, volume, rate, pitch, deviation,
                                   use_translation);
  return speech_task();
}

speech_task
polyglot::letter_task(const wstring& s)
{
  if (autolanguage)
    detect_language(s, true);
  if (talker[lang].get())
    return talker[lang]->letter_task(s);
  return speech_task();
}

speech_task
polyglot::silence(double duration)
{
  if (talker[lang].get())
    return talker[lang]->silence(duration);
  return speech_task();
}

void
polyglot::language(const string& id)
{
  for (int i = 0; i < langs.size(); i++)
    if (id == langs[i])
      {
        if (talker[i].get())
          {
            lang = i;
            autolanguage = false;
          }
        else if (server::debug)
          {
            string message("Language \"" + id + "\" is not configured");
            server::log << SyslogStream::debug << message << endl;
            if (server::verbose)
              cerr << message << endl;
          }
        return;
      }
  if (id == lang_id::autodetect)
    autolanguage = true;
  else if (server::debug)
    {
      string message("Unknown language \"" + id + '\"');
      server::log << SyslogStream::debug << message << endl;
      if (server::verbose)
        cerr << message << endl;
    }
}

const char*
polyglot::language(void) const
{
  if (autolanguage)
    return lang_id::autodetect;
  return talker[lang]->language->id();
}

void
polyglot::lang_switch(bool direction)
{
  int i = autolanguage ? (direction ? -1 : langs.size()) : lang;
  do i += direction ? 1 : -1;
  while ((i >= 0) && (i < langs.size()) && !talker[i].get());
  if ((i >= 0) && (i < langs.size()))
    {
      lang = i;
      autolanguage = false;
    }
  else autolanguage = true;
}


// Private methods:

void
polyglot::detect_language(const wstring& s, bool check_translation)
{
  if (!check_translation || (s.length() == 1) ||
      talker[lang]->language->translate(s).empty())
    for (int i = 0; i < langs.size(); i++)
      if (talker[i].get() && talker[i]->language->recognize(s))
        {
          lang = i;
          break;
        }
}

speech_engine*
polyglot::speech_backend(const string& name,
                         const string& lang,
                         const configuration& conf)
{
  if (speaker::freephone == name)
    return new freephone(conf);
  else if (speaker::ru_tts == name)
    return new ru_tts(conf);
  else if (speaker::espeak == name)
    return new espeak(conf, lang);
  else if (options::compose(speaker::espeak, speaker::mbrola) == name)
    return new mbrespeak(conf, lang);
  throw configuration::error("unknown speech backend " + name);
}
