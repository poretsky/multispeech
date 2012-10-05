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

#include <string>
#include <locale>
#include <vector>
#include <map>

#include <boost/assign.hpp>
#include <boost/range.hpp>

#include "Russian.hpp"

#include "config.hpp"
#include "strcvt.hpp"

using namespace std;
using namespace boost;
using namespace boost::assign;


// Object construction:

Russian::Russian(void):
  language_description(lang_id::ru, L"[а-яёА-ЯЁ]")
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
    (L" больше ");

  // Letters dictionary:
  dictionary = map_list_of
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
    (L"apostrophe", L"	апостроф")
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
    (intern_string(id(), locale("")).c_str(), L"русский");

  // Transliteration table:
  translit = map_list_of
    (L'a', L"а")
    (L'b', L"б")
    (L'c', L"ц")
    (L'd', L"д")
    (L'e', L"е")
    (L'f', L"ф")
    (L'g', L"г")
    (L'h', L"х")
    (L'i', L"и")
    (L'j', L"й")
    (L'k', L"к")
    (L'l', L"л")
    (L'm', L"м")
    (L'n', L"н")
    (L'o', L"о")
    (L'p', L"п")
    (L'q', L"к")
    (L'r', L"р")
    (L's', L"с")
    (L't', L"т")
    (L'u', L"у")
    (L'v', L"в")
    (L'w', L"в")
    (L'x', L"кс")
    (L'y', L"и")
    (L'z', L"з");

  // Text filtering chain:
  filter_chain.setup()
    (L"[\\+-]?[0-9]+(\\.[0-9]+)?|_", L" $& ")
    (L"-([0-9])", L"минус $1")
    (L"([а-яА-ЯёЁ][аеёиоуыэюяАЕЁИОУЫЭЮЯ]) +(([бж]|ль)\\b)", L"$1$2")
    (L"\\b[БВКС]\\.", spell(this))
    (case_conversion::lowercase)
    (L"\\<([bcdfghj-np-tv-zб-джзй-нп-тф-ъь]{2,}|[a-zбжъь])\\>", spell(this))
    (translit)
    (punctuations)
    (L"[^-\\.,!\\?;\\:0-9а-яё]", L" ")
    (L"([0-9])( +[0-9])", L"$1,$2")
    (L"(^[^-0-9а-яё]+| +$)", L"")
    (L"  +", L" ");
}

// Abbreviations spelling:

wstring
Russian::do_spell(const iterator_range<wstring::const_iterator>& abbrev)
{
  wstring result;
  map<const wstring, const wstring>::const_iterator item;
  for (wstring::const_iterator sptr = abbrev.begin(); sptr != abbrev.end(); ++sptr)
    {
      item = dictionary.find(wstring(1, toupper(*sptr, locale(""))));
      if (item == dictionary.end())
        result += *sptr;
      else result += item->second;
      result += L' ';
    }
  return result;
}
