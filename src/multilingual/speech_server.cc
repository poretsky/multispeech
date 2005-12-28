// Multilingual speech server for Emacspeak

// Author: Igor B. Poretsky <root@goga.energo.ru>

// This file is not part of GNU Emacs, but the same permissions apply.

// GNU Emacs is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.

// GNU Emacs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GNU Emacs; see the file COPYING.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "xmalloc.h"
#include "queue.h"
#include "subprocess.h"
#include "source.h"
#include "punctuations.h"
#include "language.h"
#include "Brazilian.h"
#include "English.h"
#include "French.h"
#include "German.h"
#include "Russian.h"
#include "Spanish.h"

// Default paths and names
char *letters_lib = "/usr/local/lib/multispeech/letters";
char *players_dir = "/usr/local/lib/multispeech/players";
char *tts_dir = "/usr/local/lib/multispeech/tts";
char *audio_player = "sound";
char *tone_generator = "tones";
char *default_device = "/dev/dsp";
char *speech_device;
char *sound_device;
char *tones_device;
int auto_switch;

// Common objects
queue jobs, urgent, backup;
source parent;
subprocess speaker;
int talking = 0;
int sflag = 0;
language *tts = NULL;

void define_language(char *text)
{
  if (auto_switch)
    {
      if (Russian->check(text)) tts = Russian;
      else if (English->check(text)) tts = English;
    }
}

// Select language
static language* languages[] =
  {
    Brazilian,
    English,
    French,
    German,
    Russian,
    Spanish
  };
enum {MAX_AVAILABLE_LANG = sizeof(languages) / sizeof(languages[0])};

int set_language(char *lang)
{
  int found = 0;
  int i;
  
  if (!lang)
    {
      return 0;
    }

  for (i=0; i < MAX_AVAILABLE_LANG; i++)
    {
      if (strcmp(languages[i]->id, lang) == 0) 
	{
	  tts = languages[i];
	  setlocale(LC_CTYPE, tts->locale);
	  found = 1;
	  break;
	}
    }
  return found;
}
//


void speech_task(int s)
{
  if (s) signal(s, speech_task);
  if (sflag && (!speaker.busy()))
    {
      sflag = 0;
      if (urgent.empty())
	{
	  if (talking)
	    {
	      if (jobs.empty()) talking = 0;
	      else speaker<<jobs;
	    }
	}
      else speaker<<urgent;
      sflag = 1;
    }
}

