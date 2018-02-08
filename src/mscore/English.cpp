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

#include <sysconfig.h>

#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

#include "English.hpp"

#include "config.hpp"
#include "strcvt.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Instantiation mechanism:
singleton<English> English::instance;


// Object construction:

English::English(void):
  language_description(lang_id::en, L"[a-zA-Z]")
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

    // Special key names:
    (L"double-quote", L"quote")
    (L"kp-*", L"keypad star")
    (L"kp-+", L"keypad plus")
    (L"kp--", L"keypad minus")
    (L"kp-.", L"keypad period")
    (L"kp-/", L"keypad slash")
    (L"kp-0", L"keypad 0")
    (L"kp-1", L"keypad 1")
    (L"kp-2", L"keypad 2")
    (L"kp-3", L"keypad 3")
    (L"kp-4", L"keypad 4")
    (L"kp-5", L"keypad 5")
    (L"kp-6", L"keypad 6")
    (L"kp-7", L"keypad 7")
    (L"kp-8", L"keypad 8")
    (L"kp-9", L"keypad 9")
    (L"kp-enter", L"keypad enter")
    (L"num-lock", L"numlock")
    (L"scroll-lock", L"scroll lock")

    // Language name:
    (intern_string(id(), locale("")).c_str(), L"English")
    .convert_to_container< map<const wstring, const wstring> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" dot comma ")
    (L"\\.{3}", L" dot dot dot ")
    (L"\\.{2}", L" dot dot ")
    (L"([a-z])\\.([a-z])", L"$1 dot $2", true)
    (L"([0-9])\\.([0-9])", L"$1period$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (L"\\<[bcdfghj-np-tvwxz]+\\>", spell(this), true)
    (punctuations)
    (L"[^\\.,!\\?';\\:0-9a-z]", L" ", true)
    (L"(^[^-0-9a-z]+| +$)", L"", true)
    (L"  +", L" ");
}

// Abbreviations spelling:

wstring
English::do_spell(const iterator_range<wstring::const_iterator>& abbrev)
{
  wstring result;
  for (wstring::const_iterator sptr = abbrev.begin(); sptr != abbrev.end(); ++sptr)
    {
      result += *sptr;
      result += L' ';
    }
  return to_upper_copy(result);
}

} // namespace multispeech
