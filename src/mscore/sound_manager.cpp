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

#include "sound_manager.hpp"

#include "audioplayer.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;


// Constructing / destroying:

sound_manager::sound_manager(const configuration* conf):
  state(idle),
  business(nothing),
  jobs(new jobs_queue),
  sounds(conf),
  tones(conf),
  speech(conf),
  feedback(NULL),
  service(agent(this))
{
}

sound_manager::~sound_manager(void)
{
  stop();
  die();
  service.join();
}


// Public methods:

void
sound_manager::enqueue(const job&unit, bool dominate)
{
  mutex::scoped_lock lock(access);
  list<job>::iterator position;
  for (position = jobs->begin(); position != jobs->end(); ++position)
    if ((*position) < unit)
      break;
  if ((position != jobs->end()) &&
      (position->state() == job::active))
    {
      if (dominate)
        {
          mute();
          position->postpone();
          notify(notification::job_pause, *position);
          position = jobs->insert(position, job());
        }
      ++position;
    }
  jobs->insert(position, unit)->activate();
}

void
sound_manager::cancel(unsigned long id)
{
  mutex::scoped_lock lock(access);
  list<job>::iterator position;
  for (position = jobs->begin(); position != jobs->end(); ++position)
    if (position->id() == id)
      break;
  if (position != jobs->end())
    {
      if (position->state() == job::active)
        {
          mute();
          position->kill();
        }
      else
        {
          notify(notification::job_cancel, *position);
          jobs->erase(position);
        }
    }
}

void
sound_manager::select(int urgency)
{
  mutex::scoped_lock lock(access);
  list<job>::iterator position;
  for (position = jobs->begin(); position != jobs->end(); ++position)
    if (position->urgency() < urgency)
      {
        if (position->state() == job::active)
          {
            mute();
            position->kill();
          }
        else
          {
            notify(notification::job_cancel, *position);
            position = --(jobs->erase(position));
          }
      }
}

job::status
sound_manager::query(unsigned long id)
{
  mutex::scoped_lock lock(access);
  list<job>::iterator position;
  for (position = jobs->begin(); position != jobs->end(); ++position)
    if (position->id() == id)
      break;
  return (position != jobs->end()) ? position->state() : job::unknown;
}

void
sound_manager::execute(const sound_task& task)
{
  mutex::scoped_lock lock(access);
  if (!file_player::asynchronous)
    {
      if ((state == running) && !jobs->empty())
        business = playing;
      if (!tone_generator::asynchronous)
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
  if (!tone_generator::asynchronous)
    {
      if ((state == running) && !jobs->empty())
        business = beeping;
      if (!file_player::asynchronous)
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
  if (!file_player::asynchronous)
    sounds.stop();
  if (!tone_generator::asynchronous)
    tones.stop();
  speech.stop();
  speech.start(task);
}

void
sound_manager::proceed(void)
{
  {
    mutex::scoped_lock lock(access);
    if ((state == idle) && !jobs->empty())
      {
        state = running;
        event.notify_one();
      }
  }
  thread::yield();
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
      notify(notification::job_pause, backup->front());
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
sound_manager::notify(notification::job_event status, const job& unit)
{
  if (feedback && (status & unit.notification_mode()))
    feedback->submit(status, unit.id(), unit.owner());
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
  mutex::scoped_lock lock(access);
  mute();
  state = dead;
  event.notify_one();
}

void
sound_manager::next(void)
{
  if ((state == running) && !jobs->empty())
    {
      if (jobs->front().empty())
        business = nothing;
      else if (jobs->front().item_type() == typeid(sound_task))
        {
          if (!file_player::asynchronous)
            {
              if (!tone_generator::asynchronous)
                tones.stop();
              speech.stop();
            }
          initiate();
          sounds.start(jobs->front().item<sound_task>(),
                       !file_player::asynchronous || (business != playing));
          business = playing;
        }
      else if (jobs->front().item_type() == typeid(tone_task))
        {
          if (!tone_generator::asynchronous)
            {
              if (!file_player::asynchronous)
                sounds.stop();
              speech.stop();
            }
          initiate();
          tones.start(jobs->front().item<tone_task>(),
                      !tone_generator::asynchronous || (business != beeping));
          business = beeping;
        }
      else if (jobs->front().item_type() == typeid(speech_task))
        {
          if (!file_player::asynchronous)
            sounds.stop();
          if (!tone_generator::asynchronous)
            tones.stop();
          speech.stop();
          initiate();
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
          if (file_player::asynchronous || !sounds.active())
            shift();
          else result = true;
          break;
        case beeping:
          if (tone_generator::asynchronous || !tones.active())
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
          notify(notification::job_complete, jobs->front());
          jobs->pop_front();
        }
    }
  else
    {
      if (jobs->front().id() && (jobs->front().state() == job::idle))
        notify(notification::job_cancel, jobs->front());
      jobs->pop_front();
    }
}

void
sound_manager::initiate(void)
{
  if (jobs->front().state() != job::active)
    {
      notify((jobs->front().state() != job::postponed) ?
             notification::job_start :
             notification::job_resume,
             jobs->front());
      jobs->front().activate();
    }
}


// Agent subclass members:

sound_manager::agent::agent(sound_manager* job_holder):
  holder(job_holder)
{
}

void
sound_manager::agent::operator()(void)
{
  while (holder->state != dead)
    {
      mutex::scoped_lock lock(holder->access);
      while (holder->state == idle)
        holder->event.wait(lock);
      holder->next();
      while (holder->working())
        audioplayer::complete.wait(lock);
    }
}

} // namespace multispeech
