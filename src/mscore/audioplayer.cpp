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

#include <sysconfig.h>

#include "audioplayer.hpp"

#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;
using namespace portaudio;


// Static data:
condition audioplayer::complete;


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
  PaTime latency = configuration::audio_latency();
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
  if (stream.isOpen())
    stream.close();
  while (playing)
    continue;
}

bool
audioplayer::active(void)
{
  if ((!playing) && stream.isOpen())
    stream.close();
  return playing;
}


// Protected methods:

void
audioplayer::start_playback(float volume, unsigned int rate, unsigned int channels)
{
  volume_level = volume * configuration::general_volume();
  params.setSampleRate(static_cast<double>(rate));
  params.outputParameters().setNumChannels(channels);
  if (params.isSupported())
    {
      stream.open(params, *this);
      if (stream.isOpen())
        {
          playing = true;
          stream.setStreamFinishedCallback(release);
          stream.start();
        }
    }
}


// Private methods:

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
      if (obtained)
        for (unsigned int i = 0; i < (obtained * params.outputParameters().numChannels()); i++)
          buffer[i] *= volume_level;
      for (unsigned int i = obtained; i < (numFrames * params.outputParameters().numChannels()); i++)
        buffer[i] = 0.0;
      if (obtained < numFrames)
        result = paComplete;
    }
  return result;
}

void
audioplayer::release(void* handle)
{
  reinterpret_cast<audioplayer*>(handle)->source_release();
  reinterpret_cast<audioplayer*>(handle)->playing = false;
  complete.notify_all();
}

} // namespace multispeech
