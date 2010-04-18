// notification.cpp -- SSIP notification mode setup request parser implementation
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

#include <mscore/job.hpp>

#include "notification.hpp"

#include "boolean_flag.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;
using namespace multispeech;


// Recognized requests:
const notification::Entry notification::table[] =
  {
    Entry("all", &notification::notify_all),
    Entry("begin", &notification::notify_begin),
    Entry("end", &notification::notify_end),
    Entry("cancel", &notification::notify_cancel),
    Entry("pause", &notification::notify_pause),
    Entry("resume", &notification::notify_resume),
    Entry("index_marks", &notification::notify_index_marks),
    Entry("", &notification::notify_unknown)
  };


// Main constructor:

notification::notification(unsigned int& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}


// Public methods:

message::code
notification::setup(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

message::code
notification::notify_all(void)
{
  return set_mode(job::all_events);
}

message::code
notification::notify_begin(void)
{
  return set_mode(job::started);
}

message::code
notification::notify_end(void)
{
  return set_mode(job::complete);
}

message::code
notification::notify_cancel(void)
{
  return set_mode(job::cancelled);
}

message::code
notification::notify_pause(void)
{
  return set_mode(job::paused);
}

message::code
notification::notify_resume(void)
{
  return set_mode(job::resumed);
}

message::code
notification::notify_index_marks(void)
{
  return message::ERR_NOT_IMPLEMENTED;
}

message::code
notification::notify_unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
}

message::code
notification::set_mode(unsigned int mask)
{
  bool flag;
  boolean_flag request(flag);
  message::code rc = message::ERR_PARAMETER_NOT_ON_OFF;
  if (request.parse(beyond()))
    {
      if (flag)
        value |= mask;
      else value &= ~mask;
      rc = message::OK_NOTIFICATION_SET;
    }
  return rc;
}

} // namespace SSIP
