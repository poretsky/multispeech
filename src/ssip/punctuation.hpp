// punctuation.hpp -- Punctuation mode setup request parser interface
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

#ifndef MULTISPEECH_SSIP_PUNCTUATION_HPP
#define MULTISPEECH_SSIP_PUNCTUATION_HPP

#include <string>

#include <bobcat/cmdfinder>

#include <mscore/text_filter.hpp>

namespace SSIP
{

class punctuation: private FBB::CmdFinder<bool (punctuation::*)(void)>
{
public:
  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit punctuation(multispeech::punctuations::mode& holder);

  // Parse request.
  bool parse(const std::string& request);

private:
  // Mode detectors.
  bool all(void);
  bool some(void);
  bool none(void);
  bool unknown(void);

  // Current value holder reference.
  multispeech::punctuations::mode& value;

  // Table of recognized requests.
  static const Entry table[];
};

} // namespace SSIP

#endif
