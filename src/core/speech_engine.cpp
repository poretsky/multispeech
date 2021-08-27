// speech_engine.cpp -- Basic speech engine layer
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
#include <utility>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string.hpp>

#include "speech_engine.hpp"

#include "config.hpp"
#include "iconv_codecvt.hpp"
#include "strcvt.hpp"
#include "text_filter.hpp"
#include "pipeline.hpp"
#include "English.hpp"
#include "Russian.hpp"
#include "German.hpp"
#include "French.hpp"
#include "Spanish.hpp"
#include "Portuguese.hpp"
#include "Italian.hpp"

using namespace std;
using namespace boost;

// Internal data:
static double persistent_volume = 1.0;
static double persistent_pitch = 1.0;
static double persistent_rate = 1.0;
static double persistent_deviation = 1.0;
static double persistent_char_pitch = 1.0;
static double persistent_char_rate = 1.0;

// Blank pattern:
static const wregex blank_pattern(L"\\s+");


// Common voice and speech parameters:
bool speech_engine::split_caps = false;
bool speech_engine::capitalize = false;
bool speech_engine::space_special_chars = false;

// string constants:
const string speech_engine::disabled("disabled");
const string speech_engine::novoice;


// Constructing and destroying:

speech_engine::speech_engine(const string& backend,
                             const string& voice_id,
                             const char* lang,
                             soundfile::format fmt,
                             unsigned int sampling,
                             unsigned int channels,
                             bool deviate,
                             const string& charset):
  name(backend),
  voice(voice_id),
  format(fmt),
  native_sampling(sampling),
  sound_channels(channels),
  playing_deviation(deviate),
  backend_charset(charset.empty() ?
                  locale("") :
                  locale(locale(""), new iconv_codecvt(NULL, charset.c_str())))
{
  if (lang_id::en == lang)
    language.reset(new English);
  else if (lang_id::ru == lang)
    language.reset(new Russian);
  else if (lang_id::de == lang)
    language.reset(new German);
  else if (lang_id::fr == lang)
    language.reset(new French);
  else if (lang_id::es == lang)
    language.reset(new Spanish);
  else if (lang_id::pt == lang)
    language.reset(new Portuguese);
  else if (lang_id::it == lang)
    language.reset(new Italian);
  else throw configuration::error("unsupported language " + string(lang) + " specified for " + backend);
  format_macros["%lang"] = lang;
}

speech_engine::~speech_engine(void)
{
}


// Public methods for speech and voice control:

void
speech_engine::volume(double value)
{
  persistent_volume = value;
}

void
speech_engine::voice_pitch(double value)
{
  persistent_pitch = value;
}

void
speech_engine::speech_rate(double value)
{
  persistent_rate = value;
}

void
speech_engine::sampling_deviation(double value)
{
  persistent_deviation = value;
}

void
speech_engine::char_voice_pitch(double value)
{
  persistent_char_pitch = value;
}

void
speech_engine::char_speech_rate(double value)
{
  persistent_char_rate = value;
}

void
speech_engine::split_caps_mode(bool value)
{
  split_caps = value;
}

void
speech_engine::capitalize_mode(bool value)
{
  capitalize = value;
}

void
speech_engine::space_special_chars_mode(bool value)
{
  space_special_chars = value;
}


// Speech queue control methods:

speech_task
speech_engine::text_task(const wstring& s, bool use_translation)
{
  return wrap_text(s, -1.0, -1.0, -1.0, -1.0, use_translation);
}

speech_task
speech_engine::text_task(const wstring& s,
                         voice_params* voice,
                         bool use_translation,
                         bool allpuncts)
{
  return wrap_text(s, voice->volume, voice->rate, voice->pitch, voice->deviation, use_translation, allpuncts);
}

speech_task
speech_engine::letter_task(wstring s)
{
  return wrap_letter(s, -1.0, persistent_char_rate, persistent_char_pitch, 0.0);
}

speech_task
speech_engine::letter_task(wstring s, voice_params* voice)
{
  return wrap_letter(s, voice->volume, voice->rate, voice->pitch, voice->deviation);
}

speech_task
speech_engine::silence(double duration)
{
  return speech_task(native_sampling,
                     numeric_cast<unsigned int>(nearbyint(numeric_cast<double>(native_sampling) * duration)));
}


// Protected methods:

void
speech_engine::command(const string& pattern)
{
  if (!pattern.empty())
    command_patterns.push_front(pattern);
}

void
speech_engine::voicify(double rate, double pitch)
{
  format_macros["%pitch"] = lexical_cast<string>(pitch);
  format_macros["%rate"] = lexical_cast<string>(rate);
}

void
speech_engine::sampling(unsigned int value)
{
  native_sampling = value;
}


// Private methods:

speech_task
speech_engine::wrap_text(const wstring& s,
                         double volume, double rate,
                         double pitch, double deviation,
                         bool use_translation,
                         bool allpuncts)
{
  pipeline::script commands;
  pair<string, string> fmt;
  wstring prepared;
  double freq = numeric_cast<double>(native_sampling);
  speech_task::details playing_params;

  // Parse speech parameters and prepare the voice.
  if (playing_deviation)
    {
      if (format != soundfile::autodetect)
        {
          freq /= (deviation > 0.0) ? deviation : persistent_deviation;
          playing_params.sound.sampling = numeric_cast<unsigned int>(nearbyint(freq));
          playing_params.sound.channels = sound_channels;
        }
      else playing_params.deviation = (deviation > 0.0) ? deviation : persistent_deviation;
      format_macros["%freq"] = lexical_cast<string>(native_sampling);
    }
  else
    {
      playing_params.sound.sampling = native_sampling;
      playing_params.sound.channels = sound_channels;
      freq *= (deviation > 0.0) ? deviation : persistent_deviation;
      format_macros["%freq"] = lexical_cast<string>(numeric_cast<unsigned int>(nearbyint(freq)));
    }
  voicify(language->settings.rate * ((rate > 0.0) ? rate : persistent_rate),
          language->settings.pitch * ((pitch > 0.0) ? pitch : persistent_pitch));

  // Make up the TTS script.
  BOOST_FOREACH(string cmd, command_patterns)
    {
      BOOST_FOREACH(fmt, format_macros)
        replace_all(cmd, fmt.first, fmt.second);
      commands.push(cmd);
    }

  // Prepare the text.
  if (!s.empty())
    {
      punctuations::mode preserve = punctuations::verbosity;
      if (allpuncts)
        punctuations::verbosity = punctuations::all;
      if (use_translation)
        {
          prepared = language->translate(s);
          if (prepared.empty())
            prepared = language->filter(s);
        }
      else prepared = language->filter(s);
      punctuations::verbosity = preserve;
    }

  if (!prepared.empty())
    {
      prepared = regex_replace(prepared, blank_pattern, L" ");
      trim(prepared);
    }

  // Make up and return complete task description.
  return speech_task(extern_string(prepared, backend_charset),
                     commands, format, playing_params,
                     ((volume > 0) ? volume : persistent_volume) * language->settings.volume,
                     language->settings.acceleration);
}

speech_task
speech_engine::wrap_letter(wstring s,
                           double volume, double rate,
                           double pitch, double deviation)
{
  if (s.length() == 1)
    {
      if (isupper(s[0], locale("")))
        pitch *= language->settings.caps_factor;
      else s[0] = toupper(s[0], locale(""));
    }
  return wrap_text(s, volume, rate * language->settings.char_rate, pitch * language->settings.char_pitch, deviation, true, true);
}
