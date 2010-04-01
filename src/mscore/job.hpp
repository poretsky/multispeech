// job.hpp -- General job unit definition
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

// This class provides general job unit abstraction suitable
// for use in prioritised queue container. It serves as a wrapper
// around such classes as speech_task, sound_task and tone_task
// providing some common properties by the way.

#ifndef MULTISPEECH_TASK_HPP
#define MULTISPEECH_TASK_HPP

#include <boost/any.hpp>

namespace multispeech
{

class job: public boost::any
{
public:
  // Type used for job id:
  typedef unsigned long long id_type;

  // Default constructor:
  job(void);

  // Construct job for specified task description:
  template <typename task_description>
  explicit job(const task_description& task):
    boost::any(task),
    active(false),
    owner(0),
    urgency(0),
    unit_id(++count)
  {
  }

  // Public attributes:
  bool active;
  unsigned int owner;
  int urgency;

  // Id accessor:
  id_type id(void) const;

  // Priority ordering operator. It is assumed that empty job
  // always has greater priority than non-empty one.
  // Otherwise urgency attribute comes into account.
  bool operator<(const job& other) const;

private:
  // Unit identification:
  static id_type count;
  id_type unit_id;
};

} // namespace multispeech

#endif