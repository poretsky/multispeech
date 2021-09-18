// espeak.hpp -- Espeak based speech backends interface
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

// These backends are multilingual by nature. Language is specified
// as the second constructor argument.

#ifndef MULTISPEECH_ESPEAK_HPP
#define MULTISPEECH_ESPEAK_HPP

#include <string>

#include "speech_engine.hpp"

// Backend name:
#define ESPEAK "espeak"

// Espeak backend.
class espeak: public speech_engine
{
public:
  // Object construction:
  explicit espeak(const char* lang);

  // Constant data for references:
  static const char* const name;

  // Configurable parameters:
  static std::string executable;

  // Voices assignment:
  static std::string en;
  static std::string de;
  static std::string it;
  static std::string fr;
  static std::string es;
  static std::string pt;
  static std::string ru;

private:
  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

#endif
