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

#include <sstream>

#include <boost/thread/mutex.hpp>

#include "server.hpp"

class spd_backend: public server
{
public:
  // Object instantiation:
  static spd_backend* instantiate(const configuration& conf);

private:
  // Construct the object:
  spd_backend(const configuration& conf);

  // Clear temporary data in order to start command reception cycle:
  void reset(void);

  // Data reception control, returns true when everything is acquired:
  bool extra_data(void);

  // Queue execution start:
  void start_queue(void);

  // Serving queue done events:
  void queue_done(void);

  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);

  // Additional data accumulator:
  std::ostringstream accumulator;

  // Additional data lines number;
  int lines;

  // Synchronization means:
  boost::mutex access;
  bool speaking;
};

#endif
