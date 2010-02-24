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

#include <typeinfo>

#include "sound_manager.hpp"

#include "audioplayer.hpp"

using namespace std;
using namespace boost;


// Constructing / destroying:

sound_manager::sound_manager(const configuration& conf):
  state(idle),
  business(nothing),
  jobs(new jobs_queue),
  sounds(conf),
  tones(conf),
  speech(conf),
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
    jobs->push(any());
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
          jobs->push(any());
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
  switch (state)
    {
    case running:
      while (jobs->size() > 1)
        jobs->pop();
      break;
    default:
      while (!jobs->empty())
        jobs->pop();
      break;
    }
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
sound_manager::next_job(void)
{
  mutex::scoped_lock lock(access);
  while (state == idle)
    event.wait(lock);
  if ((state == running) && !jobs->empty())
    {
      if (jobs->front().empty())
        business = nothing;
      else if (jobs->front().type() == typeid(sound_task))
        {
          if (!file_player::asynchronous)
            {
              if (!tone_generator::asynchronous)
                tones.stop();
              speech.stop();
            }
          sounds.start(any_cast<sound_task>(jobs->front()),
                       !file_player::asynchronous || (business != playing));
          business = playing;
        }
      else if (jobs->front().type() == typeid(tone_task))
        {
          if (!tone_generator::asynchronous)
            {
              if (!file_player::asynchronous)
                sounds.stop();
              speech.stop();
            }
          tones.start(any_cast<tone_task>(jobs->front()),
                      !tone_generator::asynchronous || (business != beeping));
          business = beeping;
        }
      else if (jobs->front().type() == typeid(speech_task))
        {
          if (!file_player::asynchronous)
            sounds.stop();
          if (!tone_generator::asynchronous)
            tones.stop();
          speech.stop();
          speech.start(any_cast<speech_task>(jobs->front()));
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
            jobs->pop();
          else result = true;
          break;
        case beeping:
          if (tone_generator::asynchronous || !tones.active())
            jobs->pop();
          else result = true;
          break;
        case speaking:
          if (!speech.active())
            jobs->pop();
          else result = true;
          break;
        default:
          jobs->pop();
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
      holder->next_job();
      {
        mutex::scoped_lock lock(holder->access);
        while (holder->working())
          audioplayer::complete.wait(lock);
      }
    }
}
