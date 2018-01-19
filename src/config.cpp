// config.cpp -- Configuration parsing and options storing implementation
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

#include <sysconfig.h>

#include <cstdlib>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>

#include "config.hpp"

#include "audioplayer.hpp"
#include "file_player.hpp"
#include "tone_generator.hpp"
#include "loudspeaker.hpp"
#include "speech_engine.hpp"
#include "server.hpp"

using namespace std;
using namespace boost::filesystem3;
using namespace boost::program_options;


// Configuration files:

const path configuration::global_conf(complete("multispeech.conf", SYSCONF_DIR));
const path configuration::local_conf(complete(".multispeechrc", getenv("HOME")));


// Hardcoded default paths:

const path configuration::mbrola_voices_default(complete("mbrola", DATA_DIR));
const path configuration::enlex_default(complete("freespeech/enlex.db", DATA_DIR));
const path configuration::rulex_default(complete("freespeech/rulex.db", DATA_DIR));

// Language id strings:
namespace lang_id
{
  const char* const en = "en";
  const char* const ru = "ru";
  const char* const de = "de";
  const char* const fr = "fr";
  const char* const es = "es";
  const char* const br = "br";
  const char* const it = "it";
  const char* const autodetect = "autodetect";
};

// Speech backend names:
namespace speaker
{
  const char* const mbrola = "mbrola";
  const char* const espeak = "espeak";
  const char* const freephone = "freephone";
  const char* const ru_tts = "ru_tts";
  const char* const user = "user";
};

// Various option names used in sections:
namespace option_name
{
  const char* const engine = "engine";
  const char* const executable = "executable";
  const char* const command = "command";
  const char* const format = "format";
  const char* const sampling = "sampling";
  const char* const stereo = "stereo";
  const char* const freq_control = "freq_control";
  const char* const charset = "charset";
  const char* const voices = "voices";
  const char* const lexicon = "lexicon";
  const char* const log = "log";
  const char* const volume = "volume";
  const char* const pitch = "pitch";
  const char* const rate = "rate";
  const char* const char_pitch = "char_pitch";
  const char* const char_rate = "char_rate";
  const char* const caps_factor = "caps_factor";
  const char* const acceleration = "acceleration";
};


// Option names definition
namespace options
{
  // Frontend related options:
  namespace frontend
  {
    const char* const charset = "frontend.charset";
    const char* const native_voices = "frontend.native_voices";
    const char* const dtk_voices = "frontend.dtk_voices";
  };

  // General audio output options:
  namespace audio
  {
    const char* const device = "audio.device";
    const char* const general_volume = "audio.general_volume";
    const char* const latency = "audio.latency";
  };

  // Sound files playing section:
  namespace sounds
  {
    const char* const device = "sounds.device";
    const char* const volume = "sounds.volume";
    const char* const asynchronous = "sounds.asynchronous";
  };

  // tones producing section:
  namespace tones
  {
    const char* const device = "tones.device";
    const char* const volume = "tones.volume";
    const char* const sampling = "tones.sampling";
    const char* const asynchronous = "tones.asynchronous";
  };

  // Speech section:
  namespace speech
  {
    // General speech control options:
    const char* const device = "speech.device";
    const char* const volume = "speech.volume";
    const char* const language = "speech.language";
    const char* const fallback = "speech.fallback";
  };

  // English speech section:
  namespace en
  {
    const string engine(compose(lang_id::en, option_name::engine));
    const string volume(compose(lang_id::en, option_name::volume));
    const string pitch(compose(lang_id::en, option_name::pitch));
    const string rate(compose(lang_id::en, option_name::rate));
    const string acceleration(compose(lang_id::en, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::en, option_name::char_pitch));
    const string char_rate(compose(lang_id::en, option_name::char_rate));
    const string caps_factor(compose(lang_id::en, option_name::caps_factor));
  };

  // Russian speech section:
  namespace ru
  {
    const std::string engine(compose(lang_id::ru, option_name::engine));
    const string volume(compose(lang_id::ru, option_name::volume));
    const string pitch(compose(lang_id::ru, option_name::pitch));
    const string rate(compose(lang_id::ru, option_name::rate));
    const string acceleration(compose(lang_id::ru, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::ru, option_name::char_pitch));
    const string char_rate(compose(lang_id::ru, option_name::char_rate));
    const string caps_factor(compose(lang_id::ru, option_name::caps_factor));
  };

  // German speech section:
  namespace de
  {
    const string engine(compose(lang_id::de, option_name::engine));
    const string volume(compose(lang_id::de, option_name::volume));
    const string pitch(compose(lang_id::de, option_name::pitch));
    const string rate(compose(lang_id::de, option_name::rate));
    const string acceleration(compose(lang_id::de, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::de, option_name::char_pitch));
    const string char_rate(compose(lang_id::de, option_name::char_rate));
    const string caps_factor(compose(lang_id::de, option_name::caps_factor));
  };

