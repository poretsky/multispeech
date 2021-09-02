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

// Name composing macro:
#define COMPOSE(section, name) section "." name

// Package configuration data:
namespace package
{
  extern const char* const name;
  extern const char* const string;
  extern const char* const version;
};

// Language id strings:
namespace lang_id
{
  extern const char* const en;
  extern const char* const ru;
  extern const char* const de;
  extern const char* const fr;
  extern const char* const es;
  extern const char* const pt;
  extern const char* const it;
  extern const char* const autodetect;
};

// Configuration parser and options value storage
class configuration
{
public:
  // The constructor reads and parses configuration by the way
  configuration(int argc, char* argv[], bool is_spd_backend = false);

  // What we are parsing now:
  static std::string stage;

  // Error signaling:
  class error: public std::logic_error
  {
  public:
    error(const std::string& message);
  };

private:
  // Read configuration file:
  void read(const boost::filesystem::path& config_file,
            const boost::program_options::options_description& conf,
            boost::program_options::variables_map& option_value);

  // Set when called as Speech Dispatcher backend:
  bool spd_backend;
};

#endif
