// speech_server.cpp -- General speech server functionality implementation
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <exception>

#include "speech_server.hpp"

#include "iconv_codecvt.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;
using namespace FBB;


// Open logging stream:
SyslogStream speech_server::log(package::name, NOTICE, USER, LOG_PID);
bool speech_server::verbose = false;
bool speech_server::debug = false;


// Construct / destroy:

speech_server::speech_server(const configuration& conf):
  input_charset((conf.option_value.count(options::frontend::charset) &&
                 !conf.option_value[options::frontend::charset].as<string>().empty()) ?
                locale(locale(""), new iconv_codecvt(conf.option_value[options::frontend::charset].as<string>().c_str(), NULL)) :
                locale("")),
  speechmaster(conf),
  soundmaster(this),
  exit_status(EXIT_SUCCESS)
{
}

speech_server::~speech_server(void)
{
}


// Protected methods:

void
speech_server::communication_reset(void)
{
  cmd.erase();
  accumulator.str("");
}

bool
speech_server::do_nothing(void)
{
  return true;
}


// Public methods:

int
speech_server::run(void)
{
  do
    try
      {
        get_command();
      }
    catch (std::exception& failure)
      {
        communication_reset();
        log << failure.what() << endl;
        if (verbose)
          cerr << failure.what() << endl;
      }
  while (perform_command());
  soundmaster.stop();
  return exit_status;
}

bool
speech_server::redirect_stderr(const char* file)
{
  int fd = file ?
    open(file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR) :
    open("/dev/null", O_WRONLY);
  if (fd >= 0)
    {
      dup2(fd, STDERR_FILENO);
      close(fd);
      return true;
    }
  return false;
}


// Private methods:

void
speech_server::index_mark(const string& name)
{
}

void
speech_server::queue_done(void)
{
}
