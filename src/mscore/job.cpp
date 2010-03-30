// job.cpp -- General job abstraction implementation
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

#include "job.hpp"


namespace multispeech
{

using namespace boost;

// General job counter initialization:
job::id_type job::count = 0;


// Default constructor:

job::job(void):
  any(),
  active(false),
  owner(0),
  urgency(0),
  unit_id(0)
{
}


// Public methods:

job::id_type
job::id(void) const
{
  return unit_id;
}

bool
job::operator<(const job& other) const
{
  return (this->empty() && !other.empty()) ? false
    : ((other.empty() && !this->empty()) ? true
       : (this->urgency < other.urgency));
}

} // namespace multispeech
