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

#include <mscore/text_filter.hpp>

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


// Parameter setting subcommands parser.
class settings: protected FBB::CmdFinder<message::code (settings::*)(void)>
{
protected:
  // Main constractor.
  settings(void);

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

  // Subcommands table.
  static const Entry table[];
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


// Boolean parameters parser.
class boolean_flag: protected FBB::CmdFinder<message::code (boolean_flag::*)(void)>
{
protected:
  // Main constructor.
  boolean_flag(void);

private:
  // Valid flag state reactions should be implemented in derived classes.
  virtual message::code flag_on(void);
  virtual message::code flag_off(void);

  // Invalid parameter reaction is properly provided here.
  message::code flag_invalid(void);

  // Parsing table.
  static const Entry table[];
};


// Client info parsing and representation.
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


// Notification setup request parser framework.
class notification_setup: protected FBB::CmdFinder<message::code (notification_setup::*)(void)>
{
protected:
  // Main constructor.
  notification_setup(void);

private:
  // Placeholders:
  virtual message::code notify_all(void) = 0;
  virtual message::code notify_begin(void) = 0;
  virtual message::code notify_end(void) = 0;
    virtual message::code notify_cancel(void) = 0;
  virtual message::code notify_pause(void) = 0;
  virtual message::code notify_resume(void) = 0;
  virtual message::code notify_index_marks(void) = 0;

  // Error request reaction.
  message::code notify_unknown(void);

  // Parameter parsing table:
  static const Entry table[];
};


// Notification mode parsing and representation.
class notification_mode:
  private notification_setup,
  private boolean_flag
{
public:
  // Main constructor.
  notification_mode(void);

  // Current value accessor.
  unsigned int event_mask(void) const;

  // Request parser.
  message::code setup(const std::string& request);

private:
  // Parameter parsing methods.
  message::code notify_all(void);
  message::code notify_begin(void);
  message::code notify_end(void);
  message::code notify_cancel(void);
  message::code notify_pause(void);
  message::code notify_resume(void);
  message::code notify_index_marks(void);

  // Methods required by boolean_flag.
  message::code flag_on(void);
  message::code flag_off(void);

  // Requested mask.
  unsigned int mask;

  // Current value holder.
  unsigned int value;
};


// Block mode control.
class block_mode: private FBB::CmdFinder<message::code (block_mode::*)(void)>
{
public:
  // Main constructor.
  block_mode(void);

  // Current state accessor.
  bool inside(void) const;

  // Parse toggle request.
  message::code toggle(const std::string& request);

private:
  // Request performers.
  message::code begin(void);
  message::code end(void);
  message::code unknown(void);

  // Current state holder.
  bool state;

  // Request parser table.
  static const Entry table[];
};


// Punctuation verbosity mode parsing and holding.
class punctuation_mode:
  private FBB::CmdFinder<multispeech::punctuations::mode (punctuation_mode::*)(void)>
{
public:
  // Main constructor.
  punctuation_mode(void);

  // Parse request.
  multispeech::punctuations::mode parse(const std::string& request);

  // Current state accessor.
  multispeech::punctuations::mode verbosity(void) const;

  // Change current mode.
  void verbosity(multispeech::punctuations::mode mode);

private:
  // Mode detectors.
  multispeech::punctuations::mode all(void);
  multispeech::punctuations::mode some(void);
  multispeech::punctuations::mode none(void);
  multispeech::punctuations::mode unknown(void);

  // Current value holder.
  multispeech::punctuations::mode value;

  // Parsing table.
  static const Entry table[];
};

} // namespace SSIP

#endif
