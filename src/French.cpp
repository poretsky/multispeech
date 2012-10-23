// French.cpp -- French language implementation
/*
   Copyright (C) 2012 Igor B. Poretsky <poretsky@mlbox.ru>
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
#include <boost/algorithm/string.hpp>

#include "French.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Object construction:

French::French(void):
  language_description(lang_id::fr, L"[a-zA-Zäâàçéèêë€ïîùûüµÄÇÉ]")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" point d'exclamation ")
    (L" apostrophe ")
    (L" point d'interrogation ")
    (L" virgule ")
    (L" point ")
    (L" point virgule ")
    (L" deux points ")
    (L" tiret ")
    (L" arobas ")
    (L" dièse ")
    (L" dollar ")
    (L" pourcent ")
    (L" chapeau ")
    (L" et commercial ")
    (L" astérisque ")
    (L" souligné ")
    (L" parenthèse ouvrante ")
    (L" parenthèse fermante ")
    (L" plus ")
    (L" égal ")
    (L" crochet ouvrant ")
    (L" crochet fermant ")
    (L" accolade ouvrante ")
    (L" accolade fermante ")
    (L" anti slash ")
    (L" barre verticale ")
    (L" guillemets ")
    (L" slash ")
    (L" accent grave ")
    (L" tilde ")
    (L" inférieur à ")
    (L" supérieur à ");

  // Letters dictionary:
  dictionary = map_list_of
    (L"ä", L"atréma")
    (L"â", L"aaccencirconflex")
    (L"à", L"aaccentaigü")
    (L"ç", L"cécédille")
    (L"ê", L"euaccencirconflex")
    (L"ë", L"eutréma")
    (L"€", L"euro")
    (L"ï", L"itréma")
    (L"î", L"ihaccencirconflex")
    (L"ù", L"uaccentaigü")
    (L"û", L"uaccencirconflex")
    (L"ü", L"utréma")
    (L"µ", L"micron")
    (L"Ç", L"cécédille")
    (L"£", L"livre")
    (L"§", L"paragraphe")
    (L"°", L"degré")
    (L"pound", L"dièse")
    (L"at", L"arobase")
    (L"exclamation", L"poindexclamation")
    (L"percent", L"pourcent")
    (L"caret", L"chapeau")
    (L"ampersand", L"écommercial")
    (L"star", L"étoile")
    (L"dash", L"tiret")
    (L"underscore", L"souligné")
    (L"equals", L"égal")
    (L"backslash", L"antislash")
    (L"pipe", L"barreverticale")
    (L"period", L"point")
    (L"comma", L"virgule")
    (L"semi", L"poinvirgule")
    (L"colon", L"deupoin")
    (L"quotes", L"guillemet")
    (L"questionmark", L"poindinterrogation")
    (L"backquote", L"accengrave")
    (L"tilde", L"tildé")
    (L"space", L"espace")
    (L"tab", L"tabulation")
    (L"newline", L"retouralaligne")
    (L"dot", L"point")

    // Language name:
    (intern_string(id(), locale("")).c_str(), L"Français");

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" point virgule ")
    (L"\\.{3}", L" point point point ")
    (L"\\.{2}", L" point point ")
    (L"([a-zäâàçéèêë€ïîùûüµ])\\.([a-zäâàçéèêë€ïîùûüµ])", L"$1 point $2", true)
    (L"([0-9])\\.([0-9])", L"$1point$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (L"\\<[bcdfghj-np-tvwxzç]+\\>", spell(this), true)
    (punctuations)
    (L"[^\\.,!\\?';\\:0-9a-zäâàçéèêë€ïîùûüµ]", L" ", true)
    (L"(^[^-0-9a-zäâàçéèêë€ïîùûüµ]+| +$)", L"", true)
    (L"  +", L" ");
}


// Abbreviations spelling:

wstring
French::do_spell(const iterator_range<wstring::const_iterator>& abbrev)
{
  wstring result;
  for (wstring::const_iterator sptr = abbrev.begin(); sptr != abbrev.end(); ++sptr)
    {
      result += *sptr;
      result += L' ';
    }
  return to_upper_copy(result);
}
