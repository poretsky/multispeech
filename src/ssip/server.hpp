// server.hpp -- SSIP server interface
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

#ifndef MULTISPEECH_SSIP_SERVER_HPP
#define MULTISPEECH_SSIP_SERVER_HPP

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <bobcat/serversocket>
#include <bobcat/fork>

#include <mscore/server.hpp>

namespace SSIP
{

// Forward declarations:
class session;

class server:
  public multispeech::server,
  public FBB::Fork,
  private FBB::ServerSocket
{
public:
  // Main constructor:
  server(int argc, char* argv[]);

  friend class session;

private:
  // Container for running thread handler:
  typedef boost::shared_ptr<boost::thread> process;

  // Active sessions:
  std::map<unsigned long, session*> clients;

  // Active session thread handlers:
  std::map<unsigned long, process> threads;

  // Incoming session thread:
  process competitor;

  // Synchronization means:
  boost::mutex server_access;
  boost::condition session_started;

  // Session uses this method to register itself on the server.
  void hello(unsigned long id, session* client);

  // Session uses this method to indicate it's termination.
  void bye(unsigned long id);

  // Initiate a new session for specified socket:
  void connect(int fd);

  // Methods required by Fork:
  void parentProcess(void);
  void childProcess(void);
};

} // namespace SSIP

#endif
