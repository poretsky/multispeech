// settings.cpp -- SSIP set subcommand parser basic implementation
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

#include "settings.hpp"


namespace SSIP
{

using namespace FBB;


// Recognized requests:
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


// Basic constructor:

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

} // namespace SSIP
