// capitalization.hpp -- Capital letter recognition mode setup request parser interface
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

#ifndef MULTISPEECH_SSIP_CAPITALIZATION_HPP
#define MULTISPEECH_SSIP_CAPITALIZATION_HPP

#include <string>

#include <bobcat/cmdfinder>

namespace SSIP
{

class capitalization: private FBB::CmdFinder<bool (capitalization::*)(void)>
{
public:
  // Available mode values:
  enum mode
  {
    none,
    spell,
    icon
  };

  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit capitalization(mode& holder);

  // Parse request and store result. Return true when success.
  bool parse(const std::string& request);

private:
  // Value detectors.
  bool caps_none(void);
  bool caps_spell(void);
  bool caps_icon(void);
  bool caps_unknown(void);

  // Value holder.
  mode& value;

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
