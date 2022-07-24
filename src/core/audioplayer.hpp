// audioplayer.hpp -- General audio stream player framework
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

// The audioplayer class takes care about audio stream playback process.
// It is not for direct use, but serves as the base for other classes
// that need audio playback functionality, such as sound file player,
// tone generator or speech engine.

// The following virtual methods must be defined in the derived classes:
//
// unsigned int source_read(float* buffer, unsigned int nframes)
// Read specified number of frames from the source sound stream
// into the provided buffer. Return number of frames actually read
// (0 when the source is exhausted or unavailable).
//
// void source_release(void)
// Notify source that it is not needed anymore.
//
// void notify_completion(void)
// Called when playing is actually done.
//
// All these methods are declared as private.

#ifndef MULTISPEECH_AUDIOPLAYER_HPP
#define MULTISPEECH_AUDIOPLAYER_HPP

#include <string>
#include <memory>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include <portaudiocpp/PortAudioCpp.hxx>

#include <pulse/simple.h>

class audioplayer: private portaudio::CallbackInterface
{
protected:
  // Construct / destroy:
  audioplayer(const std::string& device_name, const char* stream_id);
  ~audioplayer(void);

public:
  // Stop playback process and wait until it completes:
  virtual void stop(void);

  // Returns true when playback is in progress:
  virtual bool active(void);

  // Get canonical name for PortAudio device:
  static std::string canonical_name(portaudio::Device& device);

  // Configurable parameters:
  static std::string device;
  static PaTime suggested_latency;
  static float general_volume;
  static bool async;
  static bool use_pa;

  // Playback thread execution loop.
  void operator()(void);

protected:
  // Internal buffer size in samples for specified rate:
  unsigned int bufsize(unsigned int rate);

  // Use in derived classes to start playback process:
  void start_playback(float volume, unsigned int rate, unsigned int channels);

private:
  // Indicate that playback is in progress:
  bool playing;
  bool running;

  // Synchronization means:
  boost::mutex access;
  boost::condition abandon;
  boost::condition complete;

  // Audio playing stream:
  portaudio::Stream* stream;

  // Audio stream parameters:
  portaudio::StreamParameters params;
  float volume_level;
  unsigned int frame_size;
  double sampling_rate;
  PaTime current_time, buffer_time, finish_time;
  bool stream_time_available;

  // Pulseaudio support means:
  pa_simple* paStream;
  const char* paStreamId;
  pa_sample_spec paStreamParams;
  pa_buffer_attr paBufAttr;

  // Internal audiostream control:
  void do_sync_playback(void);
  PaTime clock_time(void);
  bool playback_in_progress(void);
  bool open_stream(void);
  void close_stream(void);

  // Audio playing callback function:
  int paCallbackFun(const void *inputBuffer, void *outputBuffer,
                    unsigned long numFrames, 
                    const PaStreamCallbackTimeInfo *timeInfo,
                    PaStreamCallbackFlags statusFlags);

  // Playing finishing callback:
  static void release(void* handle);

  // The following three methods must be defined in the derived classes:
  virtual unsigned int source_read(float* buffer, unsigned int nframes) = 0;
  virtual void source_release(void) = 0;
  virtual void notify_completion(void) = 0;
};

#endif
