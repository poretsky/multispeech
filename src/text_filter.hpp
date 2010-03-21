// text_filter.hpp -- Text filtering tools interface
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

// Text filter chain construction is as simple as follows:
//
// text_filter filter;
// filter.setup()
//   (expression_1)
//   ...
//   (expression_n);

// Here the following expressions can be used:
//
// (const wchar_t*, const wchar_t*[, bool]) specifies global regular
// expression replacement. The optional third argument indicates that
// case insensitive mode is needed.
// See simple_substitution class definition for details.
//
// (const wchar_t*, const advanced_substitution::formatter&[, bool])
// specifies global regular expression substitution using formatter
// function. The optional third argument indicates that
// case insensitive mode is needed.
// See advanced_substitution class definition for details.
//
// (case_conversion::mode) specifies case conversion.
// See case_conversion class definition for details.
//
// (const std::vector<const wchar_t*>&) specifies punctuations
// verbosing filter.
// See punctuations class definitions for details.
//
// (const std::map<wchar_t, const wchar_t*>&) specifies general
// character translation filter.
// See char_translation class definition for details.

#ifndef MULTISPEECH_TEXT_FILTER_HPP
#define MULTISPEECH_TEXT_FILTER_HPP

#include <string>
#include <vector>
#include <map>

#include <boost/range.hpp>
#include <boost/regex.hpp>
#include <boost/function.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/line.hpp>


// Filter primitives for chain construction:

// Case conversion filter:
class case_conversion: public boost::iostreams::wline_filter
{
public:
  enum mode
  {
    lowercase,
    uppercase
  };

  case_conversion(mode md);

private:
  std::wstring do_filter(const std::wstring& s);

  mode conversion_mode;
};

// Verbose punctuations:
class punctuations: public boost::iostreams::wline_filter
{
public:
  // Available punctuation verbosity modes:
  enum mode
  {
    all = 0,
    some = 8,
    none = 1000 // Beyond the list
  };

  explicit punctuations(const std::vector<const wchar_t*>& pronunciations_list);

  static mode verbosity;

private:
  std::wstring do_filter(const std::wstring& s);

  const std::vector<const wchar_t*>& pronunciation;
  static const std::wstring char_list;
};

// Character translation filter:
class char_translations: public boost::iostreams::wline_filter
{
public:
  explicit char_translations(const std::map<wchar_t, const wchar_t*>& table);

private:
  std::wstring do_filter(const std::wstring& s);

  const std::map<wchar_t, const wchar_t*>& translation_table;
};

// Define our own class for regular expression filter here
// because the one provided by the Boost library is buggy.
// Moreover, it seems more natural to derive it from the
// line filter rather than from the aggregate one.
// Anyway, it is quite simple.
class simple_substitution: public boost::iostreams::wline_filter
{
public:
  simple_substitution(const wchar_t* pattern, const wchar_t* replacement,
                      bool icase_mode = false);

private:
  std::wstring do_filter(const std::wstring& s);

  const boost::wregex re;
  const wchar_t* fmt;
};

// More flexible variant of the regular expression filter.
// It provides possibility to specify replacement as a formatter.
class advanced_substitution: public boost::iostreams::wline_filter
{
public:
  typedef boost::function<std::wstring(const boost::iterator_range<std::wstring::const_iterator>&)> formatter;

  advanced_substitution(const wchar_t* pattern, const formatter& fmt,
                        bool icase_mode = false);

private:
  std::wstring do_filter(const std::wstring& s);

  const boost::wregex re;
  const formatter replacer;
};


// Filter chain construction tools:

// This class is not for direct use. It's only purpose is to provide
// simple and natural syntax for the filter chain construction.
class text_filter_constructor
{
public:
  explicit text_filter_constructor(boost::iostreams::filtering_wostream* owner);

  text_filter_constructor& operator()(case_conversion::mode case_mode);
  text_filter_constructor& operator()(const std::vector<const wchar_t*>& pronunciations_list);
  text_filter_constructor& operator()(const std::map<wchar_t, const wchar_t*>& table);
  text_filter_constructor& operator()(const wchar_t* pattern, const wchar_t* replacement,
                                      bool icase_mode = false);
  text_filter_constructor& operator()(const wchar_t* pattern,
                                      const advanced_substitution::formatter& fmt,
                                      bool icase_mode = false);

private:
  boost::iostreams::filtering_wostream* filter;
};

// Text filter chain.
class text_filter: public boost::iostreams::filtering_wostream
{
public:
  // Object constructor:
  text_filter(void);

  // Filter chain assembling:
  text_filter_constructor setup(void);
};

#endif
