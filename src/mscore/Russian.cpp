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

#include <sysconfig.h>

#include <string>
#include <locale>
#include <vector>
#include <map>

#include <boost/assign.hpp>
#include <boost/range.hpp>

#include "Russian.hpp"

#include "config.hpp"
#include "strcvt.hpp"


namespace multispeech
{

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
    (L" обратная кавычка ")
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
    (L"newline", L"перевод строки")
    (L"dot", L"точка")

    // Special key names:
    (L"double-quote", L"кавычка")
    (L"alt", L"альт")
    (L"control", L"контрол")
    (L"hyper", L"гипер")
    (L"meta", L"мета")
    (L"shift", L"шифт")
    (L"super", L"супер")
    (L"backspace", L"бэкспэйс")
    (L"break", L"брэйк")
    (L"delete", L"забой")
    (L"down", L"вниз")
    (L"end", L"конец")
    (L"enter", L"ввод")
    (L"escape", L"эскэйп")
    (L"f1", L"эф 1")
    (L"f2", L"эф 2")
    (L"f3", L"эф 3")
    (L"f4", L"эф 4")
    (L"f5", L"эф 5")
    (L"f6", L"эф 6")
    (L"f7", L"эф 7")
    (L"f8", L"эф 8")
    (L"f9", L"эф 9")
    (L"f10", L"эф 10")
    (L"f11", L"эф 11")
    (L"f12", L"эф 12")
    (L"f13", L"эф 13")
    (L"f14", L"эф 14")
    (L"f15", L"эф 15")
    (L"f16", L"эф 16")
    (L"f17", L"эф 17")
    (L"f18", L"эф 18")
    (L"f19", L"эф 19")
    (L"f20", L"эф 20")
    (L"f21", L"эф 21")
    (L"f22", L"эф 22")
    (L"f23", L"эф 23")
    (L"f24", L"эф 24")
    (L"home", L"домой")
    (L"insert", L"вставка")
    (L"kp-*", L"цифровая звёздочка")
    (L"kp-+", L"цифровой плюс")
    (L"kp--", L"цифровой минус")
    (L"kp-.", L"цифровая точка")
    (L"kp-/", L"цифровой слэш")
    (L"kp-0", L"цифровой 0")
    (L"kp-1", L"цифровая 1")
    (L"kp-2", L"цифровая 2")
    (L"kp-3", L"цифровая 3")
    (L"kp-4", L"цифровая 4")
    (L"kp-5", L"цифровая 5")
    (L"kp-6", L"цифровая 6")
    (L"kp-7", L"цифровая 7")
    (L"kp-8", L"цифровая 8")
    (L"kp-9", L"цифровая 9")
    (L"kp-enter", L"цифровой ввод")
    (L"left", L"влево")
    (L"menu", L"меню")
    (L"next", L"вперёд")
    (L"num-lock", L"намлок")
    (L"pause", L"пауза")
    (L"print", L"печать")
    (L"prior", L"назад")
    (L"return", L"возврат")
    (L"right", L"вправо")
    (L"scroll-lock", L"скрол лок")
    (L"tab", L"табуляция")
    (L"up", L"вверх")
    (L"window", L"окно")

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
    (L"([а-яА-ЯёЁ][аеёиоуыэюяАЕЁИОУЫЭЮЯ]) +([бж]\\b)", L"$1$2")
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

} // namespace multispeech
