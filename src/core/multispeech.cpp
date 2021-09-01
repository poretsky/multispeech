// multispeech.cpp -- Main application general implementation
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

#include <unistd.h>

#include <cstdlib>
#include <exception>
#include <iostream>

#include <bobcat/syslogstream>

#include "multispeech.hpp"

#include "config.hpp"
#include "speech_server.hpp"


using namespace std;
using namespace boost;
using namespace FBB;


// Construct / destroy:

multispeech::multispeech(void):
  audio(false),
  efd(dup(STDERR_FILENO))
{
  unsetenv("DISPLAY");
}

multispeech::~multispeech(void)
{
  if (efd >= 0)
    close(efd);
}


// Public methods:

int
multispeech::execute(int argc, char* argv[])
{
  try
    {
      configuration conf(argc, argv, is_spd());

      if (speech_server::verbose)
        cerr << "Initializing audio system..." << endl;
      audio.initialize();
      if (speech_server::verbose)
        cerr << "Audio system initialization complete." << endl;

      app.reset(bootstrap());
    }
  catch (const string& info)
    {
      cout << info << endl;
      forget_stderr();
      return EXIT_SUCCESS;
    }
  catch (const configuration::error& error)
    {
      speech_server::log << SyslogStream::err << error.what() << endl;
      restore_stderr();
      string msg("ERROR: ");
      msg += error.what();
      report_error(msg);
      return EXIT_FAILURE;
    }
  catch (const std::exception& error)
    {
      speech_server::log << SyslogStream::err << error.what() << configuration::stage << endl;
      restore_stderr();
      cerr << "ERROR" << configuration::stage << ": " << error.what() << endl;
      return EXIT_FAILURE;
    }

  forget_stderr();
  return app->run();
}


// Private methods:

void
multispeech::restore_stderr(void)
{
  if (efd >= 0)
    {
      dup2(efd, STDERR_FILENO);
      close(efd);
      efd = -1;
    }
}

void
multispeech::forget_stderr(void)
{
  if (efd >= 0)
    {
      close(efd);
      efd = -1;
    }
}
