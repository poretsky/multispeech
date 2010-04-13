// session.cpp -- SSIP session implementation
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

#include <sstream>

#include <boost/foreach.hpp>

#include "session.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;
using namespace FBB;
using namespace multispeech;


// Counter for session id generation:
unsigned long session::count = 0;


// Session object construction:

session::session(proxy* origin, int socket_fd):
  connection(socket_fd),
  multispeech::session(*input),
  message(*output),
  host(*origin),
  receiving(false),
  id(++count)
{
  punctuation(host.punctuation);
  rate_factor(host.rate_factor);
  pitch_factor(host.pitch_factor);
  volume_factor(host.volume_factor);
}


// Session loop:

void
session::operator()(void)
{
  host.hello(id, this);
  run();
  host.bye(id);
}


// Parsing client requests:

bool
session::perform(string& request)
{
  mutex::scoped_lock lock(host.access);
  bool result = true;
  if (*request.rbegin() == '\r')
    request.resize(request.length() - 1);
  if (!receiving)
    {
      commands::FunctionPtr done = commands::findCmd(request);
      if (!commands::cmd().empty())
        result = (this->*done)();
    }
  else if (request == ".")
    {
      if (!host.split_multiline_messages)
        {
          ostringstream text;
          BOOST_FOREACH (const string& line, accumulator)
            text << line << ' ';
          prepare(text.str());
        }
      commit();
      receiving = false;
    }
  else
    {
      if (request == "..")
        request.resize(1);
      accumulator.push_back(request);
      if (host.split_multiline_messages)
        prepare(request);
    }
  return result;
}

void
session::prepare(const string& text)
{
  punctuations::verbosity = punctuation;
  speech_engine::volume(volume_factor);
  speech_engine::voice_pitch(pitch_factor);
  speech_engine::speech_rate(rate_factor);
  errand << host.text_task(text);
}

void
session::commit(void)
{
  bool dominate = false, update = false;
  switch (errand.urgency())
    {
    case urgency_mode::important:
      dominate = true;
    case urgency_mode::notification:
      host.reject(urgency_mode::notification);
      break;
    case urgency_mode::message:
    case urgency_mode::text:
      host.reject(urgency_mode::text);
      break;
    case urgency_mode::progress:
      update = true;
    default:
      break;
    }
  host.enqueue(errand, dominate, update);
  host.proceed();
  emit(OK_MESSAGE_QUEUED, errand.id());
  emit(OK_MESSAGE_QUEUED);
  accumulator.clear();
  errand.clear();
}


// Serving client requests:

bool
session::cmd_speak(void)
{
  accumulator.clear();
  errand = job(id, priority, notification);
  receiving = true;
  emit(OK_RECEIVE_DATA);
  return true;
}

bool
session::cmd_char(void)
{
  speech_engine::volume(volume_factor);
  speech_engine::char_voice_pitch(pitch_factor);
  speech_engine::char_speech_rate(rate_factor);
  errand = job(id, priority, notification);
  errand << host.letter_task(commands::beyond());
  commit();
  return true;
}

bool
session::cmd_key(void)
{
  istringstream keyname(commands::beyond());
  string token;
  speech_engine::volume(volume_factor);
  speech_engine::char_voice_pitch(pitch_factor);
  speech_engine::char_speech_rate(rate_factor);
  errand = job(id, priority, notification);
  while (getline(keyname, token, '_'))
    errand << host.letter_task(token);
  commit();
  return true;
}

bool
session::cmd_block(void)
{
  emit(block.toggle(commands::beyond()));
  return true;
}

bool
session::cmd_set(void)
{
  emit((this->*settings::findCmd(target.parse(commands::beyond())))());
  return true;
}

bool
session::cmd_quit(void)
{
  emit(OK_BYE);
  return false;
}

bool
session::cmd_help(void)
{
  emit_help();
  return true;
}

bool
session::cmd_unknown(void)
{
  emit(ERR_INVALID_COMMAND);
  return true;
}

