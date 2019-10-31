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
// interfaces. It comprises common functionality. The derived classes
// should define actual command set and input method by implementing
// virtual methods declared in private section.

#ifndef MULTISPEECH_SERVER_HPP
#define MULTISPEECH_SERVER_HPP

#include <string>
#include <locale>

#include <bobcat/syslogstream>

#include "config.hpp"
#include "polyglot.hpp"
#include "sound_manager.hpp"

class server
{
protected:
  // Object constructor:
  server(const configuration& conf);

public:
  // Destructor is made public to accommodate smart pointers:
  virtual ~server(void);

  // General execution loop:
  void run(void);

  // Logging stream:
  static FBB::SyslogStream log;

  // When true print messages on stderr.
  static bool verbose;

  // Whether to log debug information.
  static bool debug;

protected:
  // Data read from input:
  std::wstring cmd, data;

  // Input charset holder:
  const std::locale input_charset;

  // Speech and sounds:
  polyglot speechmaster;
  sound_manager soundmaster;

private:
  // Get command from the source and parse it placing the command itself
  // and accompanying data into the cmd and data fields respectively.
  virtual void get_command(void) = 0;

  // Perform a command placed in the cmd and data fields.
  // Return false if execution should be finished.
  virtual bool perform_command(void) = 0;
};

#endif
