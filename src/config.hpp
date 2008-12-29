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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <exception>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>


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
};

// Various option names used in sections:
namespace option_name
{
  extern const char* const engine;
  extern const char* const executable;
  extern const char* const sampling;
  extern const char* const charset;
  extern const char* const voices;
  extern const char* const lexicon;
  extern const char* const log;
  extern const char* const volume;
  extern const char* const pitch;
  extern const char* const rate;
  extern const char* const char_pitch;
  extern const char* const char_rate;
  extern const char* const caps_factor;
  extern const char* const acceleration;
};

// Options hierarchy:
namespace options
{
  // Frontend related options:
  namespace frontend
  {
    extern const char* const charset;
    extern const char* const interface;
  };

  // General audio output:
  namespace audio
  {
    extern const char* const general_volume;
    extern const char* const latency;
  };

  // Sound files playing section:
  namespace sounds
  {
    extern const char* const device;
    extern const char* const volume;
    extern const char* const asynchronous;
  };

  // tones producing section:
  namespace tones
  {
    extern const char* const device;
    extern const char* const volume;
    extern const char* const sampling;
    extern const char* const asynchronous;
  };

  // Speech synthesis section:
  namespace speech
  {
    // General speech control options:
    extern const char* const device;
    extern const char* const volume;
    extern const char* const language;
  };

  // The next options are declared as strings to be dynamically constructable.

  // English speech section:
  namespace en
  {
    extern const std::string engine;
    extern const std::string volume;
    extern const std::string pitch;
    extern const std::string rate;
    extern const std::string acceleration;

    // Single letters pronunciation parameters:
    extern const std::string char_pitch;
    extern const std::string char_rate;
    extern const std::string caps_factor;
  };

  // Russian speech section:
  namespace ru
  {
    extern const std::string engine;
    extern const std::string volume;
    extern const std::string pitch;
    extern const std::string rate;
    extern const std::string acceleration;

    // Single letters pronunciation parameters:
    extern const std::string char_pitch;
    extern const std::string char_rate;
    extern const std::string caps_factor;
  };

  // Mbrola based TTS backends options:
  namespace mbrola
  {
    extern const std::string executable;
    extern const std::string voices;
  };

  // Espeak based TTS backends options:
  namespace espeak
  {
    extern const std::string executable;

    // Espeak voices assignment:
    extern const std::string en;
    extern const std::string ru;

    // Mbrola voices assignment:
    namespace mbrola
    {
      extern const std::string en;
    };
  };

  // Freephone TTS backend options:
  namespace freephone
  {
    extern const std::string executable;
    extern const std::string lexicon;
  };

  // Ru_tts speech backend options:
  namespace ru_tts
  {
    extern const std::string executable;
    extern const std::string lexicon;
    extern const std::string log;
  };

  // Dynamic options name composing:
  extern const std::string compose(const std::string& section,
                                   const std::string& option);
};

// Configuration parser and options value storage
class configuration
{
public:
  // The constructor reads and parses configuration by the way
  configuration(void);

  // Options value container:
  boost::program_options::variables_map option_value;

private:
  // Read configuration file:
  void read(const boost::filesystem::path& config_file,
            const boost::program_options::options_description& conf);

  // Configuration files:
  static const boost::filesystem::path global_conf, local_conf;
};

class configuration_error: public std::exception
{
public:
  configuration_error(const std::string& message) throw();
  ~configuration_error(void) throw();

  const char* what(void) const throw();

private:
  const std::string reason;
};

#endif