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

#include <bobcat/cmdfinder>

#include "server.hpp"
#include "inline_parser.hpp"

class frontend:
  public server,
  private FBB::CmdFinder<bool (frontend::*)(void)>
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

  // Recognized command performers:
  bool do_exit(void);
  bool do_proceed_queue(void);
  bool do_enqueue_voice_code(void);
  bool do_enqueue_speech(void);
  bool do_say_letter(void);
  bool do_enqueue_sound(void);
  bool do_play_sound(void);
  bool do_reset(void);
  bool do_say_message(void);
  bool do_set_char_scale(void);
  bool do_set_punctuations(void);
  bool do_set_speech_rate(void);
  bool do_split_caps(void);
  bool do_capitalize(void);
  bool do_stop(void);
  bool do_pause(void);
  bool do_resume(void);
  bool do_enqueue_tone(void);
  bool do_enqueue_silence(void);
  bool do_set_language(void);
  bool do_next_language(void);
  bool do_prev_language(void);
  bool do_sync_state(void);
  bool do_say_version(void);
  bool do_nothing(void);
  bool do_unknown(void);

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

  // Recognized commands table:
  static const Entry command_table[];

  // Reference value:
  static BOOST_CONSTEXPR_OR_CONST double rate_scale = 200.0;
};

#endif
