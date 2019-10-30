// server.cpp -- General speech server functionality implementation
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

#include <iostream>
#include <exception>

#include "server.hpp"

#include "iconv_codecvt.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;
using namespace FBB;


// Open logging stream:
SyslogStream server::log(package::name, NOTICE, USER, LOG_PID);
bool server::verbose = false;
bool server::debug = false;


// Construct / destroy:

server::server(const configuration& conf):
  input_charset((conf.option_value.count(options::frontend::charset) &&
                 !conf.option_value[options::frontend::charset].as<string>().empty()) ?
                locale(locale(""), new iconv_codecvt(conf.option_value[options::frontend::charset].as<string>().c_str(), NULL)) :
                locale("")),
  speechmaster(conf),
  soundmaster(conf)
{
}

server::~server(void)
{
}


// Public methods:

void
server::run(void)
{
  do
    try
      {
        get_command();
      }
    catch (std::exception& failure)
      {
        cmd.erase();
        data.erase();
        log << failure.what() << endl;
        if (verbose)
          cerr << failure.what() << endl;
      }
  while (perform_command());
  soundmaster.stop();
}
