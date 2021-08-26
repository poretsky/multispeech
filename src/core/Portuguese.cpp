// Portuguese.cpp -- Portuguese language implementation
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

#include "Portuguese.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Static data:
language_description::options Portuguese::settings =
  {
    .engine = "",
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

Portuguese::Portuguese(void):
  language_description(lang_id::pt, settings, L"[õã]|(^| )(não|você|(meu|el[ae])s?|isso) ")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" exclamação ")
    (L" apóstrofo ")
    (L" interrogação ")
    (L" vírgula ")
    (L" ponto ")
    (L" ponto e vírgula ")
    (L" dois pontos ")
    (L" traço ")
    (L" arroba ")
    (L" sustenido ")
    (L" cifrão ")
    (L" porcento ")
    (L" chapéu ")
    (L" e comercial ")
    (L" asterísco ")
    (L" Sublinhado ")
    (L" abre parênteses ")
    (L" fecha parênteses ")
    (L" mais ")
    (L" igual ")
    (L" abre colchete ")
    (L" fecha colchete ")
    (L" abre chave ")
    (L" fecha chave ")
    (L" barra invertida ")
    (L" barra vertical ")
    (L" áspas ")
    (L" barra ")
    (L" acento ")
    (L" til ")
    (L" menor que ")
    (L" maior que ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"á", L"a acento agudo")
    (L"â", L"a acento circunflexo")
    (L"ã", L"a til")
    (L"à", L"a acento grave")
    (L"ç", L"c cedilha")
    (L"é", L"e acento agudo")
    (L"ê", L"e acento circunflexo")
    (L"í", L"i acento agudo")
    (L"ó", L"o acento agudo")
    (L"ô", L"o acento circunflexo")
    (L"õ", L"o til")
    (L"ú", L"u acento agudo")
    (L"ü", L"u metafonia")
    (L"Á", L"a acento agudo")
    (L"Â", L"a acento circunflexo")
    (L"Ã", L"a til")
    (L"À", L"a acento grave")
    (L"Ç", L"c cedilha")
    (L"É", L"e acento agudo")
    (L"Ê", L"e acento circunflexo")
    (L"Í", L"i acento agudo")
    (L"Ó", L"o acento agudo")
    (L"Ô", L"o acento circunflexo")
    (L"Õ", L"o til")
    (L"Ú", L"u acento agudo")
    (L"Ü", L"u metafonia")
    (L"dollar", L"cifrão")
    (L"pound", L"sustenido")
    (L"at", L"arroba")
    (L"exclamation", L"exclamação")
    (L"slash", L"barra")
    (L"percent", L"porcento")
    (L"caret", L"chapéu")
    (L"ampersand", L"ecomercial")
    (L"star", L"asterísco")
    (L"dash", L"traço")
    (L"underscore", L"sustenido")
    (L"plus", L"mais")
    (L"equals", L"igual")
    (L"backslash", L"barraInvertida")
    (L"pipe", L"barraVertical")
    (L"period", L"ponto")
    (L"comma", L"vírgula")
    (L"semi", L"pontoEvírgula")
    (L"colon", L"doisPontos")
    (L"apostrophe", L"apóstrofo")
    (L"quotes", L"áspas")
    (L"question mark", L"interrogação")
    (L"backquote", L"acento")
    (L"tilde", L"til")
    (L"left paren", L"abre parênteses")
    (L"right paren", L"fecha parênteses")
    (L"left bracket", L"abre colchete")
    (L"right bracket", L"fecha colchete")
    (L"left brace", L"abre chave")
    (L"right brace", L"fecha chave")
    (L"less than", L"menor que")
    (L"greater than", L"maior que")
    (L"space", L"espaço")
    (L"newline", L"novalinha")
    (L"dot", L"ponto")

    // Language name:
    (intern_string(id, locale("")).c_str(), L"Brasileiro")
    .convert_to_container< map<const wstring, const wstring> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" ponto vírgula ")
    (L"\\.{3}", L" ponto ponto ponto ")
    (L"\\.{2}", L" ponto ponto ")
    (L"([a-záâãàçéêíóôõúü])\\.([a-záâãàçéêíóôõúü])", L"$1 ponto $2", true)
    (L"([0-9])\\.([0-9])", L"$1ponto$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxzç]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-záâãàçéêíóôõúü]", L" ", true)
    (L"(^[^-0-9a-záâãàçéêíóôõúü]+| +$)", L"", true)
    (L"  +", L" ");
}
