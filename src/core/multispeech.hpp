// multispeech.hpp -- Main application abstraction
/*
   Copyright (C) 2021 Igor B. Poretsky <poretsky@mlbox.ru>
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

// This class chould be extended by providing real implementations
// for pure virtual methods. Then instantiate derived class in main()
// and call execute method with the arguments of main() function.
// Return value should be returned by main().

#ifndef MULTISPEECH_HPP
#define MULTISPEECH_HPP

#include <string>

#include <boost/scoped_ptr.hpp>

#include <portaudiocpp/AutoSystem.hxx>

#include "speech_server.hpp"

class multispeech
{
public:
  // Construct / destroy:
  multispeech(void);
  virtual ~multispeech(void);

  // Execute the application:
  int execute(int argc, char* argv[]);

private:
  // Should return an instance of speech server.
  // Must be implemented in a derived class.
  virtual speech_server* bootstrap(void) = 0;

  // Should return true if it is a Speech Dispatcher module.
  // Must be implemented in a derived class.
  virtual bool is_spd(void) = 0;

  // Execution error reporting.
  // Must be implemented in a derived class.
  virtual void report_error(const std::string& msg) = 0;

  // Stderr management helpers:
  void restore_stderr(void);
  void forget_stderr(void);

  // For automatic correct audiosystem termination:
  portaudio::AutoSystem audio;

  // Saved stderr file descriptor:
  int efd;

  // Speech server holder:
  boost::scoped_ptr<speech_server> app;
};

#endif
