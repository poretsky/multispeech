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

#include "English.hpp"
#include "server.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace FBB;


// Object construction:

freephone::freephone(const configuration& conf):
  mbrola(conf, speaker::freephone, "en1", new English, 16000)
{
  if (conf.option_value.count(options::compose(name, option_name::executable)) &&
      !conf.option_value[options::compose(name, option_name::executable)].as<string>().empty())
    {
      string cmd(conf.option_value[options::compose(name, option_name::executable)].as<string>());
      if (conf.option_value.count(options::compose(name, option_name::lexicon)) &&
          !conf.option_value[options::compose(name, option_name::lexicon)].as<string>().empty())
        {
          path lexicon(conf.option_value[options::compose(name, option_name::lexicon)].as<string>());
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
}
