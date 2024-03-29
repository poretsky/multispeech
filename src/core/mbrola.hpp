// mbrola.hpp -- Mbrola speech engine interface
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

// The mbrola class is not a complete speech backend class,
// but all Mbrola based backend classes should be derived from it.

#ifndef MULTISPEECH_MBROLA_HPP
#define MULTISPEECH_MBROLA_HPP

#include "speech_engine.hpp"

// Backend name:
#define MBROLA "mbrola"

class mbrola: public speech_engine
{
public:
  // Constructing the backend with Espeak preprocessor:
  explicit mbrola(const char* lang);

  // Constant data for references:
  static const char* const name;
  static const std::string voices_default_path;

  // Configurable parameters:
  static std::string executable;
  static std::string voices_path;

  // Voices assignment:
  static std::string en;
  static std::string de;
  static std::string it;
  static std::string fr;
  static std::string es;
  static std::string pt;

protected:
  // For constructing derived backend:
  mbrola(const std::string& backend,
         const std::string& voice_id,
         const char* lang);

private:
  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

#endif
