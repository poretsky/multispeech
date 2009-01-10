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
// conf -- reference to the current configuration;
// backend -- TTS engine name;
// voice_id -- voice name. Special protected identifier "novoice"
// is used when actual name should be determined
// from the configuration data;
// lang -- language id;
// fmt -- generated sound stream format;
// sampling -- generated sound stream native sampling frequency;
// channels -- number of channels in the sound stream (1 or 2);
// deviate -- if it is true, frequency deviation will be done
// on the playing stage. Otherwise the TTS pipeline
// should take care of it.

#ifndef SPEECH_ENGINE_HPP
#define SPEECH_ENGINE_HPP

#include <memory>
#include <string>
#include <locale>
#include <list>
#include <map>

#include "config.hpp"
#include "soundfile.hpp"
#include "loudspeaker.hpp"
#include "language_description.hpp"

class speech_engine
{
public:
  // Constructing and destroying:
  speech_engine(const configuration& conf,
                const std::string& backend,
                const std::string& voice_id,
                const std::string& lang,
                soundfile::format fmt,
                unsigned int sampling,
                unsigned int channels,
                bool deviate,
                const char* charset = NULL);
  virtual ~speech_engine(void);

  // General attributes:
  const std::string name;
  const std::string voice;

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
                        double volume, double rate,
                        double pitch, double deviation,
                        bool use_translation = false);
  speech_task letter_task(std::wstring s);

  // Make up special task to produce silence for the time
  // duration specified in seconds:
  speech_task silence(double duration);

  // Language specific stuff:
  std::auto_ptr<language_description> language;

protected:
  // Add a new command pattern to the beginning of the list:
  void command(const std::string& pattern);

  // Format substitutions to construct actual command:
  std::map<const std::string, std::string> format_macros;

  // General options:
  static bool split_caps, capitalize, space_special_chars;

  // "No voice" string:
  static const std::string novoice;

private:
  // Common voice and speech parameters.
  // It is assumed that normal value is 1.0.
  static double persistent_volume, persistent_pitch;
  static double persistent_rate, persistent_deviation;
  static double persistent_char_pitch, persistent_char_rate;

  // Instance specific voice parameters that are set up
  // from configuration at the object construction time:
  double volume_factor, rate_factor, pitch_factor, caps_factor;
  double char_pitch, char_rate, acceleration;

  // Generated sound stream sample format:
  soundfile::format format;

  // Native sampling frequency in Hz:
  unsigned int native_sampling;

  // Generated sound stream channels number:
  unsigned int sound_channels;

  // Deviate sampling frequency by player:
  bool playing_deviation;

  // Output charset holder:
  const std::locale backend_charset;

  // Shell command patterns to make up a TTS script:
  std::list<std::string> command_patterns;

  // Make up voice parameters for backend.
  // Should be defined in derived classes for particular speech engines.
  virtual void voicify(double rate, double pitch = 1.0) = 0;
};

#endif
