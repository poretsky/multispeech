// tone_generator.cpp -- Tone generator implementation
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

#include "tone_generator.hpp"

using namespace std;


// Static data definition:
bool tone_generator::asynchronous = true;
unsigned int tone_generator::sampling = 44100;
float tone_generator::relative_volume = 1.0;


// Make up a task description:

tone_task::tone_task(unsigned int tone_frequency, float tone_duration,
                     float tone_volume):
  frequency(tone_frequency),
  duration(tone_duration),
  volume(tone_volume)
{
}


// Construct / destroy:

tone_generator::tone_generator(const configuration& conf):
  audioplayer(conf.option_value[options::tones::device].as<string>())
{
}

tone_generator::~tone_generator(void)
{
}


// Public methods:

void
tone_generator::start(const tone_task& tone, bool override)
{
  if (override)
    stop();
  submit(tone);
}

void
tone_generator::stop(void)
{
  cancel();
}

bool
tone_generator::active(void)
{
  return !done();
}


// Private methods:

unsigned int
tone_generator::source_read(float* buffer, unsigned int nframes)
{
  unsigned int n;
  for (n = 0; (n < nframes) && (elapsed < limit); n++, elapsed += step)
    buffer[n] = sinf(elapsed);
  if (n)
    sprev = buffer[n - 1];
  while ((sprev != 0.0) && (n < nframes))
    {
      buffer[n] = sinf(elapsed);
      elapsed += step;
      if ((sprev * buffer[n]) < 0.0)
        buffer[n] = 0.0;
      sprev = buffer[n++];
    }
  return n;
}

void
tone_generator::source_release(void)
{
}

void
tone_generator::execute(const tone_task& tone)
{
  limit = 2.0 * M_PI * static_cast<float>(tone.frequency);
  step = limit / static_cast<float>(sampling);
  limit *= tone.duration;
  elapsed = 0.0;
  sprev = 0.0;
  start_playback(tone.volume * relative_volume, sampling, 1);
}

void
tone_generator::abort(void)
{
  audioplayer::stop();
}

bool
tone_generator::busy(void)
{
  return audioplayer::active();
}
