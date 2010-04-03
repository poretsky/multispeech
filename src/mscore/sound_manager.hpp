// sound_manager.hpp -- Sound streams producing and playing manager interface
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

// The sound_manager class combines all sound producing activities
// in one consistent mechanism. It takes care of proper sound events
// dispatching and provides all necessary control means to maintain
// and manage sound producing tasks queue as well as the ones
// to execute such tasks immediately.
// This class is aimed to be one of the server ancestors.
// It should not be instanced multiple times.

#ifndef MULTISPEECH_SOUND_MANAGER_HPP
#define MULTISPEECH_SOUND_MANAGER_HPP

#include <memory>
#include <list>

#include <portaudiocpp/PortAudioCpp.hxx>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "config.hpp"
#include "file_player.hpp"
#include "tone_generator.hpp"
#include "loudspeaker.hpp"
#include "job.hpp"
#include "notification.hpp"

namespace multispeech
{

class sound_manager:
  private portaudio::AutoSystem
{
public:
  // Job states in the queue.
  enum job_state
  {
    unknown,
    waiting,
    progressing
  };

protected:
  // Constructing / destroying.
  explicit sound_manager(const configuration* conf);
  ~sound_manager(void);

public:
  // Submit a job to the queue according to it's priority.
  // If some job is executing already at the time of submission,
  // this one will be added to the running queue and so will be
  // executed automatically when the time come.
  // Otherwise actual execution should be started explicitly.
  // When the dominate flag is specified and new job has enough
  // priority, it can interrupt and postpone currently running one.
  // By default running job is not affected by new insertions.
  void enqueue(const job& unit, bool dominate = false);

  // Cancel job by it's id. It is absolutely no harm to specify
  // non-existent id here. There will nothing be done in that case.
  void cancel(unsigned long id);

  // Jobs selection by urgency. All jobs with urgency attribute
  // less than specified value will be deleted from the queue.
  void select(int urgency);

  // Return current job state for specified id.
  job_state query(unsigned long id);

  // Execute specified task immediately. Other playing sounds
  // may be stopped depending on the asynchronous options.
  void execute(const sound_task& task);
  void execute(const tone_task& task);
  void execute(const speech_task& task);

  // Execute queued jobs one by one until queue exhaustion.
  // No effect if some job is executing already.
  void proceed(void);

  // Stop all current sounds if any and backup the jobs queue.
  void suspend(void);

  // Stop all current sounds if any and Resume execution
  // of the backed up queue.
  void resume(void);

  // Stop all playing sounds if any and clear queue.
  void stop(void);

  // Return number of items in current jobs queue.
  // Note that a job is removed from the queue only after completion.
  unsigned int capacity(void);

  // Return true if any sound is currently playing.
  bool active(void);

private:
  // Jobs queue container.
  typedef std::list<job> jobs_queue;

  // Thread states:
  enum status
  {
    idle, // No job is executing quite now.
    running, // Actually executing jobs from the queue.
    dead // Shutting down the thread.
  };

  // Activity types:
  enum activity
  {
    nothing,
    playing,
    beeping,
    speaking
  };

  // The functor to be executed in a separate thread.
  class agent
  {
  public:
    // Object constructor.
    explicit agent(sound_manager* job_holder);

    // The thread execution loop.
    void operator()(void);

  private:
    // the parent class pointer.
    sound_manager* holder;
  };

  friend class agent;
  friend class notification;

  // Child thread state.
  status state;

  // Current business in the sense of queue proceeding.
  activity business;

  // Job queues.
  std::auto_ptr<jobs_queue> jobs, backup;

  // Sound streams.
  file_player sounds;
  tone_generator tones;
  loudspeaker speech;

  // Notification service pointer.
  notification* feedback;

  // Critical data access control means.
  boost::mutex access;
  boost::condition event;

  // Thread handler.
  boost::thread service;

  // Internal routines:
  void notify(notification::job_event status, unsigned long id, unsigned long owner);
  void mute(void); // Mute all playing sounds if any.
  void die(void); // Make thread to break execution loop.
  void next_job(void); // Get and start the next job from the queue.
  bool working(void); // Return true if a job is in progress.
};

} // namespace multispeech

#endif
