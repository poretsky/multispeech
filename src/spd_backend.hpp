// spd_backend.hpp -- Speech Dispatcher module interactions
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

#ifndef MULTISPEECH_SPD_BACKEND_HPP
#define MULTISPEECH_SPD_BACKEND_HPP

#include <string>

#include <boost/thread/mutex.hpp>

#include <bobcat/cmdfinder>

#include "server.hpp"
#include "text_filter.hpp"
#include "spd_settings.hpp"

class spd_backend:
  public server,
  private FBB::CmdFinder<bool (spd_backend::*)(void)>
{
public:
  // Object instantiation:
  static spd_backend* instantiate(const configuration& conf);

private:
  // Server states:
  enum status
  {
    idle,
    speaking,
    stopping
  };

  // Construct the object:
  spd_backend(const configuration& conf);

  // Clear temporary data in order to start command reception cycle:
  void communication_reset(void);

  // Data reception control, returns true when everything is acquired:
  bool extra_data(void);

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

  // Command executors:
  bool do_speak(void);
  bool do_sound_icon(void);
  bool do_char(void);
  bool do_key(void);
  bool do_stop(void);
  bool do_set(void);
  bool do_quit(void);
  bool do_unknown(void);

  // Settings interpreter:
  spd_settings settings;

  // Sound icons directory path:
  std::string sound_icons;

  // Additional data provided with some commands:
  std::string data;

  // Additional data lines number;
  int lines;

  // Synchronization means:
  boost::mutex access;
  status state;

  // SSML tags stripper:
  text_filter stripper;

  // Recognized commands table:
  static const Entry command_table[];
};

#endif
