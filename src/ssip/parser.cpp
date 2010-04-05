// parser.cpp -- SSIP parser tables
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

#include "parser.hpp"


namespace SSIP
{

  using namespace FBB;

// General commands table:
const commands::Entry commands::table[] =
  {
    Entry("set", &commands::cmd_set),
    Entry("quit", &commands::cmd_quit),
    Entry("help", &commands::cmd_help),
    Entry("", &commands::cmd_unknown)
  };

// Settings table:
const settings::Entry settings::table[] =
  {
    Entry("client_name", &settings::set_client_name),
    Entry("", &settings::set_unknown)
  };


// General commands dispatcher:

commands::commands(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}


// Settings dispatcher:

settings::settings(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}

  message::code
settings::set_unknown(void)
{
  return message::ERR_PARAMETER_INVALID;
}

} // namespace SSIP
