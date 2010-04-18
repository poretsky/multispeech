// boolean_flag.cpp -- General boolean parameter parser implementation
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

#include "boolean_flag.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;


// Recognized requests:
const boolean_flag::Entry boolean_flag::table[] =
  {
    Entry("on", &boolean_flag::flag_on),
    Entry("off", &boolean_flag::flag_off),
    Entry("", &boolean_flag::flag_invalid)
  };


// Main constructor:

boolean_flag::boolean_flag(bool& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  value(holder)
{
}


// Public methods:

bool
boolean_flag::parse(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

bool
boolean_flag::flag_on(void)
{
  value = true;
  return true;
}

bool
boolean_flag::flag_off(void)
{
  value = false;
  return true;
}

bool
boolean_flag::flag_invalid(void)
{
  return false;
}

} // namespace SSIP
