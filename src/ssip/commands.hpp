// commands.hpp -- SSIP commands dispatcher framework
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

// This module provides general SSIP commands table and a bunch
// of dummy virtual methods for the requests execution. These
// methods are to be actually implemented in the session class
// that in turn should be derived from here.

#ifndef MULTISPEECH_SSIP_COMMANDS_HPP
#define MULTISPEECH_SSIP_COMMANDS_HPP

#include <bobcat/cmdfinder>

namespace SSIP
{

class commands: protected FBB::CmdFinder<bool (commands::*)(void)>
{
protected:
  // Main constructor.
  commands(void);

private:
  // These methods are to be implemented in the session class.
  // Return true if the session should be continued
  // or false otherwise.
  virtual bool cmd_speak(void);
  virtual bool cmd_char(void);
  virtual bool cmd_key(void);
  virtual bool cmd_sound_icon(void);
  virtual bool cmd_stop(void);
  virtual bool cmd_cancel(void);
  virtual bool cmd_pause(void);
  virtual bool cmd_resume(void);
  virtual bool cmd_block(void);
  virtual bool cmd_list(void);
  virtual bool cmd_set(void);
  virtual bool cmd_history(void);
  virtual bool cmd_quit(void);
  virtual bool cmd_help(void);
  virtual bool cmd_unknown(void);
  virtual bool cmd_unimplemented(void);

  // commands table.
  static const Entry table[];
};

} // namespace SSIP

#endif
