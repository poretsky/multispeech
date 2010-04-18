// client_info.cpp -- SSIP client info keeper implementation
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include "client_info.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;


// Client name string pattern:
const regex client_info::name_pattern("^\"?([[:word:]-]+):([[:word:]-]+):([[:word:]-]+)\"?\\s*$");


// Main constructor:

client_info::client_info(void):
  user_name("unknown"),
  application_name("unknown"),
  component_name("unknown"),
  unknown(true)
{
}


// Public methods:

message::code
client_info::name(const string& full_name)
{
  message::code rc = message::OK_CLIENT_NAME_SET;
  if (unknown)
    {
      smatch split;
      if (regex_match(full_name, split, name_pattern))
        {
          if (split[1].matched)
            user_name = string(split[1].first, split[1].second);
          if (split[2].matched)
            application_name = string(split[2].first, split[2].second);
          if (split[3].matched)
            component_name = string(split[3].first, split[3].second);
          unknown = false;
        }
      else rc = message::ERR_PARAMETER_INVALID;
    }
  else rc = message::ERR_COULDNT_SET_CLIENT_NAME;
  return rc;
}

string
client_info::name(void) const
{
  return user_name + ':' + application_name + ':' + component_name;
}

const string&
client_info::user(void) const
{
  return user_name;
}

const string&
client_info::application(void) const
{
  return application_name;
}

const string&
client_info::component(void) const
{
  return component_name;
}

} // namespace SSIP
