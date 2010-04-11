// message.cpp -- SSIP message sender implementation
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

#include "message.hpp"


namespace SSIP
{

using namespace std;
using namespace boost::assign;


// Pre-defined message dictionary:
map<message::code, const string> message::dictionary = map_list_of

  // Success messages:
  (OK_LANGUAGE_SET, "OK LANGUAGE SET")
  (OK_PRIORITY_SET, "OK PRIORITY SET")
  (OK_RATE_SET, "OK RATE SET")
  (OK_PITCH_SET, "OK PITCH SET")
  (OK_PUNCT_MODE_SET, "OK PUNCTUATION SET")
  (OK_CAP_LET_RECOGN_SET, "OK CAP LET RECOGNITION SET")
  (OK_SPELLING_SET, "OK SPELLING SET")
  (OK_CLIENT_NAME_SET, "OK CLIENT NAME SET")
  (OK_VOICE_SET, "OK VOICE SET")
  (OK_STOPPED, "OK STOPPED")
  (OK_PAUSED, "OK PAUSED")
  (OK_RESUMED, "OK RESUMED")
  (OK_CANCELLED, "OK CANCELLED")
  (OK_TABLE_SET, "OK TABLE SET")
  (OK_OUTPUT_MODULE_SET, "OK OUTPUT MODULE SET")
  (OK_PAUSE_CONTEXT_SET, "OK PAUSE CONTEXT SET")
  (OK_VOLUME_SET, "OK VOLUME SET")
  (OK_SSML_MODE_SET, "OK SSML MODE SET")
  (OK_NOTIFICATION_SET, "OK NOTIFICATION SET")
  (OK_CUR_SET_FIRST, "OK CURSOR SET FIRST")
  (OK_CUR_SET_LAST, "OK CURSOR SET LAST")
  (OK_CUR_SET_POS, "OK CURSOR SET TO POSITION")
  (OK_CUR_MOV_FOR, "OK CURSOR MOVED FORWARD")
  (OK_CUR_MOV_BACK, "OK CURSOR MOVED BACKWARD")
  (OK_MESSAGE_QUEUED, "OK MESSAGE QUEUED")
  (OK_SND_ICON_QUEUED, "OK SOUND ICON QUEUED")
  (OK_MSG_CANCELLED, "OK MESSAGE CANCELLED")
  (OK_RECEIVE_DATA, "OK RECEIVING DATA")
  (OK_BYE, "HAPPY HACKING")
  (OK_CLIENT_LIST_SENT, "OK CLIENTS LIST SENT")
  (OK_MSGS_LIST_SENT, "OK MSGS LIST SENT")
  (OK_LAST_MSG, "OK LAST MSG SAID")
  (OK_CUR_POS_RET, "OK CURSOR POSITION RETURNED")
  (OK_TABLE_LIST_SENT, "OK TABLE LIST SEND")
  (OK_CLIENT_ID_SENT, "OK CLIENT ID SENT")
  (OK_MSG_TEXT_SENT, "OK MESSAGE TEXT SENT")
  (OK_HELP_SENT, "OK HELP SENT")
  (OK_VOICE_LIST_SENT, "OK VOICE LIST SENT")
  (OK_MODULES_LIST_SENT, "OK MODULES LIST SENT")
  (OK_INSIDE_BLOCK, "OK INSIDE BLOCK")
  (OK_OUTSIDE_BLOCK, "OK OUTSIDE BLOCK")
  (OK_DEBUGGING, "OK DEBUGGING SET")
  (OK_NOT_IMPLEMENTED, "OK BUT NOT IMPLEMENTED -- DOES NOTHING")

