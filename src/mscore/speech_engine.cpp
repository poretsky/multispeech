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

#include <sysconfig.h>

#include <cmath>
#include <utility>
#include <iostream>

#include <bobcat/syslogstream>

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string.hpp>

#include "speech_engine.hpp"

#include "iconv_codecvt.hpp"
#include "strcvt.hpp"
#include "text_filter.hpp"
#include "pipeline.hpp"
#include "English.hpp"
#include "Russian.hpp"
#include "server.hpp"


namespace multispeech
{

using namespace std;
using namespace FBB;
using namespace boost;
using namespace boost::assign;


// Output charset encoders:
speech_engine::charset_map speech_engine::encoders = map_list_of
  ("", locale(""));

// Common voice and speech parameters:
float speech_engine::general_volume = 1.0;
float speech_engine::general_pitch = 1.0;
float speech_engine::general_rate = 1.0;
float speech_engine::general_deviation = 1.0;
float speech_engine::char_pitch = 1.0;
float speech_engine::char_rate = 1.0;


// Constructing and destroying:

speech_engine::speech_engine(const string& backend, bool deviate):
  name(backend),
  volume_factor(1.0),
  rate_factor(1.0),
  pitch_factor(1.0),
  acceleration(0.0),
  playing_deviation(deviate)
{
}

speech_engine::~speech_engine(void)
{
}


// Public methods for speech and voice control:

void
speech_engine::volume(float value)
{
  general_volume = value;
}

void
speech_engine::voice_pitch(float value)
{
  general_pitch = value;
}

void
speech_engine::speech_rate(float value)
{
  general_rate = value;
}

void
speech_engine::sampling_deviation(float value)
{
  general_deviation = value;
}

void
speech_engine::char_voice_pitch(float value)
{
  char_pitch = value;
}

void
speech_engine::char_speech_rate(float value)
{
  char_rate = value;
}


// Switching voices:

void
speech_engine::voice_setup(const string& voice_name)
{
  if (voice_available(voice_name))
    {
      current_voice = voices[voice_name];
      if (lang_id::en == current_voice->language)
        language = English::instance();
      else if (lang_id::ru == current_voice->language)
        language = Russian::instance();
      else throw configuration::error("Unknown language");
      format_macros["%voice"] = current_voice->id;
      format_macros["%lang"] = current_voice->language;
      if (!encoders.count(current_voice->charset))
        encoders[current_voice->charset] = locale(locale::classic(), new iconv_codecvt(NULL, current_voice->charset.c_str()));
      volume_factor = configuration::speech_volume(voice_name);
      rate_factor = configuration::speech_rate(voice_name);
      pitch_factor = configuration::voice_pitch(voice_name);
      if (speaker::user_defined == name)
        acceleration = configuration::user_tts_acceleration(voice_name);
    }
  else if (server::debug)
    {
      string message(name + " voice \"" + voice_name + "\" is not available");
      server::log << SyslogStream::debug << message << endl;
      if (server::verbose)
        cerr << message << endl;
    }
}

bool
speech_engine::voice_available(const string& voice_name)
{
  return voices.count(voice_name) ? check_voice(voice_name) : false;
}


// Speech task preparing methods:

speech_task
speech_engine::text_task(const wstring& s, bool use_translation)
{
  return text_task(s, -1.0, -1.0, -1.0, -1.0, use_translation);
}

speech_task
speech_engine::text_task(const wstring& s,
                         float volume, float rate,
                         float pitch, float deviation,
                         bool use_translation,
                         bool allpuncts)
{
  pipeline::script commands;
  pair<string, string> fmt;
  wstring prepared;
  float freq = numeric_cast<float>(current_voice->sampling);
  speech_task::details playing_params;

  // Parse speech parameters and prepare the voice.
  if (playing_deviation)
    {
      if (current_voice->format != soundfile::autodetect)
        {
          freq /= (deviation > 0.0) ? deviation : general_deviation;
          playing_params.sound.sampling = numeric_cast<unsigned int>(nearbyintf(freq));
          playing_params.sound.channels = current_voice->channels;
        }
      else playing_params.deviation = (deviation > 0.0) ? deviation : general_deviation;
      format_macros["%freq"] = lexical_cast<string>(current_voice->sampling);
    }
  else
    {
      playing_params.sound.sampling = current_voice->sampling;
      playing_params.sound.channels = current_voice->channels;
      freq *= (deviation > 0.0) ? deviation : general_deviation;
      format_macros["%freq"] = lexical_cast<string>(numeric_cast<unsigned int>(nearbyintf(freq)));
    }
  voicify(configuration::speech_rate() * rate_factor *
          ((rate > 0.0) ? rate : general_rate),
          configuration::voice_pitch() * pitch_factor * ((pitch > 0.0) ? pitch : general_pitch));

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

  // Make up and return complete task description.
  return speech_task(extern_string(prepared, encoders[current_voice->charset]),
                     commands, current_voice->format, playing_params,
                     configuration::speech_volume() * volume_factor *
                     ((volume > 0) ? volume : general_volume),
                     acceleration);
}

speech_task
speech_engine::letter_task(wstring s)
{
  float pitch = configuration::char_pitch() * char_pitch;
  float rate = configuration::char_rate() * char_rate;
  if (s.length() == 1)
    {
      if (isupper(s[0], locale("")))
        pitch *= configuration::caps_factor();
      else s[0] = toupper(s[0], locale(""));
    }
  return text_task(s, -1.0, rate, pitch, 0.0, true, true);
}

speech_task
speech_engine::silence(float duration)
{
  return speech_task(current_voice->sampling,
                     numeric_cast<unsigned int>(nearbyintf(numeric_cast<float>(current_voice->sampling) * duration)));
}


// Protected methods:

void
speech_engine::command(const string& pattern)
{
  if (!pattern.empty())
    command_patterns.push_front(pattern);
}

bool
speech_engine::check_voice(const string& voice_name)
{
  return true;
}

} // namespace multispeech
