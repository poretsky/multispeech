// parser.hpp -- SSIP requests parser framework
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

#ifndef MULTISPEECH_SSIP_COMMANDS_HPP
#define MULTISPEECH_SSIP_COMMANDS_HPP

#include <string>

#include <boost/regex.hpp>

#include <bobcat/cmdfinder>

#include "message.hpp"

namespace SSIP
{

// General commands dispatcher.
class commands: protected FBB::CmdFinder<bool (commands::*)(void)>
{
protected:
  // Main constructor.
  commands(void);

private:
  // These method are to be implemented in the session class.
  // Return true if the session should be continued
  // or false otherwise.
  virtual bool cmd_speak(void);
  virtual bool cmd_char(void);
  virtual bool cmd_key(void);
  virtual bool cmd_sound_icon(void);
  virtual bool cmd_stop(void);
  virtual bool cmd_cancel(void);
  virtual bool cmd_pause(void);
  virtual bool cmd_resume(void);
  virtual bool cmd_block(void);
  virtual bool cmd_list(void);
  virtual bool cmd_set(void);
  virtual bool cmd_history(void);
  virtual bool cmd_quit(void);
  virtual bool cmd_help(void);
  virtual bool cmd_unknown(void);
  virtual bool cmd_unimplemented(void);

  // commands table.
  static const Entry table[];
};

// Session parameters holder and parameter setting subcommands parser.
class settings: protected FBB::CmdFinder<message::code (settings::*)(void)>
{
protected:
  // Main constractor.
  settings(void);

  // Set up and access client name information.
  message::code client_name(const std::string& name);
  std::string client_name(void);

private:
  // These methods are to be implemented in the derived class.
  // Return result code for client.
  virtual message::code set_client_name(void);
  virtual message::code set_priority(void);
  virtual message::code set_output_module(void);
  virtual message::code set_language(void);
  virtual message::code set_ssml_mode(void);
  virtual message::code set_punctuation(void);
  virtual message::code set_spelling(void);
  virtual message::code set_cap_let_recogn(void);
  virtual message::code set_voice(void);
  virtual message::code set_synthesis_voice(void);
  virtual message::code set_rate(void);
  virtual message::code set_pitch(void);
  virtual message::code set_volume(void);
  virtual message::code set_pause_context(void);
  virtual message::code set_history(void);
  virtual message::code set_notification(void);

  // This method is properly provided here.
  message::code set_unknown(void);

  // Client name information:
  struct
  {
    std::string user;
    std::string application;
    std::string component;
    bool unknown;
  } client;

  // Subcommands table.
  static const Entry table[];

  // Client name string pattern.
  static const boost::regex client_name_pattern;
};

// Request destination parser.
class destination: private FBB::CmdFinder<void (destination::*)(void)>
{
public:
  // Available choices:
  enum choice
  {
    self,
    another,
    all,
    invalid
  };

  // Main constructor.
  destination(void);

  // Parse the request destination field. Usually it is used after
  // general command dispatching. Being provided by the request
  // tail as an argument it checks the first field as a destination
  // parameter and returns reference to following part of request.
  const std::string& parse(const std::string& request);

  // Return destination choice after parsing.
  choice selection(void) const;

  // Return destination id after parsing.
  // Meaningful only when selection is another.
  unsigned long id(void) const;

private:
  // Internal checkers.
  void check_self(void);
  void check_all(void);
  void check_another(void);

  // Parsing results:
  choice state;
  unsigned long value;

  // Parsing table.
  static const Entry table[];

  // Id validator.
  static const boost::regex validator;
};

} // namespace SSIP

#endif
