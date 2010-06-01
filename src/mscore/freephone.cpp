// freephone.cpp -- Freephone speech backend implementation
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

#include <sysconfig.h>

#include <iostream>

#include <boost/filesystem.hpp>

#include <bobcat/syslogstream>

#include "freephone.hpp"

#include "server.hpp"
#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace FBB;
using namespace boost::filesystem;


// Instantiation mechanism:
singleton<freephone> freephone::instance;


// Object construction:

freephone::freephone(void):
  mbrola(speaker::freephone)
{
  string cmd(configuration::backend_executable(name));
  if (!cmd.empty())
    {
      if (!configuration::backend_lexicon(name).empty())
        {
          path lexicon(configuration::backend_lexicon(name));
          if (exists(lexicon))
            cmd += " -h " + lexicon.file_string();
          else
            {
              server::log << SyslogStream::warning << lexicon.file_string() << " does not exist" << endl;
              if (server::verbose)
                cerr << "Warning: " << lexicon.file_string() << " does not exist" << endl;
            }
        }
      cmd += " -i - -o -";
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
  voices[en1] = &en1;
  if (voice_available(en1))
    voice_setup(en1);
  else throw configuration::error("no voice for " + name);
}

} // namespace multispeech
