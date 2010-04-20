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
#include <queue>

#include <portaudiocpp/PortAudioCpp.hxx>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "config.hpp"
#include "file_player.hpp"
#include "tone_generator.hpp"
#include "loudspeaker.hpp"
#include "job.hpp"

namespace multispeech
{

class sound_manager: private portaudio::AutoSystem
{
protected:
  // Constructing / destroying.
  explicit sound_manager(const configuration* conf);
  ~sound_manager(void);

public:
  // Jobs queue container.
  typedef std::list<job> jobs_queue;

  // Submit a job to the queue according to it's priority.
  // If some job is executing already at the time of submission,
  // this one will be added to the running queue and so will be
  // executed automatically when the time come.
  // Otherwise actual execution should be started explicitly.
  // When the dominate flag is specified and new job has enough
  // priority, it can interrupt and postpone currently running one.
  // By default running job is not affected by new insertions.
  // If update flag is true and newly submitted job has the same
  // priority as the currently running one, then currently running job
  // is obsoleted though not cancelled.
  void enqueue(const job& unit, bool dominate = false, bool update = false);

  // Notify that job was cancelled before going to the queue.
  void discard(const job& unit);

  // Abort currently running job if any and stop sound. Other jobs
  // waiting in active queue will not be affected in any way.
  // This action can be performed either for specified owner
  // or unconditionally for any processing job.
  void abort(unsigned long owner);
  void abort(void);

  // Jobs cancellation. The first variant allows to cancel jobs
  // by id or by owner depending on the selector flag specified
  // as the second argument. It is absolutely no harm to specify
  // non-existent key value. There will nothing be done in that case.
  // The second variant, without arguments,
  // cancels all jobs in active queue if any.
  void cancel(unsigned long key, bool by_owner = false);
  void cancel(void);

  // Jobs rejection by urgency. All jobs with urgency attribute
  // less than or equal to specified value will be cancelled
  // and deleted from the queue. If exact flag is true, then
  // only jobs with specified urgency will be affected.
  void reject(int urgency, bool exact = false);

  // Return current job state for specified id.
  job::status query(unsigned long id);

  // Remove jobs of specified owner from the active queue and return
  // pointer to the newly constructed container populated by these
  // jobs. The order of items is preserved. If any of the affected
  // jobs is active at the moment, it will be paused in usual way.
  // Note that the returned container will be dynamically allocated
  // and it is caller responsibility to delete it
  // when it is not needed anymore.
  jobs_queue* recall(unsigned long owner);

  // Execute specified task immediately. Other playing sounds
  // may be stopped depending on the asynchronous options.
  void execute(const sound_task& task);
  void execute(const tone_task& task);
  void execute(const speech_task& task);

  // Execute queued jobs one by one until queue exhaustion.
  // No effect if some job is executing already.
  void proceed(void);

  // Stop all current sounds, but don't touch jobs queue.
  // Execution can be resumed by the method proceed().
  void pause(void);

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
  // Job event info representation.
  struct event_info
  {
    job::event event;
    unsigned long id;
    unsigned long owner;
  };

  // Thread states:
  enum status
  {
    idle, // No job is executing quite now.
    running, // Actually executing jobs from the queue.
    dead, // Shutting down the thread.
    none // Doesn't exist.
  };

  // Activity types:
  enum activity
  {
    nothing,
    playing,
    beeping,
    speaking
  };

  // Jobs queue execution thread.
  class agent
  {
  public:
    explicit agent(sound_manager* master);
    void operator()(void);
  private:
    sound_manager* host;
  };

  // Events notification thread.
  class reporter
  {
  public:
    explicit reporter(sound_manager* master);
    void operator()(void);
  private:
    sound_manager* host;
  };

  friend class agent;
  friend class reporter;

  // Child thread state.
  status state;

  // Current business in the sense of queue proceeding.
  activity business;

  // Job queues.
  std::auto_ptr<jobs_queue> jobs, backup;

  // Notifications queue.
  std::queue<event_info> reports;

  // Notification thread life indicator.
  bool notifying;

  // Notification service activity flag.
  unsigned int notifications;

  // Sound streams.
  file_player sounds;
  tone_generator tones;
  loudspeaker speech;

  // Critical data access control means.
  boost::mutex access;
  boost::condition event, report, complete;

  // Event consumer callback. Just a dummy here. Actual implementation
  // should be provided by derived classes if necessary.
  virtual void feedback(job::event event, unsigned long id, unsigned long owner);

  // Internal routines:
  void notify(job::event event, const job& unit);
  void mute(void); // Mute all playing sounds if any.
  void die(void); // Make threads to break execution loop.
  void next(void); // Get and start the next job item from the queue.
  bool working(void); // Return true if a job item is in progress.
  void shift(void); // Shift to the next job or item if any.
};

} // namespace multispeech

#endif
