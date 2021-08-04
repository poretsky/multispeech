// frontend.cpp -- Speech Dispatcher module interface implementation
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
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdexcept>

#include <bobcat/string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <libspeechd_version.h>

#include "frontend.hpp"

#include "strcvt.hpp"
#include "config.hpp"
#include "file_player.hpp"

using namespace std;
using namespace FBB;
using namespace boost;
using namespace boost::filesystem;


// Common message:
const string frontend::bad_syntax("302 ERROR BAD SYNTAX");

// Speech Dispatcher module commands:
const frontend::Entry frontend::command_table[] =
  {
    Entry("SPEAK", &frontend::do_speak),
    Entry("SOUND_ICON", &frontend::do_sound_icon),
    Entry("CHAR", &frontend::do_char),
    Entry("KEY", &frontend::do_key),
    Entry("STOP", &frontend::do_stop),
    Entry("PAUSE", &frontend::do_pause),
    Entry("LIST", &frontend::do_list_voices),
    Entry("SET", &frontend::do_set),
    Entry("AUDIO", &frontend::do_audio),
    Entry("LOGLEVEL", &frontend::do_loglevel),
    Entry("DEBUG", &frontend::do_debug),
    Entry("QUIT", &frontend::do_quit),
    Entry("", &frontend::do_unknown)
  };
static const string cmd_init("INIT");


// Object instantiation:

frontend*
frontend::instantiate(const configuration& conf)
{
  string cmd;
  getline(cin, cmd);
  if (cin.eof() || cin.fail())
    throw logic_error("Broken pipe when reading INIT");
  if (cmd_init != cmd)
    throw logic_error("Wrong communication from module client: didn't call INIT");
  frontend* instance = new frontend(conf);
  cout << "299-" << package::name << ": Initialized successfully." << endl;
  cout << "299 OK LOADED SUCCESSFULLY" << endl;
  return instance;
}


// Object construction:

frontend::frontend(const configuration& conf):
  speech_server(conf),
  CmdFinder<FunctionPtr>(command_table, command_table +
                         (sizeof(command_table) / sizeof(Entry)),
                         USE_FIRST),
  settings(conf, speechmaster),
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

  int version_major = LIBSPEECHD_MAJOR_VERSION;
  int version_minor = LIBSPEECHD_MINOR_VERSION;
  regex version_format("(\\d+)(\\.(\\d+))?.*");
  smatch version;
  if (conf.option_value.count(options::spd::version)
      && regex_match( conf.option_value[options::spd::version].as<string>(), version, version_format)
      && version[1].matched)
    {
      version_major = lexical_cast<int>(string(version[1].first, version[1].second));
      version_minor = version[3].matched ? lexical_cast<int>(string(version[3].first, version[3].second)) : 0;
    }
  else
    {
      FILE* spd = popen("speech-dispatcher -v", "r");
      if (spd)
        {
          char s[100];
          ostringstream info;
          while (fgets(s, 80, spd))
            info << s;
          pclose(spd);
          if (regex_search(info.str(), version, version_format)
              && version[1].matched)
            {
              version_major = lexical_cast<int>(string(version[1].first, version[1].second));
              version_minor = version[3].matched ? lexical_cast<int>(string(version[3].first, version[3].second)) : 0;
            }
        }
    }
  atom_separator = ((version_major < 1) && (version_minor < 9)) ? ' ' : '\t';
}


// Prepare to the next command reception cycle:

void
frontend::communication_reset(void)
{
  speech_server::communication_reset();
  data.erase();
  lines = 0;
}


// Extra data reception control:

bool
frontend::extra_data(const char* msg)
{
  if (state_ok())
    {
      if (!lines)
        cout << (msg ? msg : "202 OK RECEIVING MESSAGE") << endl;
      return (lines < 0) || !data.empty();
    }
  return false;
}


// State checking:

bool
frontend::can_speak(void)
{
  bool ok = (state == idle) && !data.empty();
  if (ok)
    cout << "200 OK SPEAKING" << endl;
  else cout << "301 ERROR CANT SPEAK" << endl;
  return ok;
}

bool
frontend::single_line(void)
{
  if (lines < 2)
    return true;
  cout << "305 DATA MORE THAN ONE LINE" << endl;
  return false;
}


// Start queue execution:

void
frontend::start_queue(void)
{
  state = speaking;
  soundmaster.proceed();
  cout << "701 BEGIN" << endl;
}