  // French speech section:
  namespace fr
  {
    const string engine(compose(lang_id::fr, option_name::engine));
    const string volume(compose(lang_id::fr, option_name::volume));
    const string pitch(compose(lang_id::fr, option_name::pitch));
    const string rate(compose(lang_id::fr, option_name::rate));
    const string acceleration(compose(lang_id::fr, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::fr, option_name::char_pitch));
    const string char_rate(compose(lang_id::fr, option_name::char_rate));
    const string caps_factor(compose(lang_id::fr, option_name::caps_factor));
  };

  // Spanish speech section:
  namespace es
  {
    const string engine(compose(lang_id::es, option_name::engine));
    const string volume(compose(lang_id::es, option_name::volume));
    const string pitch(compose(lang_id::es, option_name::pitch));
    const string rate(compose(lang_id::es, option_name::rate));
    const string acceleration(compose(lang_id::es, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::es, option_name::char_pitch));
    const string char_rate(compose(lang_id::es, option_name::char_rate));
    const string caps_factor(compose(lang_id::es, option_name::caps_factor));
  };

  // Brazilian speech section:
  namespace br
  {
    const string engine(compose(lang_id::br, option_name::engine));
    const string volume(compose(lang_id::br, option_name::volume));
    const string pitch(compose(lang_id::br, option_name::pitch));
    const string rate(compose(lang_id::br, option_name::rate));
    const string acceleration(compose(lang_id::br, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::br, option_name::char_pitch));
    const string char_rate(compose(lang_id::br, option_name::char_rate));
    const string caps_factor(compose(lang_id::br, option_name::caps_factor));
  };

  // Italian speech section:
  namespace it
  {
    const string engine(compose(lang_id::it, option_name::engine));
    const string volume(compose(lang_id::it, option_name::volume));
    const string pitch(compose(lang_id::it, option_name::pitch));
    const string rate(compose(lang_id::it, option_name::rate));
    const string acceleration(compose(lang_id::it, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::it, option_name::char_pitch));
    const string char_rate(compose(lang_id::it, option_name::char_rate));
    const string caps_factor(compose(lang_id::it, option_name::caps_factor));
  };


  // Mbrola based backends options:
  namespace mbrola
  {
    const string executable(compose(speaker::mbrola, option_name::executable));
    const string voices(compose(speaker::mbrola, option_name::voices));
  };

  // Espeak based backends options:
  namespace espeak
  {
    const string executable(compose(speaker::espeak, option_name::executable));

    // Espeak voices assignment:
    const string en(compose(speaker::espeak, lang_id::en));
    const string ru(compose(speaker::espeak, lang_id::ru));
    const string de(compose(speaker::espeak, lang_id::de));
    const string fr(compose(speaker::espeak, lang_id::fr));
    const string es(compose(speaker::espeak, lang_id::es));
    const string br(compose(speaker::espeak, lang_id::br));
    const string it(compose(speaker::espeak, lang_id::it));

    // Mbrola voices assignment:
    namespace mbrola
    {
      const string en(compose(speaker::espeak, compose(speaker::mbrola, lang_id::en)));
      const string de(compose(speaker::espeak, compose(speaker::mbrola, lang_id::de)));
      const string fr(compose(speaker::espeak, compose(speaker::mbrola, lang_id::fr)));
      const string es(compose(speaker::espeak, compose(speaker::mbrola, lang_id::es)));
      const string br(compose(speaker::espeak, compose(speaker::mbrola, lang_id::br)));
      const string it(compose(speaker::espeak, compose(speaker::mbrola, lang_id::it)));
    };
  };

  // Freephone backend options:
  namespace freephone
  {
    const string executable(compose(speaker::freephone, option_name::executable));
    const string lexicon(compose(speaker::freephone, option_name::lexicon));
  };

  // Ru_tts backend options:
  namespace ru_tts
  {
    const string executable(compose(speaker::ru_tts, option_name::executable));
    const string lexicon(compose(speaker::ru_tts, option_name::lexicon));
    const string log(compose(speaker::ru_tts, option_name::log));
  };

  // User defined TTS backend options:
  namespace user
  {
    const string command(compose(speaker::user, option_name::command));
    const string format(compose(speaker::user, option_name::format));
    const string sampling(compose(speaker::user, option_name::sampling));
    const string stereo(compose(speaker::user, option_name::stereo));
    const string freq_control(compose(speaker::user, option_name::freq_control));
    const string charset(compose(speaker::user, option_name::charset));
  };

  // Dynamic options name composing:
  const string compose(const string& section, const string& option)
  {
    return section + '.' + option;
  }
};


// Parsing and applying configuration options

string configuration::stage;

