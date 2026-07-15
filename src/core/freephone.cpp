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
const char* const freephone::name = FREEPHONE;
string freephone::executable(freephone::name);
string freephone::lexicon(lexicon_path::search_default);

static const string mbrola_en1("en1");
static const path enlex_db("freespeech/enlex.db");


// Internal routines:

static bool
utilize_lexicon(path& lexicon, string& cmd)
{
  if (exists(lexicon))
    {
      cmd += " -h " + lexicon.generic_string();
      return true;
    }
  else if (exists(lexicon.replace_extension("dir")) && exists(lexicon.replace_extension("pag")))
    {
      cmd += " -h " + lexicon.replace_extension().generic_string();
      return true;
    }
  return false;
}

static bool
search_lexicon(const path& base_path, string& cmd)
{
  path lexicon(absolute(enlex_db, base_path));
  return utilize_lexicon(lexicon, cmd);
}


// Object construction:

freephone::freephone(void):
  mbrola(name, mbrola_en1, lang_id::en)
{
  if (!executable.empty())
    {
      string cmd(executable);
      if (lexicon == lexicon_path::search_default)
        {
          if (!(search_lexicon(LIB_DIR, cmd) || search_lexicon(DATA_DIR, cmd)))
            {
              speech_server::log << SyslogStream::warning << name << " lexicon is not found on its conventional places" << endl;
              if (speech_server::verbose)
                cerr << "Warning: " << name << " lexicon is not found on its conventional places" << endl;
            }
        }
      else if ((!lexicon.empty()) && (lexicon != lexicon_path::none))
        {
          path lexicon(freephone::lexicon);
          if (!utilize_lexicon(lexicon, cmd))
            {
              speech_server::log << SyslogStream::warning << lexicon.generic_string() << " does not exist" << endl;
              if (speech_server::verbose)
                cerr << "Warning: " << lexicon.generic_string() << " does not exist" << endl;
            }
        }
      cmd += " -i - -o -";
      command(cmd);
    }
  else throw configuration::error("no path to " + string(name));
}
