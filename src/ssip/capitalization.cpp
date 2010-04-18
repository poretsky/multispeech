// capitalization.cpp -- Capital letter recognition mode setup request parser implementation
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

#include "capitalization.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;


// Recognized requests:
const capitalization::Entry capitalization::table[] =
  {
    Entry("none", &capitalization::caps_none),
    Entry("spell", &capitalization::caps_spell),
    Entry("icon", &capitalization::caps_icon),
    Entry("", &capitalization::caps_unknown)
  };


// Main constructor:

capitalization::capitalization(mode& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}


// Public methods:

bool
capitalization::parse(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

bool
capitalization::caps_none(void)
{
  value = none;
  return true;
}

bool
capitalization::caps_spell(void)
{
  value = spell;
  return true;
}

bool
capitalization::caps_icon(void)
{
  value = icon;
  return true;
}

bool
capitalization::caps_unknown(void)
{
  return false;
}

} // namespace SSIP
