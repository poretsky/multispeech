// spd_settings.hpp -- Speech Dispatcher settings parser
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

#ifndef MULTISPEECH_SPD_SETTINGS_HPP
#define MULTISPEECH_SPD_SETTINGS_HPP

#include <string>

#include <boost/config.hpp>
#include <boost/regex.hpp>

#include <bobcat/cmdfinder>

#include "voice_params.hpp"
#include "polyglot.hpp"


class spd_settings:
  public voice_params,
  private FBB::CmdFinder<bool (spd_settings::*)(void)>
{
public:
  // Object construction:
  explicit spd_settings(polyglot& linguist);

  void apply(const std::string& message);

private:
  // State saver:
  class preserve: private voice_params
  {
  public:
    preserve(spd_settings* orig);

    void restore(void);

  private:
    spd_settings* master;
    std::string language;
    double voice_pitch, pitch_factor;
  };

  friend class preserve;

  // Executors:
  bool apply_volume(void);
  bool apply_rate(void);
  bool apply_pitch(void);
  bool apply_pitch_range(void);
  bool apply_punctuation_mode(void);
  bool apply_spelling_mode(void);
  bool apply_cap_let_recogn(void);
  bool apply_voice(void);
  bool apply_synthesis_voice(void);
  bool apply_language(void);
  bool apply_unknown(void);

  // check numeric value validity and return true when it fits:
  bool check(int& value);

  // Try to set numeric parameter and return true on failure:
  bool set(double& result, double lowest, double highest, double normal);

  // Speech language control:
  polyglot& speechmaster;

  // Pitch forming parameters:
  double voice_pitch, pitch_factor;

  // Value validation data:
  const boost::regex validate_integer;
  static const int min_value = -100;
  static const int max_value = 100;
  static const int normal_value = 0;

  // Numeric parameters variation:
  static BOOST_CONSTEXPR_OR_CONST double min_volume = 0.0;
  static BOOST_CONSTEXPR_OR_CONST double max_volume = 1.0;
  static BOOST_CONSTEXPR_OR_CONST double normal_volume = 0.5;
  static BOOST_CONSTEXPR_OR_CONST double min_rate = 0.25;
  static BOOST_CONSTEXPR_OR_CONST double max_rate = 4.0;
  static BOOST_CONSTEXPR_OR_CONST double normal_rate = 1.0;
  static BOOST_CONSTEXPR_OR_CONST double min_pitch = 0.5;
  static BOOST_CONSTEXPR_OR_CONST double max_pitch = 2.0;
  static BOOST_CONSTEXPR_OR_CONST double normal_pitch = 1.0;

  // Execution table:
  static const Entry settings_table[];
};

#endif
