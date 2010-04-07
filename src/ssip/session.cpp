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
using namespace boost;
using namespace FBB;


// Counter for session id generation:
unsigned long session::count = 0;


// Session object construction:

session::session(proxy* origin, int socket_fd):
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


// Parsing client requests:

bool
session::perform(string& request)
{
  mutex::scoped_lock lock(host.access);
  bool result = true;
  commands::FunctionPtr done = commands::findCmd(request);
  if (!commands::cmd().empty())
    result = (this->*done)();
  return result;
}


// Serving client requests:

bool
session::cmd_set(void)
{
  emit((this->*settings::findCmd(target.parse(commands::beyond())))());
  return true;
}

bool
session::cmd_quit(void)
{
  emit(message::OK_BYE);
  return false;
}

bool
session::cmd_help(void)
{
  emit_help();
  return true;
}

bool
session::cmd_unknown(void)
{
  emit(message::ERR_INVALID_COMMAND);
  return true;
}

bool
session::cmd_unimplemented(void)
{
  emit(message::ERR_NOT_IMPLEMENTED);
  return true;
}


// Parameter settings:

message::code
session::set_client_name(void)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (target.selection() == destination::self)
    rc = client.name(settings::beyond());
  return rc;
}

message::code
session::set_notification(void)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (target.selection() == destination::self)
    rc = notification.setup(settings::beyond());
  return rc;
}

} // namespace SSIP
