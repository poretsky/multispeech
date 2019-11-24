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

spd_settings::spd_settings(void):
  CmdFinder<FunctionPtr>(settings_table, settings_table +
                         (sizeof(settings_table) / sizeof(Entry)),
                         USE_FIRST),
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
  while (getline(content, option))
    {
      for (int i = 0; i < option.length(); i++)
        if (option[i] == '=')
          option[i] = ' ';
      FunctionPtr action = findCmd(option);
      if (beyond().empty())
        {
          cout << "302 ERROR BAD SYNTAX" << endl;
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
  pitch = voice_pitch * pitch_factor;
  cout << "203 OK SETTINGS RECEIVED" << endl;
}


// private methods:

bool
spd_settings::apply_volume(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  volume = value;
  return false;
}

bool
spd_settings::apply_rate(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  rate = value;
  return false;
}

bool
spd_settings::apply_pitch(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  pitch_factor = value;
  return false;
}

bool
spd_settings::apply_pitch_range(void)
{
  // Only parameter validity check, no actual implementation.
  return get_value() < 0.0;
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
      voice_pitch = 2.0;
      deviation = 1.75;
    }
  else return true;
  return false;
}

bool
spd_settings::apply_synthesis_voice(void)
{
  // Should be implemented.
  return false;
}

bool
spd_settings::apply_language(void)
{
  // Should be implemented.
  return false;
}

bool
spd_settings::apply_unknown(void)
{
  return true;
}

double
spd_settings::get_value(void)
{
  double result = -1.0;
  if (regex_match(beyond(), validate_integer))
    {
      int value = lexical_cast<int>(beyond());
      if ((value >= min_value) && (value <= max_value))
        {
          result = (value < normal_value) ?
            ((1.0 - min_factor) / static_cast<double>(normal_value - min_value)) :
            ((max_factor - 1.0) / static_cast<double>(max_value - normal_value));
          result *= static_cast<double>(value - normal_value);
          result += 1.0;
        }
    }
  return result;
}


// State saving and restoring:

spd_settings::preserve::preserve(spd_settings* orig):
  voice_params(orig),
  master(orig),
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
}
