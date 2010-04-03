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
map<unsigned int, const string> message::dictionary = map_list_of

  // Success messages:
  (201, "OK LANGUAGE SET")
  (202, "OK PRIORITY SET")
  (203, "OK RATE SET")
  (204, "OK PITCH SET")
  (205, "OK PUNCTUATION SET")
  (206, "OK CAP LET RECOGNITION SET")
  (207, "OK SPELLING SET")
  (208, "OK CLIENT NAME SET")
  (209, "OK VOICE SET")
  (210, "OK STOPPED")
  (211, "OK PAUSED")
  (212, "OK RESUMED")
  (213, "OK CANCELLED")
  (214, "OK TABLE SET")
  (215, "OK OUTPUT MODULE SET")
  (216, "OK PAUSE CONTEXT SET")
  (217, "OK VOLUME SET")
  (218, "OK SSML MODE SET")
  (219, "OK NOTIFICATION SET")
  (220, "OK CURSOR SET FIRST")
  (221, "OK CURSOR SET LAST")
  (222, "OK CURSOR SET TO POSITION")
  (223, "OK CURSOR MOVED FORWARD")
  (224, "OK CURSOR MOVED BACKWARD")
  (225, "OK MESSAGE QUEUED")
  (226, "OK SOUND ICON QUEUED")
  (227, "OK MESSAGE CANCELLED")
  (230, "OK RECEIVING DATA")
  (231, "HAPPY HACKING")
  (240, "OK CLIENTS LIST SENT")
  (241, "OK MSGS LIST SENT")
  (242, "OK LAST MSG SAID")
  (243, "OK CURSOR POSITION RETURNED")
  (244, "OK TABLE LIST SEND")
  (245, "OK CLIENT ID SENT")
  (246, "OK MESSAGE TEXT SENT")
  (248, "OK HELP SENT")
  (249, "OK VOICE LIST SENT")
  (260, "OK INSIDE BLOCK")
  (261, "OK OUTSIDE BLOCK")
  (262, "OK DEBUGGING SET")
  (299, "OK BUT NOT IMPLEMENTED -- DOES NOTHING")

  // Internal server errors:
  (300, "ERR INTERNAL")
  (301, "ERR COULDN'T SET PRIORITY")
  (302, "ERR COULDN'T SET LANGUAGE")
  (303, "ERR COULDN'T SET RATE")
  (304, "ERR COULDN'T SET PITCH")
  (305, "ERR COULDN'T SET PUNCT MODE")
  (306, "ERR COULDN'T SET CAP LET RECOGNITION")
  (308, "ERR COULDN'T SET SPELLING")
  (309, "ERR COULDN'T SET VOICE")
  (310, "ERR COULDN'T SET TABLE")
  (311, "ERR COULDN'T SET CLIENT NAME")
  (312, "ERR COULDN'T SET OUTPUT MODULE")
  (313, "ERR COULDN'T SET PAUSE CONTEXT")
  (314, "ERR COULDN'T SET VOLUME")
  (315, "ERR COULDN'T SET SSML MODE")
  (316, "ERR COULDN'T SET NOTIFICATION")
  (317, "ERR COULDN'T SET DEBUGGING")
  (320, "ERR NO SOUND ICONS")
  (321, "ERR MODULE CAN'T REPORT VOICES")
  (322, "ERR NO OUTPUT MODULE LOADED")
  (330, "ERR ALREADY INSIDE BLOCK")
  (331, "ERR ALREADY OUTSIDE BLOCK")
  (332, "ERR NOT ALLOWED INSIDE BLOCK")
  (380, "ERR NOT YET IMPLEMENTED")

  // Rejections:
  (401, "ERR NO CLIENT")
  (402, "ERR NO SUCH CLIENT")
  (403, "ERR NO MESSAGE")
  (404, "ERR POSITION TOO LOW")
  (405, "ERR POSITION TOO HIGH")
  (406, "ERR ID DOESN'T EXIST")
  (407, "ERR UNKNOWN ICON")
  (408, "ERR UNKNOWN PRIORITY")
  (409, "ERR RATE TOO HIGH")
  (410, "ERR RATE TOO LOW")
  (411, "ERR PITCH TOO HIGH")
  (412, "ERR PITCH TOO LOW")

  // Client errors:
  (500, "ERR INVALID COMMAND")
  (501, "ERR INVALID ENCODING")
  (510, "ERR MISSING PARAMETER")
  (511, "ERR PARAMETER NOT A NUMBER")
  (512, "ERR PARAMETER NOT A STRING")
  (513, "ERR PARAMETER NOT ON OR OFF")
  (514, "ERR PARAMETER INVALID")

  // Event notifications:
  (700, "INDEX MARK")
  (701, "BEGIN")
  (702, "END")
  (703, "CANCELLED")
  (704, "PAUSED")
  (705, "RESUMED");


// Object constructor:

message::message(ostream& destination):
  sink(destination)
{
}


// Public methods:

void
message::emit(unsigned int rc, const string& text)
{
  if (text.empty())
    sink << rc << ' ' << dictionary[rc] << "\r\n";
  else sink << rc << '-' << text << "\r\n";
}

} // namespace SSIP
