// file_player.hpp -- Sound file player interface
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

// The file_player class introduces typical sound playing task
// description and provides basic playing control means along with
// pending tasks queue maintenance.

#ifndef MULTISPEECH_FILE_PLAYER_HPP
#define MULTISPEECH_FILE_PLAYER_HPP

#include <string>

#include <boost/filesystem.hpp>

#include "soundfile.hpp"
#include "exec_queue.hpp"

// Typical task is represented by sound file specification
// and relative volume level.
class sound_task
{
public:
  // Object constructor:
  sound_task(const boost::filesystem::path& sound_file,
             float sound_volume = 1.0);

private:
  // Properties accessible only for actual executor:
  const boost::filesystem::path file;
  const float volume;

  friend class file_player;
};

class file_player:
  public soundfile,
  private exec_queue<sound_task>
{
public:
  // Object constructor:
  file_player(completion_callback* cb);

  // Start task execution. If there is a task executing already
  // and the new one is started with the override option, then
  // current task is stopped immediately and the new one is started.
  // Otherwise the new task is appended to the pending tasks queue.
  void start(const sound_task& sound, bool override);

  // Stop currently playing sound if any and clear queue:
  void stop(void);

  // Return true if something is currently playing:
  bool active(void);

  // Configurable parameters:
  static std::string device;
  static bool asynchronous;
  static float relative_volume;

private:
  // Method required by audioplayer:
  void notify_completion(void);

  // Methods required by exec_queue:
  void execute(const sound_task& task);
  void abort(void);
  bool busy(void);
};

#endif
