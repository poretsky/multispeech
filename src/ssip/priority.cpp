// priority.cpp -- Priority setup request parser implementation
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

#include "priority.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;


// Recognized requests:
const priority::Entry priority::table[] =
  {
    Entry("important", &priority::set_important),
    Entry("message", &priority::set_message),
    Entry("text", &priority::set_text),
    Entry("notification", &priority::set_notification),
    Entry("progress", &priority::set_progress),
    Entry("", &priority::set_unknown)
  };


// Main constructor:

priority::priority(category& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}


// Public methods:

message::code
priority::setup(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

message::code
priority::set_important(void)
{
  value = important;
  return message::OK_PRIORITY_SET;
}

message::code
priority::set_message(void)
{
  value = message;
  return message::OK_PRIORITY_SET;
}

message::code
priority::set_text(void)
{
  value = text;
  return message::OK_PRIORITY_SET;
}

message::code
priority::set_notification(void)
{
  value = notification;
  return message::OK_PRIORITY_SET;
}

message::code
priority::set_progress(void)
{
  value = progress;
  return message::OK_PRIORITY_SET;
}

message::code
priority::set_unknown(void)
{
  return message::ERR_UNKNOWN_PRIORITY;
}

} // namespace SSIP
