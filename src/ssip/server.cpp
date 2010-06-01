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

#include <boost/assign.hpp>
#include <boost/thread/thread.hpp>

#include <mscore/config.hpp>

#include "server.hpp"

#include "session.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace FBB;
using namespace multispeech;


// Static data:
map<job::event, message::code> server::notification = map_list_of
  (job::started, message::EVENT_BEGIN)
  (job::complete, message::EVENT_END)
  (job::cancelled, message::EVENT_CANCELLED)
  (job::paused, message::EVENT_PAUSED)
  (job::resumed, message::EVENT_RESUMED);


// Object construction:

server::server(int argc, char* argv[]):
  proxy(argc, argv),
  ServerSocket(tcp_port()),
  connecting(false)
{
  charset("UTF-8");
  listen();
}


// Private methods:

void
server::connect(int fd)
{
  mutex::scoped_lock lock(proxy::access);
  connecting = true;
  thread(session(this, fd));
  while (connecting)
    connected.wait(lock);
}

void
server::feedback(job::event event,
                 unsigned long id, unsigned long owner)
{
  mutex::scoped_lock lock(proxy::access);
  message* customer = client(owner);
  if (customer && notification.count(event))
    {
      customer->emit(notification[event], id);
      customer->emit(notification[event], owner);
      customer->emit(notification[event]);
    }
}

void
server::hello(unsigned long id, session* client)
{
  mutex::scoped_lock lock(proxy::access);
  clients[id] = client;
  connecting = false;
  connected.notify_one();
}

void
server::bye(unsigned long id)
{
  mutex::scoped_lock lock(proxy::access);
  clients.erase(id);
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
