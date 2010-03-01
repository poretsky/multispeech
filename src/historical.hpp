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

#include <memory>

#include <boost/regex.hpp>

#include "server.hpp"
#include "inline_parser.hpp"

class multispeech_historical: public server
{
public:
  // Construct the object:
  multispeech_historical(const configuration& conf,
                         inline_parser* voices);

private:
  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);

  // Additional parser for inline parameters extraction:
  std::auto_ptr<inline_parser> voice_params;

  // Set punctuations mode according to identifying character:
  void set_punctuations_mode(wchar_t mode);

  // Regular expressions for commands parsing:
  const boost::wregex command_separator, validate_float, validate_integer,
    beep_parameters, lang_parameters, tts_parameters;

  // Working area for regex match result representation:
  boost::wsmatch parse_result;

  // Reference value:
  static const double rate_scale = 200.0;
};

#endif
