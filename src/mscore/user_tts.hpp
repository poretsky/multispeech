// user_tts.hpp -- User specified speech backend interface
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

// This backend is assumed to be multilingual.

#ifndef MULTISPEECH_USER_TTS_HPP
#define MULTISPEECH_USER_TTS_HPP

#include <string>

#include "speech_engine.hpp"
#include "singleton.hpp"

namespace multispeech
{

class user_tts: public speech_engine
{
private:
  // Object construction:
  user_tts(void);

  // Supported voices:
  const voice_attributes en, ru;

  // Make up voice parameters for backend:
  void voicify(float rate, float pitch = 1.0);

  // Extract voice parameters from the configuration:
  static voice_attributes make_voice(const char* language);

public:
  // Instantiation by demand:
  static singleton<user_tts> instance;
  friend class singleton<user_tts>;

  // Compose internal voice name for specified language:
  static std::string voice(const std::string& language);
};

} // namespace multispeech

#endif
