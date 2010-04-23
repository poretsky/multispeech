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

#include <typeinfo>

#include "job.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;

// General job counter initialization:
unsigned long job::count = 0;


// Constructors:

job::job(void):
  unit_id(0),
  owner_id(0),
  urgency_level(0),
  event_mask(0),
  rewind_context(0),
  current_state(idle),
  index(0)
{
}

job::job(unsigned long owner, int urgency,
         unsigned int notification_mode,
         size_t pause_context_size):
  unit_id(++count),
  owner_id(owner),
  urgency_level(urgency),
  event_mask(notification_mode),
  rewind_context(pause_context_size),
  current_state(idle),
  index(0)
{
}


// Public methods:

unsigned long
job::id(void) const
{
  return unit_id;
}

unsigned long
job::owner(void) const
{
  return owner_id;
}

int
job::urgency(void) const
{
  return urgency_level;
}

unsigned int
job::notification_mode(void) const
{
  return event_mask;
}

job::status
job::state(void) const
{
  return current_state;
}

size_t
job::size(void) const
{
  return prescription.size();
}

bool
job::empty(void) const
{
  return prescription.empty();
}

bool
job::over(void) const
{
  return index >= prescription.size();
}

void
job::activate(void)
{
  current_state = active;
}

void
job::postpone(void)
{
  current_state = postponed;
  if (index > rewind_context)
    index -= rewind_context;
  else index = 0;
}

void
job::kill(void)
{
  index = prescription.size();
  current_state = idle;
}

void
job::obsolete(void)
{
  urgency_level = 0;
}

bool
job::shift(void)
{
  return (++index) >= prescription.size();
}

const type_info&
job::item_type(void) const
{
  return (index < prescription.size()) ?
    prescription[index].type() :
    typeid(void);
}

void
job::clear(void)
{
  prescription.clear();
  index = 0;
}

bool
job::operator<(const job& other) const
{
  return urgency_level < other.urgency_level;
}

} // namespace multispeech
