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

#include <sysconfig.h>

#include <string>
#include <iostream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <bobcat/syslogstream>

#include <text_filter.hpp>
#include <speech_engine.hpp>

#include "frontend.hpp"

#include "multispeech_voices.hpp"
#include "dtk_voices.hpp"

using namespace std;
using namespace boost;
using namespace FBB;
using namespace multispeech;


// Recognized commands table:
const frontend::Entry frontend::command_table[] =
  {
    Entry("exit", &frontend::do_exit),
    Entry("d", &frontend::do_proceed_queue),
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
    Entry("tts_split_caps", &frontend::do_nothing),
    Entry("tts_capitalize", &frontend::do_nothing),
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

frontend::frontend(int argc, char* argv[]):
  server(argc, argv),
  CmdFinder<FunctionPtr>(command_table, command_table +
                         (sizeof(command_table) / sizeof(Entry)),
                         USE_FIRST),
  session(cin),
  disbalance(0),
  validate_float("^\\d+(\\.\\d*)?$"),
  validate_integer("^\\d+$"),
  beep_parameters("^(\\d+)?(\\s+(\\d+))?$"),
  lang_parameters("^(\\S+)(\\s+(\\S+))?$"),
  tts_parameters("^[a-z]+\\s+(\\d+)\\s+\\S+\\s+(\\d+)\\s+(\\d+)"),
  garbage("\\s*\\[\\*]\\s*")
{
  if (option_value[options::frontend::native_voices].as<bool>())
    native_params.reset(new multispeech_voices);
  if (option_value[options::frontend::dtk_voices].as<bool>())
    dtk_params.reset(new dtk_voices);
}


// Command set and syntax implementation:

bool
frontend::perform(string& request)
{
  bool result = true;
  if (!disbalance)
    accumulator.erase();
  else accumulator += ' ';
  for (unsigned int i = 0; i < request.length(); i++)
    switch (request[i])
      {
      case '{':
        disbalance++;
        accumulator += ' ';
        break;
      case '}':
        disbalance--;
        accumulator += ' ';
        break;
      default:
        accumulator += request[i];
        break;
      }
  if (!disbalance)
    {
      FunctionPtr done = findCmd(accumulator);
      if (!cmd().empty())
        result = (this->*done)();
    }
  return result;
}

bool
frontend::do_exit(void)
{
  return false;
}

bool
frontend::do_proceed_queue(void)
{
  proceed();
  return true;
}

bool
frontend::do_enqueue_speech(void)
{
  string text(beyond());
  inline_parser* voice_params = extract_parameters(text);
  if (voice_params)
    {
      punctuations::mode preserve = punctuations::verbosity;
      set_punctuations_mode(voice_params->punctuations_mode);
      enqueue(job(text_task(text,
                                          voice_params->volume,
                                          voice_params->rate,
                                          voice_params->pitch,
                                          voice_params->deviation)));
      punctuations::verbosity = preserve;
    }
  else enqueue(job(text_task(text)));
  return true;
}

bool
frontend::do_say_letter(void)
{
  string text(beyond());
  execute(letter_task(regex_replace(text, garbage, " ")));
  return true;
}

bool
frontend::do_enqueue_sound(void)
{
  enqueue(job(sound_task(beyond())));
  return true;
}

bool
frontend::do_play_sound(void)
{
  execute(sound_task(beyond()));
  return true;
}

bool
frontend::do_reset(void)
{
  stop();
  speech_engine::voice_pitch();
  speech_engine::speech_rate();
  speech_engine::sampling_deviation();
  speech_engine::char_voice_pitch();
  speech_engine::char_speech_rate();
  punctuations::verbosity = punctuations::some;
  return true;
}

bool
frontend::do_say_message(void)
{
  string msg(beyond());
  inline_parser* voice_params = extract_parameters(msg);
  if (voice_params)
    {
      punctuations::mode preserve = punctuations::verbosity;
      set_punctuations_mode(voice_params->punctuations_mode);
      execute(text_task(msg,
                                      voice_params->volume,
                                      voice_params->rate,
                                      voice_params->pitch,
                                      voice_params->deviation,
                                      true));
      punctuations::verbosity = preserve;
    }
  else execute(text_task(msg, true));
  return true;
}

bool
frontend::do_set_char_scale(void)
{
  string data(beyond());
  if (regex_match(data, validate_float))
    speech_engine::char_voice_pitch(lexical_cast<double>(data));
  return true;
}

bool
frontend::do_set_punctuations(void)
{
  string mode(beyond());
  if (!mode.empty())
    set_punctuations_mode(mode[0]);
  return true;
}

bool
frontend::do_set_speech_rate(void)
{
  string value(beyond());
  if (regex_match(value, validate_float))
    speech_engine::speech_rate(lexical_cast<double>(value) / rate_scale);
  return true;
}

bool
frontend::do_stop(void)
{
  stop();
  return true;
}

bool
frontend::do_pause(void)
{
  suspend();
  return true;
}

bool
frontend::do_resume(void)
{
  resume();
  return true;
}

bool
frontend::do_enqueue_tone(void)
{
  smatch parse_result;
  string params(beyond());
  unsigned int frequency = 440;
  float duration = 0.05;
  if (regex_match(params, parse_result, beep_parameters))
    {
      if (parse_result[1].matched)
        frequency = lexical_cast<unsigned int>(string(parse_result[1].first, parse_result[1].second));
      if (parse_result[2].matched)
        duration = lexical_cast<float>(string(parse_result[3].first, parse_result[3].second)) / 1000;
    }
  enqueue(job(tone_task(frequency, duration)));
  return true;
}

bool
frontend::do_enqueue_silence(void)
{
  string value(beyond());
  double duration = 0.05;
  if (regex_match(value, validate_float))
    duration = lexical_cast<double>(value) / 1000.0;
  enqueue(job(silence(duration)));
  return true;
}

bool
frontend::do_set_language(void)
{
  string params(beyond());
  smatch parse_result;
  if (regex_match(params, parse_result, lang_parameters))
    {
      language(string(parse_result[1].first, parse_result[1].second));
      if (parse_result[2].matched &&
          (string(parse_result[3].first, parse_result[3].second) != "nil"))
        execute(text_task(language(), true));
    }
  return true;
}

bool
frontend::do_next_language(void)
{
  string flag(beyond());
  lang_switch(true);
  if (!flag.empty() && (flag != "nil"))
    execute(text_task(language(), true));
  return true;
}

bool
frontend::do_prev_language(void)
{
  string flag(beyond());
  lang_switch(false);
  if (!flag.empty() && (flag != "nil"))
    execute(text_task(language(), true));
  return true;
}

bool
frontend::do_sync_state(void)
{
  string params(beyond());
  smatch parse_result;
  if (regex_search(params, parse_result, tts_parameters))
    {
      set_punctuations_mode(params[0]);
      if (parse_result[3].matched)
        speech_engine::speech_rate(lexical_cast<double>(string(parse_result[3].first, parse_result[3].second))
                                   / rate_scale);
    }
  return true;
}

bool
frontend::do_say_version(void)
{
  execute(text_task(PACKAGE_VERSION));
  return true;
}

bool
frontend::do_nothing(void)
{
  return true;
}

bool
frontend::do_unknown(void)
{
  if (debug)
    {
      string message("Unrecognized command");
      message += " \"" + cmd() + '\"';
      log << SyslogStream::debug << message << endl;
      if (verbose)
        cerr << message << endl;
    }
  return true;
}

inline_parser*
frontend::extract_parameters(string& data)
{
  inline_parser* voice_params = 0;
  data = regex_replace(data, garbage, " ");
  if (native_params.get() && native_params->parse(data))
    voice_params = native_params.get();
  else if (dtk_params.get() && dtk_params->parse(data))
    voice_params = dtk_params.get();
  return voice_params;
}

void
frontend::set_punctuations_mode(char mode)
{
  switch (mode)
    {
    case 'n':
      punctuations::verbosity = punctuations::none;
      break;
    case 's':
      punctuations::verbosity = punctuations::some;
      break;
    case 'a':
      punctuations::verbosity = punctuations::all;
    default:
      break;
    }
}
