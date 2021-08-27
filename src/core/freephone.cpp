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

#include <iostream>

#include <boost/filesystem.hpp>

#include <bobcat/syslogstream>

#include "freephone.hpp"

#include "config.hpp"
#include "speech_server.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace FBB;


// Static data:
const string freephone::name("freephone");
const string freephone::lexicon_default_path(complete("freespeech/enlex.db", package::datadir).generic_string());
string freephone::executable(freephone::name);
string freephone::lexicon(freephone::lexicon_default_path);

static const string mbrola_en1("en1");


// Object construction:

freephone::freephone(void):
  mbrola(name, mbrola_en1, lang_id::en)
{
  if (!executable.empty())
    {
      string cmd(executable);
      if (!lexicon.empty())
        {
          path lexicon(freephone::lexicon);
          if (exists(lexicon))
            cmd += " -h " + lexicon.generic_string();
          else
            {
              speech_server::log << SyslogStream::warning << lexicon.generic_string() << " does not exist" << endl;
              if (speech_server::verbose)
                cerr << "Warning: " << lexicon.generic_string() << " does not exist" << endl;
            }
        }
      cmd += " -i - -o -";
      command(cmd);
    }
  else throw configuration::error("no path to " + name);
}
