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
// It also allows to pack several tasks in one job.

#ifndef MULTISPEECH_TASK_HPP
#define MULTISPEECH_TASK_HPP

#include <vector>

#include <boost/any.hpp>

namespace multispeech
{

class job
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

  // Construct empty job for the sake of specified owner:
  explicit job(unsigned long owner, int urgency = 0,
               unsigned int notification_mode = 0);

  // Construct anonymous job for a single task:
  template <typename task_description>
  explicit job(const task_description& task, int urgency = 0,
               unsigned int notification_mode = 0):
    unit_id(++count),
    owner_id(0),
    urgency_level(urgency),
    event_mask(notification_mode),
    current_state(idle),
    index(0)
  {
    prescription.push_back(boost::any(task));
  }

  // Attribute accessors:
  unsigned long id(void) const;
  unsigned long owner(void) const;
  int urgency(void) const;
  unsigned int notification_mode(void) const;
  status state(void) const;
  size_t size(void) const;
  bool empty(void) const;

  // Life cycle control:
  void activate(void);
  void postpone(void);
  void kill(void);

  // Step forward through prescriptions list and indicate
  // the end of list by returning true:
  bool shift(void);

  // Add a task to the prescriptions list:
  template <typename task_description>
  job& operator<<(const task_description& task)
  {
    prescription.push_back(boost::any(task));
    return *this;
  }

  // Get reference to the current item from the prescriptions:
  template <typename task_description>
  task_description item(void) const
  {
    return boost::any_cast<task_description>(prescription[index]);
  }

  // Get type of current prescription item:
  const std::type_info& item_type(void) const;

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

  // Tasks package:
  std::vector<boost::any> prescription;
  size_t index;

  // Unit identification generator:
  static unsigned long count;
};

} // namespace multispeech

#endif
