// parser.cpp -- SSIP parser tables
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

#include <boost/lexical_cast.hpp>

#include <mscore/job.hpp>

#include "parser.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;
using namespace FBB;
using namespace multispeech;


// Static data

// General commands table:
const commands::Entry commands::table[] =
  {
    Entry("speak", &commands::cmd_speak),
    Entry("char", &commands::cmd_char),
    Entry("key", &commands::cmd_key),
    Entry("sound_icon", &commands::cmd_sound_icon),
    Entry("stop", &commands::cmd_stop),
    Entry("cancel", &commands::cmd_cancel),
    Entry("pause", &commands::cmd_pause),
    Entry("resume", &commands::cmd_resume),
    Entry("block", &commands::cmd_block),
    Entry("list", &commands::cmd_list),
    Entry("set", &commands::cmd_set),
    Entry("history", &commands::cmd_history),
    Entry("quit", &commands::cmd_quit),
    Entry("help", &commands::cmd_help),
    Entry("", &commands::cmd_unknown)
  };

// Destination checkers table:
const destination::Entry destination::table[] =
  {
    Entry("self", &destination::check_self),
    Entry("all", &destination::check_all),
    Entry("", &destination::check_another)
  };

// Destination id validator:
const regex destination::validator("^\\d+$");

// Parameter setting subcommands table:
const settings::Entry settings::table[] =
  {
    Entry("client_name", &settings::set_client_name),
    Entry("priority", &settings::set_priority),
    Entry("output_module", &settings::set_output_module),
    Entry("language", &settings::set_language),
    Entry("ssml_mode", &settings::set_ssml_mode),
    Entry("punctuation", &settings::set_punctuation),
    Entry("spelling", &settings::set_spelling),
    Entry("cap_let_recogn", &settings::set_cap_let_recogn),
    Entry("voice", &settings::set_voice),
    Entry("synthesis_voice", &settings::set_synthesis_voice),
    Entry("rate", &settings::set_rate),
    Entry("pitch", &settings::set_pitch),
    Entry("volume", &settings::set_volume),
    Entry("pause_context", &settings::set_pause_context),
    Entry("history", &settings::set_history),
    Entry("notification", &settings::set_notification),
    Entry("", &settings::set_unknown)
  };

// Boolean flag parser table:
const boolean_flag::Entry boolean_flag::table[] =
  {
    Entry("on", &boolean_flag::flag_on),
    Entry("off", &boolean_flag::flag_off),
    Entry("", &boolean_flag::flag_invalid)
  };

// Client name string pattern:
const regex client_info::name_pattern("^\"?([[:word:]-]+):([[:word:]-]+):([[:word:]-]+)\"?\\s*$");

// Notification mode parsing table:
const notification_setup::Entry notification_setup::table[] =
  {
    Entry("all", &notification_setup::notify_all),
    Entry("begin", &notification_setup::notify_begin),
    Entry("end", &notification_setup::notify_end),
    Entry("cancel", &notification_setup::notify_cancel),
    Entry("pause", &notification_setup::notify_pause),
    Entry("resume", &notification_setup::notify_resume),
    Entry("index_marks", &notification_setup::notify_index_marks),
    Entry("", &notification_setup::notify_unknown)
  };

// Block command parser table:
const block_mode::Entry block_mode::table[] =
  {
    Entry("begin", &block_mode::begin),
    Entry("end", &block_mode::end),
    Entry("", &block_mode::unknown)
  };

// Punctuation mode setup request parser table:
const punctuation_mode::Entry punctuation_mode::table[] =
  {
    Entry("all", &punctuation_mode::all),
    Entry("some", &punctuation_mode::some),
    Entry("none", &punctuation_mode::none),
    Entry("", &punctuation_mode::unknown)
  };

// Priority setup requests parser table:
const urgency_mode::Entry urgency_mode::table[] =
  {
    Entry("important", &urgency_mode::set_important),
    Entry("message", &urgency_mode::set_message),
    Entry("text", &urgency_mode::set_text),
    Entry("notification", &urgency_mode::set_notification),
    Entry("progress", &urgency_mode::set_progress),
    Entry("", &urgency_mode::set_unknown)
  };

