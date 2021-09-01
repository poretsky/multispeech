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


// Option names:
#define PRIORITY "priority"
#define LOUDNESS "volume"
#define ENGINE "engine"
#define COMMAND "command"
#define FORMAT "format"
#define STEREO "stereo"
#define FREQ_CONTROL "freq_control"
#define CHARSET "charset"
#define VOICES "voices"
#define LOG "log"
#define PITCH "pitch"
#define RATE "rate"
#define CHAR_PITCH "char_pitch"
#define CHAR_RATE "char_rate"
#define CAPS_FACTOR "caps_factor"
#define ACCELERATION "acceleration"
#define EXPRESSIVENESS "expressiveness"
#define FEMALE_VOICE "female_voice"
#define DECIMAL_POINT "decimal_point"
#define DECIMAL_COMMA "decimal_comma"
#define INTERCLAUSE_GAP_FACTOR "interclause_gap_factor"
#define COMMA_GAP_FACTOR "comma_gap_factor"
#define DOT_GAP_FACTOR "dot_gap_factor"
#define SEMICOLON_GAP_FACTOR "semicolon_gap_factor"
#define COLON_GAP_FACTOR "colon_gap_factor"
#define QUESTION_GAP_FACTOR "question_gap_factor"
#define EXCLAMATION_GAP_FACTOR "exclamation_gap_factor"
#define INTONATIONAL_GAP_FACTOR "intonational_gap_factor"
#define NATIVE_VOICES "native_voices"
#define DTK_VOICES "dtk_voices"
#define SPD_VERSION "version"
#define SOUND_ICONS "sound_icons"
#define USE_VOICE_LANGUAGE "use_voice_language"
#define ACCEPT_EXPLICIT_LANGUAGE "accept_explicit_language"
#define IGNORE_UNKNOWN_VOICE "ignore_unknown_voice"
#define INDEX_MARKS "index_marks"
#define GENERAL_VOLUME "general_volume"
#define LATENCY "latency"
#define ASYNC_OPERATION "async_operation"
#define PULSEAUDIO_DIRECT "pulseaudio_direct"
#define LANG_PREF "language"
#define FALLBACK "fallback"

// Configuration sections names:
#define FRONTEND "frontend"
#define SPD "spd"
#define AUDIO "audio"
#define SOUNDS "sounds"
#define TONES "tones"
#define SPEECH "speech"

// Supported languages:
#define EN "en"
#define DE "de"
#define IT "it"
#define FR "fr"
#define ES "es"
#define PT "pt"
#define RU "ru"

// Option description items:
#define OPTION(section, name, type, variable, default) (COMPOSE(section, name), value<type>(&variable)->default_value(default))
#define STRING(section, name, variable, default) OPTION(section, name, string, variable, default)
#define INT(section, name, variable, default) OPTION(section, name, int, variable, default)
#define UINT(section, name, variable, default) OPTION(section, name, unsigned int, variable, default)
#define DOUBLE(section, name, variable, default) OPTION(section, name, double, variable, default)
#define FLOAT(section, name, variable, default) OPTION(section, name, float, variable, default)
#define BOOLEAN(section, name, variable, default) (section "." name, bool_switch(&variable)->default_value(default))

// Some common options appearing in several sections:
#define DEVICE(section, component) STRING(section, "device", component::device, "")
#define VOLUME(section, component) FLOAT(section, LOUDNESS, component::relative_volume, 1.0)
#define ASYNCHRONOUS(section, component) BOOLEAN(section, "asynchronous", component::asynchronous, true)
#define SAMPLING(section, component, default) UINT(section, "sampling", component::sampling, default)
#define EXECUTABLE(backend, component) STRING(backend, "executable", component::executable, component::name)
#define LEXICON(backend, component) STRING(backend, "lexicon", component::lexicon, component::lexicon_default_path)

// Backend specific voice items:
#define MBROLA_VOICE(lang, field, default) STRING(COMPOSE(ESPEAK, MBROLA), lang,mbrola::field, default)
#define ESPEAK_VOICE(lang, field) STRING(ESPEAK, lang,espeak::field, lang_id::field)

