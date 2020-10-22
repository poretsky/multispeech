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

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <bobcat/syslogstream>
#include <bobcat/string>

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


// Recognized commands table:
const frontend::Entry frontend::command_table[] =
  {
    Entry("exit", &frontend::do_exit),
    Entry("d", &frontend::do_proceed_queue),
    Entry("c", &frontend::do_enqueue_voice_code),
    Entry("q", &frontend::do_enqueue_speech),
    Entry("l", &frontend::do_say_letter),
    Entry("a", &frontend::do_enqueue_sound),
    Entry("p", &frontend::do_play_sound),
    Entry("tts_reset", &frontend::do_reset),
    Entry("tts_say", &frontend::do_say_message),
    Entry("tts_set_character_scale", &frontend::do_set_char_scale),
    Entry("tts_set_punctuations", &frontend::do_set_punctuations),
    Entry("r", &frontend::do_set_speech_rate),
    Entry("tts_set_speech_rate", &frontend::do_set_speech_rate),
    Entry("tts_split_caps", &frontend::do_split_caps),
    Entry("tts_capitalize", &frontend::do_capitalize),
    Entry("s", &frontend::do_stop),
    Entry("tts_pause", &frontend::do_pause),
    Entry("tts_resume", &frontend::do_resume),
    Entry("t", &frontend::do_enqueue_tone),
    Entry("sh", &frontend::do_enqueue_silence),
    Entry("set_lang", &frontend::do_set_language),
    Entry("set_next_lang", &frontend::do_next_language),
    Entry("set_previous_lang", &frontend::do_prev_language),
    Entry("tts_sync_state", &frontend::do_sync_state),
    Entry("version", &frontend::do_say_version),
    Entry("", &frontend::do_unknown)
  };


// Object construction:

frontend::frontend(const configuration& conf):
  speech_server(conf),
  CmdFinder<FunctionPtr>(command_table, command_table +
                         (sizeof(command_table) / sizeof(Entry)),
                         USE_FIRST),
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
  getline(cin, s);
  if (cin.eof() || cin.fail())
    {
      speech_server::cmd = "exit";
      exit_status = EXIT_FAILURE;
    }
  else
    {
      accumulator << s << endl;
      data = intern_string(accumulator.str(), input_charset);
      int disbalance = 0;
      for (unsigned int i = 0; i < data.length(); i++)
        switch (data[i])
          {
          case L'{':
            disbalance++;
            data[i] = L' ';
            break;
          case L'}':
            disbalance--;
          case L'\n':
            data[i] = L' ';
          default:
            break;
          }
      if (!disbalance)
        speech_server::cmd = extern_string(data, locale(""));
      else if ((disbalance < 0) || (disbalance > 1))
        communication_reset();
    }
}


// Command set and syntax implementation:

bool
frontend::perform_command(void)
{
  bool done = true;
  if (!speech_server::cmd.empty())
    {
      FunctionPtr action = findCmd(speech_server::cmd);
      data = intern_string(String::trim(beyond()), input_charset);
      done = (this->*action)();
      communication_reset();
    }
  return done;
}

bool
frontend::do_exit(void)
{
  return false;
}

bool
frontend::do_proceed_queue(void)
{
  soundmaster.proceed();
  return true;
}

bool
frontend::do_enqueue_voice_code(void)
{
  voice_params* voice = extract_parameters();
  if (voice)
    queue_voice.reset(new voice_params(voice));
  else queue_voice.reset();
  return true;
}

bool
frontend::do_enqueue_speech(void)
{
  voice_params* voice = extract_parameters();
  if (!voice)
    voice = queue_voice.get();
  if (voice)
    soundmaster.enqueue(speechmaster.text_task(data, voice));
  else soundmaster.enqueue(speechmaster.text_task(data));
  return true;
}

bool
frontend::do_say_letter(void)
{
  soundmaster.execute(speechmaster.letter_task(regex_replace(data, garbage, L" ")));
  return true;
}

bool
frontend::do_enqueue_sound(void)
{
  soundmaster.enqueue(sound_task(extern_string(data, locale("")).c_str()));
  return true;
}

bool
frontend::do_play_sound(void)
{
  soundmaster.execute(sound_task(extern_string(data, locale("")).c_str()));
  return true;
}

