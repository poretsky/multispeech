// digital_value.hpp -- General digital value validator interface
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

#ifndef MULTISPEECH_SSIP_DIGITAL_VALUE_HPP
#define MULTISPEECH_SSIP_DIGITAL_VALUE_HPP

#include <string>

#include <boost/regex.hpp>

namespace SSIP
{

class digital_value
{
public:
  // Special cases:
  enum status
  {
    acceptable,
    too_low,
    too_high,
    invalid
  };

  // Main constructor takes reference to the variable to be set
  // as an argument.
  explicit digital_value(float& holder);

  // Validate and parse request.
  status parse(const std::string& request);

private:
  // Here we store the factor that will be applied to the configured
  // parameter value. It should never be zero or negative.
  float& factor;

  // Allowed value boundaries.
  static const int bottom = -100;
  static const int top = 100;

  // Factor value used instead of zero.
  static const float epsilon = 0.0001;

  // Pattern for validation.
  static const boost::regex pattern;
};

} // namespace SSIP

#endif
