// commands.cpp -- SSIP commands dispatcher basic implementation
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

#include "commands.hpp"


namespace SSIP
{

using namespace FBB;


// Recognized commands:
const commands::Entry commands::table[] =
  {
    Entry("speak", &commands::cmd_speak),
    Entry("char", &commands::cmd_char),
    Entry("key", &commands::cmd_key),
    Entry("sound_icon", &commands::cmd_sound_icon),
    Entry("stop", &commands::cmd_stop),
    Entry("cancel", &commands::cmd_cancel),
    Entry("pause", &commands::cmd_pause),
    Entry("resume", &commands::cmd_resume),
    Entry("block", &commands::cmd_block),
    Entry("list", &commands::cmd_list),
    Entry("set", &commands::cmd_set),
    Entry("history", &commands::cmd_history),
    Entry("quit", &commands::cmd_quit),
    Entry("help", &commands::cmd_help),
    Entry("", &commands::cmd_unknown)
  };


// Basic constructor:

commands::commands(void):
  CmdFinder<FunctionPtr>(table, table +
                         (sizeof(table) / sizeof(Entry)),
                         USE_FIRST | INSENSITIVE)
{
}


// Dummy implementations:

bool
commands::cmd_speak(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_char(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_key(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_sound_icon(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_stop(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_cancel(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_pause(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_resume(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_block(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_list(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_set(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_history(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_quit(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_help(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_unknown(void)
{
  return cmd_unimplemented();
}

bool
commands::cmd_unimplemented(void)
{
  return true;
}

} // namespace SSIP
