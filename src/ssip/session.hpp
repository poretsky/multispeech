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

#include <string>
#include <vector>

#include <mscore/session.hpp>
#include <mscore/job.hpp>

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
  // Client requests parsing and execution:
  bool perform(std::string& request);

  // Prepare speech task from text string and put it into the job container:
  void prepare(const std::string& text);

  // Submit prepared job and reply to the client:
  void commit(void);

  // General commands dispatcher methods:
  bool cmd_speak(void);
  bool cmd_char(void);
  bool cmd_block(void);
  bool cmd_set(void);
  bool cmd_quit(void);
  bool cmd_help(void);
  bool cmd_unknown(void);
  bool cmd_unimplemented(void);

  // Parameter setting methods:
  message::code set_client_name(void);
  message::code set_notification(void);
  message::code set_punctuation(void);
  message::code set_priority(void);
  message::code set_rate(void);
  message::code set_pitch(void);
  message::code set_volume(void);

  // Request destination parser:
  destination target;

  // Session local data:
  client_info client;
  notification_mode notification;
  block_mode block;
  punctuation_mode punctuation;
  urgency_mode priority;
  digital_value rate_factor;
  digital_value pitch_factor;
  digital_value volume_factor;

  // Text accumulator for multiline messages:
  std::vector<std::string> accumulator;

  // Job accumulator:
  multispeech::job errand;

  // Host server reference:
  proxy& host;

  // Text receiving mode indicator:
  bool receiving;

  // Unique session id:
  unsigned long id;

  // Session counter for id generation:
  static unsigned long count;
};

} // namespace SSIP

#endif