// Language related options group:
#define LANGUAGE(lang, component)                                       \
  STRING(lang, ENGINE, component::settings.engine, "")                  \
  INT(lang, PRIORITY, component::settings.priority, 0)                  \
  DOUBLE(lang, LOUDNESS, component::settings.volume, 1.0)               \
  DOUBLE(lang, PITCH, component::settings.pitch, 1.0)                   \
  DOUBLE(lang, RATE, component::settings.rate, 1.0)                     \
  DOUBLE(lang, ACCELERATION, component::settings.acceleration, 0.0)     \
  DOUBLE(lang, CHAR_PITCH, component::settings.char_pitch, 1.0)         \
  DOUBLE(lang, CHAR_RATE, component::settings.char_rate, 1.0)           \
  DOUBLE(lang, CAPS_FACTOR, component::settings.caps_factor, 1.2)


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
  const char* const en = EN;
  const char* const ru = RU;
  const char* const de = DE;
  const char* const fr = FR;
  const char* const es = ES;
  const char* const pt = PT;
  const char* const it = IT;
  const char* const autodetect = "autodetect";
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
  conf.add_options()

    // Frontend related options:
    STRING(FRONTEND, CHARSET, speech_server::frontend_charset, "")
    BOOLEAN(FRONTEND, NATIVE_VOICES, speech_server::support_native_voices, true)
    BOOLEAN(FRONTEND, DTK_VOICES, speech_server::support_dtk_voices, false)

    // General audio options:
    DEVICE(AUDIO, audioplayer)
    FLOAT(AUDIO, GENERAL_VOLUME, audioplayer::general_volume, 0.8)
    DOUBLE(AUDIO, LATENCY, audioplayer::suggested_latency, 0.05)
    BOOLEAN(AUDIO, ASYNC_OPERATION, audioplayer::async, false)
    BOOLEAN(AUDIO, PULSEAUDIO_DIRECT, audioplayer::use_pa, true)

    // Sound files playing section:
    DEVICE(SOUNDS, file_player)
    VOLUME(SOUNDS, file_player)
    ASYNCHRONOUS(SOUNDS, file_player)

    // tones producing section:
    DEVICE(TONES, tone_generator)
    VOLUME(TONES, tone_generator)
    ASYNCHRONOUS(TONES, tone_generator)
    SAMPLING(TONES, tone_generator, 44100)

    // General speech control options:
    DEVICE(SPEECH, loudspeaker)
    VOLUME(SPEECH, loudspeaker)
    STRING(SPEECH, LANG_PREF, polyglot::language_preference, "")
    STRING(SPEECH, FALLBACK, polyglot::fallback_language, lang_id::en)

    // Language sections:
    LANGUAGE(EN, English)
    LANGUAGE(RU, Russian)
    LANGUAGE(DE, German)
    LANGUAGE(FR, French)
    LANGUAGE(ES, Spanish)
    LANGUAGE(PT, Portuguese)
    LANGUAGE(IT, Italian)

    // Mbrola based backends options:
    EXECUTABLE(MBROLA,mbrola)
    STRING(MBROLA, VOICES,mbrola::voices,mbrola::voices_default_path)

    // Espeak based backends options:
    EXECUTABLE(ESPEAK,espeak)

    // Espeak voices assignment:
    ESPEAK_VOICE(EN, en)
    ESPEAK_VOICE(RU, ru)
    ESPEAK_VOICE(DE, de)
    ESPEAK_VOICE(FR, fr)
    ESPEAK_VOICE(ES, es)
    ESPEAK_VOICE(PT, pt)
    ESPEAK_VOICE(IT, it)

    // Mbrola voices assignment:
    MBROLA_VOICE(EN, en, "en1")
    MBROLA_VOICE(DE, de, "de6")
    MBROLA_VOICE(FR, fr, "fr4")
    MBROLA_VOICE(ES, es, "es1")
    MBROLA_VOICE(PT, pt, "br3")
    MBROLA_VOICE(IT, it, "it3")

    // Freephone backend options:
    EXECUTABLE(FREEPHONE,freephone)
    LEXICON(FREEPHONE,freephone)

    // Ru_tts backend options:
    EXECUTABLE(RU_TTS,ru_tts)
    LEXICON(RU_TTS,ru_tts)
    STRING(RU_TTS, LOG,ru_tts::log_file, "")
    DOUBLE(RU_TTS, EXPRESSIVENESS,ru_tts::expressiveness, 1.0)
    BOOLEAN(RU_TTS, FEMALE_VOICE,ru_tts::female_voice, false)
    BOOLEAN(RU_TTS, DECIMAL_POINT,ru_tts::decimal_point, true)
    BOOLEAN(RU_TTS, DECIMAL_COMMA,ru_tts::decimal_comma, true)
    DOUBLE(RU_TTS, INTERCLAUSE_GAP_FACTOR,ru_tts::interclause_gap_factor, 1.0)
    DOUBLE(RU_TTS, COMMA_GAP_FACTOR,ru_tts::comma_gap_factor, 1.0)
    DOUBLE(RU_TTS, DOT_GAP_FACTOR,ru_tts::dot_gap_factor, 1.0)
    DOUBLE(RU_TTS, SEMICOLON_GAP_FACTOR,ru_tts::semicolon_gap_factor, 1.0)
    DOUBLE(RU_TTS, COLON_GAP_FACTOR,ru_tts::colon_gap_factor, 1.0)
    DOUBLE(RU_TTS, QUESTION_GAP_FACTOR,ru_tts::question_gap_factor, 1.0)
    DOUBLE(RU_TTS, EXCLAMATION_GAP_FACTOR,ru_tts::exclamation_gap_factor, 1.0)
    DOUBLE(RU_TTS, INTONATIONAL_GAP_FACTOR,ru_tts::intonational_gap_factor, 1.0)

    // User defined TTS backend options:
    STRING(USER_TTS, COMMAND, user_tts::command, "")
    STRING(USER_TTS, FORMAT, user_tts::format, "")
    SAMPLING(USER_TTS, user_tts, 22050)
    BOOLEAN(USER_TTS, STEREO, user_tts::stereo, false)
    BOOLEAN(USER_TTS, FREQ_CONTROL, user_tts::freq_control, false)
    STRING(USER_TTS, CHARSET, user_tts::charset, "")

    // Registering Speech Dispatcher backend options:
    STRING(SPD, SPD_VERSION, speech_server::spd_version, "")
    STRING(SPD, SOUND_ICONS, speech_server::spd_sound_icons, speech_server::spd_sound_icons_default_path)
    BOOLEAN(SPD, USE_VOICE_LANGUAGE, speech_server::spd_use_voice_language, true)
    BOOLEAN(SPD, ACCEPT_EXPLICIT_LANGUAGE, speech_server::spd_accept_explicit_language, true)
    BOOLEAN(SPD, IGNORE_UNKNOWN_VOICE, speech_server::spd_ignore_unknown_voice, false)
    BOOLEAN(SPD, INDEX_MARKS, speech_server::spd_support_index_marks, true);

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
