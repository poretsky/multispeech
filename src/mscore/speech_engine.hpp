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
// deviate -- if it is true, frequency deviation will be done
// on the playing stage. Otherwise the TTS pipeline
// should take care of it.

#ifndef MULTISPEECH_SPEECH_ENGINE_HPP
#define MULTISPEECH_SPEECH_ENGINE_HPP

#include <string>
#include <locale>
#include <list>
#include <map>

#include "soundfile.hpp"
#include "loudspeaker.hpp"
#include "language_description.hpp"

namespace multispeech
{

class speech_engine
{
protected:
  // Object constructor:
  speech_engine(const std::string& backend, bool deviate);

public:
  // Destructor should be public to accommodate smart pointers:
  virtual ~speech_engine(void);

  // General attributes:
  const std::string name;

  // Voice and speech parameters control:
  static void volume(double value = 1.0);
  static void voice_pitch(double value = 1.0);
  static void speech_rate(double value = 1.0);
  static void sampling_deviation(double value = 1.0);
  static void char_voice_pitch(double value = 1.0);
  static void char_speech_rate(double value = 1.0);

  // Voice switching:
  void voice_setup(const std::string& voice_name);
  bool voice_available(const std::string& voice_name);

  // Prepare speech task:
  speech_task text_task(const std::wstring& s,
                        bool use_translation = false);
  speech_task text_task(const std::wstring& s,
                        double volume, double rate,
                        double pitch, double deviation,
                        bool use_translation = false,
                        bool allpuncts = false);
  speech_task letter_task(std::wstring s);

  // Make up special task to produce silence for the time
  // duration specified in seconds:
  speech_task silence(double duration);

  // Pointer to the current language preprocessor:
  language_description* language;

protected:
  // Voice attributes mapping:
  struct voice_attributes
  {
    const std::string charset;
    const std::string language;
    const char* dialect;
    const char* id;
    soundfile::format format;
    unsigned int sampling;
    unsigned int channels;
  };
  std::map<const std::string, const voice_attributes*> voices;

  // Add a new command pattern to the beginning of the list:
  void command(const std::string& pattern);

  // Format substitutions to construct actual command:
  std::map<const std::string, std::string> format_macros;

  // Pointer to the currently used voice attributes:
  const voice_attributes* current_voice;

private:
  // Map input charsets by name:
  typedef std::map<const std::string, std::locale> charset_map;

  // Text encoders constructed by demand:
  static charset_map encoders;

  // General voice and speech parameters.
  // It is assumed that normal value is 1.0.
  static double general_volume, general_pitch, general_rate;
  static double char_pitch, char_rate;
  static double general_deviation;

  // Voice specific speech parameters that are set up
  // from configuration at the voice assigning time:
  double volume_factor, rate_factor, pitch_factor, acceleration;

  // Deviate sampling frequency by player:
  bool playing_deviation;

  // Shell command patterns to make up a TTS script:
  std::list<std::string> command_patterns;

  // Check actual voice availability. Fake implementation.
  virtual bool check_voice(const std::string& voice_name);

  // Make up voice parameters for backend.
  // Should be defined in derived classes for particular speech engines.
  virtual void voicify(double rate, double pitch = 1.0) = 0;
};

} // namespace multispeech

#endif
