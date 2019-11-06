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

#include "server.hpp"

class spd_backend: public server
{
public:
  // Construct the object:
  explicit spd_backend(const configuration& conf);

private:
  // Serving queue done events:
  void queue_done(void);

  // Input method and command syntax definition (see base class):
  void get_command(void);
  bool perform_command(void);
};

#endif
