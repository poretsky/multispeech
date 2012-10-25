// Brazilian.cpp -- Brazilian language implementation
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

#include "Brazilian.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Object construction:

Brazilian::Brazilian(void):
  language_description(lang_id::br, L"[ãôõÁÂÃÊÔÕ]|\\<(não|você|está|el[ae]|isso|meu)\\>")
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
    (L" maior que ");

  // Letters dictionary:
  dictionary = map_list_of
    (L"ç", L"cedilha")
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
    (intern_string(id(), locale("")).c_str(), L"Brasileiro");

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" ponto vírgula ")
    (L"\\.{3}", L" ponto ponto ponto ")
    (L"\\.{2}", L" ponto ponto ")
    (L"([a-záâãàçéêíóôõúü])\\.([a-záâãàçéêíóôõúü])", L"$1 ponto $2", true)
    (L"([0-9])\\.([0-9])", L"$1ponto$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (L"\\<[bcdfghj-np-tvwxzç]+\\>", spell(this), true)
    (punctuations)
    (L"[^\\.,!\\?';\\:0-9a-záâãàçéêíóôõúü]", L" ", true)
    (L"(^[^-0-9a-záâãàçéêíóôõúü]+| +$)", L"", true)
    (L"  +", L" ");
}
