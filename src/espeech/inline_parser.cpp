// inline_parser.cpp -- Inline voice parameters parser common implementation
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include <string>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "inline_parser.hpp"

using namespace std;
using namespace boost;


// Object construction / destruction:

inline_parser::inline_parser(const string& detector,
                             const string& precleaner,
                             const string& postcleaner):
  params_detector(detector),
  trash(precleaner),
  garbage(postcleaner)
{
}

inline_parser::~inline_parser(void)
{
}


// Public methods:

bool
inline_parser::parse(string& data)
{
  bool result = false;
  if (regex_search(data, params_detector, match_default | match_any))
    {
      data = regex_replace(data, trash, "");
      extract_parameters(data);
      data = regex_replace(data, garbage, "");
      result = true;
    }
  return result;
}


// Protected methods:

float
inline_parser::get_value(std::string& data, const boost::regex& extractor)
{
  smatch parse_result;
  float result_value = -1.0;
  if (regex_search(data, parse_result, extractor) &&
      parse_result[2].matched)
    result_value = lexical_cast<float>(string(parse_result[2].first, parse_result[2].second));
  return result_value;
}
