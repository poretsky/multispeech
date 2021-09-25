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

#include "German.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:

language_description::options German::settings =
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

static const wchar_t* criterion =
  L"übers|"
  L"z(?:ie(?:h|rt)|(?:eu|un)ge)|"
  L"f(?:ehl|(?:arb|ind)en)|"
  L"g(?:e(?:ben|lte)|reife)|"
  L"l(?:aufen|e(?:bend|[gb]t|ih))|"
  L"(?:acht|reit)en|"
  L"t(?:eile|reff)|"
  L"w(?:a(?:hr|chs|sch)|i(?:ckel|llig|ssen)|e(?:rte[ns]|sen))|"
  L"b(?:ringen|aute)|"
  L"h(?:a(?:ndels|uen)|iebe)|"
  L"s(?:cheren|pruch)|"
  L"mische|"
  L"kratz|"
  L"(?:\\A|\\s|[\"‘])(?:"
  L"we(?:ite|gs)|"
  L"z(?:uge|wang)|"
  L"her(?:um|ab|bei|auf)|"
  L"d(?:urchs|azu)|"
  L"rechts|"
  L"berufs|"
  L"sch(?:alt|laf|uld)|"
  L"leicht|"
  L"(?:"
  L"a(?:b(?:z|ge|sch)|u(?:f[szfr]|s[br]))|"
  L"z(?:u(?:sa|rü)|ers)|"
  L"ein(?:[gzf]|sc)|"
  L"ver(?:f|sc)|"
  L"hin(?:ei|au)|"
  L"gesc|"
  L"besch"
  L")\\B"
  L")|"
  L"(?:"
  L"(?:rinn|nisch|fahr|halt|ah[ml]|zung|gab|teil|eig)en|"
  L"tisch(?:en)?|"
  L"\\B(?:"
  L"barer|"
  L"ier(?:te(?:r|s?t)|ende?)|"
  L"(?:rung|gend|gt)en|"
  L"chst(?:e[rm])?|"
  L"hste[ns]?|"
  L"igste[nm]|"
  L"gstes?"
  L")"
  L")\\b|"
  L"\\B(?:"
  L"u(?:samme|rüc)|"
  L"(?:h|(?:t|is)ie)rt|"
  L"lichst|"
  L"ufge|"
  L"eraus|"
  L"rungs"
  L")\\B";


// Object construction:

German::German(void):
  language_description(lang_id::de, settings, L"a-zäöüß", criterion)
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
    (L" großer als ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"ä", L"a-umlaut")
    (L"ö", L"o-umlaut")
    (L"ü", L"u-umlaut")
    (L"Ä", L"a-umlaut")
    (L"Ö", L"o-umlaut")
    (L"Ü", L"u-umlaut")
    (L"ß", L"scharf-s")
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
    (L"question mark", L"Fragezeichen")
    (L"backquote", L"Beckquout")
    (L"left paren", L"runde klammer auf")
    (L"right paren", L"runde Klammer zu")
    (L"left bracket", L"eckige klammer auf")
    (L"right bracket", L"eckige klammer zu")
    (L"left brace", L"geschweifte klammer auf")
    (L"right brace", L"geschweifte klammer zu")
    (L"less than", L"kleiner als")
    (L"greater than", L"großer als")
    (L"space", L"Leerschlag")
    (L"tab", L"Tabulator")
    (L"newline", L"NeueZeile")
    (L"dot", L"Punkt")

    // Language name:
    (intern_string(id).c_str(), L"Deutsch")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" punkt komma ")
    (L"\\.{3}", L" punkt punkt punkt ")
    (L"\\.{2}", L" punkt punkt ")
    (L"([a-zäöüß])\\.([a-zäöüß])", L"$1 punkt $2", true)
    (L"([0-9])\\.([0-9])", L"$1punkt$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxzß]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-zäöüß]", L" ", true)
    (L"(^[^-0-9a-zäöüß]+| +$)", L"", true)
    (L"  +", L" ");
}
