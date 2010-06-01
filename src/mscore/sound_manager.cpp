// sound_manager.cpp -- Sound streams producing and playing manager implementation
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

#include <boost/thread/thread.hpp>

#include "sound_manager.hpp"

#include "audioplayer.hpp"
#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;


// Constructing / destroying:

sound_manager::sound_manager(void):
  state(none),
  business(nothing),
  jobs(new jobs_queue),
  notifying(false),
  notifications(false)
{
}

sound_manager::~sound_manager(void)
{
  stop();
  die();
}


// Public methods:

void
sound_manager::enqueue(const job&unit, bool dominate, bool update)
{
  mutex::scoped_lock lock(access);
  jobs_queue::iterator position = jobs->end();
  for (jobs_queue::reverse_iterator predecessor = jobs->rbegin();
       predecessor != jobs->rend();
       ++predecessor, --position)
    if (!(*predecessor < unit))
      break;
  if (position == jobs->end())
    {
      jobs->push_back(unit);
      position = --(jobs->end());
    }
  else if (position == jobs->begin())
    {
      if (!position->over())
        {
          if (position->state() == job::active)
            {
              if (dominate)
                {
                  mute();
                  position->postpone();
                  notify(job::paused, *position);
                  position = jobs->insert(position, job());
                }
              ++position;
            }
        }
      else ++position;
      if (position == jobs->end())
        {
          jobs->push_back(unit);
          position = --(jobs->end());
        }
      else position = jobs->insert(position, unit);
    }
  else position = jobs->insert(position, unit);
  if (update)
    while (position != jobs->begin())
      if ((--position)->urgency() == unit.urgency())
        position->obsolete();
      else break;
}

void
sound_manager::discard(const job& unit)
{
  mutex::scoped_lock lock(access);
  notify(job::cancelled, unit);
}

void
sound_manager::abort(unsigned long owner)
{
  mutex::scoped_lock lock(access);
  if ((state == running) && !jobs->empty() &&
      (!owner || (jobs->front().owner() == owner)))
    {
      mute();
      jobs->front().kill();
    }
}

void
sound_manager::cancel(unsigned long key, bool by_owner)
{
  mutex::scoped_lock lock(access);
  jobs_queue::iterator position = jobs->begin();
  while (position != jobs->end())
    if (!key || ((by_owner ? position->owner() : position->id()) == key))
      {
        if (position->state() == job::active)
          {
            mute();
            position->kill();
            ++position;
          }
        else if ((position != jobs->begin()) || !position->over())
          {
            notify(job::cancelled, *position);
            position = jobs->erase(position);
          }
        if (key && !by_owner)
          break;
      }
    else ++position;
}

void
sound_manager::reject(int urgency, bool exact)
{
  mutex::scoped_lock lock(access);
  for (jobs_queue::iterator position = jobs->begin(); position != jobs->end(); ++position)
    if ((position->urgency() == urgency) ||
        (!exact && (position->urgency() < urgency)))
      {
        if (position->state() == job::active)
          {
            mute();
            position->kill();
          }
        else if ((position != jobs->begin()) || !position->over())
          {
            notify(job::cancelled, *position);
            position = --(jobs->erase(position));
          }
      }
}

job::status
sound_manager::query(unsigned long id)
{
  jobs_queue::iterator position;
  mutex::scoped_lock lock(access);
  for (position = jobs->begin(); position != jobs->end(); ++position)
    if (position->id() == id)
      break;
  return (position != jobs->end()) ? position->state() : job::unknown;
}

sound_manager::jobs_queue*
sound_manager::recall(unsigned long owner)
{
  jobs_queue* recalled_jobs = new jobs_queue;
  mutex::scoped_lock lock(access);
  jobs_queue::iterator position = jobs->begin();
  while (position != jobs->end())
    if ((position->owner() == owner) &&
        ((position != jobs->begin()) || !position->over()))
      {
        recalled_jobs->push_back(*position);
        if (position->state() == job::active)
          {
            mute();
            position->kill();
            recalled_jobs->back().postpone();
            notify(job::paused, recalled_jobs->back());
            ++position;
          }
        else position = jobs->erase(position);
      }
    else ++position;
  return recalled_jobs;
}

void
sound_manager::execute(const sound_task& task)
{
  mutex::scoped_lock lock(access);
  if (!configuration::sound_async())
    {
      if ((state == running) && !jobs->empty())
        business = playing;
      if (!configuration::beep_async())
        tones.stop();
      speech.stop();
    }
  else if (business == playing)
    business = nothing;
  sounds.start(task, true);
}

void
sound_manager::execute(const tone_task& task)
{
  mutex::scoped_lock lock(access);
  if (!configuration::beep_async())
    {
      if ((state == running) && !jobs->empty())
        business = beeping;
      if (!configuration::sound_async())
        sounds.stop();
      speech.stop();
    }
  else if (business == beeping)
    business = nothing;
  tones.start(task, true);
}

void
sound_manager::execute(const speech_task& task)
{
  mutex::scoped_lock lock(access);
  if ((state == running) && !jobs->empty())
    business = speaking;
  if (!configuration::sound_async())
    sounds.stop();
  if (!configuration::beep_async())
    tones.stop();
  speech.stop();
  speech.start(task);
}

void
sound_manager::proceed(void)
{
  {
    mutex::scoped_lock lock(access);
    if (state == none)
      {
        state = idle;
        thread(agent(this));
      }
    if ((state == idle) && !jobs->empty())
      {
        state = running;
        event.notify_one();
      }
  }
  thread::yield();
}

void
sound_manager::pause(void)
{
  mutex::scoped_lock lock(access);
  if (state == running)
    {
      mute();
      state = idle;
      if (!jobs->empty() && (jobs->front().state() == job::active))
        {
          jobs->front().postpone();
          notify(job::paused, jobs->front());
        }
    }
}

