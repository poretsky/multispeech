// boolean_flag.hpp -- General boolean parameter parser interface
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

#ifndef MULTISPEECH_SSIP_BOOLEAN_FLAG_HPP
#define MULTISPEECH_SSIP_BOOLEAN_FLAG_HPP

#include <string>

#include <bobcat/cmdfinder>

namespace SSIP
{

class boolean_flag: private FBB::CmdFinder<bool (boolean_flag::*)(void)>
{
public:
  // Main constructor takes reference to the variable to be set
  // as an argument.
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

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
