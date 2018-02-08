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

#include <server.hpp>
#include <session.hpp>

#include "inline_parser.hpp"

class frontend:
  private multispeech::server,
  private FBB::CmdFinder<bool (frontend::*)(void)>,
  public multispeech::session
{
public:
  // Construct the object:
  frontend(int argc, char* argv[]);

private:
  // Perform a client request.
  bool perform(std::string& request);

  // Recognized command performers:
  bool do_exit(void);
  bool do_proceed_queue(void);
  bool do_enqueue_speech(void);
  bool do_say_letter(void);
  bool do_enqueue_sound(void);
  bool do_play_sound(void);
  bool do_reset(void);
  bool do_say_message(void);
  bool do_set_char_scale(void);
  bool do_set_punctuations(void);
  bool do_set_speech_rate(void);
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

  // Extract embedded parameters:
  inline_parser* extract_parameters(std::string& data);

  // Set punctuations mode according to identifying character:
  void set_punctuations_mode(char mode);

  // Recognized commands table:
  static const Entry command_table[];

  // Multiline request accumulator.
  std::string accumulator;

  // Braces disbalance used for multiline requests completeness detection.
  int disbalance;

  // Additional parsers for inline parameters extraction:
  boost::scoped_ptr<inline_parser> native_params, dtk_params;

  // Regular expressions for commands parsing:
  const boost::regex validate_float, validate_integer,
    beep_parameters, lang_parameters, tts_parameters, garbage;

  // Reference value:
  static constexpr float rate_scale = 200.0;
};

#endif
