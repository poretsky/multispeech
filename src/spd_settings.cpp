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

#include "speech_engine.hpp"

using namespace std;
using namespace boost;
using namespace FBB;


// Execution table:
const spd_settings::Entry spd_settings::settings_table[] =
  {
    Entry("volume", &spd_settings::apply_volume),
    Entry("rate", &spd_settings::apply_rate),
    Entry("pitch", &spd_settings::apply_pitch),
    Entry("", &spd_settings::apply_unknown)
  };


// Object construction:

spd_settings::spd_settings(void):
  CmdFinder<FunctionPtr>(settings_table, settings_table +
                         (sizeof(settings_table) / sizeof(Entry)),
                         USE_FIRST),
  validate_integer("^[+-]?\\d+$")
{
}


// Public methods:

void
spd_settings::parse(const string& message)
{
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
          return;
        }
      else if ((this->*action)())
        {
          cout << "303 ERROR INVALID PARAMETER OR VALUE" << endl;
          return;
        }
    }
  cout << "203 OK SETTINGS RECEIVED" << endl;
}


// private methods:

bool
spd_settings::apply_volume(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  speech_engine::volume(value);
  return false;
}

bool
spd_settings::apply_rate(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  speech_engine::speech_rate(value);
  return false;
}

bool
spd_settings::apply_pitch(void)
{
  double value = get_value();
  if (value < 0.0)
    return true;
  speech_engine::voice_pitch(value);
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
          value -= min_value;
          result = static_cast<double>(value) / static_cast<double>(max_value - min_value);
        }
    }
  return result;
}
