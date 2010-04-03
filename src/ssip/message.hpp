// message.hpp -- SSIP message sender interface
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

#ifndef MULTISPEECH_SSIP_MESSAGE_HPP
#define MULTISPEECH_SSIP_MESSAGE_HPP

#include <string>
#include <ostream>
#include <map>

namespace SSIP
{

class message
{
public:
  // Constructor takes output stream reference as an argument.
  explicit message(std::ostream& destination);

  // Send message along with it's result code. If text is empty or
  // not specified, it will be searched in dictionary for that code.
  void emit(unsigned int rc, const std::string& text = "");

private:
  // Output stream reference.
  std::ostream& sink;

  // Message dictionary.
  static std::map<unsigned int, const std::string> dictionary;
};

} // namespace SSIP

#endif