int main(int argc, char **argv)
{
  if (!set_language( getenv("LANGUAGE")))
  {
    set_language("ru");
    //  setlocale(LC_CTYPE, "koi8"); GC: will be ru_RU.koi8r (set_language)
  }

  // is language switching authorized?
  char* s = getenv("MULTISPEECH_LANG_SWITCH");
  auto_switch = (s && !strcmp(s,"1")); 

  speech_device = getenv("SPEECH_DEVICE");
  if (!speech_device) speech_device = default_device;
  sound_device = getenv("SOUND_DEVICE");
  if (!sound_device) sound_device = speech_device;
  tones_device = getenv("TONES_DEVICE");
  if (!tones_device) tones_device = sound_device;
  if (!setuid(0)) nice(-5);
  signal(SIGINT, SIG_IGN);
  signal(SIGCHLD, speech_task);

  while (parent.read()) // Main loop
    {
      sflag = 0;

      // Commands interpretation
      if (parent.command == "tts_capitalize")
	speech::capitalize = atoi(parent.data);

      else if (parent.command == "l")
	{
	  char *text = strdup(parent.data);
	  if (!tts->sound_access(text)) define_language(text);
	  urgent.clear();
	  urgent<<tts->letter(text);
	  jobs.clear();
	  talking = 0;
	}

      else if (parent.command == "a")
	{
	  char *t = (char *)xmalloc(strlen(parent.data) + 5);
	  sprintf(t, "a%s", parent.data);
	  jobs<<t;
	}

      else if (parent.command == "p")
	{
	  char *text = (char *)xmalloc(strlen(parent.data) + 5);
	  sprintf(text, "a%s", parent.data);
	  urgent<<text;
	}

      else if (parent.command == "tts_reset")
	{
	  talking = 0;
	  jobs.clear();
	  backup.clear();
	  urgent.clear();
	  speaker.abort();
	}

      else if (parent.command == "tts_say")
	{
	  char *text = strdup(parent.data);
	  if (!tts->sound_access(text)) define_language(text);
	  urgent.clear();
	  urgent<<tts->say(text);
	  jobs.clear();
	  talking = 0;
	}

      else if (parent.command == "tts_set_character_scale")
	speech::char_factor = atof(parent.data);

      else if (parent.command == "tts_set_punctuations")
	switch (*parent.data)
	  {
	    case 'n':
	      punctuations::mode = 0;
	      break;
	    case 's':
	      punctuations::mode =1;
	      break;
	    case 'a':
	      punctuations::mode = 2;
	      break;
	    default:
	      break;
	  }

      else if (parent.command == "tts_set_speech_rate")
	speech::rate = atoi(parent.data);

      else if (parent.command == "d")
	talking = 1;

      else if (parent.command == "r")
	{
	  char *s = (char *)xmalloc(strlen(parent.data) + 2);
	  sprintf(s, "r%s", parent.data);
	  jobs<<s;
	}

      else if (parent.command == "tts_split_caps")
	speech::split_caps = atoi(parent.data);

      else if (parent.command == "s")
	{
	  talking = 0;
	  urgent.clear();
	  jobs.clear();
	  speaker.abort();
	}

      else if (parent.command == "tts_pause")
	{
	  talking = 0;
	  jobs>>backup;
	}

      else if (parent.command == "tts_resume")
	{
	  speaker.abort();
	  jobs<<backup;
	  talking = 1;
	}

      else if (parent.command == "t")
	{
	  int pitch = 440;
	  int duration = 50;
	  char *buf = (char *)xmalloc(strlen(parent.data) + 32);
	  sscanf(parent.data, "%d %d", &pitch, &duration);
	  sprintf(buf, "t%d %d\n", duration, pitch);
	  jobs<<buf;
	}

      else if (parent.command == "q")
	{
	  char *text = strdup(parent.data);
	  define_language(text);
	  jobs<<tts->prepare(text);
	}

      else if (parent.command == "set_lang")
	{
	  char lang[4];
	  int aLength=2;
	  if (parent.data[aLength] != ' ')
	    {
	      aLength++;
	      if (parent.data[aLength] != ' ')
		{
		  break;
		}
	    }
	  strncpy(lang,parent.data,aLength);
	  lang[aLength] = 0;

	  set_language(lang);

	  if (parent.data[aLength+1] == 't') 
	    { // announce the new language
	      char *text = strdup(tts->name);
	      urgent.clear();
	      urgent<<tts->say(text);
	      jobs.clear();
	      talking = 0;
	    }
	}

      else if (parent.command == "tts_sync_state")
	{
	  int c, s, r;		// capitalize, split caps, speech rate
	  if (3 != sscanf(parent.data, "%*s %d %*s %d %d", &c, &s, &r))
	    continue;
	  switch (*parent.data)
	    {
	    case 'n':
	      punctuations::mode = 0;
	      break;
	    case 's':
	      punctuations::mode =1;
	      break;
	    case 'a':
	      punctuations::mode = 2;
	      break;
	    }
	  speech::capitalize = c;
	  speech::split_caps = s;
	  speech::rate = r;
	}

      else if (parent.command == "exit")
	break;

      sflag = 1;
      if (!speaker.busy()) speech_task(0);
    }
  sflag = 0;
  signal(SIGCHLD, SIG_DFL);
  speaker.abort();
  jobs.clear();
  backup.clear();
  urgent.clear();
  while (wait(NULL) > 0);
  return EXIT_SUCCESS;
}
