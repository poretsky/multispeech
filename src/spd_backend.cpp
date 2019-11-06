// spd_backend.cpp -- Speech Dispatcher module interface implementation
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

#include "spd_backend.hpp"


// Object construction:

spd_backend::spd_backend(const configuration& conf):
  server(conf)
{
}


// Events serving:

void
spd_backend::queue_done(void)
{
}


// Input method:

void
spd_backend::get_command(void)
{
}


// Command set and syntax implementation:

bool
spd_backend::perform_command(void)
{
  return false;
}
