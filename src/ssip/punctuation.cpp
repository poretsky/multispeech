// punctuation.cpp -- Punctuation mode setup request parser implementation
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include "punctuation.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;
using namespace multispeech;


// Recognized requests:
const punctuation::Entry punctuation::table[] =
  {
    Entry("all", &punctuation::all),
    Entry("some", &punctuation::some),
    Entry("none", &punctuation::none),
    Entry("", &punctuation::unknown)
  };


// Main constructor:

punctuation::punctuation(punctuations::mode& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}


// Public methods:

bool
punctuation::parse(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

bool
punctuation::all(void)
{
  value = punctuations::all;
  return true;
}

bool
punctuation::some(void)
{
  value = punctuations::some;
  return true;
}

bool
punctuation::none(void)
{
  value = punctuations::none;
  return true;
}

bool
punctuation::unknown(void)
{
  return false;
}

} // namespace SSIP