bool
session::cmd_unimplemented(void)
{
  emit(ERR_NOT_IMPLEMENTED);
  return true;
}


// Parameter settings:

message::code
session::set_client_name(void)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    rc = client.name(settings::beyond());
  return rc;
}

message::code
session::set_notification(void)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    rc = notification.setup(settings::beyond());
  return rc;
}

message::code
session::set_punctuation(void)
{
  message::code rc = message::OK_PUNCT_MODE_SET;
  if (!block)
    {
      punctuations::mode mode = punctuation.parse(settings::beyond());
      if (mode != punctuations::unknown)
        switch (target.selection())
          {
          case destination::self:
            punctuation(mode);
            break;
          case destination::all:
            BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
              client.second->punctuation(mode);
            host.punctuation = mode;
            break;
          case destination::another:
            {
              session* client = host.client(target.id());
              if (client)
                client->punctuation(mode);
              else rc = ERR_NO_SUCH_CLIENT;
            }
            break;
          default:
            rc = ERR_PARAMETER_INVALID;
            break;
          }
      else rc = ERR_PARAMETER_INVALID;
    }
  else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  return rc;
}

message::code
session::set_priority(void)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    rc = priority.setup(settings::beyond());
  return rc;
}

message::code
session::set_rate(void)
{
  message::code rc = OK_RATE_SET;
  int value = digital_value::extract(settings::beyond());
  switch (value)
    {
    case digital_value::invalid:
      rc = ERR_PARAMETER_INVALID;
      break;
    case digital_value::too_low:
      rc = ERR_RATE_TOO_LOW;
      break;
    case digital_value::too_high:
      rc = ERR_RATE_TOO_HIGH;
      break;
    default:
      switch (target.selection())
        {
        case destination::self:
          rate_factor(value);
          break;
        case destination::all:
          if (!block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->rate_factor(value);
              host.rate_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!block)
            {
              session* client = host.client(target.id());
              if (client)
                client->rate_factor(value);
              else rc = ERR_NO_SUCH_CLIENT;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        default:
          rc = ERR_PARAMETER_INVALID;
          break;
        }
      break;
    }
  return rc;
}

message::code
session::set_pitch(void)
{
  message::code rc = OK_PITCH_SET;
  int value = digital_value::extract(settings::beyond());
  switch (value)
    {
    case digital_value::invalid:
      rc = ERR_PARAMETER_INVALID;
      break;
    case digital_value::too_low:
      rc = ERR_PITCH_TOO_LOW;
      break;
    case digital_value::too_high:
      rc = ERR_PITCH_TOO_HIGH;
      break;
    default:
      switch (target.selection())
        {
        case destination::self:
          pitch_factor(value);
          break;
        case destination::all:
          if (!block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->pitch_factor(value);
              host.pitch_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!block)
            {
              session* client = host.client(target.id());
              if (client)
                client->pitch_factor(value);
              else rc = ERR_NO_SUCH_CLIENT;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        default:
          rc = ERR_PARAMETER_INVALID;
          break;
        }
      break;
    }
  return rc;
}

message::code
session::set_volume(void)
{
  message::code rc = OK_VOLUME_SET;
  int value = digital_value::extract(settings::beyond());
  switch (value)
    {
    case digital_value::invalid:
      rc = ERR_PARAMETER_INVALID;
      break;
    case digital_value::too_low:
      rc = ERR_VOLUME_TOO_LOW;
      break;
    case digital_value::too_high:
      rc = ERR_VOLUME_TOO_HIGH;
      break;
    default:
      switch (target.selection())
        {
        case destination::self:
          volume_factor(value);
          break;
        case destination::all:
          if (!block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->volume_factor(value);
              host.volume_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!block)
            {
              session* client = host.client(target.id());
              if (client)
                client->volume_factor(value);
              else rc = ERR_NO_SUCH_CLIENT;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        default:
          rc = ERR_PARAMETER_INVALID;
          break;
        }
      break;
    }
  return rc;
}

} // namespace SSIP
