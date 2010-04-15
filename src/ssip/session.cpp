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
#include <boost/filesystem.hpp>

#include <unicode/utf8.h>
#include <unicode/uchar.h>

#include "session.hpp"


namespace SSIP
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace FBB;
using namespace multispeech;


// Counter for session id generation:
unsigned long session::count = 0;


// Session object construction:

session::session(proxy* origin, int socket_fd):
  connection(socket_fd),
  multispeech::session(*input),
  message(*output),
  notified_events(0),
  inside_block(false),
  priority(urgency_mode::text),
  host(*origin),
  receiving(false),
  id(++count)
{
  punctuation = host.punctuation;
  rate_factor = host.rate_factor;
  pitch_factor = host.pitch_factor;
  volume_factor = host.volume_factor;
  spelling = host.spelling;
  capitalization_mode = host.capitalization_mode;
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
      commit(OK_MESSAGE_QUEUED);
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
  const char* s = text.c_str();
  size_t len = text.length();
  int i = 0;
  UChar32 c;
  punctuations::verbosity = punctuation;
  speech_engine::volume(volume_factor);
  speech_engine::voice_pitch(pitch_factor);
  speech_engine::speech_rate(rate_factor);
  speech_engine::char_voice_pitch(pitch_factor);
  speech_engine::char_speech_rate(rate_factor);
  if (spelling)
    while (i < len)
      {
        int k = i;
        U8_NEXT(s, i, len, c);
        if ((capitalization_mode == capitalization::icon) && u_isupper(c))
          caps_icon();
        errand << host.letter_task(string(s + k, s + i));
      }
  else if (capitalization_mode != capitalization::none)
    {
      int m = 0;
      while (i < len)
        {
          int k = i;
          U8_NEXT(s, i, len, c);
          if (u_isupper(c))
            {
              if (k > m)
                errand << host.text_task(string(s + m, s + k));
              switch (capitalization_mode)
                {
                case capitalization::spell:
                  errand << host.letter_task(string(s + k, s + i));
                  m = i;
                  break;
                case capitalization::icon:
                  caps_icon();
                  m = k;
                default:
                  break;
                }
            }
        }
      if (i > m)
        errand << host.text_task(string(s + m, s + i));
    }
  else errand << host.text_task(text);
}

void
session::commit(message::code rc)
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
  emit(rc, errand.id());
  emit(rc);
  accumulator.clear();
  errand.clear();
}

void
session::caps_icon(void)
{
  path sound(complete("capital", host.sounds));
  if (exists(sound))
    errand << sound_task(sound, volume_factor);
  else errand << tone_task(1000, 0.01, volume_factor);
}


// Serving client requests:

bool
session::cmd_speak(void)
{
  accumulator.clear();
  errand = job(id, priority, notified_events);
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
  errand = job(id, priority, notified_events);
  errand << host.letter_task(commands::beyond());
  commit(OK_MESSAGE_QUEUED);
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
  errand = job(id, priority, notified_events);
  while (getline(keyname, token, '_'))
    errand << host.letter_task(token);
  commit(OK_MESSAGE_QUEUED);
  return true;
}

bool
session::cmd_sound_icon(void)
{
  if (exists(host.sounds))
    {
      path sound(complete(commands::beyond(), host.sounds));
      if (exists(sound))
        {
          errand = job(id, priority, notified_events);
          errand << sound_task(sound, volume_factor);
          commit(OK_SND_ICON_QUEUED);
        }
      else emit(ERR_UNKNOWN_ICON);
    }
  else emit(ERR_NO_SND_ICONS);
  return true;
}

bool
session::cmd_block(void)
{
  block_mode block(inside_block);
  emit(block.toggle(commands::beyond()));
  return true;
}

