// voice_params.hpp -- Voice parameters container
/*
   Copyright (C) 2019 Igor B. Poretsky <poretsky@mlbox.ru>
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

#ifndef MULTISPEECH_VOICE_PARAMS_HPP
#define MULTISPEECH_VOICE_PARAMS_HPP

#include "text_filter.hpp"


class voice_params
{
protected:
  // Default constructor:
  voice_params(void);

  // Set punctuations mode according to identifying character:
  void set_punctuations_mode(wchar_t mode);

public:
  // Copy constructor:
  voice_params(voice_params* other);

  // Parameter values:
  double volume, rate, pitch, deviation;
  punctuations::mode punctuations_mode;
};

#endif
