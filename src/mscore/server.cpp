// server.cpp -- General speech server functionality implementation
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

#include <sysconfig.h>

#include "server.hpp"


namespace multispeech
{

using namespace std;
using namespace FBB;


// Open logging stream:
SyslogStream server::log(PACKAGE_NAME, NOTICE, USER, LOG_PID);
bool server::verbose = false;
bool server::debug = false;


// Object constructor:

server::server(int argc, char* argv[]):
  configuration(argc, argv),
  sound_manager(dynamic_cast<configuration*>(this)),
  polyglot(dynamic_cast<configuration*>(this))
{
}

} // namespace multispeech