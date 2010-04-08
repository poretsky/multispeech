// server.cpp -- SSIP server implementation
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

#include <sysconfig.h>

#include <mscore/config.hpp>

#include "server.hpp"

#include "session.hpp"


namespace SSIP
{

using namespace boost;
using namespace FBB;


// Object construction:

server::server(int argc, char* argv[]):
  proxy(argc, argv),
  ServerSocket(option_value[multispeech::options::ssip::port].as<unsigned int>())
{
  charset("UTF-8");
  listen();
}


// Private methods:

void
server::connect(int fd)
{
  mutex::scoped_lock lock(proxy::access);
  competitor.reset(new thread(session(this, fd)));
  while (competitor.unique())
    session_started.wait(lock);
  competitor.reset();
}

void
server::hello(unsigned long id, session* client)
{
  mutex::scoped_lock lock(proxy::access);
  clients[id] = client;
  threads[id] = competitor;
  session_started.notify_one();
}

void
server::bye(unsigned long id)
{
  mutex::scoped_lock lock(proxy::access);
  clients.erase(id);
  threads.erase(id);
}

session*
server::client(unsigned long id)
{
  if (clients.count(id))
    return clients[id];
  return NULL;
}

server::clients_list&
server::all_clients(void)
{
  return clients;
}

void
server::parentProcess(void)
{
}

void
server::childProcess(void)
{
  while (true)
    {
      SocketBase request = accept();
      connect(request.socket());
    }
}

} // namespace SSIP
