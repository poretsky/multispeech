// loudspeaker.hpp -- Loud speech producer interface
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

// The loudspeaker class takes care about external TTS pipeline
// execution and provides generated sound stream playing capability.

#ifndef MULTISPEECH_LOUDSPEAKER_HPP
#define MULTISPEECH_LOUDSPEAKER_HPP

#include <string>

#include <boost/thread/condition.hpp>

#include <soundtouch/SoundTouch.h>

#include "soundfile.hpp"
#include "sound_processor.hpp"
#include "pipeline.hpp"

// Speech producing task description is represented by text string
// to be spoken, external command set for TTS pipeline constructing,
// sound stream parameters and relative volume level.
class speech_task
{
public:
  // Sound playing details:
  union details
  {
    struct
    {
      unsigned int sampling;
      unsigned int channels;
    } sound;
    struct
    {
      unsigned int sampling;
      unsigned int length;
    } silence;
    double deviation;
  };

  // Object constructors:
  speech_task(void);
  speech_task(const std::string& txt, const pipeline::script& cmds,
              soundfile::format fmt, details playing_params,
              float loudness = 1.0, float tempo_acceleration = 0.0);
  speech_task(unsigned int sampling, unsigned int silence_length);

private:
  // Properties accessible only for actual executor:
  const std::string text;
  const pipeline::script commands;
  const soundfile::format format;
  const details playing;
  const float volume;
  const float accelerate;

  friend class loudspeaker;

  // Pack silence parameters;
  static details silence_params(unsigned int sampling, unsigned int length);
};

class loudspeaker:
  public soundfile,
  private sound_processor,
  private pipeline::consumer
{
public:
  // Construct / destroy:
  explicit loudspeaker(boost::condition& completion_event_consumer);
  ~loudspeaker(void);

  // Start task execution:
  void start(const speech_task& speech);

  // Configurable parameters:
  static std::string device;
  static float relative_volume;

private:
  // Playback completion event consumer:
  boost::condition& host;

  // Retreive a chunk of audio data:
  unsigned int source_read(float* buffer, unsigned int nframes);

  // Finish gracefully:
  void source_release(void);

  // Notify playing completion:
  void notify_completion(void);

  // Get a sound chunk for processing:
  unsigned int get_source(float* buffer, unsigned int nframes);

  // Get number of channels:
  unsigned int nChannels(void);

  // Flush remaining accelerator data:
  void flush(void);

  // Attach to the sound stream by file descriptor:
  void attach(int fd);

  friend class pipeline;

  // Speech rate accelerator:
  soundtouch::SoundTouch accelerator;

  // External TTS pipeline:
  pipeline tts;

  // Attached sound stream fd:
  int sfd;

  // Silence time counter in samples:
  unsigned int silence_timer;

  // Internally used flags:
  bool trim, need_processing;
};

#endif
