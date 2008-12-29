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

#ifndef LANGUAGE_DESCRIPTION_HPP
#define LANGUAGE_DESCRIPTION_HPP

#include <string>
#include <vector>
#include <map>

#include <boost/range.hpp>
#include <boost/regex.hpp>

#include "text_filter.hpp"

class language_description
{
public:
  // Construct / destroy:
  language_description(const std::string& language,
                       const std::wstring& language_detector);
  virtual ~language_description(void);

  // Language id string:
  const std::string id;

  // Native language presence recognition:
  bool recognize(const std::wstring& s);

  // text filtering:
  std::wstring filter(const std::wstring& s);

  // Translate by dictionary:
  std::wstring translate(const std::wstring& s) const;

protected:
  // Spelling functor to expand abbreviations:
  class spell
  {
  public:
    spell(language_description* owner);

    std::wstring operator()(const boost::iterator_range<std::wstring::const_iterator>& s);

  private:
    language_description* language;
  };

  friend class spell;

  // These members are to be initialized in derived classes.
  std::vector<const wchar_t*> punctuations;
  std::map<const std::wstring, const std::wstring> dictionary;
  text_filter filter_chain;

private:
  // Native language detector:
  const boost::wregex detector;

  // Abbreviation spelling method:
  virtual std::wstring do_spell(const boost::iterator_range<std::wstring::const_iterator>& s) = 0;
};

#endif
