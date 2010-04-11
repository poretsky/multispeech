// exec_queue.hpp -- Execution queue framework
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

// The exec_queue class template provides basic execution queue
// functionality for it's descendants. Submitted tasks are executed
// one by one in the order of submission. The actual execution
// control must be provided in the derived classes by implementing
// following methods:
//
// void execute(const task_description& task) -- to start task execution;
// void abort(void) -- to stop currently executed task;
// bool busy(void) -- to indicate execution state.

#ifndef MULTISPEECH_EXEC_QUEUE_HPP
#define MULTISPEECH_EXEC_QUEUE_HPP

#include <queue>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "audioplayer.hpp"

namespace multispeech
{

template <typename task_description>
class exec_queue: private std::queue<task_description>
{
protected:
  // construct / destroy:
  exec_queue(void):
    alive(false),
    needed(false)
  {
  }
  virtual ~exec_queue(void)
  {
    boost::mutex::scoped_lock lock(access);
    clear();
    if (alive)
      {
        needed = false;
        event.notify_one();
        while (alive)
          complete.wait(lock);
      }
  }

public:
  // Run or queue a new task:
  void submit(const task_description& task)
  {
    boost::mutex::scoped_lock lock(access);
    if (!alive)
      {
        needed = true;
        boost::thread(agent(this));
      }
    if (std::queue<task_description>::empty())
      event.notify_one();
    push(task);
  }

  // Stop current task if any and clear the queue:
  void cancel(void)
  {
    boost::mutex::scoped_lock lock(access);
    clear();
    abort();
  }

  // Return true when all the work is done and queue is empty:
  bool done(void)
  {
    boost::mutex::scoped_lock lock(access);
    return std::queue<task_description>::empty() && !busy();
  }

private:
  // The functor to be executed in a separate thread.
  class agent
  {
  public:
    // Object constructor.
    explicit agent(exec_queue<task_description>* owner):
      holder(owner)
    {
    }

    // The thread execution loop.
    void operator()(void)
    {
      holder->alive = true;
      while (holder->alive)
        {
          boost::mutex::scoped_lock lock(holder->access);
          while (holder->needed && holder->empty())
            holder->event.wait(lock);
          while (holder->needed && holder->busy())
            audioplayer::complete.wait(lock);
          if (!holder->empty())
            {
              holder->execute(holder->front());
              holder->pop();
            }
          if (!holder->needed)
            {
              holder->alive = false;
              holder->complete.notify_one();
            }
        }
    }

  private:
    // the parent class pointer.
    exec_queue<task_description>* holder;
  };

  friend class agent;

  // Thread execution control flags.
  bool alive, needed;

  // Critical data access control means.
  boost::mutex access;
  boost::condition event, complete;

  // Clear task queue:
  void clear(void)
  {
    while (!std::queue<task_description>::empty())
      std::queue<task_description>::pop();
  }

  // The following three methods are devoted to a specific task
  // execution control. Actual implementation of these methods
  // must be provided by derived classes.

  // Start execution of specified task:
  virtual void execute(const task_description& task) = 0;

  // Abort currently running task if any:
  virtual void abort(void) = 0;

  // Return true if some task execution is in progress:
  virtual bool busy(void) = 0;
};

} // namespace multispeech

#endif
