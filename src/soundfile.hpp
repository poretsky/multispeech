// soundfile.hpp -- General sound format player framework
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

// The soundfile class takes care about various sound file formats
// and provides general playing capability. It serves as a base
// class to derive actual sound file o stream players.

#ifndef MULTISPEECH_SOUNDFILE_HPP
#define MULTISPEECH_SOUNDFILE_HPP

#include <sndfile.h>

#include "audioplayer.hpp"

class soundfile:
  public audioplayer,
  protected SF_INFO
{
public:
  // Sound sample types:
  enum format
  {
    autodetect = 0, // should be detected automatically
    s8 = SF_FORMAT_RAW | SF_FORMAT_PCM_S8,  // signed 8 bits
    u8 = SF_FORMAT_RAW | SF_FORMAT_PCM_U8,  // unsigned 8 bits
    s16 = SF_FORMAT_RAW | SF_FORMAT_PCM_16, // signed 16 bits
    silence, // special value to denote silence
    none // unplayable
  };

protected:
  // Construct / destroy:
  soundfile(const std::string& device_name);
  ~soundfile(void);

  // Source file description data:
  SNDFILE* source;

private:
  // Retreive a chunk of audio data:
  unsigned int source_read(float* buffer, unsigned int nframes);

  // Close the source stream:
  void source_release(void);
};

#endif