// Events serving:

void
frontend::index_mark(const string& name)
{
  boost::mutex::scoped_lock lock(access);
  if (state == pausing)
    soundmaster.stop();
  cout << "700-" << name << endl;
  cout << "700 INDEX MARK" << endl;
}

void
frontend::queue_done(void)
{
  boost::mutex::scoped_lock lock(access);
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
frontend::get_command(void)
{
  string s;
  getline(cin, s);
  if (cin.eof() || cin.fail())
    {
      if (speech_server::cmd.empty())
        speech_server::cmd = "QUIT";
      exit_status = EXIT_FAILURE;
    }
  else if (speech_server::cmd.empty())
    speech_server::cmd = s;
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
frontend::enqueue_text_chunk(string::const_iterator start, string::const_iterator end)
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
frontend::perform_command(void)
{
  return (this->*findCmd(speech_server::cmd))();
}

bool
frontend::state_ok(void)
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
frontend::do_quit(void)
{
  if (exit_status == EXIT_SUCCESS)
    cout << "210 OK QUIT" << endl;
  return false;
}

bool
frontend::do_speak(void)
{
  if (extra_data())
    {
      boost::mutex::scoped_lock lock(access);
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
frontend::do_char(void)
{
  if (extra_data())
    {
      if (single_line())
        {
          boost::mutex::scoped_lock lock(access);
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
frontend::do_key(void)
{
  return do_char();
}

bool
frontend::do_sound_icon(void)
{
  if (extra_data())
    {
      if (single_line())
        {
          boost::mutex::scoped_lock lock(access);
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
frontend::do_stop(void)
{
  if (state_ok())
    {
      boost::mutex::scoped_lock lock(access);
      if (state != idle)
        state = stopping;
      soundmaster.stop();
      communication_reset();
    }
  return true;
}

bool
frontend::do_pause(void)
{
  if (state_ok())
    {
      boost::mutex::scoped_lock lock(access);
      if (state == speaking)
        state = pausing;
      communication_reset();
    }
  return true;
}

bool
frontend::do_list_voices(void)
{
  if (beyond() != "VOICES")
    return do_unknown();
  if (state_ok())
    {
      for (int i = 0; i < speechmaster.talker.size(); i++)
        if (speechmaster.talker[i].get())
          {
            cout << "200-" << speechmaster.talker[i]->name;
            if (!speechmaster.talker[i]->voice.empty())
              cout << '-' << speechmaster.talker[i]->voice;
            cout << atom_separator << speechmaster.talker[i]->language->id() << atom_separator << "none" << endl;
          }
      cout << "200 OK VOICE LIST SENT" << endl;
      communication_reset();
    }
  return true;
}

bool
frontend::do_set(void)
{
  if (extra_data("203 OK RECEIVING SETTINGS"))
    {
      settings.apply(data);
      communication_reset();
    }
  return true;
}

bool
frontend::do_audio(void)
{
  if (extra_data("207 OK RECEIVING AUDIO SETTINGS"))
    {
      cout << "203 OK AUDIO INITIALIZED" << endl;
      communication_reset();
    }
  return true;
}

bool
frontend::do_loglevel(void)
{
  if (extra_data("207 OK RECEIVING LOGLEVEL SETTINGS"))
    {
      cout << "203 OK LOG LEVEL SET" << endl;
      communication_reset();
    }
  return true;
}

bool
frontend::do_debug(void)
{
  if (state_ok())
    {
      if (beyond() == "OFF")
        {
          if (verbose)
            {
              redirect_stderr();
              verbose = false;
            }
          cout << "200 OK DEBUGGING OFF" << endl;
        }
      else
        {
          vector<string> args;
          if ((String::split(&args, beyond()) == 2) && (args[0] == "ON"))
            {
              if (redirect_stderr(args[1].c_str()))
                {
                  verbose = true;
                  cout << "200 OK DEBUGGING ON" << endl;
                }
              else cout << "303 CANT OPEN CUSTOM DEBUG FILE" << endl;
            }
          else cout << bad_syntax << endl;
        }
      communication_reset();
    }
  return true;
}

bool
frontend::do_unknown(void)
{
  if (state_ok())
    {
      cout << "300 ERR UNKNOWN COMMAND" << endl;
      communication_reset();
    }
  return true;
}
