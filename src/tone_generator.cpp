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
  audioplayer(conf.option_value[options::tones::device].as<string>().empty() ?
              conf.option_value[options::audio::device].as<string>() :
              conf.option_value[options::tones::device].as<string>())
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
  for (n = 0; (n < nframes) && (count < amount); n++, count++, omega_t += step)
    {
      float fade = 1.0;
      if (count < (fade_in - 1))
        fade = static_cast<float>(count + 1) / static_cast<float>(fade_in);
      else if ((amount - count) < fade_out)
        fade = static_cast<float>(count + 1) / static_cast<float>(fade_in);
      buffer[n] = sinf(omega_t) * fade * fade;
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
  float duration = floor(static_cast<float>(tone.frequency) * tone.duration + 0.5) / static_cast<float>(tone.frequency);
  float omega = 2.0 * M_PI * static_cast<float>(tone.frequency);
  amount = static_cast<int>(ceil(duration * static_cast<float>(sampling)));
  omega_t = 0.5 * (duration - (static_cast<float>(amount - 1) / static_cast<float>(sampling))) * omega;
  step = omega / static_cast<float>(sampling);
  count = 0;
  fade_in = fade_out = static_cast<int>(floor(duration / 10.0 + 0.5));
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
