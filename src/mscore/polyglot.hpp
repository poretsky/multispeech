// polyglot.hpp -- General text-to-speech transfer control interface
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

// The main purpose of the polyglot class is to provide multilingual
// speech synthesis functionality to the derived server class.

#ifndef MULTISPEECH_POLYGLOT_HPP
#define MULTISPEECH_POLYGLOT_HPP

#include <string>
#include <vector>
#include <locale>

#include "speech_engine.hpp"

namespace multispeech
{

class polyglot
{
protected:
  // Construct the object:
  polyglot(void);

public:
  // Prepare speech task:
  speech_task text_task(const std::string& s,
                        bool use_translation = false);
  speech_task text_task(const std::string& s,
                        double volume, double rate,
                        double pitch, double deviation,
                        bool use_translation = false);
  speech_task letter_task(const std::string& s);

  // Make up special task to produce silence for the time
  // duration specified in seconds:
  speech_task silence(double duration);

  // Choose language:
  void language(const std::string& id);

  // Return current language id string:
  const char* language(void) const;

  // Switch language in specified direction:
  // true -- forward, false -- backward.
  void lang_switch(bool direction);

  // Set up input charset by name:
  void charset(const char* name);

private:
  // Detect language from text content:
  void detect_language(const std::wstring& s, bool check_translation = false);

  // Construct speech backend by name:
  speech_engine* speech_backend(const std::string& name,
                                const std::string& lang);

  // Multilingual speech engine:
  std::vector<speech_engine*> talker;

  // Currently used language:
  unsigned int lang;

  // Language autodetection enabling flag:
  bool autolanguage;

  // Supported languages:
  static const std::vector<std::string> langs;

  // Input charset holder:
  std::locale input_charset;
};

} // namespace multispeech

#endif
