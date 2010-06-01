// singleton.hpp -- Singleton instantiation by demand
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

#ifndef MULTISPEECH_SINGLETON_HPP
#define MULTISPEECH_SINGLETON_HPP

#include <memory>

namespace multispeech
{

template <typename object>
class singleton: private std::auto_ptr<object>
{
public:
  // Default constructor:
  singleton(void)
  {
  }

  // Instantiate object or get pointer to it:
  object* operator()(void)
  {
    if (!std::auto_ptr<object>::get())
      reset(new object);
    return std::auto_ptr<object>::get();
  }
};

} // namespace multispeech

#endif
