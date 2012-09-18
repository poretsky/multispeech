// sound_processor.cpp -- Sound processing implementation
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

#include "sound_processor.hpp"

using namespace boost;
using namespace soundtouch;


// Static data:
const unsigned int sound_processor::soundmaster::chunk_size = 128;


// Construct / destroy:

sound_processor::sound_processor(void):
  state(inactive),
  chunk_size(0)
{
}

sound_processor::~sound_processor(void)
{
  stop_processing();
}


// Public methods:

void
sound_processor::start_processing(void)
{
  stop_processing();
  chunk_size = 0;
  state = hungry;
  thread(soundmaster(this));
}

void
sound_processor::stop_processing(void)
{
  mutex::scoped_lock lock(access);
  clear();
  if (state != inactive)
    {
      state = quit;
      event.notify_all();
      while (state != inactive)
        event.wait(lock);
    }
}

unsigned int
sound_processor::read_result(float* buffer, unsigned int nframes)
{
  unsigned int obtained;
  mutex::scoped_lock lock(access);
  if (chunk_size < nframes)
    {
      chunk_size = nframes;
      if ((state == replete) && (numSamples() < nframes))
        {
          state = hungry;
          event.notify_all();
        }
    }
  while (state == hungry)
    event.wait(lock);
  obtained = receiveSamples(buffer, nframes);
  if ((state == replete) && (numSamples() < nframes))
    {
      state = hungry;
      event.notify_all();
    }
  return obtained;
}


// The soundmaster class methods:

sound_processor::soundmaster::soundmaster(sound_processor* owner):
  holder(owner)
{
}

void
sound_processor::soundmaster::operator()(void)
{
  float buffer[chunk_size * holder->channels];
  unsigned int obtained;

  while (holder->state != quit)
    {
      {
        mutex::scoped_lock lock(holder->access);
      while (holder->state == replete)
        holder->event.wait(lock);
      }

      {
        mutex::scoped_lock lock(holder->access);
        if (holder->state == hungry)
          {
            obtained = holder->get_source(buffer, chunk_size);
            if (obtained)
              holder->putSamples(buffer, obtained);
            if (obtained < chunk_size)
              {
                holder->flush();
                holder->state = quit;
              }
            else if ((!holder->chunk_size && !holder->isEmpty()) ||
                     (holder->chunk_size && (holder->numSamples() >= holder->chunk_size)))
              {
                holder->state = replete;
                holder->event.notify_all();
              }
          }
      }
    }

  {
    mutex::scoped_lock lock(holder->access);
    holder->state = inactive;
    holder->event.notify_all();
  }
}
