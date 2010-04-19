// proxy.hpp -- Server interface for sessions
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
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

// The main purpose of this class is to define clearly the subset
// of server functionality available in sessions and to provide
// common synchronization means for concurrent access to the server
// resources. The server class should be derived from it privately
// to effectively hide this subset from external access.
//
// All server operations except of hello() and bye() are not assumed
// to be thread safe, so sessions must lock server access itself
// whenever it is appropriate.

#ifndef MULTISPEECH_SSIP_PROXY_HPP
#define MULTISPEECH_SSIP_PROXY_HPP

#include <map>

#include <boost/thread/mutex.hpp>

#include <mscore/server.hpp>
#include <mscore/text_filter.hpp>

#include "capitalization.hpp"

namespace SSIP
{

class proxy: public multispeech::server
{
protected:
  // Main constructor:
  proxy(int argc, char* argv[]):
    multispeech::server(argc, argv),
    split_multiline_messages(option_value[multispeech::options::ssip::split_multiline_messages].as<bool>()),
    sounds(option_value[multispeech::options::ssip::sounds].as<std::string>()),
    punctuation_mode(multispeech::punctuations::some),
    rate_factor(1.0),
    pitch_factor(1.0),
    volume_factor(1.0),
    spelling(false),
    capitalization_mode(capitalization::none),
    paused(false)
  {
  }

public:
  // Clients list representation:
  typedef std::map<unsigned long, class session*> clients_list;

  // Taken from configuration:
  const bool split_multiline_messages;
  const std::string sounds;

  // Initial defaults for session environment:
  multispeech::punctuations::mode punctuation_mode;
  double rate_factor, pitch_factor, volume_factor;
  bool spelling;
  capitalization::mode capitalization_mode;

  // Global state:
  bool paused;

  // Shared access synchronization means:
  boost::mutex access;

  // Register new session when starting up:
  virtual void hello(unsigned long id, class session* client) = 0;

  // Clean up terminated session when finishing:
  virtual void bye(unsigned long id) = 0;

  // Return active session object pointer by id
  // or NULL when no one is found:
  virtual session* client(unsigned long id) = 0;

  // Retrieve all active clients list reference:
  virtual clients_list& all_clients(void) = 0;
};

} // namespace SSIP

#endif
