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

#ifndef MULTISPEECH_MBROLA_ENGINE_HPP
#define MULTISPEECH_MBROLA_ENGINE_HPP

#include <string>

#include "speech_engine.hpp"

namespace multispeech
{

class mbrola: public speech_engine
{
protected:
  // Constructing the object:
  mbrola(const std::string& backend);

  // Voice name construction:
  struct voice: public voice_attributes
  {
    voice(const voice_attributes& attribs);
    operator std::string(void) const;
  };

public:
  // Known voices:
  static const voice en1, us1, us2, us3;

private:
  // Check actual voice availability:
  bool check_voice(const std::string& voice_name);

  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

} // namespace multispeech

#endif
