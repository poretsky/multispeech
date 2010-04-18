// priority.hpp -- Priority setup request parser interface
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

#ifndef MULTISPEECH_SSIP_PRIORITY_HPP
#define MULTISPEECH_SSIP_PRIORITY_HPP

#include <string>

#include <bobcat/cmdfinder>

#include "message.hpp"

namespace SSIP
{

class priority: FBB::CmdFinder<message::code (priority::*)(void)>
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

  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit priority(category& holder);

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

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
