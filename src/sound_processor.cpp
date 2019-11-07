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


// Object constructor:

sound_processor::sound_processor(FIFOSamplePipe& conveyer):
  fifo(conveyer),
  state(inactive),
  capacity(0)
{
}


// Public methods:

void
sound_processor::start_processing(unsigned int reserve)
{
  stop_processing();
  capacity = reserve;
  state = hungry;
  thread(ref(*this));
  if (reserve)
    {
      mutex::scoped_lock lock(access);
      while (state == hungry)
        event.wait(lock);
    }
}

void
sound_processor::stop_processing(void)
{
  mutex::scoped_lock lock(access);
  fifo.clear();
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
  while (state == hungry)
    event.wait(lock);
  if ((state == replete) && (fifo.numSamples() < nframes))
    {
      if (capacity < nframes)
        capacity = nframes;
      state = hungry;
      event.notify_all();
      while (state == hungry)
        event.wait(lock);
    }
  obtained = fifo.receiveSamples(buffer, nframes);
  if (state == replete)
    {
      state = hungry;
      event.notify_all();
    }
  return obtained;
}

void
sound_processor::operator()(void)
{
  float buffer[chunk_size * nChannels()];
  unsigned int obtained;
  mutex::scoped_lock lock(access);
  while (state != quit)
    {
      while (state == replete)
        event.wait(lock);
      if (state == hungry)
        {
          obtained = get_source(buffer, chunk_size);
          if (obtained)
            fifo.putSamples(buffer, obtained);
          if (obtained < chunk_size)
            {
              flush();
              state = quit;
            }
          else if (fifo.numSamples() >= capacity)
            {
              state = replete;
              event.notify_all();
            }
        }
    }
  state = inactive;
  event.notify_all();
}


// Private methods:

void
sound_processor::flush(void)
{
}
