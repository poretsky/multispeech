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

// This backend is assumed to be multilingual. Language is specified
// as the second constructor argument.

#ifndef MULTISPEECH_USER_TTS_HPP
#define MULTISPEECH_USER_TTS_HPP

#include <string>

#include "speech_engine.hpp"

// Backend name:
#define USER_TTS "user"

class user_tts: public speech_engine
{
public:
  // Object construction:
  explicit user_tts(const char* lang);

  // Constant data for reference:
  static const char* const name;

  // Configurable parameters:
  static std::string command;
  static std::string format;
  static unsigned int sampling;
  static bool stereo;
  static bool freq_control;
  static std::string charset;
};

#endif
