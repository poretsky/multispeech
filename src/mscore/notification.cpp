// notification.cpp -- General notification service framework implementation
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

#include "notification.hpp"

#include "sound_manager.hpp"


namespace multispeech
{

using namespace std;
using namespace boost;


// Constructing / destroying:

notification::notification(sound_manager* master, unsigned int mode):
  event_mask(mode),
  alive(true),
  link(master->feedback),
  access(master->access),
  service(agent(this))
{
  link = this;
}

notification::~notification(void)
{
  die();
  service.join();
}


// Private methods:

void
notification::submit(job_event status, job::id_type id, unsigned int owner)
{
  if (event_mask & status)
    {
      message report;
      report.status = status;
      report.id = id;
      report.owner = owner;
      reports.push(report);
      event.notify_one();
    }
}

void
notification::die(void)
{
  mutex::scoped_lock lock(access);
  alive = false;
  link = NULL;
  event.notify_one();
}


// Agent subclass members:

notification::agent::agent(notification* queue_holder):
  holder(queue_holder)
{
}

void
notification::agent::operator()(void)
{
  while (holder->alive)
    {
      message report;
      {
        mutex::scoped_lock lock(holder->access);
        while (holder->alive && holder->reports.empty())
          holder->event.wait(lock);
        if (holder->alive)
          {
            report = holder->reports.front();
            holder->reports.pop();
          }
      }
      if (holder->alive)
        holder->consume(report);
    }
}

} // namespace multispeech
