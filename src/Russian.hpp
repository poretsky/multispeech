// Russian.hpp -- Russian language description
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

#ifndef MULTISPEECH_RUSSIAN_HPP
#define MULTISPEECH_RUSSIAN_HPP

#include <map>

#include "language_description.hpp"

class Russian: public language_description
{
public:
  // Object construction:
  Russian(void);

private:
  // Transliteration table:
  std::map<wchar_t, const wchar_t*> translit;

  // Abbreviation speller:
  std::wstring do_spell(const boost::iterator_range<std::wstring::const_iterator>& abbrev);
};

#endif
