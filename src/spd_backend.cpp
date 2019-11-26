// spd_backend.cpp -- Speech Dispatcher module interface implementation
/*
   Copyright (C) 2019 Igor B. Poretsky <poretsky@mlbox.ru>
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

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "spd_backend.hpp"

#include "strcvt.hpp"
#include "config.hpp"
#include "file_player.hpp"

using namespace std;
using namespace FBB;
using namespace boost;
using namespace boost::filesystem;


// Speech Dispatcher module commands:
const spd_backend::Entry spd_backend::command_table[] =
  {
    Entry("SPEAK", &spd_backend::do_speak),
    Entry("SOUND_ICON", &spd_backend::do_sound_icon),
    Entry("CHAR", &spd_backend::do_char),
    Entry("KEY", &spd_backend::do_key),
    Entry("STOP", &spd_backend::do_stop),
    Entry("PAUSE", &spd_backend::do_pause),
    Entry("LIST VOICES", &spd_backend::do_list_voices),
    Entry("SET", &spd_backend::do_set),
    Entry("AUDIO", &spd_backend::do_unknown),
    Entry("LOGLEVEL", &spd_backend::do_unknown),
    Entry("DEBUG", &spd_backend::do_unknown),
    Entry("QUIT", &spd_backend::do_quit),
    Entry("", &spd_backend::do_unknown)
  };
static const string cmd_init("INIT");


// Object instantiation:

spd_backend*
spd_backend::instantiate(const configuration& conf)
{
  string cmd;
  getline(cin, cmd);
  if (cin.eof() || cin.fail())
    throw logic_error("Broken pipe when reading INIT");
  if (cmd_init != cmd)
    throw logic_error("Wrong communication from module client: didn't call INIT");
  spd_backend* instance = new spd_backend(conf);
  cout << "299-" << package::name << ": Initialized successfully." << endl;
  cout << "299 OK LOADED SUCCESSFULLY" << endl;
  return instance;
}


// Object construction:

spd_backend::spd_backend(const configuration& conf):
  server(conf),
  CmdFinder<FunctionPtr>(command_table, command_table +
                         (sizeof(command_table) / sizeof(Entry))),
  settings(speechmaster),
  sound_icons((conf.option_value.count(options::spd::sound_icons) &&
               !conf.option_value[options::spd::sound_icons].as<string>().empty()) ?
              conf.option_value[options::spd::sound_icons].as<string>() :
              string("")),
  lines(0),
  mark_pattern("<mark\\s+name=\"([^\"]*)\"\\s*/>"),
  state(idle)
{
  stripper.setup()
    (L"<[^<>]+>", L"")
    (L"&lt;", L"<")
    (L"&gt;", L">")
    (L"&amp;", L"&")
    (L"&quot;", L"\"")
    (L"&apos;", L"'");
}


// Prepare to the next command reception cycle:

void
spd_backend::communication_reset(void)
{
  server::communication_reset();
  data.erase();
  lines = 0;
}


// Extra data reception control:

bool
spd_backend::extra_data(void)
{
  if (state_ok())
    {
      if (!lines)
        cout << "202 OK RECEIVING MESSAGE" << endl;
      return (lines < 0) || !data.empty();
    }
  return false;
}


// State checking:

bool
spd_backend::can_speak(void)
{
  bool ok = (state == idle) && !data.empty();
  if (ok)
    cout << "200 OK SPEAKING" << endl;
  else cout << "301 ERROR CANT SPEAK" << endl;
  return ok;
}

bool
spd_backend::single_line(void)
{
  if (lines < 2)
    return true;
  cout << "305 DATA MORE THAN ONE LINE" << endl;
  return false;
}


// Start queue execution:

void
spd_backend::start_queue(void)
{
  state = speaking;
  soundmaster.proceed();
  cout << "701 BEGIN" << endl;
}


// Events serving:

void
spd_backend::index_mark(const string& name)
{
  mutex::scoped_lock lock(access);
  if (state == pausing)
    soundmaster.stop();
  cout << "700-" << name << endl;
  cout << "700 INDEX MARK" << endl;
}

void
spd_backend::queue_done(void)
{
  mutex::scoped_lock lock(access);
  switch (state)
    {
    case speaking:
      cout << "702 END" << endl;
      break;
    case stopping:
      cout << "703 STOP" << endl;
      break;
    case pausing:
      cout << "704 PAUSE" << endl;
      break;
    default:
      break;
    }
  state = idle;
}


// Input method:

