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

#ifndef ESPEAK_HPP
#define ESPEAK_HPP

#include <string>

#include "language_description.hpp"
#include "mbrola.hpp"

// Espeak backend.
class espeak: public speech_engine
{
public:
  // Object construction:
  espeak(const configuration& conf, const std::string& lang);

private:
  // Construct new language description object:
  static language_description* make_language(const std::string& lang);

  // Make up voice parameters for backend:
  void voicify(double rate, double pitch = 1.0);
};

// Espeak+Mbrola backend.
class mbrespeak: public mbrola
{
public:
  // Object construction:
  mbrespeak(const configuration& conf, const std::string& lang);

private:
  // Construct new language description object:
  static language_description* make_language(const std::string& lang);
};

#endif
