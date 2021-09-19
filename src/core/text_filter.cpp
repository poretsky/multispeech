// text_filter.cpp -- Text filter implementation
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
#include <sstream>
#include <vector>
#include <map>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>

#include "text_filter.hpp"

using namespace std;
using namespace boost;
using namespace boost::iostreams;
using namespace boost::algorithm;


// case_conversion members:

case_conversion::case_conversion(mode md):
  conversion_mode(md)
{
}

wstring
case_conversion::do_filter(const wstring& s)
{
  switch (conversion_mode)
    {
    case lowercase:
      return to_lower_copy(s, locale(""));
    case uppercase:
      return to_upper_copy(s, locale(""));
    }
  return s;
}


// punctuations members:

const wstring punctuations::char_list(L"!'?,.;:-@#$%^&*_()+=[]{}\\|\"/`~<>");
punctuations::mode punctuations::verbosity = punctuations::some;

punctuations::punctuations(const vector<const wchar_t*>& pronunciations_list):
  pronunciation(pronunciations_list)
{
}

punctuations::mode
punctuations::from_char(char c)
{
  return from_wchar(static_cast<wchar_t>(c));
}

punctuations::mode
punctuations::from_wchar(wchar_t wc)
{
  switch (wc)
    {
    case L'n':
      return punctuations::none;
    case L's':
      return punctuations::some;
    case L'a':
      return punctuations::all;
    default:
      break;
    }
  return punctuations::verbosity;
}

void
punctuations::set_mode(wchar_t wc)
{
  verbosity = from_wchar(wc);
}

wstring
punctuations::do_filter(const wstring& s)
{
  wstring result(s);
  for (unsigned int i = verbosity; i < char_list.length(); i++)
    replace_all(result, char_list.substr(i, 1), pronunciation[i]);
  return result;
}


// char_translations members:

char_translations::char_translations(const map<wchar_t, const wchar_t*>& table):
  translation_table(table)
{
}

wstring
char_translations::do_filter(const wstring& s)
{
  wstring result;
  map<wchar_t, const wchar_t*>::const_iterator item;
  BOOST_FOREACH(wchar_t ch, s)
    {
      item = translation_table.find(ch);
      if (item != translation_table.end())
        result += item->second;
      else result += ch;
    }
  return result;
}


// simple_substitution members:

simple_substitution::simple_substitution(const wchar_t* pattern, const wchar_t* replacement,
                                         bool icase_mode):
  re(pattern, icase_mode ? (regex::normal | regex::icase) : regex::normal),
  fmt(replacement)
{
}

wstring
simple_substitution::do_filter(const wstring& s)
{
  return regex_replace(s, re, fmt);
}


// advanced_substitution members:

advanced_substitution::advanced_substitution(const wchar_t* pattern, const formatter& fmt,
                                             bool icase_mode):
  re(pattern, icase_mode ? (regex::normal | regex::icase) : regex::normal),
  replacer(fmt)
{
}

wstring
advanced_substitution::do_filter(const wstring& s)
{
  return find_format_all_copy(s, regex_finder(re), replacer);
}


// text_filter_constructor members:

text_filter_constructor::text_filter_constructor(filtering_wostream* owner):
  filter(owner)
{
}

text_filter_constructor&
text_filter_constructor::operator()(case_conversion::mode case_mode)
{
  filter->push(case_conversion(case_mode));
  return *this;
}

text_filter_constructor&
text_filter_constructor::operator()(const vector<const wchar_t*>& pronunciations_list)
{
  filter->push(punctuations(pronunciations_list));
  return *this;
}

text_filter_constructor&
text_filter_constructor::operator()(const map<wchar_t, const wchar_t*>& table)
{
  filter->push(char_translations(table));
  return *this;
}

text_filter_constructor&
text_filter_constructor::operator()(const wchar_t* pattern, const wchar_t* replacement,
                                    bool icase_mode)
{
  filter->push(simple_substitution(pattern, replacement, icase_mode));
  return *this;
}

text_filter_constructor&
text_filter_constructor::operator()(const wchar_t* pattern,
                                    const advanced_substitution::formatter& fmt,
                                    bool icase_mode)
{
  filter->push(advanced_substitution(pattern, fmt, icase_mode));
  return *this;
}


// text_filter members:

text_filter::text_filter(void)
{
}

wstring
text_filter::process(const wstring& text)
{
  wostringstream result;
  push(result);
  (*this) << text;
  pop();
  return result.str();
}

text_filter_constructor
text_filter::setup(void)
{
  return text_filter_constructor(this);
}
