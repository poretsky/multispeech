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

#ifndef MULTISPEECH_SYSCONFIG_H
#define MULTISPEECH_SYSCONFIG_H
#include <sysconfig.h>
#endif

// Package configuration data:
namespace package
{
  extern const char* const name = PACKAGE_NAME;
  extern const char* const string = PACKAGE_STRING;
  extern const char* const version = PACKAGE_VERSION;
  extern const char* const datadir = DATA_DIR;
};

#include <cstdlib>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>

#include <portaudiocpp/PortAudioCpp.hxx>

#include "config.hpp"

#include "audioplayer.hpp"
#include "file_player.hpp"
#include "tone_generator.hpp"
#include "loudspeaker.hpp"

#include "speech_engine.hpp"
#include "speech_server.hpp"

#include "mbrola.hpp"
#include "freephone.hpp"
#include "espeak.hpp"
#include "ru_tts.hpp"
#include "user_tts.hpp"

#include "polyglot.hpp"
#include "English.hpp"
#include "German.hpp"
#include "French.hpp"
#include "Italian.hpp"
#include "Spanish.hpp"
#include "Portuguese.hpp"
#include "Russian.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;
using namespace portaudio;


// Configuration files:

static const path global_conf(complete("multispeech.conf", SYSCONF_DIR));
static const path local_conf(complete(".multispeechrc", getenv("HOME")));


// Language id strings:
namespace lang_id
{
  const char* const en = "en";
  const char* const ru = "ru";
  const char* const de = "de";
  const char* const fr = "fr";
  const char* const es = "es";
  const char* const pt = "pt";
  const char* const it = "it";
  const char* const autodetect = "autodetect";
};

// Various option names used in sections:
namespace option_name
{
  const char* const priority = "priority";
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
  const char* const expressiveness = "expressiveness";
  const char* const female_voice = "female_voice";
  const char* const decimal_point = "decimal_point";
  const char* const decimal_comma = "decimal_comma";
  const char* const interclause_gap_factor = "interclause_gap_factor";
  const char* const comma_gap_factor = "comma_gap_factor";
  const char* const dot_gap_factor = "dot_gap_factor";
  const char* const semicolon_gap_factor = "semicolon_gap_factor";
  const char* const colon_gap_factor = "colon_gap_factor";
  const char* const question_gap_factor = "question_gap_factor";
  const char* const exclamation_gap_factor = "exclamation_gap_factor";
  const char* const intonational_gap_factor = "intonational_gap_factor";
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

  // Speech Dispatcher backend options:
  namespace spd
  {
    const char* const version = "spd.version";
    const char* const sound_icons = "spd.sound_icons";
    const char* const use_voice_language = "spd.use_voice_language";
    const char* const accept_explicit_language = "spd.accept_explicit_language";
    const char* const ignore_unknown_voice = "spd.ignore_unknown_voice";
    const char* const index_marks = "spd.index_marks";
  };

  // General audio output options:
  namespace audio
  {
    const char* const device = "audio.device";
    const char* const general_volume = "audio.general_volume";
    const char* const latency = "audio.latency";
    const char* const async_operation = "audio.async_operation";
    const char* const pulseaudio_direct = "audio.pulseaudio_direct";
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
    const string priority(compose(lang_id::en, option_name::priority));
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
    const string engine(compose(lang_id::ru, option_name::engine));
    const string priority(compose(lang_id::ru, option_name::priority));
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
    const string priority(compose(lang_id::de, option_name::priority));
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
    const string priority(compose(lang_id::fr, option_name::priority));
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
    const string priority(compose(lang_id::es, option_name::priority));
    const string volume(compose(lang_id::es, option_name::volume));
    const string pitch(compose(lang_id::es, option_name::pitch));
    const string rate(compose(lang_id::es, option_name::rate));
    const string acceleration(compose(lang_id::es, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::es, option_name::char_pitch));
    const string char_rate(compose(lang_id::es, option_name::char_rate));
    const string caps_factor(compose(lang_id::es, option_name::caps_factor));
  };

