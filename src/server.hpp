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

// The class server is designed as common ancestor for all speech server
// interfaces. It comprises core functionality. Actual execution loop
// must be defined in the derived classes by implementing
// virtual method run().
//
// Only one instance of this class is allowed for a program.


#ifndef MULTISPEECH_SERVER_HPP
#define MULTISPEECH_SERVER_HPP

#include <locale>

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
  server(int argc, char* argv[]);
  virtual ~server(void);

public:
  // General execution loop:
  virtual void run(void) = 0;

  // Logging stream:
  static FBB::SyslogStream log;

  // When true print messages on stderr.
  static bool verbose;

  // Whether to log debug information.
  static bool debug;

protected:
  // Input charset holder:
  const std::locale input_charset;
};

#endif
