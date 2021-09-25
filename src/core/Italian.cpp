// Italian.cpp -- Italian language implementation
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

#include "Italian.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:

language_description::options Italian::settings =
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
  L"[^o]ù|fatto|spetta|"
  L"(?:abb|lasc|facc|migl)ia|"
  L"(?:\\A|\\s|[\"‘])(?:"
  L"qu(?:indi|atto)|"
  L"figli[oae]|"
  L"ragazz[oai]|"
  L"piaceva|"
  L"(?:"
  L"que(?:st[oie]|ll[oai]|gli)|"
  L"(?:[nv]|[ptsb]u)oi|"
  L"st?iamo|"
  L"(?:d?[ae]|ne)gli|"
  L"buon[aeio]?|"
  L"(?:[csn]')?è"
  L")\\b|"
  L"(?:"
  L"(?:(?:(?:[sn]u|(?:[dnb]|qu)e|d?a|co)l|[nd]eg|qua)?l|(?:u|buo)n|glie[ln])'[aieou]|"
  L"[lctm]'h|"
  L"(?:[csd]'e|o(?:lt|sse))r|"
  L"r(?:a(?:vv|ggr)|i(?:n(?:v|fr|ca)|un|pro|mpi|gua|s(?:ta|c[hi])|vend|c(?:re|o[nmsp])|f(?:u|o?r)|a(?:[pfv]|ss|mm|tt)))|"
  L"a(?:zz|gg(?:h|iu)|(?:cc|bb)on|vv(?:[ea]|ol|i[ncsl]))|"
  L"s(?:g[ou]|b(?:u|a[rl])|o(?:tto|vv))"
  L")\\B"
  L")|"
  L"(?:"
  L"v(?:ogli[oae]|uo(?:i|le)|ediamo)|"
  L"s(?:udiamo|ezione)|"
  L"a(?:veva|ndiamo)|"
  L"[rf](?:idiamo|azione)|"
  L"(?:lo|ba|c[ea]|in)diamo|"
  L"nazione|"
  L"piace|"
  L"\\B(?:"
  L"(?:r|it)à|"
  L"(?:bber|er(?:emm|ann))o|"
  L"eresti|"
  L"i(?:a(?:v(?:[io]|a(?:[nm]o|te))|s(?:s(?:i(?:mo)?|ero)|ti)|(?:mm|ron)o)|(?:sser|mm)o|r(?:e(?:st[ie]|bbe|(?:mm|on)o)|anno)|scono)|"
  L"t(?:ere(?:st|bb)e|a(?:ss(?:i(?:mo)?|ero)|(?:mm|(?:va|ro)n)o))|"
  L"[ncz](?:ere(?:s?t|bb)e|a(?:ssi(?:mo)?|(?:sser|mm|(?:va|ro)n)o))|"
  L"l(?:a(?:ss(?:i(?:mo)?|ero)|(?:mm|(?:va|ro)n)o)|ere(?:bb|st)e)|"
  L"zz(?:a(?:t[oe]|(?:r|s[st])e|(?:st)?i|v(?:[oi]|a(?:te|mo)?))|i(?:n|am)o|er(?:[ae]i|emo))|"
  L"[rdmpb](?:a(?:ss(?:er|im)|ron)o|ere(?:bb|st)e)|"
  L"g(?:assero|ere(?:bb|st)e)|"
  L"here(?:bb|st)e|"
  L"(?:essi|chia)mo"
  L")"
  L")\\b|"
  L"\\B(?:"
  L"(?:[tmc]|[aioe][lrn])izz|"
  L"i(?:zza[vs]|ficher)|"
  L"g(?:gia[sv]|iun)|"
  L"razion|"
  L"h(?:egg|iud)|"
  L"seg(?:g|na)|"
  L"[aeuo]tt[ai]v|"
  L"p(?:ezz|i(?:cc|gl)i)|"
  L"[nd]eggi|"
  L"(?:ua|vi)gl"
  L")\\B";


// Object construction:

Italian::Italian(void):
  language_description(lang_id::it, settings, L"a-zàòùìèé", criterion)
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" punto esclamativo ")
    (L" apostrofo ")
    (L" punto interrogativo ")
    (L" virgola ")
    (L" punto ")
    (L" punto e virgola ")
    (L" due punti ")
    (L" lineetta ")
    (L" commerciale at ")
    (L" cancelletto ")
    (L" dollaro ")
    (L" percentuale ")
    (L" circonflesso ")
    (L" ampersand ")
    (L" asterisco ")
    (L" trattino basso ")
    (L" parentesi tonde aperta ")
    (L" parentesi tonde chiusa ")
    (L" più ")
    (L" uguale ")
    (L" parentesi quadre aperta ")
    (L" parentesi quadre chiusa ")
    (L" parentesi graffe aperta ")
    (L" parentesi graffe chiusa ")
    (L" barra inversa ")
    (L" barra verticale ")
    (L" virgolette ")
    (L" barra ")
    (L" accento ")
    (L" tilde ")
    (L" minore ")
    (L" maggiore ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"à", L"a accento grave")
    (L"è", L"e accento grave")
    (L"é", L"e accento acuto")
    (L"ì", L"i accento grave")
    (L"í", L"i accento acuto")
    (L"î", L"i circonflesso")
    (L"ò", L"o accento grave")
    (L"ó", L"o accento acuto")
    (L"ù", L"u accento grave")
    (L"ú", L"u accento acuto")
    (L"À", L"a accento grave")
    (L"É", L"e accento acuto")
    (L"È", L"e accento grave")
    (L"Í", L"i accento acuto")
    (L"Ì", L"i accento grave")
    (L"Î", L"i circonflesso")
    (L"Ò", L"o accento grave")
    (L"Ó", L"o accento acuto")
    (L"Ú", L"u accento acuto")
    (L"Ù", L"u accento grave")
    (L"dollar", L"Dollaro")
    (L"pound", L"Cancelletto")
    (L"at", L"CommercialeAt")
    (L"exclamation", L"Esclamativo")
    (L"slash", L"Barra")
    (L"percent", L"Percentuale")
    (L"caret", L"Circonflesso")
    (L"star", L"asterisco")
    (L"dash", L"Lineetta")
    (L"underscore", L"TrattinoBasso")
    (L"plus", L"Più")
    (L"equals", L"uguale")
    (L"backslash", L"BarraInversa")
    (L"pipe", L"BarraVerticale ")
    (L"period", L"Punto")
    (L"comma", L"Virgola")
    (L"semi", L"PuntoEVirgola")
    (L"colon", L"DuePunti")
    (L"apostrophe", L"Apostrofo")
    (L"quotes", L"Virgolette")
    (L"question mark", L"PuntoInterrogativo")
    (L"backquote", L"accento")
    (L"left paren", L"parentesi tonde aperta")
    (L"right paren", L"parentesi tonde chiusa")
    (L"left bracket", L"parentesi quadre aperta")
    (L"right bracket", L"parentesi quadre chiusa")
    (L"left brace", L"parentesi graffe aperta")
    (L"right brace", L"parentesi graffe chiusa")
    (L"less than", L"minore")
    (L"greater than", L"maggiore")
    (L"space", L"spazio")
    (L"tab", L"Tabulatore")
    (L"newline", L"NuovaLinea")
    (L"dot", L"Punto")

    // Language name:
    (intern_string(id).c_str(), L"Italiano")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" punto virgola ")
    (L"\\.{3}", L" punto punto punto ")
    (L"\\.{2}", L" punto punto ")
    (L"([a-zàèéìíîòóùú])\\.([a-zàèéìíîòóùú])", L"$1 punto $2", true)
    (L"([0-9])\\.([0-9])", L"$1punto$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxz]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-zàèéìíîòóùú]", L" ", true)
    (L"(^[^-0-9a-zàèéìíîòóùú]+| +$)", L"", true)
    (L"  +", L" ");
}
