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

#include <boost/filesystem/fstream.hpp>

#include "speech_server.hpp"

#include "iconv_codecvt.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace FBB;


// Configured input charset:
string speech_server::frontend_charset;

// Open logging stream:
SyslogStream speech_server::log(package::name, NOTICE, USER, LOG_PID);
bool speech_server::verbose = false;
bool speech_server::debug = false;

// Whether to accept embedded voice switching commands.
bool speech_server::support_native_voices = true;
bool speech_server::support_dtk_voices = false;

// Speech dispatcher backend related data:
const string speech_server::spd_sound_icons_default_path(complete("sounds/sound-icons", DATA_DIR).generic_string());
string speech_server::spd_version;
string speech_server::spd_sound_icons(speech_server::spd_sound_icons_default_path);
bool speech_server::spd_use_voice_language = true;
bool speech_server::spd_accept_explicit_language = true;
bool speech_server::spd_ignore_unknown_voice = false;
bool speech_server::spd_support_index_marks = true;


// Construct / destroy:

speech_server::speech_server(void):
  input_charset(frontend_charset.empty() ?
                locale("") :
                locale(locale(""), new iconv_codecvt(frontend_charset.c_str(), NULL))),
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
