// Spanish.cpp -- Spanish language implementation
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

#include "Spanish.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Object construction:

Spanish::Spanish(void):
  language_description(lang_id::es, L"[ñÑ]|(^| )(qué|los|el|te) ")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" Exclamación ")
    (L" Apostrofo ")
    (L" Interrogación ")
    (L" Coma ")
    (L" Punto ")
    (L" Punto y coma ")
    (L" Dos puntos ")
    (L" Guión-Menos ")
    (L" Arroba ")
    (L" Signo de número ")
    (L" Signo de dólar ")
    (L" Por ciento ")
    (L" Acento circunflejo ")
    (L" y comercial ")
    (L" Asterísco ")
    (L" Guión Bajo ")
    (L" Abrir paréntesis ")
    (L" Cerrar paréntesis ")
    (L" Más ")
    (L" Igual ")
    (L" Abrir corchete ")
    (L" Cerrar corchete ")
    (L" Abrir llave ")
    (L" Cerra llave ")
    (L" barra invertida ")
    (L" línea vertical ")
    (L" Comillas ")
    (L" Barra ")
    (L" acento grave ")
    (L" tilde ")
    (L" Menor que ")
    (L" Mayor que ");

  // Letters dictionary:
  dictionary = map_list_of
    (L"á", L"a acento agudo")
    (L"é", L"e acento agudo")
    (L"í", L"i acento agudo")
    (L"ñ", L"n tilde")
    (L"ó", L"o acento agudo")
    (L"ú", L"u acento agudo")
    (L"Ñ", L"n tilde")
    (L"dollar", L"SignoDeDólar")
    (L"pound", L"SignoDeNúmero")
    (L"at", L"arroba")
    (L"exclamation", L"Exclamación")
    (L"slash", L"barra")
    (L"percent", L"PorCiento")
    (L"caret", L"AcentoCircunflejo")
    (L"ampersand", L"yComercial")
    (L"star", L"asterísco")
    (L"dash", L"GuiónMenos")
    (L"underscore", L"GuiónBajo")
    (L"plus", L"Más")
    (L"equals", L"Igual")
    (L"backslash", L"barraInvertida")
    (L"pipe", L"líneaVertical")
    (L"period", L"punto")
    (L"comma", L"coma")
    (L"semi", L"puntoYcoma")
    (L"colon", L"dosPuntos")
    (L"apostrophe", L"apostrofo")
    (L"quotes", L"Comillas")
    (L"question mark", L"Interrogación")
    (L"backquote", L"acento")
    (L"left paren", L"Abrir paréntesis")
    (L"right paren", L"Cerrar paréntesis")
    (L"left bracket", L"Abrir corchete")
    (L"right bracket", L"Cerrar corchete")
    (L"left brace", L"Abrir llave")
    (L"right brace", L"Cerra llave")
    (L"less than", L"Menor que")
    (L"greater than", L"Mayor que")
    (L"space", L"espacio")
    (L"tab", L"tabulación")
    (L"newline", L"alimentaciónDeLínea")
    (L"¿", L"interrogaciónInversa")
    (L"¡", L"inversoDeExclamación")
    (L"dot", L"punto")

    // Language name:
    (intern_string(id(), locale("")).c_str(), L"Español");

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" punto coma ")
    (L"\\.{3}", L" punto punto punto ")
    (L"\\.{2}", L" punto punto ")
    (L"([a-záéíñóú])\\.([a-záéíñóú])", L"$1 punto $2", true)
    (L"([0-9])\\.([0-9])", L"$1punto$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxzñ]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-záéíñóú]", L" ", true)
    (L"(^[^-0-9a-záéíñóú]+| +$)", L"", true)
    (L"  +", L" ");
}
