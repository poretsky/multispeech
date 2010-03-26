// strcvt.hpp -- String conversion interface
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>
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

// Use these classes to transform strings from their external  to internal
// representation and vice versa according to specified locale.

#ifndef MULTISPEECH_STRCVT_HPP
#define MULTISPEECH_STRCVT_HPP

#include <string>
#include <locale>

namespace multispeech
{

// Initialize wide character string from ordinary string decoding it
// by the way according to specified locale.
class intern_string: public std::wstring
{
public:
  intern_string(const std::string& s, const std::locale& charset_holder);
};

// Initialize ordinary string from wide character string encoding it
// by the way according to specified locale.
class extern_string: public std::string
{
public:
  extern_string(const std::wstring& s, const std::locale& charset_holder);
};

} // namespace multispeech

#endif
