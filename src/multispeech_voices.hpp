// multispeech_voices.hpp -- Native embedded voice parameters parser
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

// This class provides parser for native voice control commands
// that are traditionally used by Emacspeak.

#ifndef MULTISPEECH_VOICES_HPP
#define MULTISPEECH_VOICES_HPP

#include <string>

#include <boost/regex.hpp>

#include "inline_parser.hpp"

class multispeech_voices: public inline_parser
{
public:
  // Object construction:
  multispeech_voices(void);

private:
  // Reference values:
  static const double ref_freq = 16000.0, rate_scale = 200.0;

  // Patterns for parameters extraction:
  const boost::wregex freq_extractor, pitch_extractor, rate_extractor, volume_extractor;

  // Actual parameters extraction method:
  void extract_parameters(std::wstring& data);
};

#endif
