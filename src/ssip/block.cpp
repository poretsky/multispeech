// block.cpp -- Block in/out requests parser implementation
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

#include "block.hpp"


namespace SSIP
{

using namespace std;
using namespace FBB;


// Recognized requests:
const block::Entry block::table[] =
  {
    Entry("begin", &block::begin),
    Entry("end", &block::end),
    Entry("", &block::unknown)
  };


// Main constructor:

block::block(bool& holder):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE),
  state(holder)
{
}


// Public methods:

message::code
block::toggle(const string& request)
{
  return (this->*findCmd(request))();
}


// Private methods:

message::code
block::begin(void)
{
  message::code rc = message::ERR_ALREADY_INSIDE_BLOCK;
  if (!state)
    {
      state = true;
      rc = message::OK_INSIDE_BLOCK;
    }
  return rc;
}

message::code
block::end(void)
{
  message::code rc = message::ERR_ALREADY_OUTSIDE_BLOCK;
  if (state)
    {
      state = false;
      rc = message::OK_OUTSIDE_BLOCK;
    }
  return rc;
}

message::code
block::unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
}

} // namespace SSIP
