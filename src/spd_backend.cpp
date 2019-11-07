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

#include <string>

#include "spd_backend.hpp"

using namespace std;


// Speech Dispatcher module commands:
static const wstring cmd_init = L"INIT";
static const wstring cmd_speak = L"SPEAK";
static const wstring cmd_sound_icon = L"SOUND_ICON";
static const wstring cmd_char = L"CHAR";
static const wstring cmd_key = L"KEY";
static const wstring cmd_stop = L"STOP";
static const wstring cmd_pause = L"PAUSE";
static const wstring cmd_list_voices = L"LIST VOICES";
static const wstring cmd_set = L"SET";
static const wstring cmd_audio = L"AUDIO";
static const wstring cmd_loglevel = L"LOGLEVEL";
static const wstring cmd_debug = L"DEBUG";
static const wstring cmd_quit = L"QUIT";


// Object construction:

spd_backend::spd_backend(const configuration& conf):
  server(conf)
{
}


// Events serving:

void
spd_backend::queue_done(void)
{
}


// Input method:

void
spd_backend::get_command(void)
{
}


// Command set and syntax implementation:

bool
spd_backend::perform_command(void)
{
  return false;
}
