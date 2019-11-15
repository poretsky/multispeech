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

#include "spd_backend.hpp"

#include "strcvt.hpp"
#include "config.hpp"

using namespace std;
using namespace boost;


// Speech Dispatcher module commands:
static const string cmd_init("INIT");
static const wstring cmd_speak(L"SPEAK");
static const wstring cmd_sound_icon(L"SOUND_ICON");
static const wstring cmd_char(L"CHAR");
static const wstring cmd_key(L"KEY");
static const wstring cmd_stop(L"STOP");
static const wstring cmd_pause(L"PAUSE");
static const wstring cmd_list_voices(L"LIST VOICES");
static const wstring cmd_set(L"SET");
static const wstring cmd_audio(L"AUDIO");
static const wstring cmd_loglevel(L"LOGLEVEL");
static const wstring cmd_debug(L"DEBUG");
static const wstring cmd_quit(L"QUIT");


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
  lines(0),
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
spd_backend::reset(void)
{
  cmd.erase();
  data.erase();
  accumulator.str("");
  lines = 0;
}


// Extra data reception control:

bool
spd_backend::extra_data(void)
{
  if (!lines)
    cout << "202 OK RECEIVING MESSAGE" << endl;
  return (lines < 0) || !data.empty();
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
      if (cmd.empty())
        cmd = cmd_quit;
      exit_status = EXIT_FAILURE;
    }
  else if (cmd.empty())
    cmd = intern_string(s, input_charset);
  else if (s == ".")
    {
      if (lines)
        {
          accumulator << flush;
          data = intern_string(accumulator.str(), input_charset);
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


// Command set and syntax implementation:

bool
spd_backend::perform_command(void)
{
  if (cmd_quit == cmd)
    {
      if (exit_status == EXIT_SUCCESS)
        cout << "210 OK QUIT" << endl;
      return false;
    }
  else if (exit_status != EXIT_SUCCESS)
    {
      cout << "401 ERROR INTERNAL" <<endl;
      reset();
      exit_status = EXIT_SUCCESS;
    }
  else if (cmd_speak == cmd)
    {
      if (extra_data())
        {
          mutex::scoped_lock lock(access);
          if (can_speak())
            {
              wostringstream text;
              stripper.push(text);
              stripper << data;
              stripper.pop();
              soundmaster.enqueue(speechmaster.text_task(text.str()));
              start_queue();
            }
          reset();
        }
    }
  else if ((cmd_char == cmd) || (cmd_key == cmd))
    {
      if (extra_data())
        {
          if (single_line())
            {
              mutex::scoped_lock lock(access);
              if (can_speak())
                {
                  soundmaster.enqueue(speechmaster.letter_task(data));
                  start_queue();
                }
            }
          reset();
        }
    }
  else if (cmd_stop == cmd)
    {
      mutex::scoped_lock lock(access);
      if (state == speaking)
        {
          state = stopping;
          soundmaster.stop();
        }
      reset();
    }
  else
    {
      cout << "300 ERR UNKNOWN COMMAND" << endl;
      reset();
    }
  return true;
}
