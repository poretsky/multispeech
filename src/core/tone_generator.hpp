// tone_generator.hpp -- Tone generator interface
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

// The tone_generator class introduces typical tone producing task
// description and provides basic playing control means along with
// pending tasks queue maintenance.

#ifndef MULTISPEECH_TONE_GENERATOR_HPP
#define MULTISPEECH_TONE_GENERATOR_HPP

#include <boost/thread/condition.hpp>

#include <soundtouch/FIFOSampleBuffer.h>

#include "audioplayer.hpp"
#include "sound_processor.hpp"
#include "exec_queue.hpp"

// Tone producing task description is represented by frequency in Hz,
// duration in seconds and relative volume level.
class tone_task
{
public:
  // Object constructor:
  tone_task(unsigned int tone_frequency, float tone_duration,
            float tone_volume = 1.0);

private:
  // Properties accessible only for actual executor:
  const unsigned int frequency;
  const float duration;
  const float volume;

  friend class tone_generator;
};

class tone_generator:
  public audioplayer,
  private sound_processor,
  private exec_queue<tone_task>
{
public:
  // Construct / destroy:
  explicit tone_generator(boost::condition& completion_event_consumer);
  ~tone_generator(void);

  // Start task execution. If there is a task executing already
  // and the new one is started with the override option, then
  // current task is stopped immediately and the new one is started.
  // Otherwise the new task is appended to the pending tasks queue.
  void start(const tone_task& tone, bool override);

  // Stop currently playing sound if any and clear queue:
  void stop(void);

  // Return true if something is currently playing:
  bool active(void);

  // Configurable parameters:
  static std::string device;
  static bool asynchronous;
  static unsigned int sampling;
  static float relative_volume;

private:
  // Playback completion event consumer:
  boost::condition& host;

  // Internal operating data:
  float omega_t, step;
  int count, amount, fade_in, fade_out;

  soundtouch::FIFOSampleBuffer fifo;

  // Methods required by audioplayer:
  unsigned int source_read(float* buffer, unsigned int nframes);
  void source_release(void);
  void notify_completion(void);

  // Methods required by sound_processor:
  unsigned int get_source(float* buffer, unsigned int nframes);
  unsigned int nChannels(void);

  // Methods required by exec_queue:
  void execute(const tone_task& task);
  void abort(void);
  bool busy(void);
};

#endif
