// language_description.cpp -- Language specific stuff implementation
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

#include <sysconfig.h>

#include <sstream>

#include "language_description.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;


// Construct / destroy:

language_description::language_description(const char* language_id,
                                           const wchar_t* language_detector):
  id_str(language_id),
  detector(language_detector)
{
  filter_chain.setup()
    (L"\\s+", L" ");
}

language_description::~language_description(void)
{
}


// General actions:

const char*
language_description::id(void) const
{
  return id_str;
}

bool
language_description::recognize(const wstring& s)
{
  return regex_search(s, detector, match_default | match_any);
}

wstring
language_description::filter(const wstring& s)
{
  wostringstream result;
  filter_chain.push(result);
  filter_chain << s;
  filter_chain.pop();
  return result.str();
}

wstring
language_description::translate(const wstring& s) const
{
  map<const wstring, const wstring>::const_iterator item = dictionary.find(s);
  return (item == dictionary.end()) ? wstring(L"") : item->second;
}


// Abbreviation speller:

language_description::spell::spell(language_description* owner):
  language(owner)
{
}

wstring
language_description::spell::operator()(const iterator_range<wstring::const_iterator>& s)
{
  return language->do_spell(s);
}

} // namespace multispeech
