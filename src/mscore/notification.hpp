// notification.hpp -- Notification service framework
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

// Class notification represents general framework for sound_manager
// notification service. Being constructed, it connects to the supplied
// instance of sound_manager and runs separate thread of execution
// to take care about notifications queue. Derived class must provide
// actual implementation for pure virtual method consume() that will
// be called for each event of interest coming from the sound_manager.

#ifndef MULTISPEECH_NOTIFICATION_HPP
#define MULTISPEECH_NOTIFICATION_HPP

#include <queue>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "job.hpp"

namespace multispeech
{

class notification
{
public:
  // These values represent various events that can be reported.
  // It may be combined by "or" to form notification mode
  // in the constructor.
  enum job_event
  {
    job_start = 0x01,
    job_complete = 0x02,
    job_cancel = 0x04,
    job_pause = 0x08,
    job_resume = 0x10
  };

  // Job event notification message structure.
  struct message
  {
    job_event status;
    unsigned long id;
    unsigned long owner;
  };

protected:
  // Construct / destroy.
  explicit notification(class sound_manager* master);
  virtual ~notification(void);

private:
  // The functor to be executed in a separate thread.
  class agent
  {
  public:
    // Object constructor.
    explicit agent(notification* queue_holder);

    // The thread execution loop.
    void operator()(void);

  private:
    // Notification queue holder.
    notification* holder;
  };

  friend class sound_manager;
  friend class agent;

  // Notifications queue.
  std::queue<message> reports;

  // Life indicator.
  bool alive;

  // Master link.
  notification*& link;

  // New data arriving events.
  boost::condition event;

  // Access lock means are shared with master.
  boost::mutex& access;

  // Thread handler.
  boost::thread service;

  // Event consumer callback. Must be provided by derived class.
  virtual void consume(const message& report) = 0;

  // Submit an event. It is only for use by master.
  void submit(job_event status, unsigned long id, unsigned long owner);

  // Stop notification thread.
  void die(void);
};

} // namespace multispeech

#endif
