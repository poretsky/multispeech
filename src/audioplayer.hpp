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

// The audioplayer class takes care about audio stream playing process.
// It is not for direct use, but serves as the base for other classes
// that need audio playing functionality, such as sound file player,
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
// All these methods are declared as private.

#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include <string>
#include <memory>

#include <boost/thread/condition.hpp>

#include <portaudiocpp/PortAudioCpp.hxx>

class audioplayer: private portaudio::CallbackInterface
{
public:
  // Construct / destroy:
  audioplayer(const std::string& device_name);
  virtual ~audioplayer(void);

  // Playing process control:
  virtual void stop(void);
  virtual bool active(void);

  // Completion notifier:
  static boost::condition complete;

  // Configurable parameters:
  static PaTime suggested_latency;
  static float general_volume;

protected:
  // Use in derived classes to start audio stream playing:
  void start_playback(float volume, unsigned int rate, unsigned int channels);

private:
  // Indicate that playback is scheduled to stop:
  bool no_playback;

  // Audio playing stream:
  portaudio::InterfaceCallbackStream stream;

  // Audio stream parameters:
  portaudio::StreamParameters params;
  float volume_level;

  // Audio playing callback function:
  int paCallbackFun(const void *inputBuffer, void *outputBuffer,
                    unsigned long numFrames, 
                    const PaStreamCallbackTimeInfo *timeInfo,
                    PaStreamCallbackFlags statusFlags);

  // Playing finishing callback:
  static void release(void* handle);

  // The following two methods must be defined in the derived classes:
  virtual unsigned int source_read(float* buffer, unsigned int nframes) = 0;
  virtual void source_release(void) = 0;
};

#endif
