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

#include "Spanish.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:

language_description::options Spanish::settings =
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
  L"[¿¡]|"
  L"[ai]rán|"
  L"v(?:i(?:vir|ento)|aina)|"
  L"c(?:a(?:mina|llar)|uchar|o(?:l[ge]ar|(?:mis|raz)a))|"
  L"pas(?:é|a(?:d[oa]|rá))|"
  L"(?:\\A|\\s|[\"‘])(?:"
  L"l(?:la(?:m(?:[eé]|a[rdn])|na)|(?:impi|uch)a)|"
  L"habla|"
  L"cual|"
  L"quier|"
  L"(?:"
  L"qu(?:é|ieres)|"
  L"a(?:quí|hora|llá)|"
  L"(?:mu|(?:v|est)o)y"
  L")\\b|"
  L"(?:"
  L"ll(?:e[vngctri]|u[veb]|o[rti])|"
  L"cua(?:n[td]|dr[au]|ja)|"
  L"[jrlhb]ueg|"
  L"eje(?:[rm]|cu)|"
  L"desh[oieu]|"
  L"quiera"
  L")\\B"
  L")|"
  L"(?:"
  L"i(?:r(?:éis|ías?)|za(?:d|ba[sn]|r(?:é|on|ais))|céis)|"
  L"(?:[ncdtlvp]|(?:[tf]|[vsh]e)r)ías?|"
  L"[vc]alidad|"
  L"puerta|"
  L"mía?|"
  L"ación|"
  L"sí|"
  L"\\B(?:"
  L"ía(?:n|is)|"
  L"á(?:i|bamo)s|"
  L"(?:[gsbjehü]|(?:[aodurbgn]|(?:[d-gl-pijrtuaby]|[eanrol]c)e)r)ías?|"
  L"(?:[a-drtghpnv]|(?:s|[icuroelb]c)i)ón|"
  L"(?:[tn]|iz?)a(?:r(?:é(?:is)?|ais)|(?:ba|ste)is)|"
  L"e(?:é(?:is)?|a(?:r(?:é(?:is)?|ais|on)|ba(?:[sn]|is)|steis))|"
  L"i(?:za(?:d|ba[ns]|ron)|a(?:ba|r[oa])n|(?:e|a?li)dad)|"
  L"rar(?:on|[aé]is|se)|"
  L"za(?:ste|ré|ba)is|"
  L"l(?:aste|(?:ar)?é)is|"
  L"er(?:éis|án)|"
  L"ca(?:ban|(?:ré|ste)is)|"
  L"(?:[uvtr]e|[ao]r|[ieu]n|(?:[srmet]|r[nv]|[nutm]al|a[cv]|[tr]ic)i)dad|"
  L"[zsfyx]ón?|"
  L"[euo]ó"
  L")"
  L")\\b|"
  L"\\B(?:"
  L"[b-js-vlmpz]án|"
  L"(?:"
  L"l(?:l(?:á|ar[íáé])|ecci)|"
  L"[rnlzsmh]amient|"
  L"ezc(?:l|am)|"
  L"puest|"
  L"scuch"
  L")\\B"
  L")|"
  L"(?:"
  L"cuent|"
  L"mueb|"
  L"izq"
  L")\\B";


// Object construction:

Spanish::Spanish(void):
  language_description(lang_id::es, settings, L"a-zñáéíóúü", criterion)
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
    (L" Mayor que ")
    .convert_to_container< vector<const wchar_t*> >();

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
    (intern_string(id).c_str(), L"Español")
    .convert_to_container< map<const wstring, const wchar_t*> >();

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
