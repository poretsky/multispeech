// loudspeaker.cpp -- Loud speech producer implementation
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

#include <boost/numeric/conversion/cast.hpp>

#include "loudspeaker.hpp"

using namespace std;
using namespace boost;


// Static data definition:
float loudspeaker::relative_volume = 1.0;


// Make up a task description:

speech_task::speech_task(void):
  format(soundfile::none),
  playing(silence_params(0, 0)),
  volume(0.0),
  accelerate(0.0)
{
}

speech_task::speech_task(const string& txt, const pipeline::script& cmds,
                         soundfile::format fmt, details playing_params,
                         float loudness, float tempo_acceleration):
  text(txt),
  commands(cmds),
  format(fmt),
  playing(playing_params),
  volume(loudness),
  accelerate(tempo_acceleration)
{
}

speech_task::speech_task(unsigned int sampling, unsigned int silence_length):
  format(soundfile::silence),
  playing(silence_params(sampling, silence_length)),
  volume(0.0),
  accelerate(0.0)
{
}

speech_task::details
speech_task::silence_params(unsigned int sampling, unsigned int length)
{
  details params;
  params.silence.sampling = sampling;
  params.silence.length = length;
  return params;
}


// Construct / destroy:

loudspeaker::loudspeaker(const configuration* conf):
  soundfile(conf->option_value[options::speech::device].as<string>().empty() ?
            conf->option_value[options::audio::device].as<string>() :
            conf->option_value[options::speech::device].as<string>()),
  silence_timer(0),
  need_processing(false)
{
}

loudspeaker::~loudspeaker(void)
{
}


// Public methods:

void
loudspeaker::start(const speech_task& speech)
{
  if (speech.format == silence)
    {
      silence_timer = speech.playing.silence.length;
      start_playback(0.0, speech.playing.silence.sampling, 1);
    }
  else if ((speech.format != none) &&
           !speech.text.empty() &&
           !speech.commands.empty())
    {
      unsigned int playing_rate;
      silence_timer = 0;
      trim = true;
      SF_INFO::format = speech.format;
      if (speech.format != autodetect)
        {
          soundfile::channels = speech.playing.sound.channels;
          samplerate = speech.playing.sound.sampling;
        }
      sfd = -1;
      tts.run(speech.commands, this);
      tts << speech.text << endl;
      tts.complete();
      if (sfd >= 0)
        source = sf_open_fd(sfd, SFM_READ, this, 0);
      playing_rate = samplerate;
      if (source && (speech.format == autodetect) && !frames)
        {
          sf_close(source);
          SF_INFO::format &= ~SF_FORMAT_TYPEMASK;
          SF_INFO::format |= SF_FORMAT_RAW;
          source = sf_open_fd(sfd, SFM_READ, this, 0);
          if (speech.playing.deviation > 0.0)
            playing_rate = numeric_cast<unsigned int>(nearbyint(numeric_cast<double>(samplerate) / speech.playing.deviation));
        }
      if (source)
        {
          if (speech.accelerate != 0.0)
            {
              need_processing = true;
              setChannels(soundfile::channels);
              setSampleRate(samplerate);
              setTempoChange(speech.accelerate);
              start_processing();
            }
          start_playback(speech.volume * relative_volume,
                         playing_rate, soundfile::channels);
        }
      else source_release();
    }
}


// Private methods:


unsigned int
loudspeaker::source_read(float* buffer, unsigned int nframes)
{
  unsigned int result;
  if (source)
    result = need_processing ?
      read_result(buffer, nframes) :
      get_source(buffer, nframes);
  else if (silence_timer > nframes)
    {
      silence_timer -= nframes;
      result = nframes;
    }
  else
    {
      result = silence_timer;
      silence_timer = 0;
    }
  return result;
}

void
loudspeaker::source_release(void)
{
  silence_timer = 0;
  if (need_processing)
    {
      stop_processing();
      need_processing = false;
    }
  if (source)
    {
      sf_close(source);
      source = NULL;
    }
  tts.stop();
  tts.wait();
}

unsigned int
loudspeaker::get_source(float* buffer, unsigned int nframes)
{
  unsigned int obtained = source ? sf_readf_float(source, buffer, nframes) : 0;
  while (trim && obtained)
    {
      unsigned int i;
      for (i = 0; i < (obtained * soundfile::channels); i++)
        if (buffer[i] != 0.0)
          break;
      if (i < (obtained * soundfile::channels))
        {
          unsigned int skip = i / soundfile::channels;
          if (skip)
            {
              obtained -= skip;
              skip *= soundfile::channels;
              for (i = 0; i < (obtained * soundfile::channels); i++)
                buffer[i] = buffer[i + skip];
              obtained += sf_readf_float(source, buffer + (obtained * soundfile::channels),
                                         nframes - obtained);
            }
          trim = false;
        }
      else obtained = sf_readf_float(source, buffer, nframes);
    }
  return obtained;
}

void
loudspeaker::attach(int fd)
{
  sfd = fd;
}
