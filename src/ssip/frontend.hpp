// frontend.hpp -- Speech Dispatcher module interactions
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

#ifndef MULTISPEECH_SSIP_FRONTEND_HPP
#define MULTISPEECH_SSIP_FRONTEND_HPP

#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/regex.hpp>

#include <bobcat/cmdfinder>

#include "config.hpp"
#include "speech_server.hpp"
#include "text_filter.hpp"
#include "spd_settings.hpp"

class frontend:
  public speech_server,
  private FBB::CmdFinder<bool (frontend::*)(void)>
{
public:
  // Object instantiation:
  static frontend* instantiate(const configuration& conf);

  // Common message:
  static const std::string bad_syntax;

private:
  // Speech_Server states:
  enum status
  {
    idle,
    speaking,
    pausing,
    stopping
  };

  // Construct the object:
  frontend(const configuration& conf);

  // Clear temporary data in order to start command reception cycle:
  void communication_reset(void);

  // Data reception control, returns true when everything is acquired:
  bool extra_data(const char* msg = 0);

  // Check if it is possible to speak:
  bool can_speak(void);
  bool single_line(void);

  // Queue execution start:
  void start_queue(void);

  // Serving events:
  void index_mark(const std::string& name);
  void queue_done(void);

  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);

  // Check for internal error:
  bool state_ok(void);

  // Place text chunk into the speech queue stripping SSML tags by the way:
  void enqueue_text_chunk(std::string::const_iterator start, std::string::const_iterator end);

  // Command executors:
  bool do_speak(void);
  bool do_sound_icon(void);
  bool do_char(void);
  bool do_key(void);
  bool do_stop(void);
  bool do_pause(void);
  bool do_list_voices(void);
  bool do_set(void);
  bool do_audio(void);
  bool do_loglevel(void);
  bool do_debug(void);
  bool do_quit(void);
  bool do_unknown(void);

  // Whether index marks reporting is enabled:
  bool index_marks_support;

  // Settings interpreter:
  spd_settings settings;

  // Sound icons directory path:
  std::string sound_icons;

  // Additional data provided with some commands:
  std::string data;

  // Additional data lines number;
  int lines;

  // API version dependent atom separator for voice list:
  char atom_separator;

  // SSML mark pattern:
  const boost::regex mark_pattern;

  // SSML tags stripper:
  text_filter stripper;

  // Synchronization means:
  boost::mutex access;
  status state;

  // Recognized commands table:
  static const Entry command_table[];
};

#endif
