// config.hpp -- Configuration options access
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

#ifndef MULTISPEECH_CONFIG_HPP
#define MULTISPEECH_CONFIG_HPP

#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "soundfile.hpp"


namespace multispeech
{

// Language id strings:
namespace lang_id
{
  extern const char* const en;
  extern const char* const ru;
  extern const char* const autodetect;
};

// Speech backend names:
namespace speaker
{
  extern const char* const mbrola;
  extern const char* const espeak;
  extern const char* const freephone;
  extern const char* const ru_tts;
  extern const char* const user_defined;
};

// This class is for deriving by the main server class.
// It provides configuration parser and options value storage
// and protects from duplicated server initialization.
class configuration
{
protected:
  // The constructor reads and parses configuration by the way.
  // it's argument are usually transferred from the main().
  configuration(int argc, char* argv[]);

public:
  // Options value container:
  static boost::program_options::variables_map option_value;

  // Options value accessors for common use:
  static const char* input_charset(void);
  static bool native_notation(void);
  static bool dtk_notation(void);
  static unsigned int tcp_port(void);
  static const std::string& sounds_repository(void);
  static bool split_multiline_messages(void);
  static const std::string& default_audio_device(void);
  static float general_volume(void);
  static float audio_latency(void);
  static const std::string& sound_output_device(void);
  static float sound_volume(void);
  static bool sound_async(void);
  static const std::string& beep_device(void);
  static float beep_volume(void);
  static unsigned int beep_sampling(void);
  static bool beep_async(void);
  static const std::string& speech_output_device(void);
  static float speech_volume(const std::string& voice = "");
  static float speech_rate(const std::string& voice = "");
  static float voice_pitch(const std::string& voice = "");
  static float char_rate(void);
  static float char_pitch(void);
  static float caps_factor(void);
  static const std::string& default_language(void);
  static const std::string& speech_backend(const std::string& language);
  static const std::string& backend_voice(const std::string& backend, const std::string& language);
  static const std::string& backend_executable(const std::string& backend);
  static const std::string& mbrola_voices(void);
  static const std::string& backend_lexicon(const std::string& backend);
  static const std::string& backend_log(const std::string& backend);
  static const std::string& user_tts_command(void);
  static bool user_freq_control(void);
  static soundfile::format user_sound_format(const std::string& language);
  static unsigned int user_sampling(const std::string& language);
  static unsigned int user_sound_channels(const std::string& language);
  static const char* user_tts_charset(const std::string& language);
  static const char* user_tts_dialect(const std::string& language);
  static float user_tts_acceleration(const std::string& voice);

  // What we are parsing now:
  static std::string stage;

  // Error signaling:
  class error: public std::logic_error
  {
  public:
    error(const std::string& message);
  };

private:
  // Option name composer:
  class option: public std::string
  {
  public:
    // Remember option name:
    explicit option(const std::string& name);

    // Compose full name relative to specified section:
    option operator()(const std::string& section);

    // Access name as C-string:
    operator const char*(void);

    // Relative option names:
    static option charset;
    static option native_notation;
    static option dtk_notation;
    static option port;
    static option sounds;
    static option split_multiline_messages;
    static option device;
    static option latency;
    static option asynchronous;
    static option engine;
    static option executable;
    static option command;
    static option format;
    static option sampling;
    static option stereo;
    static option freq_control;
    static option voices;
    static option lexicon;
    static option log;
    static option language;
    static option voice;
    static option voice_name;
    static option dialect;
    static option volume;
    static option pitch;
    static option rate;
    static option caps_factor;
    static option acceleration;
  };

  // General sections:
  static const std::string frontend;
  static const std::string ssip;
  static const std::string audio;
  static const std::string sounds;
  static const std::string tones;
  static const std::string speech;
  static const std::string character;

  // Read configuration file:
  void read(const boost::filesystem::path& config_file,
            const boost::program_options::options_description& conf);

  // Hardcoded default paths:
  static const boost::filesystem::path mbrola_voices_default;
  static const boost::filesystem::path enlex_default;
  static const boost::filesystem::path rulex_default;
  static const boost::filesystem::path sounds_default;
};

} // namespace multispeech

#endif
