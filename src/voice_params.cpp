// voice_params.cpp -- Voice parameters container implementation
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

#include "voice_params.hpp"


// Object construction:

voice_params::voice_params(void)
{
}

voice_params::voice_params(voice_params* other):
  volume(other->volume),
  rate(other->rate),
  pitch(other->pitch),
  deviation(other->deviation),
  punctuations_mode(other->punctuations_mode)
{
}


// Protected methods:

void
voice_params::set_punctuations_mode(wchar_t mode)
{
  punctuations_mode = punctuations::from_wchar(mode);
}