// Digital parameter value pattern:
const boost::regex digital_value::pattern("^([+-]?\\d+)\\s*$");

// Capitalization mode request parser table:
const capitalization::Entry capitalization::table[] =
  {
    Entry("none", &capitalization::caps_none),
    Entry("spell", &capitalization::caps_spell),
    Entry("icon", &capitalization::caps_icon),
    Entry("", &capitalization::caps_unknown)
  };


// General commands dispatcher:

commands::commands(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

// Dummy implementations:

bool
commands::cmd_speak(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_char(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_key(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_sound_icon(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_stop(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_cancel(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_pause(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_resume(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_block(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_list(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_set(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_history(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_quit(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_help(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_unknown(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_unimplemented(void)
{
  return true;
}


// Request destination parser:

destination::destination(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

// Public methods:

const string&
destination::parse(const string& request)
{
  (this->*findCmd(request))();
  return beyond();
}

destination::choice
destination::selection(void) const
{
  return state;
}

unsigned long
destination::id(void) const
{
  return value;
}

// Private methods:

void
destination::check_self(void)
{
  state = self;
}

void
destination::check_all(void)
{
  state = all;
}

void
destination::check_another(void)
{
  if (regex_match(cmd(), validator))
    {
      state = another;
      value = lexical_cast<unsigned long>(cmd());
    }
  else state = invalid;
}


// Settings dispatcher:

settings::settings(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

// Dummy implementations:

message::code
settings::set_client_name(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_priority(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_output_module(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_language(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_ssml_mode(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_punctuation(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_spelling(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_cap_let_recogn(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_voice(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_synthesis_voice(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_rate(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_pitch(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_volume(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_pause_context(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_history(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_notification(destination& target)
{
  return message::ERR_NOT_IMPLEMENTED;
}

// Actual implementation:

message::code
settings::set_unknown(destination& target)
{
  return message::ERR_PARAMETER_INVALID;
}


// Boolean parameters parser:

boolean_flag::boolean_flag(bool& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}

// Public methods:

bool
boolean_flag::parse(const string& request)
{
  return (this->*findCmd(request))();
}

// Private methods:

bool
boolean_flag::flag_on(void)
{
  value = true;
  return true;
}

bool
boolean_flag::flag_off(void)
{
  value = false;
  return true;
}

bool
boolean_flag::flag_invalid(void)
{
  return false;
}


// Client info parsing and access:

client_info::client_info(void):
  user_name("unknown"),
  application_name("unknown"),
  component_name("unknown"),
  unknown(true)
{
}

message::code
client_info::name(const string& full_name)
{
  message::code rc = message::OK_CLIENT_NAME_SET;
  if (unknown)
    {
      smatch split;
      if (regex_match(full_name, split, name_pattern))
        {
          if (split[1].matched)
            user_name = string(split[1].first, split[1].second);
          if (split[2].matched)
            application_name = string(split[2].first, split[2].second);
          if (split[3].matched)
            component_name = string(split[3].first, split[3].second);
          unknown = false;
        }
      else rc = message::ERR_PARAMETER_INVALID;
    }
  else rc = message::ERR_COULDNT_SET_CLIENT_NAME;
  return rc;
}

string
client_info::name(void) const
{
  return user_name + ':' + application_name + ':' + component_name;
}

const string&
client_info::user(void) const
{
  return user_name;
}

const string&
client_info::application(void) const
{
  return application_name;
}

const string&
client_info::component(void) const
{
  return component_name;
}


// Notification setup request parser:

notification_setup::notification_setup(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

message::code
notification_setup::notify_unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
}


// Notification mode setup:

notification_mode::notification_mode(unsigned int& holder):
  boolean_flag(flag),
  value(holder)
{
}

// Public methods:

message::code
notification_mode::setup(const string& request)
{
  return (this->*notification_setup::findCmd(request))();
}

// Private methods:

message::code
notification_mode::notify_all(void)
{
  return set_mode(job::all_events);
}

message::code
notification_mode::notify_begin(void)
{
  return set_mode(job::started);
}

message::code
notification_mode::notify_end(void)
{
  return set_mode(job::complete);
}

message::code
notification_mode::notify_cancel(void)
{
  return set_mode(job::cancelled);
}

message::code
notification_mode::notify_pause(void)
{
  return set_mode(job::paused);
}

message::code
notification_mode::notify_resume(void)
{
  return set_mode(job::resumed);
}

message::code
notification_mode::notify_index_marks(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
notification_mode::set_mode(unsigned int mask)
{
  message::code rc = message::ERR_PARAMETER_NOT_ON_OFF;
  if (boolean_flag::parse(notification_setup::beyond()))
    {
      if (flag)
        value |= mask;
      else value &= ~mask;
      rc = message::OK_NOTIFICATION_SET;
    }
  return rc;
}


// Block mode support:

block_mode::block_mode(bool& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  state(holder)
{
}

// Public methods:

message::code
block_mode::toggle(const string& request)
{
  return (this->*findCmd(request))();
}

// Private methods:

message::code
block_mode::begin(void)
{
  message::code rc = message::ERR_ALREADY_INSIDE_BLOCK;
  if (!state)
    {
      state = true;
      rc = message::OK_INSIDE_BLOCK;
    }
  return rc;
}

message::code
block_mode::end(void)
{
  message::code rc = message::ERR_ALREADY_OUTSIDE_BLOCK;
  if (state)
    {
      state = false;
      rc = message::OK_OUTSIDE_BLOCK;
    }
  return rc;
}

message::code
block_mode::unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
}


// Punctuation mode control means:

punctuation_mode::punctuation_mode(punctuations::mode& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}

// Public methods:

bool
punctuation_mode::parse(const string& request)
{
  return (this->*findCmd(request))();
}

// Private methods:

bool
punctuation_mode::all(void)
{
  value = punctuations::all;
  return true;
}

bool
punctuation_mode::some(void)
{
  value = punctuations::some;
  return true;
}

bool
punctuation_mode::none(void)
{
  value = punctuations::none;
  return true;
}

bool
punctuation_mode::unknown(void)
{
  return false;
}


// Priority control:

urgency_mode::urgency_mode(category& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}

// Public methods:

message::code
urgency_mode::setup(const string& request)
{
  return (this->*findCmd(request))();
}

// Private methods:

message::code
urgency_mode::set_important(void)
{
  value = important;
  return message::OK_PRIORITY_SET;
}

message::code
urgency_mode::set_message(void)
{
  value = message;
  return message::OK_PRIORITY_SET;
}

message::code
urgency_mode::set_text(void)
{
  value = text;
  return message::OK_PRIORITY_SET;
}

message::code
urgency_mode::set_notification(void)
{
  value = notification;
  return message::OK_PRIORITY_SET;
}

message::code
urgency_mode::set_progress(void)
{
  value = progress;
  return message::OK_PRIORITY_SET;
}

message::code
urgency_mode::set_unknown(void)
{
  return message::ERR_UNKNOWN_PRIORITY;
}


// Dealing with digital parameters:

digital_value::digital_value(double& holder):
  factor(holder)
{
}

// Public methods:

digital_value::status
digital_value::parse(const string& request)
{
  smatch extractor;
  status result = acceptable;
  if (regex_match(request, extractor, pattern))
    {
      int value = lexical_cast<int>(string(extractor[1].first, extractor[1].second));
      if (value < bottom)
        result = too_low;
      else if (value > top)
        result = too_high;
      else
        {
          factor = static_cast<double>(value);
          factor -= (top + bottom) / 2;
          factor /= (top - bottom) / 2;
          factor += 1.0;
          if (factor <= 0.0)
            factor = epsilon;
        }
    }
  else result = invalid;
  return result;
}


// Capitalization mode request parser:

capitalization::capitalization(mode& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}

// Public methods:

bool
capitalization::parse(const string& request)
{
  return (this->*findCmd(request))();
}

// Private methods:

bool
capitalization::caps_none(void)
{
  value = none;
  return true;
}

bool
capitalization::caps_spell(void)
{
  value = spell;
  return true;
}

bool
capitalization::caps_icon(void)
{
  value = icon;
  return true;
}

bool
capitalization::caps_unknown(void)
{
  return false;
}

} // namespace SSIP
