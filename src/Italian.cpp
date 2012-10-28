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
#include <boost/algorithm/string.hpp>

#include "Italian.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;


// Object construction:

Italian::Italian(void):
  language_description(lang_id::it, L"[ìòÈÌÎÒÙ]|(^| )(di|c(h[ei]|'era)|il|sono|ho) |\\<è\\>")
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
    (L" maggiore ");

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
    (intern_string(id(), locale("")).c_str(), L"Italiano");

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
