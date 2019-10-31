// frontend.hpp -- Multispeech frontend interactions
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

#ifndef MULTISPEECH_FRONTEND_HPP
#define MULTISPEECH_FRONTEND_HPP

#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>

#include "server.hpp"
#include "inline_parser.hpp"

class frontend: public server
{
public:
  // Construct the object:
  explicit frontend(const configuration& conf);

private:
  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);

  // Additional parsers for inline parameters extraction:
  boost::scoped_ptr<inline_parser> native_params, dtk_params;

  // Default voice for subsequent speech queue: 
  boost::scoped_ptr<voice_params> queue_voice;

  // Extract embedded parameters:
  voice_params* extract_parameters(void);

  // Set punctuations mode according to identifying character:
  void set_punctuations_mode(wchar_t mode);

  // Regular expressions for commands parsing:
  const boost::wregex command_separator, validate_float, validate_integer,
    beep_parameters, lang_parameters, tts_parameters, garbage;

  // Working area for regex match result representation:
  boost::wsmatch parse_result;

  // Reference value:
  static BOOST_CONSTEXPR_OR_CONST double rate_scale = 200.0;
};

#endif
