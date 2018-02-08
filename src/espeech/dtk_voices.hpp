// dtk_voices.hpp -- DECtalk like embedded voice parameters parser
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

// This class provides some DECtalk voices emulation.
// The parser accepts predefined voice choosing commands and
// pitch range, average pitch and head size as voice parameters
// and adjusts pitch and deviation to achieve corresponding voice
// variations. If both average pitch and head size are specified
// then average pitch takes precedence. Other voice parameters
// are ignored. Voice val for saving and restoring voice settings
// is also provided. Besides that, the parser supports
// speech rate and volume control.

#ifndef MULTISPEECH_DTK_VOICES_HPP
#define MULTISPEECH_DTK_VOICES_HPP

#include <string>

#include <boost/regex.hpp>

#include "inline_parser.hpp"

class dtk_voices: public inline_parser
{
public:
  // Object construction:
  dtk_voices(void);

private:
  // Reference values:
  static constexpr float rate_scale = 200.0, volume_scale = 50.0;

  // Patterns for parameters extraction:
  const boost::regex person_extractor, pitch_range_extractor,
    average_pitch_extractor, head_size_extractor, rate_extractor,
    volume_extractor, mode_extractor, save_cmd_detector;

  // Saved voice parameters:
  float val_pitch, val_deviation;

  // Actual parameters extraction methods:
  void get_person(std::string& data);
  void extract_parameters(std::string& data);
};

#endif
