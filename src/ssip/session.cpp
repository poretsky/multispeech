// session.cpp -- SSIP session implementation
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include "session.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;


// Recognized client requests:
session::Entry session::command_table[] =
  {
    Entry("quit", &session::do_quit),
    Entry("", &session::do_unknown)
  };

// Counter for session id generation:
unsigned long session::count = 0;


// Object construction:

session::session(proxy* origin, int socket_fd):
  CmdFinder<FunctionPtr>(command_table, command_table +
                         (sizeof(command_table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  connection(socket_fd),
  multispeech::session(*input),
  message(*output),
  host(*origin),
  id(++count)
{
}


// Session loop:

void
session::operator()(void)
{
  host.hello(id, this);
  run();
  host.bye(id);
}


// Serving client requests:

bool
session::do_quit(void)
{
  emit(231);
  return false;
}

bool
session::do_unknown(void)
{
  emit(500);
  return true;
}


// Parsing client requests:

bool
session::perform(string& request)
{
  bool result = true;
  FunctionPtr done = findCmd(request);
  if (!cmd().empty())
    result = (this->*done)();
  return result;
}

} // namespace SSIP
