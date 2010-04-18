// digital_value.cpp -- General digital value validator implementation
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

#include <boost/lexical_cast.hpp>

#include "digital_value.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;


// Value pattern:
const boost::regex digital_value::pattern("^([+-]?\\d+)\\s*$");


// Main constructor:

digital_value::digital_value(double& holder):
  factor(holder)
{
}


// Public methods:

digital_value::status
digital_value::parse(const string& request)
{
  smatch extractor;
  status result = acceptable;
  if (regex_match(request, extractor, pattern))
    {
      int value = lexical_cast<int>(string(extractor[1].first, extractor[1].second));
      if (value < bottom)
        result = too_low;
      else if (value > top)
        result = too_high;
      else
        {
          factor = static_cast<double>(value);
          factor -= (top + bottom) / 2;
          factor /= (top - bottom) / 2;
          factor += 1.0;
          if (factor <= 0.0)
            factor = epsilon;
        }
    }
  else result = invalid;
  return result;
}

} // namespace SSIP
