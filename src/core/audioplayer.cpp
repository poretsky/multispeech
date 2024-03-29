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

#include <cmath>
#include <ctime>
#include <exception>

#include <boost/regex.hpp>

#include "audioplayer.hpp"

#include "config.hpp"

using namespace std;
using namespace boost;
using namespace portaudio;


// Internal data:
static const regex devname_pattern("\\((.+)\\)");


// Internal routines:

static PaDeviceIndex
find_device(const string& device_name)
{
  System& system = System::instance();
  System::DeviceIterator found;
  for (found = system.devicesBegin(); found != system.devicesEnd(); ++found)
    if (audioplayer::canonical_name(*found) == device_name)
      return found->isInputOnlyDevice() ? paNoDevice : found->index();
  return paNoDevice;
}


// Static data:
string audioplayer::device;
PaTime audioplayer::suggested_latency = 0;
float audioplayer::general_volume = 0.8;
bool audioplayer::async = false;
bool audioplayer::use_pa = true;


// Construct / destroy:

audioplayer::audioplayer(const string& device_name, const char* stream_id):
  playing(false),
  playing_async(false),
  running(false),
  alive(true),
  service(boost::ref(*this)),
  stream(NULL),
  params(DirectionSpecificStreamParameters::null(),
         DirectionSpecificStreamParameters::null(),
         0.0,
         paFramesPerBufferUnspecified,
         paPrimeOutputBuffersUsingStreamCallback),
  paStream(NULL),
  paStreamId(stream_id)
{
  System& system = System::instance();
  PaDeviceIndex devidx = device_name.empty() ?
    system.defaultOutputDevice().index() :
    find_device(device_name);
  if (devidx == paNoDevice)
    throw configuration::error("audio output device \"" + device_name + "\" is not found");
  Device& device = system.deviceByIndex(devidx);
  if (device.isInputOnlyDevice())
    throw configuration::error("audio device \"" + canonical_name(device) + "\" is not valid");
  devidx = use_pa ? find_device("pulse") : paNoDevice;
  if ((devidx == paNoDevice) || ((device.index() != system.defaultOutputDevice().index()) && (device.index() != devidx)))
    stream = async ?
      static_cast<Stream*>(new InterfaceCallbackStream) :
      static_cast<Stream*>(new BlockingStream);
  if (stream)
    {
      PaTime latency = (suggested_latency > 0.0) ?
        suggested_latency :
        device.defaultLowOutputLatency();
      params.setOutputParameters(DirectionSpecificStreamParameters(device,
                                                                   2, FLOAT32, true,
                                                                   latency, NULL));
      params.setSampleRate(device.defaultSampleRate());
      if (!params.isSupported())
        throw configuration::error("cannot properly initialize audio device \"" + canonical_name(device) + "\"");
    }
  else
    {
      paStreamParams.format = PA_SAMPLE_FLOAT32;
      paBufAttr.maxlength = -1;
      paBufAttr.tlength = -1;
      paBufAttr.prebuf = -1;
      paBufAttr.minreq = -1;
      paBufAttr.fragsize = -1;
    }
}

audioplayer::~audioplayer(void)
{
  audioplayer::stop();
  {
    boost::mutex::scoped_lock lock(access);
    alive = false;
    start.notify_one();
  }
  if (stream)
    delete stream;
  service.join();
}


// Public methods:

void
audioplayer::stop(void)
{
  boost::mutex::scoped_lock lock(access);
  if (stream ? stream->isOpen() : (paStream != NULL))
    {
      playing = false;
      if (!(async && stream))
        abandon.notify_one();
    }
  while (running)
    complete.wait(lock);
}

bool
audioplayer::active(void)
{
  boost::mutex::scoped_lock lock(access);
  return running;
}

string
audioplayer::canonical_name(Device& device)
{
  smatch match_result;
  string devname(device.name());
  return regex_search(devname, match_result, devname_pattern) ?
    string(match_result[1].first, match_result[1].second) :
    devname;
}

void
audioplayer::operator()(void)
{
  while (wait_start())
    {
      if (open_stream())
        {
          if (stream && async)
            {
              boost::mutex::scoped_lock lock(access);
              while (playing_async)
                abandon.wait(lock);
              playing = false;
            }
          else do_sync_playback();
        }
      source_release();
      close_stream();
    }
}


// Protected methods:

unsigned int
audioplayer::bufsize(unsigned int rate)
{
  return ((rate + 5000) / 10000) << 8;
}

