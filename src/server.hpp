// server.hpp -- Speech server engine framework
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>
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

// The server class comprises core multilingual speech server functionality.
// See ancestors definition for details about available capabilities.
//
// Only one instance of this class is allowed for a program.


#ifndef MULTISPEECH_SERVER_HPP
#define MULTISPEECH_SERVER_HPP

#include <string>
#include <istream>

#include <bobcat/syslogstream>

#include "config.hpp"
#include "polyglot.hpp"
#include "sound_manager.hpp"

class server:
  protected configuration,
  protected sound_manager,
  protected polyglot
{
protected:
  // Construct / destroy:
  server(int argc, char* argv[], const char* conf_file,
         const char* eos_command);
  virtual ~server(void);

public:
  // Run session on specified input stream.
  // The stream must be already opened.
  void run_session(std::istream& source);

  // Logging stream:
  static FBB::SyslogStream log;

  // When true print messages on stderr.
  static bool verbose;

  // Whether to log debug information.
  static bool debug;

protected:
  // Pseudo-command that should be generated when the source
  // stream is exhausted.
  const std::string eos_cmd;

  // Generally, commands are retrieved as simple text lines,
  // but derived classes can provide some more elaborate method
  // if it is necessary.
  virtual std::string request(std::istream& source);

  // Actual command parser must be implemented in derived class.
  // Return true to continue the session or false otherwise.
  virtual bool perform(std::string command) = 0;
};

#endif
