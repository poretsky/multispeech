// ru_tts.hpp -- Ru_tts speech backend interface
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

// Russian speech backend.

#ifndef MULTISPEECH_RU_TTS_ENGINE_HPP
#define MULTISPEECH_RU_TTS_ENGINE_HPP

#include "speech_engine.hpp"

namespace multispeech
{

class ru_tts: public speech_engine
{
public:
  // Object construction:
  explicit ru_tts(const configuration* conf);

private:
  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

} // namespace multispeech

#endif
