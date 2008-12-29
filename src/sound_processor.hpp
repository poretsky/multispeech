// sound_processor.hpp -- Sound processing interface
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

// The sound_processor class provides the SoundTouch sound processing
// functionality allowing to do all the work as a background process.
// To retrieve source data this process uses private virtual method
// get_source() that must be defined in the derived classes.

#ifndef SOUND_PROCESSOR_HPP
#define SOUND_PROCESSOR_HPP

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <soundtouch/SoundTouch.h>

class sound_processor: public soundtouch::SoundTouch
{
public:
  // Construct / destroy:
  sound_processor(void);
  ~sound_processor(void);

  // Background processing control:
  void start_processing(void);
  void stop_processing(void);
  unsigned int read_result(float* buffer, unsigned int nframes);

private:
  // Processing thread states:
  enum status
  {
    hungry, // Need more input data
    replete, // Has sufficient data for output
    quit, // About to quit
    inactive // Already complete or not started yet
  };

  // The functor to be executed in a separate thread.
  class soundmaster
  {
  public:
    // Object constructor.
    soundmaster(sound_processor* owner);

    // The thread execution loop.
    void operator()(void);

  private:
    // the parent class pointer.
    sound_processor* holder;

    // Chunk size for data input:
    static const unsigned int chunk_size;
  };

  friend class soundmaster;

  // Thread synchronization control:
  boost::mutex access;
  boost::condition event;
  status state;

  // Suggested chunk size for output:
  unsigned int chunk_size;

  // Query source. Must be implemented in the derived classes.
  virtual unsigned int get_source(float* buffer, unsigned int nframes) = 0;
};

#endif
