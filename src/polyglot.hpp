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

// Main purpose of the polyglot class is to provide general interface
// to the multilingual speech synthesis functionality.

#ifndef POLYGLOT_HPP
#define POLYGLOT_HPP

#include <string>
#include <vector>

#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include "config.hpp"
#include "speech_engine.hpp"

class polyglot
{
public:
  // Construct the object:
  polyglot(const configuration& conf);

  // Prepare speech task:
  speech_task text_task(const std::wstring& s,
                        bool use_translation = false);
  speech_task text_task(const std::wstring& s,
                        double volume, double rate,
                        double pitch, double deviation,
                        bool use_translation = false);
  speech_task letter_task(const std::wstring& s);

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

private:
  // Detect language from text content:
  void detect_language(const std::wstring& s, bool check_translation = false);

  // Construct speech backend by name:
  speech_engine* speech_backend(const std::string& name,
                                const std::string& lang,
                                const configuration& conf);

  // Multilingual speech engine:
  std::vector< boost::shared_ptr<speech_engine> > talker;

  // Currently used language:
  unsigned int lang;

  // Language autodetection enabling flag:
  bool autolanguage;

  // Supported languages:
  static const std::vector<std::string> langs;
};

#endif
