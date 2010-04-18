// block.hpp -- Block in/out requests parser interface
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

#ifndef MULTISPEECH_SSIP_BLOCK_HPP
#define MULTISPEECH_SSIP_BLOCK_HPP

#include <string>

#include <bobcat/cmdfinder>

#include "message.hpp"

namespace SSIP
{

class block: private FBB::CmdFinder<message::code (block::*)(void)>
{
public:
  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit block(bool& holder);

  // Parse toggle request.
  message::code toggle(const std::string& request);

private:
  // Request performers.
  message::code begin(void);
  message::code end(void);
  message::code unknown(void);

  // Current state holder reference.
  bool& state;

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
