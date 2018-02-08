// file_player.cpp -- Sound file player implementation
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

#include "file_player.hpp"

#include "config.hpp"


namespace multispeech
{

using namespace std;
using namespace boost::filesystem3;


// Make up a task description:

sound_task::sound_task(const path& sound_file, float sound_volume):
  file(sound_file),
  volume(sound_volume)
{
}


// Construct / destroy:

file_player::file_player(void):
  soundfile(configuration::sound_output_device())
{
}

file_player::~file_player(void)
{
}


// Public methods:

void
file_player::start(const sound_task& sound, bool override)
{
  if (override)
    stop();
  submit(sound);
}

void
file_player::stop(void)
{
  cancel();
}

bool
file_player::active(void)
{
  return !done();
}


// Private methods:

void
file_player::execute(const sound_task& sound)
{
  SF_INFO::format = autodetect;
  source = sf_open(sound.file.generic_string().c_str(), SFM_READ, this);
  if (source)
    start_playback(sound.volume * configuration::sound_volume(), samplerate, channels);
}

void
file_player::abort(void)
{
  audioplayer::stop();
}

bool
file_player::busy(void)
{
  return audioplayer::active();
}

} // namespace multispeech
