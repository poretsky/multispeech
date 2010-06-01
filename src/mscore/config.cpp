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
#include "speech_engine.hpp"
#include "server.hpp"
#include "mbrola.hpp"
#include "espeak.hpp"
#include "user_tts.hpp"


namespace multispeech
{

using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;


// Options value repository:
variables_map configuration::option_value;

// Relative option names:
configuration::option configuration::option::charset("charset");
configuration::option configuration::option::native_notation("native_notation");
configuration::option configuration::option::dtk_notation("dtk_notation");
configuration::option configuration::option::port("port");
configuration::option configuration::option::sounds("sounds");
configuration::option configuration::option::split_multiline_messages("split_multiline_messages");
configuration::option configuration::option::device("device");
configuration::option configuration::option::latency("latency");
configuration::option configuration::option::asynchronous("asynchronous");
configuration::option configuration::option::engine("engine");
configuration::option configuration::option::executable("executable");
configuration::option configuration::option::command("command");
configuration::option configuration::option::format("format");
configuration::option configuration::option::sampling("sampling");
configuration::option configuration::option::stereo("stereo");
configuration::option configuration::option::freq_control("freq_control");
configuration::option configuration::option::voices("voices");
configuration::option configuration::option::lexicon("lexicon");
configuration::option configuration::option::log("log");
configuration::option configuration::option::language("language");
configuration::option configuration::option::voice("voice");
configuration::option configuration::option::voice_name("voice_name");
configuration::option configuration::option::dialect("dialect");
configuration::option configuration::option::volume("volume");
configuration::option configuration::option::pitch("pitch");
configuration::option configuration::option::rate("rate");
configuration::option configuration::option::caps_factor("caps_factor");
configuration::option configuration::option::acceleration("acceleration");

// General sections:
const string configuration::frontend("frontend");
const string configuration::ssip("ssip");
const string configuration::audio("audio");
const string configuration::sounds("sounds");
const string configuration::tones("tones");
const string configuration::speech("speech");
const string configuration::character("char");

// Hardcoded default paths:
const path configuration::mbrola_voices_default(complete("mbrola-voices", DATA_DIR));
const path configuration::enlex_default(complete("freespeech/enlex.db", DATA_DIR));
const path configuration::rulex_default(complete("freespeech/rulex.db", DATA_DIR));
const path configuration::sounds_default(complete("sounds/sound-icons", DATA_DIR));


// Language id strings:
namespace lang_id
{
  const char* const en = "en";
  const char* const ru = "ru";
  const char* const autodetect = "autodetect";
};

// Speech backend names:
namespace speaker
{
  const char* const mbrola = "mbrola";
  const char* const espeak = "espeak";
  const char* const freephone = "freephone";
  const char* const ru_tts = "ru_tts";
  const char* const user_defined = "user";
};


// Parsing and applying configuration options

string configuration::stage;

configuration::configuration(int argc, char* argv[])
{
  path global_conf(complete(string(PACKAGE) + ".conf", SYSCONF_DIR));
  path alternate_conf(complete(path(argv[0]).leaf() + ".conf", SYSCONF_DIR));
  path local_conf(complete("." + string(PACKAGE) + "rc", getenv("HOME")));
  options_description conf, cl_desc("Available options");
  variables_map cl_opt;
  bool noconf = true;

  // Check if configured already:
  if (!option_value.empty())
    throw std::logic_error("Duplicated server initialization");

  // Prevent searching for X:
  unsetenv("DISPLAY");

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
  conf.add_options()

    // Frontend related options:
    (option::charset(frontend), value<string>()->default_value(""))
    (option::native_notation(frontend), bool_switch()->default_value(true))
    (option::dtk_notation(frontend), bool_switch()->default_value(false))

    // SSIP related options:
    (option::port(ssip), value<unsigned int>()->default_value(6560))
    (option::sounds(ssip), value<string>()->default_value(sounds_default.file_string()))
    (option::split_multiline_messages(ssip), bool_switch()->default_value(false))

    // General audio options:
    (option::device(audio), value<string>()->default_value(""))
    (option::volume(audio), value<float>(&audioplayer::general_volume)->default_value(0.8))
    (option::latency(audio), value<double>()->default_value(0.0))

    // Sound files playing section:
    (option::device(sounds), value<string>()->default_value(""))
    (option::volume(sounds), value<float>()->default_value(1.0))
    (option::asynchronous(sounds), bool_switch()->default_value(true))

    // tones producing section:
    (option::device(tones), value<string>()->default_value(""))
    (option::volume(tones), value<float>()->default_value(1.0))
    (option::sampling(tones), value<unsigned int>()->default_value(44100))
    (option::asynchronous(tones), bool_switch()->default_value(true))

    // General speech control options:
    (option::device(speech), value<string>()->default_value(""))
    (option::volume(speech), value<double>(&speech_engine::general_volume))
    (option::rate(speech), value<double>(&speech_engine::general_rate))
    (option::pitch(speech), value<double>(&speech_engine::general_pitch))
    (option::rate(character)(speech), value<double>(&speech_engine::char_rate))
    (option::pitch(character)(speech), value<double>(&speech_engine::char_pitch))
    (option::caps_factor(speech), value<double>(&speech_engine::caps_factor))
    (option::language(speech), value<string>()->default_value(lang_id::autodetect))

    // Speech engine choosing options:
    (option::engine(lang_id::en)(speech), value<string>()->default_value(""))
    (option::engine(lang_id::ru)(speech), value<string>()->default_value(""))

    // Per voice speech options for Mbrola voices:
    (option::voice_name(mbrola::en1), value<string>())
    (option::volume(mbrola::en1), value<float>()->default_value(1.0))
    (option::rate(mbrola::en1), value<double>()->default_value(1.0))
    (option::pitch(mbrola::en1), value<double>()->default_value(1.0))
    (option::voice_name(mbrola::us1), value<string>())
    (option::volume(mbrola::us1), value<float>()->default_value(1.0))
    (option::rate(mbrola::us1), value<double>()->default_value(1.0))
    (option::pitch(mbrola::us1), value<double>()->default_value(1.0))
    (option::voice_name(mbrola::us2), value<string>())
    (option::volume(mbrola::us2), value<float>()->default_value(1.0))
    (option::rate(mbrola::us2), value<double>()->default_value(1.0))
    (option::pitch(mbrola::us2), value<double>()->default_value(1.0))
    (option::voice_name(mbrola::us3), value<string>())
    (option::volume(mbrola::us3), value<float>()->default_value(1.0))
    (option::rate(mbrola::us3), value<double>()->default_value(1.0))
    (option::pitch(mbrola::us3), value<double>()->default_value(1.0))

    // Per voice speech options for Espeak voices:
    (option::voice_name(espeak::en_default), value<string>()->default_value("default"))
    (option::volume(espeak::en_default), value<float>()->default_value(1.0))
    (option::rate(espeak::en_default), value<double>()->default_value(1.0))
    (option::pitch(espeak::en_default), value<double>()->default_value(1.0))
    (option::voice_name(espeak::ru), value<string>()->default_value("russian_test"))
    (option::volume(espeak::ru), value<float>()->default_value(1.0))
    (option::rate(espeak::ru), value<double>()->default_value(1.0))
    (option::pitch(espeak::ru), value<double>()->default_value(1.0))

    // Mbrola based backends options:
    (option::executable(speaker::mbrola), value<string>()->default_value(speaker::mbrola))
    (option::voices(speaker::mbrola), value<string>()->default_value(mbrola_voices_default.file_string()))

    // Espeak based backends options:
    (option::executable(speaker::espeak), value<string>()->default_value(speaker::espeak))
    (option::voice(lang_id::en)(speaker::espeak), value<string>()->default_value(espeak::en_default))
    (option::voice(lang_id::ru)(speaker::espeak), value<string>()->default_value(espeak::ru))

    // Freephone backend options:
    (option::executable(speaker::freephone), value<string>()->default_value(speaker::freephone))
    (option::lexicon(speaker::freephone), value<string>()->default_value(enlex_default.file_string()))

    // Ru_tts backend options:
    (option::executable(speaker::ru_tts), value<string>()->default_value(speaker::ru_tts))
    (option::lexicon(speaker::ru_tts), value<string>()->default_value(rulex_default.file_string()))
    (option::log(speaker::ru_tts), value<string>()->default_value(""))
    (option::voice_name(speaker::ru_tts), value<string>())
    (option::volume(speaker::ru_tts), value<float>()->default_value(1.0))
    (option::rate(speaker::ru_tts), value<double>()->default_value(1.0))
    (option::pitch(speaker::ru_tts), value<double>()->default_value(1.0))

    // User defined TTS backend options:
    (option::command(speaker::user_defined), value<string>()->default_value(""))
    (option::freq_control(speaker::user_defined), bool_switch()->default_value(false))

    // Per language options for user defined TTS:
    (option::format(user_tts::voice(lang_id::en)), value<string>())
    (option::sampling(user_tts::voice(lang_id::en)), value<unsigned int>()->default_value(22050))
    (option::stereo(user_tts::voice(lang_id::en)), bool_switch()->default_value(false))
    (option::charset(user_tts::voice(lang_id::en)), value<string>()->default_value(""))
    (option::acceleration(user_tts::voice(lang_id::en)), value<double>()->default_value(0.0))
    (option::voice_name(user_tts::voice(lang_id::en)), value<string>())
    (option::dialect(user_tts::voice(lang_id::en)), value<string>()->default_value(""))
    (option::volume(user_tts::voice(lang_id::en)), value<float>()->default_value(1.0))
    (option::rate(user_tts::voice(lang_id::en)), value<double>()->default_value(1.0))
    (option::pitch(user_tts::voice(lang_id::en)), value<double>()->default_value(1.0))
    (option::format(user_tts::voice(lang_id::ru)), value<string>())
    (option::sampling(user_tts::voice(lang_id::ru)), value<unsigned int>()->default_value(22050))
    (option::stereo(user_tts::voice(lang_id::ru)), bool_switch()->default_value(false))
    (option::charset(user_tts::voice(lang_id::ru)), value<string>()->default_value(""))
    (option::acceleration(user_tts::voice(lang_id::ru)), value<double>()->default_value(0.0))
    (option::voice_name(user_tts::voice(lang_id::ru)), value<string>())
    (option::dialect(user_tts::voice(lang_id::ru)), value<string>()->default_value(""))
    (option::volume(user_tts::voice(lang_id::ru)), value<float>()->default_value(1.0))
    (option::rate(user_tts::voice(lang_id::ru)), value<double>()->default_value(1.0))
    (option::pitch(user_tts::voice(lang_id::ru)), value<double>()->default_value(1.0));

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
  if (exists(alternate_conf))
    {
      read(alternate_conf, conf);
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


// Options value access:

const char*
configuration::input_charset(void)
{
  const string& charset = option_value[option::charset(frontend)].as<string>();
  return charset.empty() ? NULL : charset.c_str();
}

bool
configuration::native_notation(void)
{
  return option_value[option::native_notation(frontend)].as<bool>();
}

bool
configuration::dtk_notation(void)
{
  return option_value[option::dtk_notation(frontend)].as<bool>();
}

unsigned int
configuration::tcp_port(void)
{
  return option_value[option::port(ssip)].as<unsigned int>();
}

const string&
configuration::sounds_repository(void)
{
  return option_value[option::sounds(ssip)].as<string>();
}

bool
configuration::split_multiline_messages(void)
{
  return option_value[option::split_multiline_messages(ssip)].as<bool>();
}

const string&
configuration::default_audio_device(void)
{
  return option_value[option::device(audio)].as<string>();
}

float
configuration::general_volume(void)
{
  return option_value[option::volume(audio)].as<float>();
}

double
configuration::audio_latency(void)
{
  return option_value[option::latency(audio)].as<double>();
}

const string&
configuration::sound_output_device(void)
{
  const string& device = option_value[option::device(sounds)].as<string>();
  return device.empty() ?
    default_audio_device() :
    device;
}

float
configuration::sound_volume(void)
{
  return option_value[option::volume(sounds)].as<float>();
}

bool
configuration::sound_async(void)
{
  return option_value[option::asynchronous(sounds)].as<bool>();
}

const string&
configuration::beep_device(void)
{
  const string& device = option_value[option::device(tones)].as<string>();
  return device.empty() ?
    default_audio_device() :
    device;
}

float
configuration::beep_volume(void)
{
  return option_value[option::volume(tones)].as<float>();
}

unsigned int
configuration::beep_sampling(void)
{
  return option_value[option::sampling(tones)].as<unsigned int>();
}

bool
configuration::beep_async(void)
{
  return option_value[option::asynchronous(tones)].as<bool>();
}

const string&
configuration::speech_output_device(void)
{
  const string& device = option_value[option::device(speech)].as<string>();
  return device.empty() ?
    default_audio_device() :
    device;
}

float
configuration::speech_volume(const string& voice)
{
  return option_value[option::volume(voice)].as<float>();
}

double
configuration::speech_rate(const string& voice)
{
  return option_value[option::rate(voice)].as<double>();
}

double
configuration::voice_pitch(const string& voice)
{
  return option_value[option::pitch(voice)].as<double>();
}

double
configuration::char_rate(void)
{
  return option_value[option::rate(character)(speech)].as<double>();
}

double
configuration::char_pitch(void)
{
  return option_value[option::pitch(character)(speech)].as<double>();
}

double
configuration::caps_factor(void)
{
  return option_value[option::caps_factor(speech)].as<double>();
}

const string&
configuration::default_language(void)
{
  return option_value[option::language(speech)].as<string>();
}

const string&
configuration::speech_backend(const string& language)
{
  return option_value[option::engine(language)(speech)].as<string>();
}

const string&
configuration::backend_voice(const string& backend, const string& language)
{
  return option_value[option::voice(language)(backend)].as<string>();
}

const string&
configuration::backend_executable(const string& backend)
{
  return option_value[option::executable(backend)].as<string>();
}

const string&
configuration::mbrola_voices(void)
{
  return option_value[option::voices(speaker::mbrola)].as<string>();
}

const string&
configuration::backend_lexicon(const string& backend)
{
  return option_value[option::lexicon(backend)].as<string>();
}

const string&
configuration::backend_log(const string& backend)
{
  return option_value[option::log(backend)].as<string>();
}

const string&
configuration::user_tts_command(void)
{
  return option_value[option::command(speaker::user_defined)].as<string>();
}

bool
configuration::user_freq_control(void)
{
  return option_value[option::freq_control(speaker::user_defined)].as<bool>();
}

soundfile::format
configuration::user_sound_format(const string& language)
{
  if (!option_value.count(option::format(user_tts::voice(language))))
    {
      const string& fmt = option_value[option::format(user_tts::voice(language))].as<string>();
      if ("s8" == fmt)
        return soundfile::s8;
      else if ("u8" == fmt)
        return soundfile::u8;
      else if ("s16" == fmt)
        return soundfile::s16;
      else throw configuration::error("unknown sound format specification \"" + fmt + '\"');
    }
  return soundfile::autodetect;
}

unsigned int
configuration::user_sampling(const string& language)
{
  return option_value[option::sampling(user_tts::voice(language))].as<unsigned int>();
}

unsigned int
configuration::user_sound_channels(const string& language)
{
  return option_value[option::stereo(user_tts::voice(language))].as<bool>() ?
    2 : 1;
}

const char*
configuration::user_tts_charset(const string& language)
{
  const string& charset = option_value[option::charset(user_tts::voice(language))].as<string>();
  return charset.empty() ? NULL : charset.c_str();
}

const char*
configuration::user_tts_dialect(const string& language)
{
  const string& dialect = option_value[option::dialect(user_tts::voice(language))].as<string>();
  return dialect.empty() ? "none" : dialect.c_str();
}

double
configuration::user_tts_acceleration(const string& voice)
{
  return option_value[option::acceleration(voice)].as<double>();
}


// Private members:

void
configuration::read(const path& config_file, const options_description& conf)
{
  boost::filesystem::ifstream source(config_file);
  stage = " in " + config_file.file_string();
  store(parse_config_file(source, conf), option_value);
  stage.erase();
}


// Configuration errors signaling:

configuration::error::error(const string& message):
  logic_error(message)
{
}


// Option name composer:

configuration::option::option(const string& name):
  string(name)
{
}

configuration::option
configuration::option::operator()(const string& section)
{
  return option(section + '.' + (*this));
}

configuration::option::operator const char*(void)
{
  return c_str();
}

} // namespace multispeech
