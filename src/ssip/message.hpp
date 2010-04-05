// message.hpp -- SSIP message sender interface
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

#ifndef MULTISPEECH_SSIP_MESSAGE_HPP
#define MULTISPEECH_SSIP_MESSAGE_HPP

#include <string>
#include <ostream>
#include <map>

namespace SSIP
{

class message
{
public:
  // Message codes:
  enum code
  {
    // Success messages:
    OK_LANGUAGE_SET = 201,
    OK_PRIORITY_SET = 202,
    OK_RATE_SET = 203,
    OK_PITCH_SET = 204,
    OK_PUNCT_MODE_SET = 205,
    OK_CAP_LET_RECOGN_SET = 206,
    OK_SPELLING_SET = 207,
    OK_CLIENT_NAME_SET = 208,
    OK_VOICE_SET = 209,
    OK_STOPPED = 210,
    OK_PAUSED = 211,
    OK_RESUMED = 212,
    OK_CANCELLED = 213,
    OK_TABLE_SET = 214,
    OK_OUTPUT_MODULE_SET = 215,
    OK_PAUSE_CONTEXT_SET = 216,
    OK_VOLUME_SET = 217,
    OK_SSML_MODE_SET = 218,
    OK_NOTIFICATION_SET = 219,
    OK_CUR_SET_FIRST = 220,
    OK_CUR_SET_LAST = 221,
    OK_CUR_SET_POS = 222,
    OK_CUR_MOV_FOR = 223,
    OK_CUR_MOV_BACK = 224,
    OK_MESSAGE_QUEUED = 225,
    OK_SND_ICON_QUEUED = 226,
    OK_MSG_CANCELLED = 227,
    OK_RECEIVE_DATA = 230,
    OK_BYE = 231,
    OK_CLIENT_LIST_SENT = 240,
    OK_MSGS_LIST_SENT = 241,
    OK_LAST_MSG = 242,
    OK_CUR_POS_RET = 243,
    OK_TABLE_LIST_SENT = 244,
    OK_CLIENT_ID_SENT = 245,
    OK_MSG_TEXT_SENT = 246,
    OK_HELP_SENT = 248,
    OK_VOICE_LIST_SENT = 249,
    OK_MODULES_LIST_SENT = 250,
    OK_INSIDE_BLOCK = 260,
    OK_OUTSIDE_BLOCK = 261,
    OK_DEBUGGING = 262,
    OK_NOT_IMPLEMENTED = 299,

    // Internal server errors:
    ERR_INTERNAL = 300,
    ERR_COULDNT_SET_PRIORITY = 301,
    ERR_COULDNT_SET_LANGUAGE = 302,
    ERR_COULDNT_SET_RATE = 303,
    ERR_COULDNT_SET_PITCH = 304,
    ERR_COULDNT_SET_PUNCT_MODE = 305,
    ERR_COULDNT_SET_CAP_LET_RECOG = 306,
    ERR_COULDNT_SET_SPELLING = 308,
    ERR_COULDNT_SET_VOICE = 309,
    ERR_COULDNT_SET_TABLE = 310,
    ERR_COULDNT_SET_CLIENT_NAME = 311,
    ERR_COULDNT_SET_OUTPUT_MODULE = 312,
    ERR_COULDNT_SET_PAUSE_CONTEXT = 313,
    ERR_COULDNT_SET_VOLUME = 314,
    ERR_COULDNT_SET_SSML_MODE = 315,
    ERR_COULDNT_SET_NOTIFICATION = 316,
    ERR_COULDNT_SET_DEBUGGING = 317,
    ERR_NO_SND_ICONS = 320,
    ERR_CANT_REPORT_VOICES = 321,
    ERR_NO_OUTPUT_MODULE = 322,
    ERR_ALREADY_INSIDE_BLOCK = 330,
    ERR_ALREADY_OUTSIDE_BLOCK = 331,
    ERR_NOT_ALLOWED_INSIDE_BLOCK = 332,
    ERR_NOT_IMPLEMENTED = 380,

    // Rejections:
    ERR_NO_CLIENT = 401,
    ERR_NO_SUCH_CLIENT = 402,
    ERR_NO_MESSAGE = 403,
    ERR_POS_LOW = 404,
    ERR_POS_HIGH = 405,
    ERR_ID_NOT_EXIST = 406,
    ERR_UNKNOWN_ICON = 407,
    ERR_UNKNOWN_PRIORITY = 408,
    ERR_RATE_TOO_HIGH = 409,
    ERR_RATE_TOO_LOW = 410,
    ERR_PITCH_TOO_HIGH = 411,
    ERR_PITCH_TOO_LOW = 412,
    ERR_VOLUME_TOO_HIGH = 413,
    ERR_VOLUME_TOO_LOW = 414,

    // Client errors:
    ERR_INVALID_COMMAND = 500,
    ERR_INVALID_ENCODING = 501,
    ERR_MISSING_PARAMETER = 510,
    ERR_NOT_A_NUMBER = 511,
    ERR_NOT_A_STRING = 512,
    ERR_PARAMETER_NOT_ON_OFF = 513,
    ERR_PARAMETER_INVALID = 514,

    // Event notifications:
    EVENT_INDEX_MARK = 700,
    EVENT_BEGIN = 701,
    EVENT_END = 702,
    EVENT_CANCELLED = 703,
    EVENT_PAUSED = 704,
    EVENT_RESUMED = 705
  };

  // Constructor takes output stream reference as an argument.
  explicit message(std::ostream& destination);

  // Send message along with it's result code. If text is empty or
  // not specified, it will be searched in dictionary for that code.
  void emit(code rc, const std::string& text = "");

  // Send help message.
  void emit_help(void);

private:
  // Output stream reference.
  std::ostream& sink;

  // Message dictionary.
  static std::map<code, const std::string> dictionary;

  // Help message.
  static const char* help[];
};

} // namespace SSIP

#endif
