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

// These backends are multilingual by nature.

#ifndef MULTISPEECH_ESPEAK_HPP
#define MULTISPEECH_ESPEAK_HPP

#include <string>

#include "mbrola.hpp"
#include "singleton.hpp"

namespace multispeech
{

// Espeak backend.
class espeak: public speech_engine
{
private:
  // Object construction:
  espeak(void);

  // Voice name construction:
  struct voice: public voice_attributes
  {
    voice(const voice_attributes& attribs);
    operator std::string(void) const;
  };

public:
  // Instantiation by demand:
  static singleton<espeak> instance;
  friend class singleton<espeak>;

  // Known voices:
  static const voice en_default, ru;

  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

// Espeak+Mbrola backend.
class mbrespeak: public mbrola
{
private:
  // Object construction:
  mbrespeak(void);

public:
  // Instantiation by demand:
  static singleton<mbrespeak> instance;
  friend class singleton<mbrespeak>;
};

} // namespace multispeech

#endif
