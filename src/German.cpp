// German.cpp -- German language implementation
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

#include "German.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Object construction:

German::German(void):
  language_description(lang_id::de, L"[Ööß]")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" ausrufezeichen ")
    (L" apostroph ")
    (L" fragezeichen ")
    (L" komma ")
    (L" punkt ")
    (L" strichpunkt ")
    (L" doppelpunkt ")
    (L" strich ")
    (L" kommerzielles at ")
    (L" pfund ")
    (L" dollar ")
    (L" prozent ")
    (L" karet ")
    (L" kaufmännisches ")
    (L" asterisk ")
    (L" unterstrich ")
    (L" runde klammer auf ")
    (L" runde Klammer zu ")
    (L" pluszeichen ")
    (L" gleichheitszeichen ")
    (L" eckige klammer auf ")
    (L" eckige klammer zu ")
    (L" geschweifte klammer auf ")
    (L" geschweifte klammer zu ")
    (L" rückschrägstrich ")
    (L" senkrechter strich ")
    (L" anführungszeichen ")
    (L" schrägstrich ")
    (L" grave-akzent ")
    (L" tilde ")
    (L" kleiner als ")
    (L" großer als ");

  // Letters dictionary:
  dictionary = map_list_of
    (L"pound", L"Pfund")
    (L"at", L"KommerziellesAt")
    (L"exclamation", L"Ausrufezeichen")
    (L"slash", L"Slesch")
    (L"percent", L"Prozent")
    (L"caret", L"Karet")
    (L"ampersand", L"KaufmaennischUnd")
    (L"star", L"Stern")
    (L"dash", L"Strich")
    (L"underscore", L"Unterstrich")
    (L"equals", L"Gleich")
    (L"backslash", L"Beckslesch")
    (L"pipe", L"Paip")
    (L"period", L"Punkt")
    (L"comma", L"Komma")
    (L"semi", L"Strichpunkt")
    (L"colon", L"Doppelpunkt")
    (L"apostrophe", L"Apostroph")
    (L"quotes", L"Anfuehrungszeichen")
    (L"questionmark", L"Fragezeichen")
    (L"backquote", L"Beckquout")
    (L"space", L"Leerschlag")
    (L"tab", L"Tabulator")
    (L"newline", L"NeueZeile")
    (L"dot", L"Punkt")

    // Language name:
    (intern_string(id(), locale("")).c_str(), L"Deutsch");

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" punkt komma ")
    (L"\\.{3}", L" punkt punkt punkt ")
    (L"\\.{2}", L" punkt punkt ")
    (L"([a-zäöüß])\\.([a-zäöüß])", L"$1 punkt $2", true)
    (L"([0-9])\\.([0-9])", L"$1punkt$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (L"\\<[bcdfghj-np-tvwxzß]+\\>", spell(this), true)
    (punctuations)
    (L"[^\\.,!\\?';\\:0-9a-zäöüß]", L" ", true)
    (L"(^[^-0-9a-zäöüß]+| +$)", L"", true)
    (L"  +", L" ");
}
