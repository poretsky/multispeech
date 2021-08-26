// spd_settings.cpp -- Speech Dispatcher settings parser implementation
/*
   Copyright (C) 2019 Igor B. Poretsky <poretsky@mlbox.ru>
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
#include <sstream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "spd_settings.hpp"

#include "config.hpp"
#include "frontend.hpp"

using namespace std;
using namespace boost;
using namespace FBB;


// Execution table:
const spd_settings::Entry spd_settings::settings_table[] =
  {
    Entry("volume", &spd_settings::apply_volume),
    Entry("rate", &spd_settings::apply_rate),
    Entry("pitch", &spd_settings::apply_pitch),
    Entry("pitch_range", &spd_settings::apply_pitch_range),
    Entry("punctuation_mode", &spd_settings::apply_punctuation_mode),
    Entry("spelling_mode", &spd_settings::apply_spelling_mode),
    Entry("cap_let_recogn", &spd_settings::apply_cap_let_recogn),
    Entry("voice", &spd_settings::apply_voice),
    Entry("synthesis_voice", &spd_settings::apply_synthesis_voice),
    Entry("language", &spd_settings::apply_language),
    Entry("", &spd_settings::apply_unknown)
  };


// Object construction:

spd_settings::spd_settings(const configuration& conf, polyglot& linguist):
  CmdFinder<FunctionPtr>(settings_table, settings_table +
                         (sizeof(settings_table) / sizeof(Entry)),
                         USE_FIRST),
  use_voice_language(conf.option_value[options::spd::use_voice_language].as<bool>()),
  accept_explicit_language(conf.option_value[options::spd::accept_explicit_language].as<bool>()),
  ignore_unknown_voice(conf.option_value[options::spd::ignore_unknown_voice].as<bool>()),
  speechmaster(linguist),
  voice_pitch(1.0),
  pitch_factor(1.0),
  validate_integer("^[+-]?\\d+$")
{
}


// Public methods:

void
spd_settings::apply(const string& message)
{
  preserve state(this);
  istringstream content(message);
  string option;
  accept_language = accept_explicit_language;
  while (getline(content, option))
    {
      for (int i = 0; i < option.length(); i++)
        if (option[i] == '=')
          option[i] = ' ';
      FunctionPtr action = findCmd(option);
      if (beyond().empty())
        {
          cout << frontend::bad_syntax << endl;
          state.restore();
          return;
        }
      else if ((this->*action)())
        {
          cout << "303 ERROR INVALID PARAMETER OR VALUE" << endl;
          state.restore();
          return;
        }
    }
  cout << "203 OK SETTINGS RECEIVED" << endl;
}


// private methods:

bool
spd_settings::apply_volume(void)
{
  return set(volume, min_volume, max_volume, normal_volume);
}

bool
spd_settings::apply_rate(void)
{
  return set(rate, min_rate, max_rate, normal_rate);
}

bool
spd_settings::apply_pitch(void)
{
  if (set(pitch_factor, min_pitch, max_pitch, normal_pitch))
    return true;
  pitch = voice_pitch * pitch_factor;
  return false;
}

bool
spd_settings::apply_pitch_range(void)
{
  // Only parameter validity check, no actual implementation.
  int value;
  return !check(value);
}

bool
spd_settings::apply_punctuation_mode(void)
{
  set_punctuations_mode(static_cast<wchar_t>(beyond()[0]));
  return false;
}

bool
spd_settings::apply_spelling_mode(void)
{
  // Dummy, is not actually implemented.
  return false;
}

bool
spd_settings::apply_cap_let_recogn(void)
{
  // Dummy, is not actually implemented.
  return false;
}

bool
spd_settings::apply_voice(void)
{
  if (beyond() == "male1") // Paul
    {
      voice_pitch = 1.0;
      deviation = 1.0;
    }
  else if (beyond() == "male2") // Harry
    {
      voice_pitch = 0.5;
      deviation = 1.0;
    }
  else if (beyond() == "male3") // Dennis
    {
      voice_pitch = 0.7;
      deviation = 0.875;
    }
  else if (beyond() == "female1") // Betty
    {
      voice_pitch = 1.4;
      deviation = 1.0625;
    }
  else if (beyond() == "female2") // Ursula
    {
      voice_pitch = 1.3;
      deviation = 1.0;
    }
  else if (beyond() == "female3") // Rita
    {
      voice_pitch = 1.4;
      deviation = 1.125;
    }
  else if (beyond() == "child_male") // Kit
    {
      voice_pitch = 2.0;
      deviation = 1.25;
    }
  else if (beyond() == "child_female")
    {
      voice_pitch = 2.25;
      deviation = 1.125;
    }
  else return true;
  pitch = voice_pitch * pitch_factor;
  return false;
}

bool
spd_settings::apply_synthesis_voice(void)
{
  if (beyond() != "NULL")
    {
      ostringstream voice(ios::app);
      int i;
      for (i = 0; i < speechmaster.talker.size(); i++)
        if (speechmaster.talker[i].get())
          {
            voice.str(speechmaster.talker[i]->name);
            if (!speechmaster.talker[i]->voice.empty())
              voice << '-' << speechmaster.talker[i]->voice << flush;
            if (beyond() == voice.str())
              {
                speechmaster.language(speechmaster.talker[i]->language->id);
                accept_language = accept_language && !use_voice_language;
                break;
              }
          }
      if ((!ignore_unknown_voice) && (i >= speechmaster.talker.size()))
        {
          speechmaster.language(lang_id::autodetect);
          accept_language = accept_language && !use_voice_language;
        }
    }
  else
    {
      speechmaster.language(lang_id::autodetect);
      accept_language = accept_language && !use_voice_language;
    }
  return false;
}

bool
spd_settings::apply_language(void)
{
  if (accept_language)
    {
      string lang((beyond() == "NULL") ? lang_id::autodetect : beyond().c_str());
      speechmaster.language(lang);
    }
  return false;
}

bool
spd_settings::apply_unknown(void)
{
  return true;
}

bool
spd_settings::check(int& value)
{
  if (regex_match(beyond(), validate_integer))
    {
      value = lexical_cast<int>(beyond());
      return (value >= min_value) && (value <= max_value);
    }
  return false;
}

bool
spd_settings::set(double& result, double lowest, double highest, double normal)
{
  int value;
  if (check(value))
    {
      result = (value < normal_value) ?
        ((normal - lowest) / static_cast<double>(normal_value - min_value)) :
        ((highest - normal) / static_cast<double>(max_value - normal_value));
      result *= static_cast<double>(value - normal_value);
      result += normal;
      return false;
    }
  return true;
}


// State saving and restoring:

spd_settings::preserve::preserve(spd_settings* orig):
  voice_params(orig),
  master(orig),
  language(orig->speechmaster.language()),
  voice_pitch(orig->voice_pitch),
  pitch_factor(orig->pitch_factor)
{
}

void
spd_settings::preserve::restore(void)
{
  master->volume = this->volume;
  master->rate = this->rate;
  master->pitch = this->pitch;
  master->deviation = this->deviation;
  master->voice_pitch = this->voice_pitch;
  master->pitch_factor = this->pitch_factor;
  master->speechmaster.language(this->language);
}
