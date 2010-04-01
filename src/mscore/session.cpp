// session.cpp -- General Multispeech session framework implementation
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

#include "session.hpp"


namespace multispeech
{

using namespace std;


// Object constructor:

session::session(istream& client):
  source(client)
{
}


// Session loop:

void
session::run(void)
{
  string request;
  while(input(request) && perform(request))
    continue;
}


// Simple source reading method:

bool
session::input(string& request)
{
  getline(source, request);
  return !(source.eof() || source.fail());
}

} // namespace multispeech
