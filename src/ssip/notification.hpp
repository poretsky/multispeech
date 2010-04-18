// notification.hpp -- SSIP notification mode setup request parser interface
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

#ifndef MULTISPEECH_SSIP_NOTIFICATION_HPP
#define MULTISPEECH_SSIP_NOTIFICATION_HPP

#include <string>

#include <bobcat/cmdfinder>

#include "message.hpp"

namespace SSIP
{

class notification: private FBB::CmdFinder<message::code (notification::*)(void)>
{
public:
  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit notification(unsigned int& holder);

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
  message::code notify_unknown(void);

  // Parse and setup requested mode.
  message::code set_mode(unsigned int mask);

  // Value holder reference.
  unsigned int& value;

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
