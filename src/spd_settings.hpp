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

#include <boost/regex.hpp>

#include <bobcat/cmdfinder>

class spd_settings: private FBB::CmdFinder<bool (spd_settings::*)(void)>
{
public:
  // Object construction:
  spd_settings(void);

  void parse(const std::string& message);

private:
  // Executors:
  bool apply_volume(void);
  bool apply_rate(void);
  bool apply_pitch(void);
  bool apply_unknown(void);

  // Extract numeric value:
  double get_value(void);

  // Value validation data:
  const boost::regex validate_integer;
  static const int min_value = -100;
  static const int max_value = 100;

  // Execution table:
  static const Entry settings_table[];
};

#endif
