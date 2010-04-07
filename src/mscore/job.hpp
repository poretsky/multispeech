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
  // Job life cycle states:
  enum status
  {
    idle,
    active,
    postponed,
    unknown
  };

  // Default constructor:
  job(void);

  // Construct job for specified task description:
  template <typename task_description>
  job(const task_description& task, unsigned long owner = 0,
      int urgency = 0, unsigned int notification_mode = 0):
    boost::any(task),
    unit_id(++count),
    owner_id(owner),
    urgency_level(urgency),
    event_mask(notification_mode),
    current_state(idle)
  {
  }

  // Attribute accessors:
  unsigned long id(void) const;
  unsigned long owner(void) const;
  int urgency(void) const;
  unsigned int notification_mode(void) const;
  status state(void) const;

  // Life cycle control:
  void activate(void);
  void postpone(void);
  void kill(void);

  // Priority ordering operator. It is assumed that empty job
  // always has greater priority than non-empty one.
  // Otherwise urgency attribute comes into account.
  bool operator<(const job& other) const;

private:
  // Job attributes:
  unsigned long unit_id, owner_id;
  int urgency_level;
  unsigned int event_mask;

  // Job state:
  status current_state;

  // Unit identification generator:
  static unsigned long count;
};

} // namespace multispeech

#endif
