// settings.hpp -- SSIP set subcommand parser framework
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

// This module provides parameters table for SSIP set subcommand
// and a bunch of dummy virtual methods for the requests execution.
// These methods are to be actually implemented in the session class
// that in turn should be derived from here.

#ifndef MULTISPEECH_SSIP_SETTINGS_HPP
#define MULTISPEECH_SSIP_SETTINGS_HPP

#include <bobcat/cmdfinder>

#include "destination.hpp"
#include "message.hpp"

namespace SSIP
{

class settings: protected FBB::CmdFinder<message::code (settings::*)(destination&)>
{
protected:
  // Main constractor.
  settings(void);

private:
  // These methods are to be implemented in the derived class.
  // Return result code for client.
  virtual message::code set_client_name(destination& target);
  virtual message::code set_priority(destination& target);
  virtual message::code set_output_module(destination& target);
  virtual message::code set_language(destination& target);
  virtual message::code set_ssml_mode(destination& target);
  virtual message::code set_punctuation(destination& target);
  virtual message::code set_spelling(destination& target);
  virtual message::code set_cap_let_recogn(destination& target);
  virtual message::code set_voice(destination& target);
  virtual message::code set_synthesis_voice(destination& target);
  virtual message::code set_rate(destination& target);
  virtual message::code set_pitch(destination& target);
  virtual message::code set_volume(destination& target);
  virtual message::code set_pause_context(destination& target);
  virtual message::code set_history(destination& target);
  virtual message::code set_notification(destination& target);

  // This method is properly provided here.
  message::code set_unknown(destination& target);

  // Subcommands table.
  static const Entry table[];
};

} // namespace SSIP

#endif
