// freephone.hpp -- Freephone speech backend interface
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

// English speech backend. Uses Mbrola voice en1.

#ifndef MULTISPEECH_FREEPHONE_HPP
#define MULTISPEECH_FREEPHONE_HPP

#include "mbrola.hpp"

class freephone: public mbrola
{
public:
  // Object construction:
  freephone(const configuration& conf);
};

#endif