void
spd_backend::get_command(void)
{
  string s;
  getline(cin, s);
  if (cin.eof() || cin.fail())
    {
      if (server::cmd.empty())
        server::cmd = "QUIT";
      exit_status = EXIT_FAILURE;
    }
  else if (server::cmd.empty())
    server::cmd = s;
  else if (s == ".")
    {
      if (lines)
        {
          accumulator << flush;
          data = accumulator.str();
          accumulator.str("");
        }
      else lines = -1;
    }
  else
    {
      if (lines++)
        accumulator << endl;
      if (s == "..")
        accumulator << ".";
      else accumulator << s;
    }
}


// Place text chunk into the speech queue:

void
spd_backend::enqueue_text_chunk(string::const_iterator start, string::const_iterator end)
{
  if (start != end)
    {
      wostringstream text;
      stripper.push(text);
      stripper << intern_string(string(start, end), input_charset);
      stripper.pop();
      soundmaster.enqueue(speechmaster.text_task(text.str(), &settings));
    }
}


// Command set and syntax implementation:

bool
spd_backend::perform_command(void)
{
  return (this->*findCmd(server::cmd))();
}

bool
spd_backend::state_ok(void)
{
  if (exit_status != EXIT_SUCCESS)
    {
      cout << "401 ERROR INTERNAL" <<endl;
      communication_reset();
      exit_status = EXIT_SUCCESS;
      return false;
    }
  return true;
}

bool
spd_backend::do_quit(void)
{
  if (exit_status == EXIT_SUCCESS)
    cout << "210 OK QUIT" << endl;
  return false;
}

bool
spd_backend::do_speak(void)
{
  if (extra_data())
    {
      mutex::scoped_lock lock(access);
      if (can_speak())
        {
          string::const_iterator start = data.begin();
          string::const_iterator end = data.end();
          smatch found;
          while (regex_search(start, end, found, mark_pattern))
            {
              enqueue_text_chunk(start, found[0].first);
              soundmaster.enqueue(string(found[1].first, found[1].second));
              start = found[0].second;
            }
          enqueue_text_chunk(start, end);
          start_queue();
        }
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_char(void)
{
  if (extra_data())
    {
      if (single_line())
        {
          mutex::scoped_lock lock(access);
          if (can_speak())
            {
              soundmaster.enqueue(speechmaster.letter_task(intern_string(data, input_charset), &settings));
              start_queue();
            }
        }
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_key(void)
{
  return do_char();
}

bool
spd_backend::do_sound_icon(void)
{
  if (extra_data())
    {
      if (single_line())
        {
          mutex::scoped_lock lock(access);
          if (can_speak())
            {
              if (sound_icons.empty())
                soundmaster.enqueue(speechmaster.text_task(intern_string(data, input_charset), &settings));
              else
                {
                  path icon_file(complete(data, sound_icons));
                  if (exists(icon_file))
                    soundmaster.enqueue(sound_task(icon_file));
                  else soundmaster.enqueue(speechmaster.text_task(intern_string(data, input_charset), &settings));
                }
              start_queue();
            }
        }
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_stop(void)
{
  if (state_ok())
    {
      mutex::scoped_lock lock(access);
      if (state != idle)
        state = stopping;
      soundmaster.stop();
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_pause(void)
{
  if (state_ok())
    {
      mutex::scoped_lock lock(access);
      if (state == speaking)
        state = pausing;
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_list_voices(void)
{
  if (state_ok())
    {
      for (int i = 0; i < speechmaster.talker.size(); i++)
        if (speechmaster.talker[i].get())
          {
            cout << "200-" << speechmaster.talker[i]->name;
            if (!speechmaster.talker[i]->voice.empty())
              cout << '-' << speechmaster.talker[i]->voice;
            string lang(speechmaster.talker[i]->language->id());
            cout << ' ';
            if (lang != lang_id::br)
              cout << lang << " none";
            else cout << "pt br";
            cout << endl;
          }
      cout << "200 OK VOICE LIST SENT" << endl;
      communication_reset();
    }
  return true;
}

bool
spd_backend::do_set(void)
{
  if (state_ok())
    {
      if (!lines)
        cout << "203 OK RECEIVING SETTINGS" << endl;
      if ((lines < 0) || !data.empty())
        {
          settings.apply(data);
          communication_reset();
        }
    }
  return true;
}

bool
spd_backend::do_unknown(void)
{
  if (state_ok())
    {
      cout << "300 ERR UNKNOWN COMMAND" << endl;
      communication_reset();
    }
  return true;
}
