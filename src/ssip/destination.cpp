// destination.cpp -- SSIP request destination parameter parser implementation
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

#include <boost/lexical_cast.hpp>

#include "destination.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;
using namespace FBB;


// Recognized requests:
const destination::Entry destination::table[] =
  {
    Entry("self", &destination::check_self),
    Entry("all", &destination::check_all),
    Entry("", &destination::check_another)
  };

// Id validator:
const regex destination::validator("^\\d+$");


// Main constructor:

destination::destination(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}


// Public methods:

const string&
destination::parse(const string& request)
{
  (this->*findCmd(request))();
  return beyond();
}

destination::choice
destination::selection(void) const
{
  return state;
}

unsigned long
destination::id(void) const
{
  return value;
}


// Private methods:

void
destination::check_self(void)
{
  state = self;
}

void
destination::check_all(void)
{
  state = all;
}

void
destination::check_another(void)
{
  if (regex_match(cmd(), validator))
    {
      state = another;
      value = lexical_cast<unsigned long>(cmd());
    }
  else state = invalid;
}

} // namespace SSIP
