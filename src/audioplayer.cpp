// audioplayer.cpp -- General audio stream player implementation
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

#include <ctime>

#include <boost/thread/thread.hpp>

#include "audioplayer.hpp"

#include "config.hpp"

using namespace std;
using namespace boost;
using namespace portaudio;


// Static data:
condition audioplayer::complete;
mutex audioplayer::control;
PaTime audioplayer::suggested_latency = 20;
float audioplayer::general_volume = 0.8;


// Construct / destroy:

audioplayer::audioplayer(const string& device_name):
  playing(false),
  params(DirectionSpecificStreamParameters::null(),
         DirectionSpecificStreamParameters::null(),
         0.0, 0,
         paPrimeOutputBuffersUsingStreamCallback)
{
  string devname(device_name);
  System& system = System::instance();
  PaDeviceIndex device = system.defaultOutputDevice().index();
  PaTime latency = suggested_latency;
  if (devname.empty())
    {
      devname = system.deviceByIndex(device).name();
      if (devname.empty())
        devname = "default";
      device = find_device(devname);
    }
  else device = find_device(devname);
  if (latency <= 0.0)
    latency = system.deviceByIndex(device).defaultLowOutputLatency();
  params.setOutputParameters(DirectionSpecificStreamParameters(system.deviceByIndex(device),
                                                               2, FLOAT32, true,
                                                               latency, NULL));
  params.setSampleRate(system.deviceByIndex(device).defaultSampleRate());
  if (!params.isSupported())
    throw configuration::error("cannot properly initialize audio device \"" + devname + "\"");
}

audioplayer::~audioplayer(void)
{
}


// Public methods:

void
audioplayer::stop(void)
{
  cancel_stream();
  while (stream_is_active())
    thread::yield();
  close_stream();
}

bool
audioplayer::active(void)
{
  if (stream_is_over())
    {
      while (stream_is_active())
        thread::yield();
      close_stream();
    }
  mutex::scoped_lock lock(access);
  return playing;
}


// Protected methods:

void
audioplayer::start_playback(float volume, unsigned int rate, unsigned int channels)
{
  mutex::scoped_lock exclusive(control);
  volume_level = volume * general_volume;
  frame_size = channels;
  sampling_rate = static_cast<double>(rate);
  params.setSampleRate(sampling_rate);
  params.outputParameters().setNumChannels(channels);
  if (params.isSupported())
    {
      stream.open(params, *this);
      if (stream.isOpen())
        {
          playing = true;
          finish_time = 0;
          stream_time_available = stream.time() != 0;
          if (!stream_time_available)
            buffer_time = clock_time() + stream.outputLatency();
          stream.setStreamFinishedCallback(release);
          stream.start();
        }
      else source_release();
    }
  else source_release();
}


// Private methods:

PaTime
audioplayer::clock_time(void)
{
  timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return static_cast<PaTime>(tv.tv_nsec) * 1e-9 + static_cast<PaTime>(tv.tv_sec);
}

bool
audioplayer::stream_is_active(void)
{
  mutex::scoped_lock exclusive(control);
  return stream.isOpen() && stream.isActive();
}

bool
audioplayer::stream_is_over(void)
{
  mutex::scoped_lock lock(access);
  return !playing;
}

void
audioplayer::close_stream(void)
{
  mutex::scoped_lock exclusive(control);
  if (stream.isOpen())
    stream.close();
}

void
audioplayer::cancel_stream(void)
{
  mutex::scoped_lock exclusive(control);
  if (stream.isOpen() && stream.isActive())
    stream.abort();
}

PaDeviceIndex
audioplayer::find_device(const string& device_name)
{
  System& system = System::instance();
  System::DeviceIterator found;
  for (found = system.devicesBegin(); found != system.devicesEnd(); ++found)
    if (found->name() == device_name)
      break;
  if (found == system.devicesEnd())
    throw configuration::error("audio device \"" + device_name + "\" is not found");
  if (found->isInputOnlyDevice())
    throw configuration::error("audio device \"" + device_name + "\" is not valid");
  return found->index();
}

int
audioplayer::paCallbackFun(const void *inputBuffer, void *outputBuffer,
                           unsigned long numFrames,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags)
{
  int result = paContinue;
  if (!(statusFlags & paOutputOverflow))
    {
      float* buffer = reinterpret_cast<float*>(outputBuffer);
      unsigned int obtained = source_read(buffer, numFrames);
      if (stream_time_available)
        {
          current_time = timeInfo->currentTime;
          buffer_time = timeInfo->outputBufferDacTime;
        }
      else current_time = clock_time();
      if (obtained)
        for (unsigned int i = 0; i < (obtained * frame_size); i++)
          buffer[i] *= volume_level;
      else if (finish_time == 0)
        finish_time = buffer_time;
      else if (finish_time <= current_time)
        result = paComplete;
      if (obtained < numFrames)
        for (unsigned int i = obtained * frame_size; i < (numFrames * frame_size); i++)
          buffer[i] = 0.0;
      if (!stream_time_available)
        buffer_time += static_cast<double>(numFrames) / sampling_rate;
    }
  return result;
}

void
audioplayer::release(void* handle)
{
  reinterpret_cast<audioplayer*>(handle)->source_release();
  mutex::scoped_lock lock(reinterpret_cast<audioplayer*>(handle)->access);
  reinterpret_cast<audioplayer*>(handle)->playing = false;
  complete.notify_all();
}
