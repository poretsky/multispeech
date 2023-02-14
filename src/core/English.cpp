// English.cpp -- English language implementation
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

#include <boost/assign.hpp>

#include "English.hpp"

#include "config.hpp"
#include "strcvt.hpp"
#include "espeak.hpp"

using namespace std;
using namespace boost::assign;


// Static data:

language_description::options English::settings =
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
  L"h(?:anded|ye|e(?:ep|a(?:rt|ts)))|"
  L"g(?:o|(?:a|et)t)ing|"
  L"d(?:e(?:ed|bt|lay)|(?:o|ur|a[tr])ing)|"
  L"t(?:ween|oys|reaty|(?:a(?:k|pp)|ell)ing)|"
  L"s(?:h(?:o(?:u|wer)|ea|ine)|oo[nt]|igh|eal|nap|kill)|"
  L"n(?:e(?:ph|ed|vil|ath)|arrow)|"
  L"(?:[kvg]|a[rl]|ir|el)ish|"
  L"w(?:ife|rong|eak)|"
  L"b(?:o(?:th|wer)|e(?:(?:nd)?ing|gs|lly)|ump|room)|"
  L"[mh]oney|"
  L"f(?:ew|o(?:am|r(?:bi|ce)d)|a(?:int|wn)|lea|uck)|"
  L"[bngdf]oon|"
  L"(?:\\A|\\s|[\"‘])(?:"
  L"o(?:ver[st]|ther|wn)|"
  L"w(?:atch|h(?:y|om|ich|e(?:[nt]|r?e)))|"
  L"t(?:h(?:e[ny]|us|i(?:s|ng))|ree|o(?:day|ok))|"
  L"m(?:outh|aybe)|"
  L"s(?:eve(?:n|ral)|(?:oldi|ist)er)|"
  L"l(?:eft|augh)|"
  L"f(?:ear|o(?:rgetllow))|"
  L"e(?:a(?:t|rly)|ntire)|"
  L"a(?:void|pp(?:ear|ly)|(?:wa|bilit)y)|"
  L"c(?:areer|ould)|"
  L"i(?:nvolve|ts)|"
  L"d(?:ear|ays)|"
  L"p(?:l?eas|retty)|"
  L"ra(?:ise|ther)|"
  L"[mlrtj]akes|"
  L"(?:"
  L"h(?:ave|im)|"
  L"y(?:our|et)|"
  L"g(?:et|ive|reat)|"
  L"s(?:tay|h(?:e|it))|"
  L"a(?:bout|ny|gain)|"
  L"we(?:ll|re)|"
  L"d(?:id|oor)|"
  L"f(rom|ound)|"
  L"t(?:ake|old|hanks)|"
  L"be(?:en|cause)"
  L")\\b|"
  L"(?:"
  L"w(?:ith|eap|o(?:u|rr)|h(?:i(?:[nmflzd]|tt|sp)|o[rlsdp]|a[ltnmc]))|"
  L"over(?:[pgmwnvju]|c[oarhu]|[dlf]?[ei]|b?r|[lhf]a|a[cs])|"
  L"non(?:[rmblgh]|d(?:e|is)|c(?:a|om)|pe|ex|su)|"
  L"un(?:c(?:[al]|on)|ex|[st]h|pre)|"
  L"mis[pdrbg]|"
  L"th(?:re|ir|os|r?ou|ei)"
  L")\\B"
  L")|"
  L"(?:"
  L"[a-z]'(?:[ds]|ll|[rv]e)|"
  L"(?:[es](?:ab)?|co?|ar?|pi?|in|ful)ly|"
  L"(?:[ky]|[st]h|[lrgm]at|[oa]w|i[src]|f[il]|us)ed|"
  L"w(?:i(?:s[eh]|thin)|(?:rit|h(?:[io]l|er))e|orry|aiting)|"
  L"t(?:h(?:ere|i(?:ng)?s|a(?:t|nk))|reat|ough)|"
  L"g(?:ood|row)|"
  L"m(?:other|eet|ight)|"
  L"f(?:i(?:ll|ght|nd|ve)|act|eel|loor)|"
  L"co(?:st|ld)|"
  L"a(?:llow|dd|round|cross|head|shing)|"
  L"b(?:uy|elieve)|"
  L"h(?:appen|usband|ealth)|"
  L"s(?:pell(?:s|e[dr])|uffer)|"
  L"ot(?:ic|hers)|"
  L"poo(?:r|[npt]s?|k(?:s|ing))|"
  L"l(?:ea(?:ve|rn)s?|ooking)|"
  L"u(?:rned|ntil)|"
  L"y(?:ields?|u(?:ps?|ng)|ea(?:rs)?)|"
  L"(?:wit|ill|sad)ness|"
  L"[wmcbhgjfy]elled|"
  L"i(?:(?:c|gg|tch)ing|shes)|"
  L"\\B(?:"
  L"(?:[sghkymbfwp]|[l-oeirwud]d|[aueio]l|[r-unhepaflox]t|[aou]r)ness(?:es)?|"
  L"(?:[a-df-ik-or-uw-z]|[b-ik-ps-wzy]e)less|"
  L"(?:[hymw]|[ae]n|[gkdt]i?|(?:[rls]|[tp]h)i|(?:[im]a?|[trnvdlch]a)b|(?:[inrtumdes]|[grnhmpdlsf]ic)al)ly|"
  L"i(?:(?:at|ck|th)ing|(?:v|:bil)it(?:y|ies))|"
  L"n(?:other|etic|gy|tment|(?:n|ch|gl?)ed|a(?:mic|t(?:ed|ing))|i(?:s(?:ts|ing|ms?)|z(?:ed|ing)))|"
  L"(?:[cdmgvbsk]|he?|tr?|[ay]l?|[ou][lr]?|e[lr]|[il]l)isms?|"
  L"(?:a|is)tics?|"
  L"e(?:ther|r?ous)|"
  L"(?:[yiwf]|t[he]?|[lsgrmkpdn]e?|c[eh]|[vu]e)fuls?|"
  L"(?:a(?:[ygvw]|th)|u(?:th)?|[rtc](?:y|at|ter|i[zs])|e(?:[de]|[nca]t|r?th)|o(?:[sv]|rn|r?th)|g(?:ht|th)|p(?:er|th)|m(?:ak|iz)|[dvhz]en|[yswk]th)ing|"
  L"l(?:thing|ways|ready|(?:i[zs]|at)ing)|"
  L"ea(?:[fh]|[pcs]ons?|ch(?:ers?|ing)|rn)|"
  L"(?:y|me)(?:one|body)|"
  L"[rmyte]self|"
  L"aught?"
  L")"
  L")\\b|"
  L"\\B(?:"
  L"ceae|"
  L"fish|"
  L"(?:"
  L"(?:[lk]|iv|(?:i|mo)g|[ri]s|a[tgr]|o[ms]|u[tr]?|[nh](?:og)?)enes|"
  L"(?:u|(?:t|ac)i)ou|"
  L"[nrt]izatio|"
  L"i(?:id|fy)|"
  L"e(?:ph(?:r|alo)|ather|sthes)|"
  L"o(?:rshi|lour)|"
  L"a(?:li[zs]in|ught)|"
  L"ce(?:ph|iv)|"
  L"wh(?:e|i[tlprd]|o[ulr]|a[cltm])|"
  L"le(?:dge|pho|som|ev)|"
  L"phic|"
  L"fyi"
  L")\\B"
  L")";


