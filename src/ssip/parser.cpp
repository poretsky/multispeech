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

#include <mscore/notification.hpp>

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

// Destination checkers table:
const destination::Entry destination::table[] =
  {
    Entry("self", &destination::check_self),
    Entry("all", &destination::check_all),
    Entry("", &destination::check_another)
  };

// Destination id validator:
const regex destination::validator("^\\d+$");

// Boolean flag parser table:
const boolean_flag::Entry boolean_flag::table[] =
  {
    Entry("on", &boolean_flag::flag_on),
    Entry("off", &boolean_flag::flag_off),
    Entry("", &boolean_flag::flag_invalid)
  };

// Client name string pattern:
const regex client_info::name_pattern("^([[:word:]-]+):([[:word:]-]+):([[:word:]-]+)$");

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


// Settings dispatcher:

settings::settings(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

// Dummy implementations:

message::code
settings::set_client_name(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_priority(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_output_module(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_language(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_ssml_mode(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_punctuation(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_spelling(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_cap_let_recogn(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_voice(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_synthesis_voice(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_rate(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_pitch(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_volume(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_pause_context(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_history(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
settings::set_notification(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

// Actual implementation:

message::code
settings::set_unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
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


// Boolean parameters parser:

boolean_flag::boolean_flag(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

  // Dummy implementation:

message::code
boolean_flag::flag_on(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
boolean_flag::flag_off(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

// Real implementation:

message::code
boolean_flag::flag_invalid(void)
{
  return message::ERR_PARAMETER_NOT_ON_OFF;
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


// Notification mode setup and access:

notification_mode::notification_mode(void):
  value(0)
{
}

// Public methods:

notification_mode::operator unsigned int(void) const
{
  return value;
}

message::code
notification_mode::setup(const string& request)
{
  return (this->*notification_setup::findCmd(request))();
}

// Private methods:

message::code
notification_mode::notify_all(void)
{
  mask = notification::job_start |
    notification::job_complete |
    notification::job_cancel |
    notification::job_pause |
    notification::job_resume;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_begin(void)
{
  mask = notification::job_start;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_end(void)
{
  mask = notification::job_complete;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_cancel(void)
{
  mask = notification::job_cancel;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_pause(void)
{
  mask = notification::job_pause;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_resume(void)
{
  mask = notification::job_resume;
  return (this->*boolean_flag::findCmd(notification_setup::beyond()))();
}

message::code
notification_mode::notify_index_marks(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
notification_mode::flag_on(void)
{
  value |= mask;
  return message::OK_NOTIFICATION_SET;
}

message::code
notification_mode::flag_off(void)
{
  value &= ~mask;
  return message::OK_NOTIFICATION_SET;
}


// Block mode support:

block_mode::block_mode(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  state(false)
{
}

// Public methods:

block_mode::operator bool(void) const
{
  return state;
}

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

punctuation_mode::punctuation_mode(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(punctuations::unknown)
{
}

// Public methods:

punctuations::mode
punctuation_mode::parse(const string& request)
{
  return (this->*findCmd(request))();
}

punctuation_mode::operator punctuations::mode(void) const
{
  return value;
}

void
punctuation_mode::operator()(punctuations::mode mode)
{
  value = mode;
}

// Private methods:

punctuations::mode
punctuation_mode::all(void)
{
  return punctuations::all;
}

punctuations::mode
punctuation_mode::some(void)
{
  return punctuations::some;
}

punctuations::mode
punctuation_mode::none(void)
{
  return punctuations::none;
}

punctuations::mode
punctuation_mode::unknown(void)
{
  return punctuations::unknown;
}


// Priority control:

urgency_mode::urgency_mode(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(text)
{
}

// Public methods:

message::code
urgency_mode::setup(const string& request)
{
  return (this->*findCmd(request))();
}

urgency_mode::operator category(void) const
{
  return value;
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

} // namespace SSIP
