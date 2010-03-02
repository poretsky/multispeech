// multispeech_voices.cpp -- Native embedded voice parameters parser implementation
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

#include "multispeech_voices.hpp"

using namespace std;
using namespace boost;


// Object construction:

multispeech_voices::multispeech_voices(void):
  inline_parser(L"^\\[_:.*]", L"\\s*\\[(:np\\s*|\\*)]\\s*", L"\\[[^]]*]"),
  freq_extractor(L"^\\[_:(.*\\s)?fr:(\\d+).*]"),
  pitch_extractor(L"^\\[_:(.*\\s)?pi:(\\d+(\\.\\d*)?).*]"),
  rate_extractor(L"^\\[_:(.*\\s)?ra:(\\d+(\\.\\d*)?).*]"),
  volume_extractor(L"^\\[_:(.*\\s)?vo:(\\d+(\\.\\d*)?).*]")
{
}


// Actual parameters extractor:

void
multispeech_voices::extract_parameters(wstring& data)
{
  volume = get_value(data, volume_extractor);
  rate = get_value(data, rate_extractor) / rate_scale;
  pitch = get_value(data, pitch_extractor);
  deviation = get_value(data, freq_extractor) / ref_freq;
}