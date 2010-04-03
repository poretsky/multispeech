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
#include "server.hpp"

namespace SSIP
{

class session:
  private FBB::CmdFinder<bool (session::*)(void)>,
  private SSIP::connection,
  private multispeech::session,
  public SSIP::message
{
public:
  // Object constructor:
  session(class SSIP::server* origin, int socket_fd);

  // Session loop execution functor:
  void operator()(void);

private:
  // Serving requests:
  bool do_quit(void);
  bool do_unknown(void);

  // Client requests parsing and execution:
  bool perform(std::string& request);

  // Host server reference:
  class SSIP::server& host;

  // Unique session id:
  unsigned long id;

  // Recognized client requests:
  static Entry command_table[];

  // Session counter for id generation:
  static unsigned long count;
};

} // namespace SSIP

#endif
