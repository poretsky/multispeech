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

#include <boost/regex.hpp>

#include <mscore/session.hpp>

#include "connection.hpp"
#include "message.hpp"
#include "proxy.hpp"
#include "parser.hpp"

namespace SSIP
{

// Client info representation.
class client_info
{
public:
  // Construct and initialize:
  client_info(void);

  // Setup client name information. It can be done only once
  // after instantiation. Return response code for the requester.
  message::code name(const std::string& full_name);

  // Retrieve client full name and it's components.
  std::string name(void) const;
  const std::string& user(void) const;
  const std::string& application(void) const;
  const std::string& component(void) const;

private:
  // Client name information:
  std::string user_name, application_name, component_name;

  // Indicate that it is not set up yet.
  bool unknown;

  // Client name string pattern.
  static const boost::regex name_pattern;
};

// Driving a session.
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
  // Client requests parsing and execution:
  bool perform(std::string& request);

  // General commands dispatcher:
  bool cmd_set(void);
  bool cmd_quit(void);
  bool cmd_help(void);
  bool cmd_unknown(void);
  bool cmd_unimplemented(void);

  // Parameter settings:
  message::code set_client_name(void);

  // Request destination parser:
  destination target;

  // Session local data:
  client_info client;

  // Host server reference:
  proxy& host;

  // Unique session id:
  unsigned long id;

  // Session counter for id generation:
  static unsigned long count;
};

} // namespace SSIP

#endif
