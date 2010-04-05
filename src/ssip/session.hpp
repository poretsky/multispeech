// session.hpp -- SSIP client communication
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

#ifndef MULTISPEECH_SSIP_SESSION_HPP
#define MULTISPEECH_SSIP_SESSION_HPP

#include <bobcat/cmdfinder>

#include <mscore/session.hpp>

#include "connection.hpp"
#include "message.hpp"
#include "proxy.hpp"
#include "parser.hpp"

namespace SSIP
{

class session:
  private commands,
  private settings,
  private connection,
  private multispeech::session,
  public message
{
public:
  // Object constructor:
  session(proxy* origin, int socket_fd);

  // Session loop execution functor:
  void operator()(void);

private:
  // General commands dispatcher:
  bool cmd_set(void);
  bool cmd_quit(void);
  bool cmd_help(void);
  bool cmd_unknown(void);

  // Parameter settings:
  message::code set_client_name(void);

  // Client requests parsing and execution:
  bool perform(std::string& request);

  // Host server reference:
  proxy& host;

  // Unique session id:
  unsigned long id;

  // Session counter for id generation:
  static unsigned long count;
};

} // namespace SSIP

#endif
