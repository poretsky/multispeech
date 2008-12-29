// historical.hpp -- Historical Multispeech interface
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

#ifndef MULTISPEECH_HISTORICAL_HPP
#define MULTISPEECH_HISTORICAL_HPP

#include <boost/regex.hpp>

#include "server.hpp"

class multispeech_historical: public server
{
public:
  // Construct the object:
  multispeech_historical(const configuration& conf);

private:
  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);

  // Additional parser for inline parameters extraction:
  void extract_parameters(void);
  double get_value(const boost::wregex& extractor);

  // Set punctuations mode according to identifying character:
  void set_punctuations_mode(wchar_t mode);

  // Regular expressions for inline parameters parsing:
  static const boost::wregex command_separator;
  static const boost::wregex params_detector;
  static const boost::wregex freq_extractor;
  static const boost::wregex pitch_extractor;
  static const boost::wregex rate_extractor;
  static const boost::wregex volume_extractor;
  static const boost::wregex precleaner;
  static const boost::wregex postcleaner;

  // Regular expressions for commands arguments validation:
  static const boost::wregex validate_float;
  static const boost::wregex validate_integer;
  static const boost::wregex beep_parameters;
  static const boost::wregex tts_parameters;

  // Parsing result representation:
  boost::wsmatch parse_result;

  // Temporary speech parameters:
  double volume, rate, pitch, deviation;

  // Reference frequency to calculate deviation:
  static const double ref_freq;

  // Speech rate scale factor:
  static const double rate_scale;
};

#endif
