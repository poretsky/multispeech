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
#include <sstream>
#include <locale>

#include <bobcat/syslogstream>

#include "config.hpp"
#include "polyglot.hpp"
#include "sound_manager.hpp"

class server: private sound_manager::callback
{
protected:
  // Object constructor:
  server(const configuration& conf);

public:
  // Destructor is made public to accommodate smart pointers:
  virtual ~server(void);

  // General execution loop:
  int run(void);

  // Redirect standard error output to the specified file or to /dev/null.
  // Return true on success.
  static bool redirect_stderr(const char* file = 0);

  // Logging stream:
  static FBB::SyslogStream log;

  // When true print messages on stderr.
  static bool verbose;

  // Whether to log debug information.
  static bool debug;

protected:
  // Prepare for new command reception cycle:
  virtual void communication_reset(void);

  // Dummy command executor:
  bool do_nothing(void);

  // Data read from input:
  std::string cmd;
  std::ostringstream accumulator;

  // Input charset holder:
  const std::locale input_charset;

  // Speech and sounds:
  polyglot speechmaster;
  sound_manager soundmaster;

  // Exit status:
  int exit_status;

private:
  // Implement these methods to take over queue events.
  // Default implementation does nothing.
  void index_mark(const std::string& name);
  void queue_done(void);

  // Get command from the source and parse it placing the command itself
  // and accompanying data into the cmd and data fields respectively.
  virtual void get_command(void) = 0;

  // Perform a command placed in the cmd and data fields.
  // Return false if execution should be finished.
  virtual bool perform_command(void) = 0;
};

#endif