// startup.cpp -- The main entry point
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

#include <sysconfig.h>

#include <exception>
#include <string>
#include <iostream>

#include <bobcat/syslogstream>

#include "server.hpp"
#include "config.hpp"

using namespace std;
using namespace FBB;
using namespace multispeech;


// This procedure should be defined in the main module
// instead of usual main() that is defined here instead.
extern void multispeech_start(int argc, char* argv[]);


int main(int argc, char* argv[])
  try
    {
      multispeech_start(argc, argv);
      return EXIT_SUCCESS;
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