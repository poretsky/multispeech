// dtk_voices.cpp -- DECtalk like embedded voice parameters parser implementation
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

#include "dtk_voices.hpp"

using namespace std;
using namespace boost;


// Object construction:

dtk_voices::dtk_voices(void):
  inline_parser(L"^\\[\\s*:.*]", L"\\[\\s*:np\\s*]", L"\\[[^]]*]"),
  person_extractor(L"^\\[\\s*:n([phfdbuwrkv]).*]"),
  pitch_range_extractor(L"^\\[.*:dv\\s(.*\\s)?pr\\s+(\\d+(\\.\\d*)?).*]"),
  average_pitch_extractor(L"^\\[.*:dv\\s(.*\\s)?ap\\s+(\\d+(\\.\\d*)?).*]"),
  head_size_extractor(L"^\\[.*:dv\\s(.*\\s)?hs\\s+(\\d+(\\.\\d*)?).*]"),
  rate_extractor(L"^\\[.*:ra(te)?\\s+(\\d+(\\.\\d*)?).*]"),
  volume_extractor(L"^\\[.*:volu(me)?\\s+set\\s+(\\d+(\\.\\d*)?).*]"),
  save_cmd_detector(L"^\\[.*:dv\\s(.*\\s)?save(\\s.*)?]"),
  val_pitch(1.0),
  val_deviation(1.0)
{
}


// Actual parameters extraction methods:

void
dtk_voices::get_person(wstring& data)
{
  wsmatch parse_result;
  if (regex_search(data, parse_result, person_extractor) &&
      parse_result[1].matched)
    switch (parse_result[1].first[0])
      {
      case L'p': // Paul
        pitch = 1.0;
        deviation = 1.0;
        break;
      case L'h': // Harry
        pitch = 0.5;
        deviation = 1.0;
        break;
      case L'd': // Dennis
        pitch = 0.7;
        deviation = 0.875;
        break;
      case L'f': // Frank
        pitch = 0.7;
        deviation = 0.75;
        break;
      case L'b': // Betty
        pitch = 1.4;
        deviation = 1.0625;
        break;
      case L'u': // Ursula
        pitch = 1.3;
        deviation = 1.0;
        break;
      case L'r': // Rita
        pitch = 1.4;
        deviation = 1.125;
        break;
      case L'w': // Wendy
        pitch = 1.5;
        deviation = 1.0625;
        break;
      case L'k': // Kit
        pitch = 2.0;
        deviation = 1.25;
        break;
      case L'v': // Val
        pitch = val_pitch;
        deviation = val_deviation;
      default:
        break;
      }
}

void
dtk_voices::extract_parameters(wstring& data)
{
  double pitch_range = get_value(data, pitch_range_extractor),
    average_pitch = get_value(data, average_pitch_extractor),
    head_size = get_value(data, head_size_extractor);
  pitch = 1.0;
  deviation = 1.0;
  volume = -1.0;
  get_person(data);
  if (pitch_range >= 0.0)
    deviation = (400.0 + pitch_range) / 500.0;
  if (average_pitch >= 0.0)
    pitch = average_pitch / 150.0;
  else if (head_size >= 0.0)
    pitch = 5.0 - (head_size * 3.0 / 80.0);
  rate = get_value(data, rate_extractor) / rate_scale;
  volume = get_value(data, volume_extractor) / volume_scale;
  if (regex_search(data, save_cmd_detector, match_default | match_any))
    {
      val_pitch = pitch;
      val_deviation = deviation;
    }
}