  // Portuguese speech section:
  namespace pt
  {
    const string engine(compose(lang_id::pt, option_name::engine));
    const string priority(compose(lang_id::pt, option_name::priority));
    const string volume(compose(lang_id::pt, option_name::volume));
    const string pitch(compose(lang_id::pt, option_name::pitch));
    const string rate(compose(lang_id::pt, option_name::rate));
    const string acceleration(compose(lang_id::pt, option_name::acceleration));

    // Single letters pronunciation parameters:
    const string char_pitch(compose(lang_id::pt, option_name::char_pitch));
    const string char_rate(compose(lang_id::pt, option_name::char_rate));
    const string caps_factor(compose(lang_id::pt, option_name::caps_factor));
  };

  // Italian speech section:
  namespace it
  {
    const string engine(compose(lang_id::it, option_name::engine));
    const string priority(compose(lang_id::it, option_name::priority));
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
    const string executable(compose(::mbrola::name, option_name::executable));
    const string voices(compose(::mbrola::name, option_name::voices));
  };

  // Espeak based backends options:
  namespace espeak
  {
    const string executable(compose(::espeak::name, option_name::executable));

    // Espeak voices assignment:
    const string en(compose(::espeak::name, lang_id::en));
    const string ru(compose(::espeak::name, lang_id::ru));
    const string de(compose(::espeak::name, lang_id::de));
    const string fr(compose(::espeak::name, lang_id::fr));
    const string es(compose(::espeak::name, lang_id::es));
    const string pt(compose(::espeak::name, lang_id::pt));
    const string it(compose(::espeak::name, lang_id::it));

    // Mbrola voices assignment:
    namespace mbrola
    {
      const string en(compose(::espeak::name, compose(::mbrola::name, lang_id::en)));
      const string de(compose(::espeak::name, compose(::mbrola::name, lang_id::de)));
      const string fr(compose(::espeak::name, compose(::mbrola::name, lang_id::fr)));
      const string es(compose(::espeak::name, compose(::mbrola::name, lang_id::es)));
      const string pt(compose(::espeak::name, compose(::mbrola::name, lang_id::pt)));
      const string it(compose(::espeak::name, compose(::mbrola::name, lang_id::it)));
    };
  };

  // Freephone backend options:
  namespace freephone
  {
    const string executable(compose(::freephone::name, option_name::executable));
    const string lexicon(compose(::freephone::name, option_name::lexicon));
  };

  // Ru_tts backend options:
  namespace ru_tts
  {
    const string executable(compose(::ru_tts::name, option_name::executable));
    const string lexicon(compose(::ru_tts::name, option_name::lexicon));
    const string log(compose(::ru_tts::name, option_name::log));
    const string expressiveness(compose(::ru_tts::name, option_name::expressiveness));
    const string female_voice(compose(::ru_tts::name, option_name::female_voice));
    const string decimal_point(compose(::ru_tts::name, option_name::decimal_point));
    const string decimal_comma(compose(::ru_tts::name, option_name::decimal_comma));
    const string interclause_gap_factor(compose(::ru_tts::name, option_name::interclause_gap_factor));
    const string comma_gap_factor(compose(::ru_tts::name, option_name::comma_gap_factor));
    const string dot_gap_factor(compose(::ru_tts::name, option_name::dot_gap_factor));
    const string semicolon_gap_factor(compose(::ru_tts::name, option_name::semicolon_gap_factor));
    const string colon_gap_factor(compose(::ru_tts::name, option_name::colon_gap_factor));
    const string question_gap_factor(compose(::ru_tts::name, option_name::question_gap_factor));
    const string exclamation_gap_factor(compose(::ru_tts::name, option_name::exclamation_gap_factor));
    const string intonational_gap_factor(compose(::ru_tts::name, option_name::intonational_gap_factor));
  };

  // User defined TTS backend options:
  namespace user
  {
    const string command(compose(user_tts::name, option_name::command));
    const string format(compose(user_tts::name, option_name::format));
    const string sampling(compose(user_tts::name, option_name::sampling));
    const string stereo(compose(user_tts::name, option_name::stereo));
    const string freq_control(compose(user_tts::name, option_name::freq_control));
    const string charset(compose(user_tts::name, option_name::charset));
  };

