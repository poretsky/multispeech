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

#include "parser.hpp"


namespace SSIP
{

  using namespace FBB;

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

// Settings table:
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

} // namespace SSIP
