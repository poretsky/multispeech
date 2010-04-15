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


// Parameter setting subcommands parser.
class settings: protected FBB::CmdFinder<message::code (settings::*)(destination&)>
{
protected:
  // Main constractor.
  settings(void);

private:
  // These methods are to be implemented in the derived class.
  // Return result code for client.
  virtual message::code set_client_name(destination& target);
  virtual message::code set_priority(destination& target);
  virtual message::code set_output_module(destination& target);
  virtual message::code set_language(destination& target);
  virtual message::code set_ssml_mode(destination& target);
  virtual message::code set_punctuation(destination& target);
  virtual message::code set_spelling(destination& target);
  virtual message::code set_cap_let_recogn(destination& target);
  virtual message::code set_voice(destination& target);
  virtual message::code set_synthesis_voice(destination& target);
  virtual message::code set_rate(destination& target);
  virtual message::code set_pitch(destination& target);
  virtual message::code set_volume(destination& target);
  virtual message::code set_pause_context(destination& target);
  virtual message::code set_history(destination& target);
  virtual message::code set_notification(destination& target);

  // This method is properly provided here.
  message::code set_unknown(destination& target);

  // Subcommands table.
  static const Entry table[];
};


// Boolean parameters parser.
class boolean_flag: private FBB::CmdFinder<bool (boolean_flag::*)(void)>
{
public:
  // Main constructor.
  explicit boolean_flag(bool& holder);

  // Request parser. Returns true if the request is valid.
  bool parse(const std::string& request);

private:
  // These methods are used in valid cases, so return true.
  bool flag_on(void);
  bool flag_off(void);

  // This method returns false as a reaction on invalid request.
  bool flag_invalid(void);

  // Target value holder reference.
  bool& value;

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


// Notification mode parsing.
class notification_mode:
  private notification_setup,
  private boolean_flag
{
public:
  // Main constructor.
  explicit notification_mode(unsigned int& holder);

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

  // Parse and setup requested mode.
  message::code set_mode(unsigned int mask);

  // Boolean flag parse result.
  bool flag;

  // Value holder reference.
  unsigned int& value;
};


// Block mode control.
class block_mode: private FBB::CmdFinder<message::code (block_mode::*)(void)>
{
public:
  // Main constructor.
  explicit block_mode(bool& holder);

  // Parse toggle request.
  message::code toggle(const std::string& request);

private:
  // Request performers.
  message::code begin(void);
  message::code end(void);
  message::code unknown(void);

  // Current state holder reference.
  bool& state;

  // Request parser table.
  static const Entry table[];
};


// Punctuation verbosity mode parsing.
class punctuation_mode: private FBB::CmdFinder<bool (punctuation_mode::*)(void)>
{
public:
  // Main constructor.
  explicit punctuation_mode(multispeech::punctuations::mode& holder);

  // Parse request.
  bool parse(const std::string& request);

private:
  // Mode detectors.
  bool all(void);
  bool some(void);
  bool none(void);
  bool unknown(void);

  // Current value holder reference.
  multispeech::punctuations::mode& value;

  // Parsing table.
  static const Entry table[];
};


// Urgency mode parsing.
class urgency_mode: FBB::CmdFinder<message::code (urgency_mode::*)(void)>
{
public:
  // Recognized categories:
  enum category
  {
    important = 5,
    message = 4,
    text = 2,
    notification = 1,
    progress = 3
  };

  // Main constructor.
  explicit urgency_mode(category& holder);

  // Parse request and setup value.
  message::code setup(const std::string& request);

private:
  // Set value according to the request.
  message::code set_important(void);
  message::code set_message(void);
  message::code set_text(void);
  message::code set_notification(void);
  message::code set_progress(void);
  message::code set_unknown(void);

  // Current value holder reference.
  category& value;

  // Parsing table.
  static const Entry table[];
};


// Dealing with digital parameters.
class digital_value
{
public:
  // Special cases:
  enum status
  {
    acceptable,
    too_low,
    too_high,
    invalid
  };

  // Main constructor.
  explicit digital_value(double& holder);

  // Validate and parse request.
  status parse(const std::string& request);

private:
  // Here we store the factor that will be applied to the configured
  // parameter value. It should never be zero or negative.
  double& factor;

  // Allowed value boundaries.
  static const int bottom = -100;
  static const int top = 100;

  // Factor value used instead of zero.
  static const double epsilon = 0.0001;

  // Pattern for validation.
  static const boost::regex pattern;
};


// Capital letter recognition mode parser.
class capitalization: private FBB::CmdFinder<bool (capitalization::*)(void)>
{
public:
  // Available mode values:
  enum mode
  {
    none,
    spell,
    icon
  };

  // Main constructor.
  explicit capitalization(mode& holder);

  // Parse request and store result. Return true when success.
  bool parse(const std::string& request);

private:
  // Value detectors.
  bool caps_none(void);
  bool caps_spell(void);
  bool caps_icon(void);
  bool caps_unknown(void);

  // Value holder.
  mode& value;

  // Parser table.
  static const Entry table[];
};

} // namespace SSIP

#endif