  // Internal server errors:
  (ERR_INTERNAL, "ERR INTERNAL")
  (ERR_COULDNT_SET_PRIORITY, "ERR COULDN'T SET PRIORITY")
  (ERR_COULDNT_SET_LANGUAGE, "ERR COULDN'T SET LANGUAGE")
  (ERR_COULDNT_SET_RATE, "ERR COULDN'T SET RATE")
  (ERR_COULDNT_SET_PITCH, "ERR COULDN'T SET PITCH")
  (ERR_COULDNT_SET_PUNCT_MODE, "ERR COULDN'T SET PUNCT MODE")
  (ERR_COULDNT_SET_CAP_LET_RECOG, "ERR COULDN'T SET CAP LET RECOGNITION")
  (ERR_COULDNT_SET_SPELLING, "ERR COULDN'T SET SPELLING")
  (ERR_COULDNT_SET_VOICE, "ERR COULDN'T SET VOICE")
  (ERR_COULDNT_SET_TABLE, "ERR COULDN'T SET TABLE")
  (ERR_COULDNT_SET_CLIENT_NAME, "ERR COULDN'T SET CLIENT NAME")
  (ERR_COULDNT_SET_OUTPUT_MODULE, "ERR COULDN'T SET OUTPUT MODULE")
  (ERR_COULDNT_SET_PAUSE_CONTEXT, "ERR COULDN'T SET PAUSE CONTEXT")
  (ERR_COULDNT_SET_VOLUME, "ERR COULDN'T SET VOLUME")
  (ERR_COULDNT_SET_SSML_MODE, "ERR COULDN'T SET SSML MODE")
  (ERR_COULDNT_SET_NOTIFICATION, "ERR COULDN'T SET NOTIFICATION")
  (ERR_COULDNT_SET_DEBUGGING, "ERR COULDN'T SET DEBUGGING")
  (ERR_NO_SND_ICONS, "ERR NO SOUND ICONS")
  (ERR_CANT_REPORT_VOICES, "ERR MODULE CAN'T REPORT VOICES")
  (ERR_NO_OUTPUT_MODULE, "ERR NO OUTPUT MODULE LOADED")
  (ERR_ALREADY_INSIDE_BLOCK, "ERR ALREADY INSIDE BLOCK")
  (ERR_ALREADY_OUTSIDE_BLOCK, "ERR ALREADY OUTSIDE BLOCK")
  (ERR_NOT_ALLOWED_INSIDE_BLOCK, "ERR NOT ALLOWED INSIDE BLOCK")
  (ERR_NOT_IMPLEMENTED, "ERR NOT YET IMPLEMENTED")

  // Rejections:
  (ERR_NO_CLIENT, "ERR NO CLIENT")
  (ERR_NO_SUCH_CLIENT, "ERR NO SUCH CLIENT")
  (ERR_NO_MESSAGE, "ERR NO MESSAGE")
  (ERR_POS_LOW, "ERR POSITION TOO LOW")
  (ERR_POS_HIGH, "ERR POSITION TOO HIGH")
  (ERR_ID_NOT_EXIST, "ERR ID DOESN'T EXIST")
  (ERR_UNKNOWN_ICON, "ERR UNKNOWN ICON")
  (ERR_UNKNOWN_PRIORITY, "ERR UNKNOWN PRIORITY")
  (ERR_RATE_TOO_HIGH, "ERR RATE TOO HIGH")
  (ERR_RATE_TOO_LOW, "ERR RATE TOO LOW")
  (ERR_PITCH_TOO_HIGH, "ERR PITCH TOO HIGH")
  (ERR_PITCH_TOO_LOW, "ERR PITCH TOO LOW")
  (ERR_VOLUME_TOO_HIGH, "ERR VOLUME TOO HIGH")
  (ERR_VOLUME_TOO_LOW, "ERR VOLUME TOO LOW")

  // Client errors:
  (ERR_INVALID_COMMAND, "ERR INVALID COMMAND")
  (ERR_INVALID_ENCODING, "ERR INVALID ENCODING")
  (ERR_MISSING_PARAMETER, "ERR MISSING PARAMETER")
  (ERR_NOT_A_NUMBER, "ERR PARAMETER NOT A NUMBER")
  (ERR_NOT_A_STRING, "ERR PARAMETER NOT A STRING")
  (ERR_PARAMETER_NOT_ON_OFF, "ERR PARAMETER NOT ON OR OFF")
  (ERR_PARAMETER_INVALID, "ERR PARAMETER INVALID")

  // Event notifications:
  (EVENT_INDEX_MARK, "INDEX MARK")
  (EVENT_BEGIN, "BEGIN")
  (EVENT_END, "END")
  (EVENT_CANCELLED, "CANCELLED")
  (EVENT_PAUSED, "PAUSED")
  (EVENT_RESUMED, "RESUMED");

// Help message for a client:
const char* message::help[] =
  {
    "  SPEAK           -- say text",
    "  KEY             -- say a combination of keys",
    "  CHAR            -- say a character",
    "  SOUND_ICON      -- execute a sound icon",
    "  SET             -- set a parameter",
    "  LIST            -- list available arguments",
    "  HISTORY         -- commands related to history",
    "  QUIT            -- close the connection"
  };


// Object constructor:

message::message(ostream& destination):
  sink(destination)
{
}


// Public methods:

void
message::emit(code rc, const string& text)
{
  if (text.empty())
    sink << rc << ' ' << dictionary[rc] << "\r\n";
  else sink << rc << '-' << text << "\r\n";
}

void
message::emit(code rc, unsigned int id)
{
  sink << rc << '-' << id << "\r\n";
}

void
message::emit_help(void)
{
  for (int i = 0; i < (sizeof(help) / sizeof(char*)); i++)
    emit(OK_HELP_SENT, help[i]);
  emit(OK_HELP_SENT);
}

} // namespace SSIP
