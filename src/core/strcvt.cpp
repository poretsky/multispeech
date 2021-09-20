// strcvt.cpp -- String conversion implementation
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

#include <string>

#include <boost/locale/encoding.hpp>

#include "strcvt.hpp"

using namespace std;
using namespace boost::locale::conv;


intern_string::intern_string(const string& s, const locale& charset_holder):
  wstring(to_utf<wchar_t>(s, charset_holder))
{
}

intern_string::intern_string(const string& s, const string& charset):
  wstring(charset.empty() ?
          to_utf<wchar_t>(s, locale()) :
          to_utf<wchar_t>(s, charset))
{
}


extern_string::extern_string(const wstring& s, const locale& charset_holder):
  string(from_utf(s, charset_holder))
{
}

extern_string::extern_string(const wstring& s, const string& charset):
  string(charset.empty() ?
         from_utf(s, locale()) :
         from_utf(s, charset))
{
}
