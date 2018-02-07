// frontend.cpp -- Multispeech frontend interface implementation
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

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <bobcat/syslogstream>

#include "frontend.hpp"

#include "strcvt.hpp"
#include "text_filter.hpp"
#include "speech_engine.hpp"
#include "multispeech_voices.hpp"
#include "dtk_voices.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;
using namespace FBB;


// Object construction:

frontend::frontend(const configuration& conf):
  server(conf),
  command_separator(L"^\\s*(\\S+)(\\s+(.*\\S))?\\s*$"),
  validate_float(L"^\\d+(\\.\\d*)?$"),
  validate_integer(L"^\\d+$"),
  beep_parameters(L"^(\\d+)?(\\s+(\\d+))?$"),
  lang_parameters(L"^(\\S+)(\\s+(\\S+))?$"),
  tts_parameters(L"^[a-z]+\\s+(\\d+)\\s+\\S+\\s+(\\d+)\\s+(\\d+)"),
  garbage(L"\\s*\\[\\*]\\s*")
{
  if (conf.option_value[options::frontend::native_voices].as<bool>())
    native_params.reset(new multispeech_voices);
  if (conf.option_value[options::frontend::dtk_voices].as<bool>())
    dtk_params.reset(new dtk_voices);
}


// Input method:

void
frontend::get_command(void)
{
  string s;
  int count = 0;
  cmd.erase();
  while (cmd.empty() || count)
    {
      getline(cin, s);
      if (cin.eof() || cin.fail())
        s = "exit";
      data = intern_string(s, input_charset);
      if (count)
        cmd += L' ';
      for (unsigned int i = 0; i < data.length(); i++)
        switch (data[i])
          {
          case L'{':
            count++;
            data[i] = L' ';
            break;
          case L'}':
            count--;
            data[i] = L' ';
          default:
            break;
          }
      cmd += data;
    }
  if (regex_match(cmd, parse_result, command_separator))
    {
      if (parse_result[3].matched)
        data = wstring(parse_result[3].first, parse_result[3].second);
      else data = L"";
      if (parse_result[1].matched)
        cmd = wstring(parse_result[1].first, parse_result[1].second);
    }
}


// Command set and syntax implementation:

bool
frontend::perform_command(void)
{
  inline_parser* voice_params;

  if (cmd.empty())
    return true;

  else if (L"exit" == cmd)
    return false;

  else if (L"d" == cmd)
    soundmaster.proceed();

  else if (L"q" == cmd)
    {
      voice_params = extract_parameters();
      if (voice_params)
        {
          punctuations::mode preserve = punctuations::verbosity;
          set_punctuations_mode(voice_params->punctuations_mode);
          soundmaster.enqueue(speechmaster.text_task(data,
                                                     voice_params->volume,
                                                     voice_params->rate,
                                                     voice_params->pitch,
                                                     voice_params->deviation));
          punctuations::verbosity = preserve;
        }
      else soundmaster.enqueue(speechmaster.text_task(data));
    }

  else if (L"l" == cmd)
    soundmaster.execute(speechmaster.letter_task(regex_replace(data, garbage, L" ")));

  else if (L"a" == cmd)
    soundmaster.enqueue(sound_task(extern_string(data, locale("")).c_str()));

  else if (L"p" == cmd)
    soundmaster.execute(sound_task(extern_string(data, locale("")).c_str()));

  else if (L"tts_reset" == cmd)
    {
      soundmaster.stop();
      speech_engine::voice_pitch();
      speech_engine::speech_rate();
      speech_engine::sampling_deviation();
      speech_engine::char_voice_pitch();
      speech_engine::char_speech_rate();
      speech_engine::split_caps_mode();
      speech_engine::capitalize_mode();
      speech_engine::space_special_chars_mode();
      punctuations::verbosity = punctuations::some;
    }

  else if (L"tts_say" == cmd)
    {
      soundmaster.stop();
      voice_params = extract_parameters();
      if (voice_params)
        {
          punctuations::mode preserve = punctuations::verbosity;
          set_punctuations_mode(voice_params->punctuations_mode);
          soundmaster.execute(speechmaster.text_task(data,
                                                     voice_params->volume,
                                                     voice_params->rate,
                                                     voice_params->pitch,
                                                     voice_params->deviation,
                                                     true));
          punctuations::verbosity = preserve;
        }
      else soundmaster.execute(speechmaster.text_task(data, true));
    }

  else if (L"tts_set_character_scale" == cmd)
    {
      if (regex_match(data, validate_float))
        speech_engine::char_voice_pitch(lexical_cast<double>(data));
    }

  else if (L"tts_set_punctuations" == cmd)
    {
      if (!data.empty())
        set_punctuations_mode(data[0]);
    }

  else if ((L"r" == cmd) ||
           (L"tts_set_speech_rate" == cmd))
    {
      if (regex_match(data, validate_float))
        speech_engine::speech_rate(lexical_cast<double>(data) / rate_scale);
    }

  else if (L"tts_split_caps" == cmd)
    {
      if (regex_match(data, validate_integer))
        speech_engine::split_caps_mode(lexical_cast<int>(data));
    }

  else if (L"tts_capitalize" == cmd)
    {
      if (regex_match(data, validate_integer))
        speech_engine::capitalize_mode(lexical_cast<int>(data));
    }

  else if (L"s" == cmd)
    soundmaster.stop();

  else if (L"tts_pause" == cmd)
    soundmaster.suspend();

  else if (L"tts_resume" == cmd)
    soundmaster.resume();

  else if (L"t" == cmd)
    {
      unsigned int frequency = 440;
      float duration = 0.05;
      if (regex_match(data, parse_result, beep_parameters))
        {
          if (parse_result[1].matched)
            frequency = lexical_cast<unsigned int>(wstring(parse_result[1].first, parse_result[1].second));
          if (parse_result[2].matched)
            duration = lexical_cast<float>(wstring(parse_result[3].first, parse_result[3].second)) / 1000;
        }
      soundmaster.enqueue(tone_task(frequency, duration));
    }

  else if (L"sh" == cmd)
    {
      double duration = 0.05;
      if (regex_match(data, validate_float))
        duration = lexical_cast<double>(data) / 1000.0;
      soundmaster.enqueue(speechmaster.silence(duration));
    }

  else if (L"set_lang" == cmd)
    {
      if (regex_match(data, parse_result, lang_parameters))
        {
          speechmaster.language(extern_string(wstring(parse_result[1].first, parse_result[1].second), locale("")));
          if (parse_result[2].matched &&
              (wstring(parse_result[3].first, parse_result[3].second) != L"nil"))
            soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
        }
    }

  else if (L"set_next_lang" == cmd)
    {
      speechmaster.lang_switch(true);
      if (!data.empty() && (data != L"nil"))
        soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
    }

  else if (L"set_previous_lang" == cmd)
    {
      speechmaster.lang_switch(false);
      if (!data.empty() && (data != L"nil"))
        soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
    }

  else if (L"tts_sync_state" == cmd)
    {
      if (regex_search(data, parse_result, tts_parameters))
        {
          set_punctuations_mode(data[0]);
          if (parse_result[1].matched)
            speech_engine::capitalize_mode(lexical_cast<int>(wstring(parse_result[1].first, parse_result[1].second)));
          if (parse_result[2].matched)
            speech_engine::split_caps_mode(lexical_cast<int>(wstring(parse_result[2].first, parse_result[2].second)));
          if (parse_result[3].matched)
            speech_engine::speech_rate(lexical_cast<double>(wstring(parse_result[3].first, parse_result[3].second))
                                       / rate_scale);
        }
    }

  else if (L"version" == cmd)
    soundmaster.execute(speechmaster.text_task(intern_string(package::version, locale(""))));

  else if (debug)
    {
      string message("Unrecognized command");
      message += " \"" + extern_string(cmd, locale(""));
      if (!data.empty())
        message += " " + extern_string(data, locale(""));
      message += '\"';
      log << SyslogStream::debug << message << endl;
      if (verbose)
        cerr << message << endl;
    }

  return true;
}

inline_parser*
frontend::extract_parameters(void)
{
  inline_parser* voice_params = 0;
  data = regex_replace(data, garbage, L" ");
  if (native_params.get() && native_params->parse(data))
    voice_params = native_params.get();
  else if (dtk_params.get() && dtk_params->parse(data))
    voice_params = dtk_params.get();
  return voice_params;
}

void
frontend::set_punctuations_mode(wchar_t mode)
{
  switch (mode)
    {
    case L'n':
      punctuations::verbosity = punctuations::none;
      break;
    case L's':
      punctuations::verbosity = punctuations::some;
      break;
    case L'a':
      punctuations::verbosity = punctuations::all;
    default:
      break;
    }
}