  // Dynamic options name composing:
  const string compose(const string& section, const string& option)
  {
    return section + '.' + option;
  }
};


// Parsing and applying configuration options

string configuration::stage;

configuration::configuration(int argc, char* argv[], bool is_spd_backend):
  spd_backend(is_spd_backend)
{
  options_description conf, cl_desc("Available options");
  positional_options_description args;
  variables_map option_value;
  bool noconf = true;
  ostringstream info;

  // Declare command line options:
  if (spd_backend)
    {
      cl_desc.add_options()
        ("config", value<string>());
      args.add("config", 1);
    }
  else
    {
      cl_desc.add_options()
        ("help,h", "produce help message and exit")
        ("list-devices,l", "print list of available audio output devices and exit")
        ("config,c", value<string>(), "read configuration from specified file")
        ("debug,d", "log debug information")
        ("verbose,v", "print diagnostic messages on stderr")
        ("version,V", "print program version and exit");
    }

  // Parse command line:
  stage = " in command line";
  store(command_line_parser(argc, argv).options(cl_desc).positional(args).run(), option_value);
  stage.erase();
  notify(option_value);

  if (option_value.count("version"))
    info << package::string << endl;
  if (option_value.count("help"))
    info << "Usage: " << argv[0] << " [options]" << endl << cl_desc;
  if (option_value.count("verbose"))
    speech_server::verbose = true;
  else speech_server::redirect_stderr();
  if (option_value.count("list-devices"))
    {
      AutoSystem audio;
      System& system = System::instance();
      System::DeviceIterator device;
      info << "Available audio devices:" << endl;
      for (device = system.devicesBegin(); device != system.devicesEnd(); ++device)
        if (!device->isInputOnlyDevice())
          info << "  " << audioplayer::canonical_name(*device) << endl;
    }
  if (!info.str().empty())
    throw info.str();
  if (option_value.count("debug"))
    speech_server::debug = true;

  // Declare configuration options:
  using namespace options;
  conf.add_options()

    // Frontend related options:
    (frontend::charset, value<string>(&speech_server::frontend_charset))
    (frontend::native_voices, bool_switch(&speech_server::support_native_voices)->default_value(true))
    (frontend::dtk_voices, bool_switch(&speech_server::support_dtk_voices)->default_value(false))

    // General audio options:
    (audio::device, value<string>(&audioplayer::device)->default_value(""))
    (audio::general_volume, value<float>(&audioplayer::general_volume)->default_value(0.8))
    (audio::latency, value<double>(&audioplayer::suggested_latency)->default_value(0.05))
    (audio::async_operation, bool_switch(&audioplayer::async)->default_value(false))
    (audio::pulseaudio_direct, bool_switch(&audioplayer::use_pa)->default_value(true))

    // Sound files playing section:
    (sounds::device, value<string>(&file_player::device)->default_value(""))
    (sounds::volume, value<float>(&file_player::relative_volume)->default_value(1.0))
    (sounds::asynchronous, bool_switch(&file_player::asynchronous)->default_value(true))

    // tones producing section:
    (tones::device, value<string>(&tone_generator::device)->default_value(""))
    (tones::volume, value<float>(&tone_generator::relative_volume)->default_value(1.0))
    (tones::sampling, value<unsigned int>(&tone_generator::sampling)->default_value(44100))
    (tones::asynchronous, bool_switch(&tone_generator::asynchronous)->default_value(true))

    // General speech control options:
    (speech::device, value<string>(&loudspeaker::device)->default_value(""))
    (speech::volume, value<float>(&loudspeaker::relative_volume)->default_value(1.0))
    (speech::language, value<string>(&polyglot::language_preference))
    (speech::fallback, value<string>(&polyglot::fallback_language)->default_value(lang_id::en))

    // English speech options:
    (en::engine.c_str(), value<string>(&English::settings.engine))
    (en::priority.c_str(), value<int>(&English::settings.priority)->default_value(0))
    (en::volume.c_str(), value<double>(&English::settings.volume)->default_value(1.0))
    (en::pitch.c_str(), value<double>(&English::settings.pitch)->default_value(1.0))
    (en::rate.c_str(), value<double>(&English::settings.rate)->default_value(1.0))
    (en::acceleration.c_str(), value<double>(&English::settings.acceleration)->default_value(0.0))
    (en::char_pitch.c_str(), value<double>(&English::settings.char_pitch)->default_value(1.0))
    (en::char_rate.c_str(), value<double>(&English::settings.char_rate)->default_value(1.0))
    (en::caps_factor.c_str(), value<double>(&English::settings.caps_factor)->default_value(1.2))

    // Russian speech options:
    (ru::engine.c_str(), value<string>(&Russian::settings.engine))
    (ru::priority.c_str(), value<int>(&Russian::settings.priority)->default_value(0))
    (ru::volume.c_str(), value<double>(&Russian::settings.volume)->default_value(1.0))
    (ru::pitch.c_str(), value<double>(&Russian::settings.pitch)->default_value(1.0))
    (ru::rate.c_str(), value<double>(&Russian::settings.rate)->default_value(1.0))
    (ru::acceleration.c_str(), value<double>(&Russian::settings.acceleration)->default_value(0.0))
    (ru::char_pitch.c_str(), value<double>(&Russian::settings.char_pitch)->default_value(1.0))
    (ru::char_rate.c_str(), value<double>(&Russian::settings.char_rate)->default_value(1.0))
    (ru::caps_factor.c_str(), value<double>(&Russian::settings.caps_factor)->default_value(1.2))

    // German speech options:
    (de::engine.c_str(), value<string>(&German::settings.engine))
    (de::priority.c_str(), value<int>(&German::settings.priority)->default_value(0))
    (de::volume.c_str(), value<double>(&German::settings.volume)->default_value(1.0))
    (de::pitch.c_str(), value<double>(&German::settings.pitch)->default_value(1.0))
    (de::rate.c_str(), value<double>(&German::settings.rate)->default_value(1.0))
    (de::acceleration.c_str(), value<double>(&German::settings.acceleration)->default_value(0.0))
    (de::char_pitch.c_str(), value<double>(&German::settings.char_pitch)->default_value(1.0))
    (de::char_rate.c_str(), value<double>(&German::settings.char_rate)->default_value(1.0))
    (de::caps_factor.c_str(), value<double>(&German::settings.caps_factor)->default_value(1.2))

    // French speech options:
    (fr::engine.c_str(), value<string>(&French::settings.engine))
    (fr::priority.c_str(), value<int>(&French::settings.priority)->default_value(0))
    (fr::volume.c_str(), value<double>(&French::settings.volume)->default_value(1.0))
    (fr::pitch.c_str(), value<double>(&French::settings.pitch)->default_value(1.0))
    (fr::rate.c_str(), value<double>(&French::settings.rate)->default_value(1.0))
    (fr::acceleration.c_str(), value<double>(&French::settings.acceleration)->default_value(0.0))
    (fr::char_pitch.c_str(), value<double>(&French::settings.char_pitch)->default_value(1.0))
    (fr::char_rate.c_str(), value<double>(&French::settings.char_rate)->default_value(1.0))
    (fr::caps_factor.c_str(), value<double>(&French::settings.caps_factor)->default_value(1.2))

    // Spanish speech options:
    (es::engine.c_str(), value<string>(&Spanish::settings.engine))
    (es::priority.c_str(), value<int>(&Spanish::settings.priority)->default_value(0))
    (es::volume.c_str(), value<double>(&Spanish::settings.volume)->default_value(1.0))
    (es::pitch.c_str(), value<double>(&Spanish::settings.pitch)->default_value(1.0))
    (es::rate.c_str(), value<double>(&Spanish::settings.rate)->default_value(1.0))
    (es::acceleration.c_str(), value<double>(&Spanish::settings.acceleration)->default_value(0.0))
    (es::char_pitch.c_str(), value<double>(&Spanish::settings.char_pitch)->default_value(1.0))
    (es::char_rate.c_str(), value<double>(&Spanish::settings.char_rate)->default_value(1.0))
    (es::caps_factor.c_str(), value<double>(&Spanish::settings.caps_factor)->default_value(1.2))

    // Portuguese speech options:
    (pt::engine.c_str(), value<string>(&Portuguese::settings.engine))
    (pt::priority.c_str(), value<int>(&Portuguese::settings.priority)->default_value(0))
    (pt::volume.c_str(), value<double>(&Portuguese::settings.volume)->default_value(1.0))
    (pt::pitch.c_str(), value<double>(&Portuguese::settings.pitch)->default_value(1.0))
    (pt::rate.c_str(), value<double>(&Portuguese::settings.rate)->default_value(1.0))
    (pt::acceleration.c_str(), value<double>(&Portuguese::settings.acceleration)->default_value(0.0))
    (pt::char_pitch.c_str(), value<double>(&Portuguese::settings.char_pitch)->default_value(1.0))
    (pt::char_rate.c_str(), value<double>(&Portuguese::settings.char_rate)->default_value(1.0))
    (pt::caps_factor.c_str(), value<double>(&Portuguese::settings.caps_factor)->default_value(1.2))

    // Italian speech options:
    (it::engine.c_str(), value<string>(&Italian::settings.engine))
    (it::priority.c_str(), value<int>(&Italian::settings.priority)->default_value(0))
    (it::volume.c_str(), value<double>(&Italian::settings.volume)->default_value(1.0))
    (it::pitch.c_str(), value<double>(&Italian::settings.pitch)->default_value(1.0))
    (it::rate.c_str(), value<double>(&Italian::settings.rate)->default_value(1.0))
    (it::acceleration.c_str(), value<double>(&Italian::settings.acceleration)->default_value(0.0))
    (it::char_pitch.c_str(), value<double>(&Italian::settings.char_pitch)->default_value(1.0))
    (it::char_rate.c_str(), value<double>(&Italian::settings.char_rate)->default_value(1.0))
    (it::caps_factor.c_str(), value<double>(&Italian::settings.caps_factor)->default_value(1.2))

    // Mbrola based backends options:
    (mbrola::executable.c_str(), value<string>(&::mbrola::executable)->default_value(::mbrola::name))
    (mbrola::voices.c_str(), value<string>(&::mbrola::voices)->default_value(::mbrola::voices_default_path))

    // Espeak based backends options:
    (espeak::executable.c_str(), value<string>(&::espeak::executable)->default_value(::espeak::name))

    // Espeak voices assignment:
    (espeak::en.c_str(), value<string>(&::espeak::en)->default_value(lang_id::en))
    (espeak::ru.c_str(), value<string>(&::espeak::ru)->default_value(lang_id::ru))
    (espeak::de.c_str(), value<string>(&::espeak::de)->default_value(lang_id::de))
    (espeak::fr.c_str(), value<string>(&::espeak::fr)->default_value(lang_id::fr))
    (espeak::es.c_str(), value<string>(&::espeak::es)->default_value(lang_id::es))
    (espeak::pt.c_str(), value<string>(&::espeak::pt)->default_value(lang_id::pt))
    (espeak::it.c_str(), value<string>(&::espeak::it)->default_value(lang_id::it))

    // Mbrola voices assignment:
    (espeak::mbrola::en.c_str(), value<string>(&::mbrola::en)->default_value("en1"))
    (espeak::mbrola::de.c_str(), value<string>(&::mbrola::de)->default_value("de6"))
    (espeak::mbrola::fr.c_str(), value<string>(&::mbrola::fr)->default_value("fr4"))
    (espeak::mbrola::es.c_str(), value<string>(&::mbrola::es)->default_value("es1"))
    (espeak::mbrola::pt.c_str(), value<string>(&::mbrola::pt)->default_value("br3"))
    (espeak::mbrola::it.c_str(), value<string>(&::mbrola::it)->default_value("it3"))

    // Freephone backend options:
    (freephone::executable.c_str(), value<string>(&::freephone::executable)->default_value(::freephone::name))
    (freephone::lexicon.c_str(), value<string>(&::freephone::lexicon)->default_value(::freephone::lexicon_default_path))

    // Ru_tts backend options:
    (ru_tts::executable.c_str(), value<string>(&::ru_tts::executable)->default_value(::ru_tts::name))
    (ru_tts::lexicon.c_str(), value<string>(&::ru_tts::lexicon)->default_value(::ru_tts::lexicon_default_path))
    (ru_tts::log.c_str(), value<string>(&::ru_tts::log_file))
    (ru_tts::expressiveness.c_str(), value<double>(&::ru_tts::expressiveness)->default_value(1.0))
    (ru_tts::female_voice.c_str(), bool_switch(&::ru_tts::female_voice)->default_value(false))
    (ru_tts::decimal_point.c_str(), bool_switch(&::ru_tts::decimal_point)->default_value(true))
    (ru_tts::decimal_comma.c_str(), bool_switch(&::ru_tts::decimal_comma)->default_value(true))
    (ru_tts::interclause_gap_factor.c_str(), value<double>(&::ru_tts::interclause_gap_factor)->default_value(1.0))
    (ru_tts::comma_gap_factor.c_str(), value<double>(&::ru_tts::comma_gap_factor)->default_value(1.0))
    (ru_tts::dot_gap_factor.c_str(), value<double>(&::ru_tts::dot_gap_factor)->default_value(1.0))
    (ru_tts::semicolon_gap_factor.c_str(), value<double>(&::ru_tts::semicolon_gap_factor)->default_value(1.0))
    (ru_tts::colon_gap_factor.c_str(), value<double>(&::ru_tts::colon_gap_factor)->default_value(1.0))
    (ru_tts::question_gap_factor.c_str(), value<double>(&::ru_tts::question_gap_factor)->default_value(1.0))
    (ru_tts::exclamation_gap_factor.c_str(), value<double>(&::ru_tts::exclamation_gap_factor)->default_value(1.0))
    (ru_tts::intonational_gap_factor.c_str(), value<double>(&::ru_tts::intonational_gap_factor)->default_value(1.0))

    // User defined TTS backend options:
    (user::command.c_str(), value<string>(&user_tts::command))
    (user::format.c_str(), value<string>(&user_tts::format))
    (user::sampling.c_str(), value<unsigned int>(&user_tts::sampling)->default_value(22050))
    (user::stereo.c_str(), bool_switch(&user_tts::stereo)->default_value(false))
    (user::freq_control.c_str(), bool_switch(&user_tts::freq_control)->default_value(false))
    (user::charset.c_str(), value<string>(&user_tts::charset)->default_value(""))

    // Registering Speech Dispatcher backend options:
    (spd::version, value<string>(&speech_server::spd_version))
    (spd::sound_icons, value<string>(&speech_server::spd_sound_icons)->default_value(speech_server::spd_sound_icons_default_path))
    (spd::use_voice_language, bool_switch(&speech_server::spd_use_voice_language)->default_value(true))
    (spd::accept_explicit_language, bool_switch(&speech_server::spd_accept_explicit_language)->default_value(true))
    (spd::ignore_unknown_voice, bool_switch(&speech_server::spd_ignore_unknown_voice)->default_value(false))
    (spd::index_marks, bool_switch(&speech_server::spd_support_index_marks)->default_value(true));

  // Parse config files and store values
  path extra_conf;
  if (option_value.count("config"))
    extra_conf = option_value["config"].as<string>();
  option_value.clear();
  if (spd_backend)
    {
      if (exists(local_conf))
        {
          read(local_conf, conf, option_value);
          noconf = false;
        }
      if (exists(extra_conf))
        {
          read(extra_conf, conf, option_value);
          noconf = false;
        }
    }
  else
    {
      if (exists(extra_conf))
        {
          read(extra_conf, conf, option_value);
          noconf = false;
        }
      if (exists(local_conf))
        {
          read(local_conf, conf, option_value);
          noconf = false;
        }
    }
  if (exists(global_conf))
    {
      read(global_conf, conf, option_value);
      noconf = false;
    }
  if (noconf && !spd_backend)
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
configuration::read(const path& config_file, const options_description& conf, variables_map& option_value)
{
  boost::filesystem::ifstream source(config_file);
  stage = " in " + config_file.generic_string();
  store(parse_config_file(source, conf), option_value);
  stage.erase();
}