bool
frontend::do_reset(void)
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
  queue_voice.reset();
  return true;
}

bool
frontend::do_say_message(void)
{
  soundmaster.stop();
  voice_params* voice = extract_parameters();
  if (voice)
    soundmaster.execute(speechmaster.text_task(data, voice, true));
  else soundmaster.execute(speechmaster.text_task(data, true));
  return true;
}

bool
frontend::do_set_char_scale(void)
{
  if (regex_match(data, validate_float))
    speech_engine::char_voice_pitch(lexical_cast<double>(data));
  return true;
}

bool
frontend::do_set_punctuations(void)
{
  if (!data.empty())
    punctuations::set_mode(data[0]);
  return true;
}

bool
frontend::do_set_speech_rate(void)
{
  if (regex_match(data, validate_float))
    speech_engine::speech_rate(lexical_cast<double>(data) / rate_scale);
  return true;
}

bool
frontend::do_split_caps(void)
{
  if (regex_match(data, validate_integer))
    speech_engine::split_caps_mode(lexical_cast<int>(data));
  return true;
}

bool
frontend::do_capitalize(void)
{
  if (regex_match(data, validate_integer))
    speech_engine::capitalize_mode(lexical_cast<int>(data));
  return true;
}

bool
frontend::do_stop(void)
{
  soundmaster.stop();
  return true;
}

bool
frontend::do_pause(void)
{
  soundmaster.suspend();
  return true;
}
 
bool
frontend::do_resume(void)
{
  soundmaster.resume();
  return true;
}

bool
frontend::do_enqueue_tone(void)
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
  return true;
}

bool
frontend::do_enqueue_silence(void)
{
  double duration = 0.05;
  if (regex_match(data, validate_float))
    duration = lexical_cast<double>(data) / 1000.0;
  soundmaster.enqueue(speechmaster.silence(duration));
  return true;
}

bool
frontend::do_set_language(void)
{
  if (regex_match(data, parse_result, lang_parameters))
    {
      speechmaster.language(extern_string(wstring(parse_result[1].first, parse_result[1].second), locale("")));
      if (parse_result[2].matched &&
          (wstring(parse_result[3].first, parse_result[3].second) != L"nil"))
        soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
    }
  return true;
}

bool
frontend::do_next_language(void)
{
  speechmaster.lang_switch(true);
  if (!data.empty() && (data != L"nil"))
    soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
  return true;
}

bool
frontend::do_prev_language(void)
{
  speechmaster.lang_switch(false);
  if (!data.empty() && (data != L"nil"))
    soundmaster.execute(speechmaster.text_task(intern_string(speechmaster.language(), locale("")), true));
  return true;
}

bool
frontend::do_sync_state(void)
{
  if (regex_search(data, parse_result, tts_parameters))
    {
      punctuations::set_mode(data[0]);
      if (parse_result[1].matched)
        speech_engine::capitalize_mode(lexical_cast<int>(wstring(parse_result[1].first, parse_result[1].second)));
      if (parse_result[2].matched)
        speech_engine::split_caps_mode(lexical_cast<int>(wstring(parse_result[2].first, parse_result[2].second)));
      if (parse_result[3].matched)
        speech_engine::speech_rate(lexical_cast<double>(wstring(parse_result[3].first, parse_result[3].second))
                                   / rate_scale);
    }
  return true;
}

bool
frontend::do_say_version(void)
{
  soundmaster.execute(speechmaster.text_task(intern_string(package::version, locale(""))));
  return true;
}

bool
frontend::do_unknown(void)
{
  if (debug)
    {
      ostringstream message;
      message << "Unrecognized command \"" << speech_server::cmd << '\"' << flush;
      log << SyslogStream::debug << message.str() << endl;
      if (verbose)
        cerr << message.str() << endl;
    }
  return true;
}

voice_params*
frontend::extract_parameters(void)
{
  voice_params* voice = 0;
  data = regex_replace(data, garbage, L" ");
  if (native_params.get() && native_params->parse(data))
    voice = native_params.get();
  else if (dtk_params.get() && dtk_params->parse(data))
    voice = dtk_params.get();
  return voice;
}
