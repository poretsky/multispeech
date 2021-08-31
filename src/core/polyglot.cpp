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
#include <algorithm>
#include <vector>
#include <map>

#include <bobcat/syslogstream>

#include <boost/assign.hpp>

#include "polyglot.hpp"

#include "config.hpp"
#include "speech_server.hpp"
#include "freephone.hpp"
#include "ru_tts.hpp"
#include "espeak.hpp"
#include "user_tts.hpp"
#include "text_filter.hpp"

#include "English.hpp"
#include "German.hpp"
#include "Italian.hpp"
#include "French.hpp"
#include "Spanish.hpp"
#include "Portuguese.hpp"
#include "Russian.hpp"

using namespace std;
using namespace FBB;
using namespace boost;
using namespace boost::assign;


// Static data:
string polyglot::language_preference;
string polyglot::fallback_language(lang_id::en);

// Supported languages:
static vector<const char*> langs = list_of
  // Default detection order
  (lang_id::ru)
  (lang_id::pt)
  (lang_id::it)
  (lang_id::fr)
  (lang_id::de)
  (lang_id::es)
  (lang_id::en);

static const map<const char*, const string*> backends = map_list_of
  (lang_id::en, &English::settings.engine)
  (lang_id::de, &German::settings.engine)
  (lang_id::it, &Italian::settings.engine)
  (lang_id::fr, &French::settings.engine)
  (lang_id::es, &Spanish::settings.engine)
  (lang_id::pt, &Portuguese::settings.engine)
  (lang_id::ru, &Russian::settings.engine)
  .convert_to_container< map<const char*, const string*> >();

static const map<const char*, const int*> priorities = map_list_of
  (lang_id::en, &English::settings.priority)
  (lang_id::de, &German::settings.priority)
  (lang_id::it, &Italian::settings.priority)
  (lang_id::fr, &French::settings.priority)
  (lang_id::es, &Spanish::settings.priority)
  (lang_id::pt, &Portuguese::settings.priority)
  (lang_id::ru, &Russian::settings.priority)
  .convert_to_container< map<const char*, const int*> >();

// Language comparator:
static bool
order(const char* lang1, const char* lang2)
{
  return *priorities.at(lang1) < *priorities.at(lang2);
}


// Construct the object:

polyglot::polyglot(void):
  talker(langs.size()),
  lang(langs.size()),
  fallback(langs.size()),
  autolanguage(false)
{
  bool initialized = false;
  stable_sort(langs.begin(), langs.end(), order);
  for (unsigned int i = 0; i < langs.size(); i++)
    if (backends.count(langs[i]))
      {
        const string& engine = *backends.at(langs[i]);
        if ((engine != speech_engine::disabled) &&
            !engine.empty())
          {
            talker[i].reset(speech_backend(engine, langs[i]));
            if (langs[i] == fallback_language)
              fallback = i;
            initialized = true;
          }
      }
  if (!initialized)
    throw configuration::error("no speech backends are defined");
  if (fallback >= langs.size())
    throw configuration::error("fallback language is unavailable");
  if (!language_preference.empty())
    language(language_preference);
  else language(lang_id::autodetect);
  if (autolanguage)
    {
      if (!language_preference.empty() &&
          (language_preference != lang_id::autodetect))
        throw configuration::error("unsupported language " + language_preference);
      for (unsigned int i = 0; i < langs.size(); i++)
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
                    voice_params* voice,
                    bool use_translation)
{
  if (autolanguage)
    detect_language(s, use_translation);
  if (talker[lang].get())
    {
      punctuations::mode preserve = punctuations::verbosity;
      punctuations::verbosity = voice->punctuations_mode;
      speech_task task = talker[lang]->text_task(s, voice, use_translation);
      punctuations::verbosity = preserve;
      return task;
    }
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
polyglot::letter_task(const wstring& s, voice_params* voice)
{
  if (autolanguage)
    detect_language(s, true);
  if (talker[lang].get())
    return talker[lang]->letter_task(s, voice);
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
  for (unsigned int i = 0; i < langs.size(); i++)
    if (id == langs[i])
      {
        if (talker[i].get())
          {
            lang = i;
            autolanguage = false;
          }
        else if (speech_server::debug)
          {
            string message("Language \"" + id + "\" is not configured");
            speech_server::log << SyslogStream::debug << message << endl;
            if (speech_server::verbose)
              cerr << message << endl;
          }
        return;
      }
  if (id == lang_id::autodetect)
    autolanguage = true;
  else if (speech_server::debug)
    {
      string message("Unknown language \"" + id + '\"');
      speech_server::log << SyslogStream::debug << message << endl;
      if (speech_server::verbose)
        cerr << message << endl;
    }
}

const char*
polyglot::language(void) const
{
  if (autolanguage)
    return lang_id::autodetect;
  return talker[lang]->language->id;
}

void
polyglot::lang_switch(bool direction)
{
  int i = autolanguage ? (direction ? -1 : langs.size()) : lang;
  do i += direction ? 1 : -1;
  while ((i >= 0) && (static_cast<unsigned int>(i) < langs.size()) && !talker[i].get());
  if ((i >= 0) && (static_cast<unsigned int>(i) < langs.size()))
    {
      lang = static_cast<unsigned int>(i);
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
    {
      unsigned int newlang = langs.size();
      for (unsigned int i = 0; i < langs.size(); i++)
        if (talker[i].get() && talker[i]->language->recognize(s))
          {
            newlang = i;
            break;
          }
      if (newlang < langs.size())
        lang = newlang;
      else if (talker[lang]->language->foreign(s))
        lang = fallback;
    }
}

speech_engine*
polyglot::speech_backend(const string& name,
                         const char* lang)
{
  if (freephone::name == name)
    return new freephone;
  else if (ru_tts::name == name)
    return new ru_tts;
  else if (espeak::name == name)
    return new espeak(lang);
  else if ((espeak::name + '.' + mbrola::name) == name)
    return new mbrespeak(lang);
  else if (user_tts::name == name)
    return new user_tts(lang);
  throw configuration::error("unknown speech backend " + name);
}
