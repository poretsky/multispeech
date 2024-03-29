// language_description.hpp -- General framework for language description
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

// The language_description class is designed as common ancestor
// for the particular language description classes. It is only a framework.
// It comprises some basic methods and data structures that must be
// actually set up by descendants in a particular language specific way.

#ifndef MULTISPEECH_LANGUAGE_DESCRIPTION_HPP
#define MULTISPEECH_LANGUAGE_DESCRIPTION_HPP

#include <string>
#include <vector>
#include <map>

#include <boost/range.hpp>
#include <boost/regex.hpp>

#include "text_filter.hpp"

class language_description
{
protected:
  // Language specific options container:
  typedef struct
  {
    std::string engine;
    int priority;
    double volume;double pitch;
    double rate;
    double acceleration;
    double char_pitch;
    double char_rate;
    double caps_factor;
    bool speak_numbers;
  } options;

  // Object constructor:
  language_description(const char* language_id,
                       const options& language_settings,
                       const wchar_t* alphabet,
                       const wchar_t* language_detector);

public:
  // Destructor is made public to accommodate smart pointers:
  virtual ~language_description(void);

  // Language id string:
  const char* const id;

  // Configured speech parameters:
  const options& settings;

  // Native language presence recognition:
  bool recognize(const std::wstring& s);

  // Foreign language presence recognition:
  bool foreign(const std::wstring& s);

  // text filtering:
  std::wstring filter(const std::wstring& s);

  // Translate by dictionary:
  std::wstring translate(const std::wstring& s) const;

protected:
  // Spelling functor to expand abbreviations:
  class spell
  {
  public:
    explicit spell(language_description* owner);

    std::wstring operator()(const boost::iterator_range<std::wstring::const_iterator>& s);

  private:
    language_description* language;
  };

  // These members are to be initialized in derived classes.
  std::vector<const wchar_t*> punctuations;
  std::map<const std::wstring, const wchar_t*> dictionary;
  text_filter filter_chain;

private:
  // Language belonging criteria:
  const wchar_t* const patterns;
  const boost::wregex test;
  boost::wregex detector;
};

#endif
