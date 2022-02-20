// Russian.cpp -- Russian language implementation
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include <map>

#include <boost/assign.hpp>
#include <boost/regex.hpp>

#include "Russian.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;


// Static data:
language_description::options Russian::settings =
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

// Transliteration table:
static const map<wchar_t, const wchar_t*> translit = map_list_of
  (L'a', L"а")
  (L'b', L"б")
  (L'c', L"ц")
  (L'd', L"д")
  (L'e', L"е")
  (L'f', L"ф")
  (L'g', L"г")
  (L'h', L"х")
  (L'i', L"и")
  (L'j', L"дж")
  (L'k', L"к")
  (L'l', L"л")
  (L'm', L"м")
  (L'n', L"н")
  (L'o', L"о")
  (L'p', L"п")
  (L'q', L"ку")
  (L'r', L"р")
  (L's', L"с")
  (L't', L"т")
  (L'u', L"у")
  (L'v', L"в")
  (L'w', L"в")
  (L'x', L"кс")
  (L'y', L"и")
  (L'z', L"з")
  (L'á', L"а\u0301")
  (L'à', L"а\u0301")
  (L'ä', L"я")
  (L'ç', L"с")
  (L'é', L"э\u0301")
  (L'è', L"э\u0301")
  (L'í', L"и\u0301")
  (L'ì', L"и\u0301")
  (L'ï', L"й")
  (L'î', L"и")
  (L'ñ', L"нь")
  (L'ó', L"о\u0301")
  (L'ò', L"о\u0301")
  (L'ö', L"ё")
  (L'ô', L"о")
  (L'œ', L"ё")
  (L'ß', L"сс")
  (L'ú', L"у\u0301")
  (L'ù', L"у\u0301")
  (L'ü', L"ю")
  (L'û', L"ю")
  (L'є', L"е")
  (L'і', L"и")
  (L'ї', L"йи")
  .convert_to_container< map<wchar_t, const wchar_t*> >();


// Object construction:

Russian::Russian(void):
  language_description(lang_id::ru, settings, L"а-яё", L"[а-яё]")
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" восклицательный знак ")
    (L" апостроф ")
    (L" вопросительный знак ")
    (L" запятая ")
    (L" точка ")
    (L" точка с запятой ")
    (L" двоеточие ")
    (L" тирэ ")
    (L" собака ")
    (L" решётка ")
    (L" доллар ")
    (L" процент ")
    (L" домик ")
    (L" амперсанд ")
    (L" звезда ")
    (L" подчёркивание ")
    (L" круглая скобка открыть ")
    (L" круглая скобка закрыть ")
    (L" плюс ")
    (L" равно ")
    (L" квадратная скобка открыть ")
    (L" квадратная скобка закрыть ")
    (L" фигурная скобка открыть ")
    (L" фигурная скобка закрыть ")
    (L" бэкслэш ")
    (L" вертикальная черта ")
    (L" кавычка ")
    (L" слэш ")
    (L" одинарная кавычка ")
    (L" тильда ")
    (L" меньше ")
    (L" больше ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"б", L"бэ")
    (L"в", L"вэ")
    (L"й", L"и краткое")
    (L"к", L"ка")
    (L"с", L"эс")
    (L"ъ", L"твёрдый знак")
    (L"ь", L"мягкий знак")
    (L"b", L"бэ")
    (L"g", L"ж")
    (L"h", L"аш")
    (L"j", L"йот")
    (L"k", L"ка")
    (L"q", L"ку")
    (L"s", L"эс")
    (L"v", L"вэ")
    (L"w", L"дубльвэ")
    (L"x", L"икс")
    (L"y", L"игрек")
    (L"z", L"зэт")
    (L"Б", L"бэ")
    (L"В", L"вэ")
    (L"Й", L"и краткое")
    (L"К", L"ка")
    (L"С", L"эс")
    (L"Ъ", L"твёрдый знак")
    (L"Ь", L"мягкий знак")
    (L"B", L"бэ")
    (L"G", L"ж")
    (L"H", L"аш")
    (L"J", L"йот")
    (L"K", L"ка")
    (L"Q", L"ку")
    (L"S", L"эс")
    (L"V", L"вэ")
    (L"W", L"дубльвэ")
    (L"X", L"икс")
    (L"Y", L"игрек")
    (L"Z", L"зэт")
    (L"dollar", L"доллар")
    (L"pound", L"решётка")
    (L"at", L"собака")
    (L"exclamation", L"восклицательный знак")
    (L"slash", L"слэш")
    (L"percent", L"процент")
    (L"caret", L"домик")
    (L"ampersand", L"амперсанд")
    (L"star", L"звезда")
    (L"dash", L"тирэ")
    (L"underscore", L"подчёркивание")
    (L"plus", L"плюс")
    (L"equals", L"равно")
    (L"backslash", L"бэкслэш")
    (L"pipe", L"вертикальная черта")
    (L"period", L"точка")
    (L"comma", L"запятая")
    (L"semi", L"точка с запятой")
    (L"colon", L"двоеточие")
    (L"apostrophe", L"апостроф")
    (L"quotes", L"кавычка")
    (L"question mark", L"вопросительный знак")
    (L"backquote", L"одинарная кавычка")
    (L"tilde", L"тильда")
    (L"left paren", L"круглая скобка открыть")
    (L"right paren", L"круглая скобка закрыть")
    (L"left bracket", L"квадратная скобка открыть")
    (L"right bracket", L"квадратная скобка закрыть")
    (L"left brace", L"фигурная скобка открыть")
    (L"right brace", L"фигурная скобка закрыть")
    (L"less than", L"меньше")
    (L"greater than", L"больше")
    (L"space", L"пробел")
    (L"tab", L"табуляция")
    (L"newline", L"новая строка")
    (L"dot", L"точка")

    // Language name:
    (intern_string(id).c_str(), L"русский")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"[\\+-]?\\d+([\\.,]\\d+)?|_", L" $& ")
    (L"-([0-9])", L"минус $1")
    (L"(([а-яА-ЯёЁ][аеёиоуыэю]|[яЯ])\\x{301}?)( +|-)(([бж]|ль)\\b)", L"$1$4")
    (L"\\b[БВКС]\\.", spell(this))
    (case_conversion::lowercase)
    (L"([^[:alpha:]\\x{301}])([bcdfghj-np-tv-zб-джзй-нп-тф-ъь]{2,}|[a-zбжъь])\\>", L"$1 $2")
    (L" ([bcdfghj-np-tv-zб-джзй-нп-тф-ъь]{2,}|[a-zбжъь])\\>", spell(this))
    (translit)
    (punctuations)
    (L"[^-\\.,!\\?;\\:0-9а-яё\\x{301}]", L" ")
    (L"([^аеёиоуыэюя])\\x{301}", L"$1 ")
    (L"(^[^-0-9а-яё]+| +$)", L"")
    (L"  +", L" ");
}
