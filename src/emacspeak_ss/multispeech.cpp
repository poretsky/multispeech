// multispeech.cpp -- Multilingual speech server
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <unistd.h>

#include <cstdlib>
#include <exception>
#include <string>
#include <iostream>

#include <boost/scoped_ptr.hpp>

#include <bobcat/syslogstream>

#include <portaudiocpp/PortAudioCpp.hxx>

#include "config.hpp"
#include "speech_server.hpp"
#include "frontend.hpp"

using namespace std;
using namespace boost;
using namespace FBB;
using namespace portaudio;

int main(int argc, char* argv[])
{
  unsetenv("DISPLAY");

  scoped_ptr<speech_server> multispeech;
  AutoSystem audio(false);
  int efd = dup(STDERR_FILENO);

  try
    {
      configuration conf(argc, argv);

      if (speech_server::verbose)
        cerr << "Initializing audio system..." << endl;
      audio.initialize();
      if (speech_server::verbose)
        cerr << "Audio system initialization complete." << endl;
      multispeech.reset(new frontend(conf));
    }
  catch (const string& info)
    {
      cout << info << endl;
      if (efd >= 0)
        close(efd);
      return EXIT_SUCCESS;
    }
  catch (const configuration::error& error)
    {
      speech_server::log << SyslogStream::err << error.what() << endl;
      if (efd >= 0)
        {
          dup2(efd, STDERR_FILENO);
          close(efd);
        }
      string msg("ERROR: ");
      msg += error.what();
      cerr << msg << endl;
      return EXIT_FAILURE;
    }
  catch (const std::exception& error)
    {
      speech_server::log << SyslogStream::err << error.what() << configuration::stage << endl;
      if (efd >= 0)
        {
          dup2(efd, STDERR_FILENO);
          close(efd);
        }
      cerr << "ERROR" << configuration::stage << ": " << error.what() << endl;
      return EXIT_FAILURE;
    }

  if (efd >= 0)
    {
      close(efd);
      efd = -1;
    }

  int exit_status = multispeech->run();

  return exit_status;
}
