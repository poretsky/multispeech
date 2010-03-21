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

#include <exception>
#include <string>
#include <iostream>

#include <bobcat/syslogstream>

#include "config.hpp"
#include "server.hpp"
#include "frontend.hpp"

using namespace std;
using namespace FBB;

int main(int argc, char* argv[])
try
{
  frontend multispeech(argc, argv);

  multispeech.run();

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
