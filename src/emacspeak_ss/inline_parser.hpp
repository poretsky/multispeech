// inline_parser.hpp -- Inline voice parameters parser framework
/*
   Copyright (C) 2019 Igor B. Poretsky <poretsky@mlbox.ru>
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

#ifndef MULTISPEECH_INLINE_PARSER_HPP
#define MULTISPEECH_INLINE_PARSER_HPP

#include <string>

#include <boost/regex.hpp>

#include "voice_params.hpp"


class inline_parser: public voice_params
{
protected:
  // Object constructor:
  inline_parser(const std::wstring& detector,
                const std::wstring& precleaner,
                const std::wstring& postcleaner);

public:
  // Destructor is made public to accommodate smart pointers:
  virtual ~inline_parser(void);

  // Test given string for embedded voice parameters and extract them.
  // Return true if parameters were detected and extracted.
  bool parse(std::wstring& data);

protected:
  // Value extraction helper to ease actual parser design
  // in derived classes:
  double get_value(std::wstring& data, const boost::wregex& extractor);

private:
  // Patterns for parsing:
  const boost::wregex params_detector, trash, garbage;

  // Actual parameters extractor must be implemented in derived classes:
  virtual void extract_parameters(std::wstring& data) = 0;
};

#endif
