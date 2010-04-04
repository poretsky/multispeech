// parser.hpp -- SSIP requests parser framework
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

#ifndef MULTISPEECH_SSIP_COMMANDS_HPP
#define MULTISPEECH_SSIP_COMMANDS_HPP

#include <bobcat/cmdfinder>

namespace SSIP
{

// General commands dispatcher.
class commands: protected FBB::CmdFinder<bool (commands::*)(void)>
{
protected:
  // Main constructor.
  commands(void);

private:
  // These method are to be the session class. Return true
  // if the session should be continued or false otherwise.
  virtual bool cmd_set(void) = 0;
  virtual bool cmd_quit(void) = 0;
  virtual bool cmd_unknown(void) = 0;

  // commands table.
  static const Entry table[];
};

// Parameter setting subcommands.
class settings: protected FBB::CmdFinder<unsigned int (settings::*)(void)>
{
protected:
  // Main constractor.
  settings(void);

private:
  // These methods are to be implemented in the derived class.
  // Return result code for client.
  virtual unsigned int set_client_name(void) = 0;
  unsigned int set_unknown(void);

  // Settings table.
  static const Entry table[];
};

} // namespace SSIP

#endif
