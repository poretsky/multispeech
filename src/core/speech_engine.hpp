// speech_engine.hpp -- Base speech engine interface
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

// The speech_engine class is a base from which all speech backend
// interface classes are derived. It contains a bunch of static methods
// providing general voice and speech control along with the dynamic
// ones to make up an appropriate loudspeaker task. This class also
// takes care about constructing external TTS pipeline from the shell
// command patterns that should be defined in the derived classes
// by using protected method command() in their constructors.
// There are several special macros that can be used in such patterns.
// These macros are recognized and replaced by actual values
// on a pipeline construction stage. Use following syntax
// to assign value to a macro:
//
// format_macro["name"] = value;
//
// Macro values should be represented by strings.
//
// Two such macro substitutions are defined by the methods of this class:
// "%lang" is replaced by the actual language id;
// "%freq" is replaced by the generated sound stream sampling frequency.
//
// Yet two macro substitutions are expected to be defined in the virtual
// method voicify() that must be implemented in derived classes:
// "%pitch" and "%rate". These macro specifications
// are to be replaced by the values defining voice pitch
// and speech rate in the backend's sensible way.
//
// Additional macros can be easily added by derived classes if needed.
// All necessary information for it can be obtained from the respective
// data members placed in the protected section (accessible for descendants).
//
// The class constructor arguments are as follows:
// backend -- TTS engine name;
// voice_id -- voice name;
// lang -- language id;
// fmt -- generated sound stream format;
// sampling -- generated sound stream native sampling frequency;
// channels -- number of channels in the sound stream (1 or 2);
// deviate -- if it is true, frequency deviation will be done
// on the playing stage. Otherwise the TTS pipeline
// should take care of it;
// charset -- backend native charset specification.

#ifndef MULTISPEECH_SPEECH_ENGINE_HPP
#define MULTISPEECH_SPEECH_ENGINE_HPP

#include <string>
#include <list>
#include <map>

#include <boost/scoped_ptr.hpp>

#include "soundfile.hpp"
#include "loudspeaker.hpp"
#include "language_description.hpp"
#include "voice_params.hpp"
#include "text_filter.hpp"

class speech_engine
{
protected:
  // Object constructor:
  speech_engine(const std::string& backend,
                const std::string& voice_id,
                const char* lang,
                soundfile::format fmt,
                unsigned int sampling,
                unsigned int channels,
                bool deviate,
                const std::string& charset = "");

public:
  // Destructor should be public to accommodate smart pointers:
  virtual ~speech_engine(void);

  // General attributes:
  const std::string name;
  const std::string& voice;

  // Voice and speech parameters control:
  static void volume(double value = 1.0);
  static void voice_pitch(double value = 1.0);
  static void speech_rate(double value = 1.0);
  static void sampling_deviation(double value = 1.0);
  static void char_voice_pitch(double value = 1.0);
  static void char_speech_rate(double value = 1.0);

  // Dummy methods (unimplemented yet)
  static void split_caps_mode(bool value = false);
  static void capitalize_mode(bool value = false);
  static void space_special_chars_mode(bool value = false);

  // Prepare speech task:
  speech_task text_task(const std::wstring& s,
                        bool use_translation = false);
  speech_task text_task(const std::wstring& s,
                        voice_params* voice,
                        bool use_translation = false,
                        bool allpuncts = false);
  speech_task letter_task(std::wstring s);
  speech_task letter_task(std::wstring s, voice_params* voice);

  // Make up special task to produce silence for the time
  // duration specified in seconds:
  speech_task silence(double duration);

  // Language specific stuff:
  boost::scoped_ptr<language_description> language;

  // Special value for disabling language:
  static const char* const disabled;

  // Empty string for no specific voice designation:
  static const std::string novoice;

protected:
  // Add a new command pattern to the beginning of the list:
  void command(const std::string& pattern);

  // Make up voice parameters for backend. The base implementation
  // passes them as is. If a backend needs some special treatment
  // this method should be redefined in derived classes.
  virtual void voicify(double rate, double pitch = 1.0);

  // Change native sampling frequency:
  void sampling(unsigned int value);

  // Backend specific text preparation:
  text_filter extra_fixes;

  // Format substitutions to construct actual command:
  std::map<const char*, std::string> format_macros;

  // command pattern macros:
  static const char* const lang_macro;
  static const char* const pitch_macro;
  static const char* const rate_macro;
  static const char* const freq_macro;

  // General options:
  static bool split_caps, capitalize, space_special_chars;

  // Utility function for choosing voice for language from provided map:
  static const std::string& getvoiceid(const char* lang, const std::map<const char*, const std::string*>& voices);

private:
  // Generated sound stream sample format:
  soundfile::format format;

  // Native sampling frequency in Hz:
  unsigned int native_sampling;

  // Generated sound stream channels number:
  unsigned int sound_channels;

  // Deviate sampling frequency by player:
  bool playing_deviation;

  // Output charset:
  const std::string backend_charset;

  // Shell command patterns to make up a TTS script:
  std::list<std::string> command_patterns;

  // Construct speech task according to specified parameters:
  speech_task wrap_text(const std::wstring& s,
                        double volume, double rate,
                        double pitch, double deviation,
                        bool use_translation = false,
                        bool allpuncts = false);
  speech_task wrap_letter(std::wstring s,
                          double volume, double rate,
                          double pitch, double deviation);
};

#endif