void
audioplayer::start_playback(float volume, unsigned int rate, unsigned int channels)
{
  boost::mutex::scoped_lock lock(access);
  while (running)
    complete.wait(lock);
  volume_level = volume * general_volume;
  frame_size = channels;
  sampling_rate = static_cast<double>(rate);
  params.setSampleRate(sampling_rate);
  params.setFramesPerBuffer(bufsize(rate));
  if (stream)
    params.outputParameters().setNumChannels(channels);
  else
    {
      paStreamParams.rate = rate;
      paStreamParams.channels = channels;
    }
  running = true;
  playing = true;
  start.notify_one();
}


// Private methods:

void
audioplayer::do_sync_playback(void)
{
  BlockingStream* blockingStream = stream ? dynamic_cast<BlockingStream*>(stream) : NULL;
  unsigned int paFrameSize = blockingStream ? 0 : pa_frame_size(&paStreamParams);
  unsigned int chunk_size = params.framesPerBuffer();
  float buffer[chunk_size * frame_size];
  while (playback_in_progress())
    {
      unsigned int obtained = source_read(buffer, chunk_size);
      if (obtained)
        try
          {
            for (unsigned int i = 0; i < (obtained * frame_size); i++)
              buffer[i] *= volume_level;
            if (blockingStream)
              blockingStream->write(buffer, obtained);
            else pa_simple_write(paStream, buffer, obtained * paFrameSize, NULL);
            buffer_time += static_cast<double>(obtained) / sampling_rate;
          }
        catch (const std::exception& error)
          {
            if ((!blockingStream) || blockingStream->isActive())
              continue;
            else break;
          }
      else break;
    }
  boost::mutex::scoped_lock lock(access);
  while (playing)
    if (!abandon.timed_wait(lock, posix_time::milliseconds(static_cast<int>((buffer_time - clock_time()) * 1000))))
      break;
  if (playing)
    {
      if (!blockingStream)
        pa_simple_drain(paStream, NULL);
      else if (blockingStream->isActive())
        blockingStream->stop();
      playing = false;
    }
  else if (!blockingStream)
    pa_simple_flush(paStream, NULL);
  else if (blockingStream->isActive())
    blockingStream->abort();
}

PaTime
audioplayer::clock_time(void)
{
  timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return static_cast<PaTime>(tv.tv_nsec) * 1e-9 + static_cast<PaTime>(tv.tv_sec);
}

bool
audioplayer::playback_in_progress(void)
{
  boost::mutex::scoped_lock lock(access);
  return playing;
}

bool
audioplayer::wait_start(void)
{
  boost::mutex::scoped_lock lock(access);
  while (alive && !running)
    start.wait(lock);
  return alive;
}

bool
audioplayer::open_stream(void)
{
  boost::mutex::scoped_lock lock(access);
  if (playing)
    {
      if (stream)
        {
          if (params.isSupported())
            {
              if (async)
                dynamic_cast<InterfaceCallbackStream*>(stream)->open(params, *this);
              else dynamic_cast<BlockingStream*>(stream)->open(params);
              if (stream->isOpen())
                {
                  finish_time = 0;
                  if (async)
                    {
                      playing_async = true;
                      stream_time_available = stream->time() != 0;
                      stream->setStreamFinishedCallback(release);
                    }
                  else stream_time_available = false;
                  if (!stream_time_available)
                    buffer_time = clock_time() + stream->outputLatency();
                  stream->start();
                }
              else playing = false;
            }
          else playing = false;
        }
      else
        {
          if (pa_sample_spec_valid(&paStreamParams))
            {
              paBufAttr.tlength = params.framesPerBuffer() * pa_frame_size(&paStreamParams);
              paStream = pa_simple_new(NULL, package::name, PA_STREAM_PLAYBACK, NULL, paStreamId, &paStreamParams, NULL, &paBufAttr, NULL);
              if (paStream)
                buffer_time = clock_time() + fmax(suggested_latency, static_cast<double>(pa_simple_get_latency(paStream, NULL)) * 1e-6);
              else playing = false;
            }
          else playing = false;
        }
    }
  return playing;
}

void
audioplayer::close_stream(void)
{
  boost::mutex::scoped_lock lock(access);
  if (stream)
    {
      if (stream->isOpen())
        stream->close();
    }
  else if (paStream)
    {
      pa_simple_free(paStream);
      paStream = NULL;
    }
  running = false;
  complete.notify_all();
  notify_completion();
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
      float* buffer = static_cast<float*>(outputBuffer);
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
  if (!playback_in_progress())
    result = paAbort;
  return result;
}

void
audioplayer::release(void* handle)
{
  audioplayer* player = static_cast<audioplayer*>(handle);
  boost::mutex::scoped_lock lock(player->access);
  player->playing_async = false;
  player->abandon.notify_one();
}
