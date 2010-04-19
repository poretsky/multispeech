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
#include "commands.hpp"
#include "settings.hpp"
#include "client_info.hpp"
#include "priority.hpp"
#include "capitalization.hpp"

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
  void commit(message::code rc);

  // Remember job as paused for later processing.
  // Pointer to the list of paused jobs must be valid.
  void remember(const multispeech::job& unit);

  // Reject paused jobs of specified or less urgency.
  // Pointer to the list of paused jobs must be valid.
  void reject(int urgency);

  // Resume previously paused jobs if any
  // and return result code for client.
  message::code resume(void);

  // Forget all paused jobs if any, but do not change paused state.
  void forget(void);

  // Submit capital letter marking sound icon or fall back
  // to beep signal if it is not exist:
  void caps_icon(void);

  // Check text encoding and set validity flag accordingly:
  void check_encoding(const std::string& text);

  // General commands dispatcher methods:
  bool cmd_speak(void);
  bool cmd_char(void);
  bool cmd_key(void);
  bool cmd_sound_icon(void);
  bool cmd_stop(void);
  bool cmd_cancel(void);
  bool cmd_pause(void);
  bool cmd_resume(void);
  bool cmd_block(void);
  bool cmd_set(void);
  bool cmd_quit(void);
  bool cmd_help(void);
  bool cmd_unknown(void);
  bool cmd_unimplemented(void);

  // Parameter setting methods:
  message::code set_client_name(destination& target);
  message::code set_notification(destination& target);
  message::code set_punctuation(destination& target);
  message::code set_priority(destination& target);
  message::code set_rate(destination& target);
  message::code set_pitch(destination& target);
  message::code set_volume(destination& target);
  message::code set_spelling(destination& target);
  message::code set_cap_let_recogn(destination& target);

  // Local environment:
  client_info client;
  unsigned int notified_events;
  bool inside_block;
  priority::category importance;
  multispeech::punctuations::mode punctuation_mode;
  double rate_factor;
  double pitch_factor;
  double volume_factor;
  bool spelling;
  capitalization::mode capitalization_mode;

  // Text accumulator for multiline messages:
  std::vector<std::string> accumulator;

  // Job accumulator:
  multispeech::job errand;

  // Pointer to the list of recalled jobs:
  multispeech::server::jobs_queue* paused;

  // Host server reference:
  proxy& host;

  // Text receiving mode indicator:
  bool receiving;

  // Encoding validity state while receiving a text:
  bool valid;

  // Unique session id:
  unsigned long id;

  // Session counter for id generation:
  static unsigned long count;
};

} // namespace SSIP

#endif