void
sound_manager::suspend(void)
{
  mutex::scoped_lock lock(access);
  mute();
  backup = jobs;
  jobs.reset(new jobs_queue);
  if ((state == running) && !backup->empty())
    {
      backup->front().postpone();
      notify(job::paused, backup->front());
      jobs->push_front(job());
    }
}

void
sound_manager::resume(void)
{
  if (backup.get() && !backup->empty())
    {
      mutex::scoped_lock lock(access);
      mute();
      jobs = backup;
      switch (state)
        {
        case idle:
          state = running;
          event.notify_one();
          break;
        case running:
          jobs->push_front(job());
          break;
        case none:
          state = running;
          thread(agent(this));
        default:
          break;
        }
    }
  else stop();
  thread::yield();
}

void
sound_manager::stop(void)
{
  mutex::scoped_lock lock(access);
  mute();
  jobs->clear();
  if (state == running)
    jobs->push_front(job());
}

unsigned int
sound_manager::capacity(void)
{
  mutex::scoped_lock lock(access);
  return jobs->size();
}

bool
sound_manager::active(void)
{
  return sounds.active() || tones.active() || speech.active();
}


// Private methods:

void
sound_manager::feedback(job::event event, unsigned long id, unsigned long owner)
{
}

void
sound_manager::notify(job::event event, const job& unit)
{
  if (event & unit.notification_mode())
    {
      event_info message;
      if (!notifying)
        {
          notifications = true;
          thread(reporter(this));
        }
      message.event = event;
      message.id = unit.id();
      message.owner = unit.owner();
      reports.push(message);
      report.notify_one();
    }
}

void
sound_manager::mute(void)
{
  sounds.stop();
  tones.stop();
  speech.stop();
}

void
sound_manager::die(void)
{
  mute();
  if (notifying || (state != none))
    {
      mutex::scoped_lock lock(access);
      state = dead;
      notifications = false;
      event.notify_one();
      report.notify_one();
      while (notifying || (state != none))
        complete.wait(lock);
    }
}

void
sound_manager::next(void)
{
  if ((state == running) && !jobs->empty())
    {
      if (jobs->front().id() && (jobs->front().state() != job::active))
        {
          notify((jobs->front().state() != job::postponed) ?
                 job::started :
                 job::resumed,
                 jobs->front());
          jobs->front().activate();
        }
      if (jobs->front().empty())
        business = nothing;
      else if (jobs->front().item_type() == typeid(sound_task))
        {
          if (!configuration::sound_async())
            {
              if (!configuration::beep_async())
                tones.stop();
              speech.stop();
            }
          sounds.start(jobs->front().item<sound_task>(),
                       !configuration::sound_async() || (business != playing));
          business = playing;
        }
      else if (jobs->front().item_type() == typeid(tone_task))
        {
          if (!configuration::beep_async())
            {
              if (!configuration::sound_async())
                sounds.stop();
              speech.stop();
            }
          tones.start(jobs->front().item<tone_task>(),
                      !configuration::beep_async() || (business != beeping));
          business = beeping;
        }
      else if (jobs->front().item_type() == typeid(speech_task))
        {
          if (!configuration::sound_async())
            sounds.stop();
          if (!configuration::beep_async())
            tones.stop();
          speech.stop();
          speech.start(jobs->front().item<speech_task>());
          business = speaking;
        }
      else business = nothing;
    }
}

bool
sound_manager::working(void)
{
  bool result = false;
  if (state == running)
    {
      if (jobs->empty())
        {
          state = idle;
          business = nothing;
        }
      else switch (business)
        {
        case playing:
          if (configuration::sound_async() || !sounds.active())
            shift();
          else result = true;
          break;
        case beeping:
          if (configuration::beep_async() || !tones.active())
            shift();
          else result = true;
          break;
        case speaking:
          if (!speech.active())
            shift();
          else result = true;
          break;
        default:
          shift();
          break;
        }
      if (jobs->empty())
        {
          state = idle;
          business = nothing;
        }
    }
  return result;
}

void
sound_manager::shift(void)
{
  if (jobs->front().state() == job::active)
    {
      if (jobs->front().shift())
        {
          notify(job::complete, jobs->front());
          jobs->pop_front();
        }
    }
  else
    {
      if (jobs->front().id() && (jobs->front().state() == job::idle))
        notify(job::cancelled, jobs->front());
      jobs->pop_front();
    }
}


// Agent subclass members:

sound_manager::agent::agent(sound_manager* master):
  host(master)
{
}

void
sound_manager::agent::operator()(void)
{
  while (host->state != none)
    {
      mutex::scoped_lock lock(host->access);
      while (host->state == idle)
        host->event.wait(lock);
      switch (host->state)
        {
        case running:
          host->next();
          while (host->working())
            audioplayer::complete.wait(lock);
          break;
        case dead:
          host->state = none;
          host->complete.notify_one();
        default:
          break;
        }
    }
}


// Reporter subclass members:

sound_manager::reporter::reporter(sound_manager* master):
  host(master)
{
}

void
sound_manager::reporter::operator()(void)
{
  host->notifying = true;
  while (host->notifying)
    {
      event_info message = { job::all_events, 0, 0 };
      {
        mutex::scoped_lock lock(host->access);
        while (host->notifications && host->reports.empty())
          host->report.wait(lock);
        if (host->notifications)
          {
            message = host->reports.front();
            host->reports.pop();
          }
        else
          {
            host->notifying = false;
            host->complete.notify_one();
          }
      }
      if (host->notifying)
        host->feedback(message.event, message.id, message.owner);
    }
}

} // namespace multispeech
