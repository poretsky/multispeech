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

#include <cstdlib>
#include <memory>
#include <exception>
#include <string>
#include <iostream>

#include <bobcat/syslogstream>

#include <portaudiocpp/PortAudioCpp.hxx>

#include "config.hpp"
#include "server.hpp"
#include "historical.hpp"
#include "multispeech_voices.hpp"

using namespace std;
using namespace FBB;
using namespace portaudio;

int main(int argc, char* argv[])
{
  unsetenv("DISPLAY");
  auto_ptr<server> multispeech;
  AutoSystem audio;

  try
    {
      configuration conf(argc, argv);
      if (conf.option_value.count(options::frontend::interface))
        {
          if (conf.option_value[options::frontend::interface].as<string>()
              == "historical")
            multispeech.reset(new multispeech_historical(conf, new multispeech_voices));
          else throw configuration::error("unknown interface type " +
                                         conf.option_value[options::frontend::interface].as<string>());
        }
      else multispeech.reset(new multispeech_historical(conf, new multispeech_voices));
    }
  catch (const string& info)
    {
      cout << info << endl;
      return EXIT_SUCCESS;
    }
  catch (const configuration::error& error)
    {
      server::log << SyslogStream::err << error.what() << endl;
      if (server::verbose)
        cerr << "Configuration error: " << error.what() << endl;
      return EXIT_FAILURE;
    }
  catch (const exception& error)
    {
      server::log << SyslogStream::err << error.what() << configuration::stage << endl;
      if (server::verbose)
        cerr << "Error" << configuration::stage << ": " << error.what() << endl;
      return EXIT_FAILURE;
    }

  multispeech->run();

  return EXIT_SUCCESS;
}
