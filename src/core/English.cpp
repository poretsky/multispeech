// English.cpp -- English language implementation
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

#include <boost/assign.hpp>

#include "English.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:
language_description::options English::settings =
  {
    .engine = espeak::name,
    .priority = 0,
    .volume = 1.0,
    .pitch = 1.0,
    .rate = 1.0,
    .acceleration = 0.0,
    .char_pitch = 1.0,
    .char_rate = 1.0,
    .caps_factor = 1.2
  };


// Object construction:

English::English(void):
  language_description(lang_id::en, settings, L"th|wh[aeioy]|([abefhlmnrstw]i|[ao]u)ghts? ")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" exclamation ")
    (L" apostrophe ")
    (L" question mark ")
    (L" comma ")
    (L" period ")
    (L" semicolon ")
    (L" colon ")
    (L" dash ")
    (L" at ")
    (L" pound ")
    (L" dollar ")
    (L" percent ")
    (L" caret ")
    (L" ampersand ")
    (L" star ")
    (L" underscore ")
    (L" left paren ")
    (L" right paren ")
    (L" plus ")
    (L" equals ")
    (L" left bracket ")
    (L" right bracket ")
    (L" left brace ")
    (L" right brace ")
    (L" backslash ")
    (L" pipe ")
    (L" quotes ")
    (L" slash ")
    (L" backquote ")
    (L" tilde ")
    (L" less than ")
    (L" greater than ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"newline", L"new line")

    // Language name:
    (intern_string(id).c_str(), L"English")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" dot comma ")
    (L"\\.{3}", L" dot dot dot ")
    (L"\\.{2}", L" dot dot ")
    (L"([a-z])\\.([a-z])", L"$1 dot $2", true)
    (L"([0-9])\\.([0-9])", L"$1period$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxz]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-z]", L" ", true)
    (L"(^[^-0-9a-z]+| +$)", L"", true)
    (L"  +", L" ");
}
