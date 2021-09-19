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

#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "strcvt.hpp"

using namespace std;
using namespace boost::iostreams;

intern_string::intern_string(const string& s, const locale& charset_holder)
{
  size_t size = s.length();
  if (size)
    {
      int chunk_size;
      wchar_t chunk[size];
      code_converter<array_source> source(array_source(s.c_str(), size));
      source.imbue(charset_holder);
      while ((chunk_size = source.read(chunk, size)) > 0)
        append(chunk, chunk + chunk_size);
    }
}

extern_string::extern_string(const wstring& s, const locale& charset_holder)
{
  size_t size = s.length();
  if (size)
    {
      code_converter< back_insert_device<string> > sink(boost::iostreams::back_inserter(*static_cast<string*>(this)));
      sink.imbue(charset_holder);
      sink.write(s.c_str(), size);
    }
}