configuration::configuration(int argc, char* argv[])
{
  options_description conf, cl_desc("Available options");
  variables_map cl_opt;
  bool noconf = true;

  // Declare command line options:
  cl_desc.add_options()
    ("help,h", "produce help message and exit")
    ("config,c", value<string>(), "read configuration from specified file")
    ("debug,d", "log debug information")
    ("verbose,v", "print diagnostic messages on stderr")
    ("version,V", "print program version and exit");

  // Parse command line:
  stage = " in command line";
  store(parse_command_line(argc, argv, cl_desc), cl_opt);
  stage.erase();
  notify(cl_opt);
  if (cl_opt.count("help"))
    {
      ostringstream usage;
      usage << "Usage: " << argv[0] << " [options]\n" << cl_desc;
      throw usage.str();
    }
  if (cl_opt.count("version"))
    throw string(PACKAGE_STRING);
  if (cl_opt.count("debug"))
    server::debug = true;
  if (cl_opt.count("verbose"))
    server::verbose = true;

  // Declare configuration options:
  using namespace options;
  conf.add_options()

    // Frontend related options:
    (frontend::charset, value<string>())
    (frontend::native_voices, bool_switch()->default_value(true))
    (frontend::dtk_voices, bool_switch()->default_value(false))

    // General audio options:
    (audio::device, value<string>()->default_value(""))
    (audio::general_volume, value<float>(&audioplayer::general_volume)->default_value(0.8))
    (audio::latency, value<double>(&audioplayer::suggested_latency)->default_value(0.05))

    // Sound files playing section:
    (sounds::device, value<string>()->default_value(""))
    (sounds::volume, value<float>(&file_player::relative_volume)->default_value(1.0))
    (sounds::asynchronous, bool_switch(&file_player::asynchronous)->default_value(true))

    // tones producing section:
    (tones::device, value<string>()->default_value(""))
    (tones::volume, value<float>(&tone_generator::relative_volume)->default_value(1.0))
    (tones::sampling, value<unsigned int>(&tone_generator::sampling)->default_value(44100))
    (tones::asynchronous, bool_switch(&tone_generator::asynchronous)->default_value(true))

    // General speech control options:
    (speech::device, value<string>()->default_value(""))
    (speech::volume, value<float>(&loudspeaker::relative_volume)->default_value(1.0))
    (speech::language, value<string>())
    (speech::fallback, value<string>()->default_value(lang_id::en))

    // English speech options:
    (en::engine.c_str(), value<string>())
    (en::volume.c_str(), value<double>()->default_value(1.0))
    (en::pitch.c_str(), value<double>()->default_value(1.0))
    (en::rate.c_str(), value<double>()->default_value(1.0))
    (en::acceleration.c_str(), value<double>()->default_value(0.0))
    (en::char_pitch.c_str(), value<double>()->default_value(1.0))
    (en::char_rate.c_str(), value<double>()->default_value(1.0))
    (en::caps_factor.c_str(), value<double>()->default_value(1.2))

    // Russian speech options:
    (ru::engine.c_str(), value<string>())
    (ru::volume.c_str(), value<double>()->default_value(1.0))
    (ru::pitch.c_str(), value<double>()->default_value(1.0))
    (ru::rate.c_str(), value<double>()->default_value(1.0))
    (ru::acceleration.c_str(), value<double>()->default_value(0.0))
    (ru::char_pitch.c_str(), value<double>()->default_value(1.0))
    (ru::char_rate.c_str(), value<double>()->default_value(1.0))
    (ru::caps_factor.c_str(), value<double>()->default_value(1.2))

    // German speech options:
    (de::engine.c_str(), value<string>())
    (de::volume.c_str(), value<double>()->default_value(1.0))
    (de::pitch.c_str(), value<double>()->default_value(1.0))
    (de::rate.c_str(), value<double>()->default_value(1.0))
    (de::acceleration.c_str(), value<double>()->default_value(0.0))
    (de::char_pitch.c_str(), value<double>()->default_value(1.0))
    (de::char_rate.c_str(), value<double>()->default_value(1.0))
    (de::caps_factor.c_str(), value<double>()->default_value(1.2))

    // French speech options:
    (fr::engine.c_str(), value<string>())
    (fr::volume.c_str(), value<double>()->default_value(1.0))
    (fr::pitch.c_str(), value<double>()->default_value(1.0))
    (fr::rate.c_str(), value<double>()->default_value(1.0))
    (fr::acceleration.c_str(), value<double>()->default_value(0.0))
    (fr::char_pitch.c_str(), value<double>()->default_value(1.0))
    (fr::char_rate.c_str(), value<double>()->default_value(1.0))
    (fr::caps_factor.c_str(), value<double>()->default_value(1.2))

    // Spanish speech options:
    (es::engine.c_str(), value<string>())
    (es::volume.c_str(), value<double>()->default_value(1.0))
    (es::pitch.c_str(), value<double>()->default_value(1.0))
    (es::rate.c_str(), value<double>()->default_value(1.0))
    (es::acceleration.c_str(), value<double>()->default_value(0.0))
    (es::char_pitch.c_str(), value<double>()->default_value(1.0))
    (es::char_rate.c_str(), value<double>()->default_value(1.0))
    (es::caps_factor.c_str(), value<double>()->default_value(1.2))

    // Brazilian speech options:
    (br::engine.c_str(), value<string>())
    (br::volume.c_str(), value<double>()->default_value(1.0))
    (br::pitch.c_str(), value<double>()->default_value(1.0))
    (br::rate.c_str(), value<double>()->default_value(1.0))
    (br::acceleration.c_str(), value<double>()->default_value(0.0))
    (br::char_pitch.c_str(), value<double>()->default_value(1.0))
    (br::char_rate.c_str(), value<double>()->default_value(1.0))
    (br::caps_factor.c_str(), value<double>()->default_value(1.2))

    // Italian speech options:
    (it::engine.c_str(), value<string>())
    (it::volume.c_str(), value<double>()->default_value(1.0))
    (it::pitch.c_str(), value<double>()->default_value(1.0))
    (it::rate.c_str(), value<double>()->default_value(1.0))
    (it::acceleration.c_str(), value<double>()->default_value(0.0))
    (it::char_pitch.c_str(), value<double>()->default_value(1.0))
    (it::char_rate.c_str(), value<double>()->default_value(1.0))
    (it::caps_factor.c_str(), value<double>()->default_value(1.2))

    // Mbrola based backends options:
    (mbrola::executable.c_str(), value<string>()->default_value(speaker::mbrola))
    (mbrola::voices.c_str(), value<string>()->default_value(mbrola_voices_default.generic_string()))

    // Espeak based backends options:
    (espeak::executable.c_str(), value<string>()->default_value(speaker::espeak))

    // Espeak voices assignment:
    (espeak::en.c_str(), value<string>()->default_value(lang_id::en))
    (espeak::ru.c_str(), value<string>()->default_value(lang_id::ru))
    (espeak::de.c_str(), value<string>()->default_value(lang_id::de))
    (espeak::fr.c_str(), value<string>()->default_value(lang_id::fr))
    (espeak::es.c_str(), value<string>()->default_value(lang_id::es))
    (espeak::br.c_str(), value<string>()->default_value("pt"))
    (espeak::it.c_str(), value<string>()->default_value(lang_id::it))

    // Mbrola voices assignment:
    (espeak::mbrola::en.c_str(), value<string>()->default_value("en1"))
    (espeak::mbrola::de.c_str(), value<string>()->default_value("de6"))
    (espeak::mbrola::fr.c_str(), value<string>()->default_value("fr4"))
    (espeak::mbrola::es.c_str(), value<string>()->default_value("es1"))
    (espeak::mbrola::br.c_str(), value<string>()->default_value("br3"))
    (espeak::mbrola::it.c_str(), value<string>()->default_value("it3"))

    // Freephone backend options:
    (freephone::executable.c_str(), value<string>()->default_value(speaker::freephone))
    (freephone::lexicon.c_str(), value<string>()->default_value(enlex_default.generic_string()))

    // Ru_tts backend options:
    (ru_tts::executable.c_str(), value<string>()->default_value(speaker::ru_tts))
    (ru_tts::lexicon.c_str(), value<string>()->default_value(rulex_default.generic_string()))
    (ru_tts::log.c_str(), value<string>())

    // User defined TTS backend options:
    (user::command.c_str(), value<string>())
    (user::format.c_str(), value<string>())
    (user::sampling.c_str(), value<unsigned int>()->default_value(22050))
    (user::stereo.c_str(), bool_switch()->default_value(false))
    (user::freq_control.c_str(), bool_switch()->default_value(false))
    (user::charset.c_str(), value<string>()->default_value(""));

  // Parse config files and store values
  if (cl_opt.count("config"))
    {
      path extra_conf(cl_opt["config"].as<string>());
      if (exists(extra_conf))
        {
          read(extra_conf, conf);
          noconf = false;
        }
    }
  if (exists(local_conf))
    {
      read(local_conf, conf);
      noconf = false;
    }
  if (exists(global_conf))
    {
      read(global_conf, conf);
      noconf = false;
    }
  if (noconf)
    throw configuration::error("No configuration files found");
  notify(option_value);
}


// Configuration errors signaling:

configuration::error::error(const string& message):
  logic_error(message)
{
}


// Private members:

void
configuration::read(const path& config_file, const options_description& conf)
{
  boost::filesystem::ifstream source(config_file);
  stage = " in " + config_file.generic_string();
  store(parse_config_file(source, conf), option_value);
  stage.erase();
}
