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

#include "French.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:

language_description::options French::settings =
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
  L"(?:[pm]â|vê)t|"
  L"t(?:o(?:mburn)|aill)er|"
  L"(?:cass|unit)é|"
  L"(?:\\A|\\s|[\"‘])(?:"
  L"p(?:rêt|énal|ou(?:sser|pon)|luriel|ieds)|"
  L"mé(?:tal|gi)|"
  L"(?:(?:lai|pou)ss|(?:devi|four|gag)n|(?:rê|(?:tro|sa)u)v)er|"
  L"nu(?:age|it)|"
  L"v(?:o(?:ir|mis)|errou)|"
  L"déjà|"
  L"bébé|"
  L"coeur|"
  L"(?:"
  L"a(?:vec|lors)|"
  L"v(?:ue|o(?:tre|ulez))|"
  L"t(?:ous|uer)|"
  L"ça"
  L")\\b|"
  L"(?:"
  L"dé(?:n|se|li|ro|f[ra]|v[aie]|p[olr]|c(?:[re]|ha|o[num]))|"
  L"ré(?:[feh]|v?[oé]|i[nm]|a[clpsdrfmb]|t[ra]|s(?:o|ul)|g[la]|p[ouae]|c(?:[urea]|o(?:n|lt)))"
  L")\\B"
  L")|"
  L"\\B(?:"
  L"(?:e|nd)raie|"
  L"é(?:r(?:aie|er)|con|sent|mé|nage)|"
  L"r(?:odui|ése)|"
  L"a(?:ît|raî)|"
  L"è[nt]|"
  L"[îê]tr|"
  L"xpé|"
  L"tionne|"
  L"ectionn|"
  L"phiq"
  L")\\B|"
  L"(?:"
  L"ser(?:ions|ez|(?:on|ai)t)|"
  L"ir(?:iez|(?:ai|on)t)|"
  L"v(?:e(?:ux|nions)|oies)|"
  L"passe(?:z|ra)|"
  L"ouais|"
  L"aient|"
  L"âmes|"
  L"niez|"
  L"\\B(?:"
  L"(?:[f-is-vmy]|er)iez|"
  L"è(?:re(?:me)?nt|(?:[nsd]|tr?|qu)es?)|"
  L"âtr?es|"
  L"[f-it-zlrdmpbcokçé]assent|"
  L"(?:r|[idv]e?|[tslnugm]e)r(?:ai|on)t|"
  L"(?:re|tt)r(?:ions|(?:ai|on)[ts])|"
  L"(?:î|[tslnr]â)t|"
  L"i(?:s?ait|onner|s(?:ez|ées)|ées?)|"
  L"é(?:té|(?:r|gl?)?er)|"
  L"êt(?:ez|ir|res?)|"
  L"[altorvng]issera?"
  L")"
  L")\\b";


// Object construction:

French::French(void):
  language_description(lang_id::fr, settings, L"a-zàâæçéèêëîïôœùûüÿ", criterion)
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
    (L" supérieur à ")
    .convert_to_container< vector<const wchar_t*> >();

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
    (L"question mark", L"poindinterrogation")
    (L"backquote", L"accengrave")
    (L"tilde", L"tildé")
    (L"left paren", L"parenthèse ouvrante")
    (L"right paren", L"parenthèse fermante")
    (L"left bracket", L"crochet ouvrant")
    (L"right bracket", L"crochet fermant")
    (L"left brace", L"accolade ouvrante")
    (L"right brace", L"accolade fermante")
    (L"less than", L"inférieur à")
    (L"greater than", L"supérieur à")
    (L"space", L"espace")
    (L"tab", L"tabulation")
    (L"newline", L"retouralaligne")
    (L"dot", L"point")

    // Language name:
    (intern_string(id).c_str(), L"Français")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" point virgule ")
    (L"\\.{3}", L" point point point ")
    (L"\\.{2}", L" point point ")
    (L"([a-zäâàçéèêë€ïîùûüµ])\\.([a-zäâàçéèêë€ïîùûüµ])", L"$1 point $2", true)
    (L"([0-9])\\.([0-9])", L"$1point$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxzç]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-zäâàçéèêë€ïîùûüµ]", L" ", true)
    (L"(^[^-0-9a-zäâàçéèêë€ïîùûüµ]+| +$)", L"", true)
    (L"  +", L" ");
}
