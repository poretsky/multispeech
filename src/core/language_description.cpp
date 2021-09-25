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

#include <map>

#include "language_description.hpp"

using namespace std;
using namespace boost;


// Alphabets of all instantiated languages:
static map<const char*, const wchar_t*> alphabets;


// Construct / destroy:

language_description::language_description(const char* language_id,
                                           const options& language_settings,
                                           const wchar_t* alphabet,
                                           const wchar_t* language_detector):
  id(language_id),
  settings(language_settings),
  patterns(language_detector),
  test(wstring(L"(?:\\P{alpha}|[") + alphabet + L"])*", regex::normal | regex::icase)
{
  filter_chain.setup()
    (L"\\s+", L" ");
  alphabets[id] = alphabet;
}

language_description::~language_description(void)
{
}


// General actions:

bool
language_description::recognize(const wstring& s)
{
  if (detector.empty())
    {
      wstring others;
      for (auto const& item : alphabets)
        if (item.first != id)
          others += item.second;
      if (others.empty())
        detector.assign(L".", regex::normal);
      else
        {
          const wchar_t* latin = L"a-z";
          wstring symbols;
          if (alphabets[id][0] == L'a')
            for (const wchar_t* s = alphabets[id] + 3; *s; s++)
              if (others.find(*s) == string::npos)
                symbols += *s;
          if ((alphabets[id][0] == L'a') && (others.find(latin) == string::npos))
            detector.assign(L"[" + symbols + latin + L"]", regex::normal | regex::icase);
          else if (symbols.empty())
            detector.assign(patterns, regex::normal | regex::icase);
          else detector.assign(L"[" + symbols + L"]|" + patterns, regex::normal | regex::icase);
        }
    }
  return regex_search(s, detector, match_default | match_any);
}

bool
language_description::foreign(const wstring& s)
{
  return !regex_match(s, test);
}

wstring
language_description::filter(const wstring& s)
{
  return filter_chain.process(s);
}

wstring
language_description::translate(const wstring& s) const
{
  map<const wstring, const wchar_t*>::const_iterator item = dictionary.find(s);
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
  wstring result, item;
  for (wstring::const_iterator sptr = s.begin(); sptr != s.end(); ++sptr)
    {
      item = language->translate(wstring(1, *sptr));
      if (!result.empty())
        result += L' ';
      if (item.empty())
        result += *sptr;
      else result += item;
    }
  return result;
}