bool
session::cmd_set(void)
{
  destination target;
  emit((this->*settings::findCmd(target.parse(commands::beyond())))(target));
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
session::set_client_name(destination& target)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (inside_block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    rc = client.name(settings::beyond());
  return rc;
}

message::code
session::set_notification(destination& target)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (inside_block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    {
      notification_mode notification(notified_events);
      rc = notification.setup(settings::beyond());
    }
  return rc;
}

message::code
session::set_punctuation(destination& target)
{
  message::code rc = message::OK_PUNCT_MODE_SET;
  if (!inside_block)
    {
      punctuations::mode customized;
      punctuation_mode mode(customized);
      if (mode.parse(settings::beyond()))
        switch (target.selection())
          {
          case destination::self:
            punctuation = customized;
            break;
          case destination::all:
            BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
              client.second->punctuation = customized;
            host.punctuation = customized;
            break;
          case destination::another:
            {
              session* client = host.client(target.id());
              if (client)
                client->punctuation = customized;
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
session::set_priority(destination& target)
{
  message::code rc = ERR_PARAMETER_INVALID;
  if (inside_block)
    rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  else if (target.selection() == destination::self)
    {
      urgency_mode mode(priority);
      rc = mode.setup(settings::beyond());
    }
  return rc;
}

message::code
session::set_rate(destination& target)
{
  message::code rc = OK_RATE_SET;
  double value;
  digital_value rate(value);
  digital_value::status result = rate.parse(settings::beyond());
  switch (result)
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
          rate_factor = value;
          break;
        case destination::all:
          if (!inside_block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->rate_factor = value;
              host.rate_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!inside_block)
            {
              session* client = host.client(target.id());
              if (client)
                client->rate_factor = value;
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
session::set_pitch(destination& target)
{
  message::code rc = OK_PITCH_SET;
  double value;
  digital_value pitch(value);
  digital_value::status result = pitch.parse(settings::beyond());
  switch (result)
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
          pitch_factor = value;
          break;
        case destination::all:
          if (!inside_block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->pitch_factor = value;
              host.pitch_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!inside_block)
            {
              session* client = host.client(target.id());
              if (client)
                client->pitch_factor = value;
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
session::set_volume(destination& target)
{
  message::code rc = OK_VOLUME_SET;
  double value;
  digital_value volume(value);
  digital_value::status result = volume.parse(settings::beyond());
  switch (result)
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
          volume_factor = value;
          break;
        case destination::all:
          if (!inside_block)
            {
              BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
                client.second->volume_factor = value;
              host.volume_factor = value;
            }
          else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
          break;
        case destination::another:
          if (!inside_block)
            {
              session* client = host.client(target.id());
              if (client)
                client->volume_factor = value;
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
session::set_spelling(destination& target)
{
  message::code rc = OK_SPELLING_SET;
  if (!inside_block)
    {
      bool value;
      boolean_flag mode(value);
      if (mode.parse(settings::beyond()))
        switch (target.selection())
          {
          case destination::self:
            spelling = value;
            break;
          case destination::all:
            BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
              client.second->spelling = value;
            host.spelling = value;
            break;
          case destination::another:
            {
              session* client = host.client(target.id());
              if (client)
                client->spelling = value;
              else rc = ERR_NO_SUCH_CLIENT;
            }
            break;
          default:
            rc = ERR_PARAMETER_INVALID;
            break;
          }
      else rc = ERR_PARAMETER_NOT_ON_OFF;
    }
  else rc = ERR_NOT_ALLOWED_INSIDE_BLOCK;
  return rc;
}

message::code
session::set_cap_let_recogn(destination& target)
{
  message::code rc = OK_CAP_LET_RECOGN_SET;
  if (!inside_block)
    {
      capitalization::mode value;
      capitalization mode(value);
      if (mode.parse(settings::beyond()))
        switch (target.selection())
          {
          case destination::self:
            capitalization_mode = value;
            break;
          case destination::all:
            BOOST_FOREACH (proxy::clients_list::value_type client, host.all_clients())
              client.second->capitalization_mode = value;
            host.capitalization_mode = value;
            break;
          case destination::another:
            {
              session* client = host.client(target.id());
              if (client)
                client->capitalization_mode = value;
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

} // namespace SSIP