// Object construction:

English::English(void):
  language_description(lang_id::en, settings, L"a-z", criterion)
{
  // Punctuations pronunciation:
  punctuations = list_of
    (L" exclamation ")
    (L" apostrophe ")
    (L" question mark ")
    (L" comma ")
    (L" period ")
    (L" semicolon ")
    (L" colon ")
    (L" dash ")
    (L" at ")
    (L" pound ")
    (L" dollar ")
    (L" percent ")
    (L" caret ")
    (L" ampersand ")
    (L" star ")
    (L" underscore ")
    (L" left paren ")
    (L" right paren ")
    (L" plus ")
    (L" equals ")
    (L" left bracket ")
    (L" right bracket ")
    (L" left brace ")
    (L" right brace ")
    (L" backslash ")
    (L" pipe ")
    (L" quotes ")
    (L" slash ")
    (L" backquote ")
    (L" tilde ")
    (L" less than ")
    (L" greater than ")
    .convert_to_container< vector<const wchar_t*> >();

  // Letters dictionary:
  dictionary = map_list_of
    (L"newline", L"new line")

    // Language name:
    (intern_string(id).c_str(), L"English")
    .convert_to_container< map<const wstring, const wchar_t*> >();

  // Text filtering chain:
  filter_chain.setup()
    (L"\\.,", L" dot comma ")
    (L"\\.{3}", L" dot dot dot ")
    (L"\\.{2}", L" dot dot ")
    (L"([a-z])\\.([a-z])", L"$1 dot $2", true)
    (L"([0-9])\\.([0-9])", L"$1period$2")
    (L"[\\+-]?[0-9]+|_", L" $& ")
    (L"(^| ) ((?:[0-9]*[02-9])?1) (st)\\>", L"$1$2$3", true)
    (L"(^| ) ((?:[0-9]*[02-9])?2) (nd)\\>", L"$1$2$3", true)
    (L"(^| ) ((?:[0-9]*[02-9])?3) (rd)\\>", L"$1$2$3", true)
    (L"(^| ) ([0-9]*[04-9]) (th)\\>", L"$1$2$3", true)
    (L"(^| ) ([0-9]*1[1-3]) (th)\\>", L"$1$2$3", true)
    (punctuations)
    (L"(^|[^'])\\<[bcdfghj-np-tvwxz]+\\>([^']|$)", spell(this), true)
    (L"[^\\.,!\\?';\\:0-9a-z]", L" ", true)
    (L"(^[^-0-9a-z]+| +$)", L"", true)
    (L"  +", L" ");
}
