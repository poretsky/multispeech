// Subprocesses handling routines

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "queue.h"
#include "language.h"
#include "subprocess.h"

extern char *players_dir;
extern char *tts_dir;
extern char *audio_player;
extern char *tone_generator;
extern char *speech_device;
extern char *sound_device;
extern char *tones_device;

int subprocess::pid = 0;
int subprocess::fd;
int subprocess::flag = 1;

void subprocess::run(char *program, char *input_file, char **argv)
{
  int channel[2], prev_pid = pid;
  if (!input_file)
    if (pipe(channel) < 0) return;
  pid = fork();
  if (pid < 0)
    {
      pid = 0;
      return;
    }
  else if (pid > 0)
    {
      if (!input_file)
	{
	  close(channel[0]);
	  fd  = channel[1];
	}
    }
  else
    {
      nice(-5);
      if (input_file) freopen(input_file, "r", stdin);
      else
	{
	  close(channel[1]);
	  if (dup2(channel[0], 0) < 0) _exit(EXIT_FAILURE);
	  close(channel[0]);
	}
      setbuf(stdin, NULL);
      if (prev_pid)
	{
	  if (dup2(fd, 1) < 0) _exit(EXIT_FAILURE);
	  close(fd);
	  setbuf(stdout, NULL);
	}
      else
	{
	  int dsp_fd;
	  freopen("/dev/null", "w", stdout);
	  while ((dsp_fd = open(dsp_device, O_WRONLY )) < 0);
	  close(dsp_fd);
	}
      freopen("/dev/null", "w", stderr);
      setenv("DSP_DEVICE", dsp_device, 1);
      execv(program, argv);
      _exit(EXIT_FAILURE);
    }
}

int subprocess::busy(void)
{
  if (pid)
    if (pid == waitpid(-1, NULL, WNOHANG))
      {
	pid = 0;
	flag = 1;
      }
  return pid != 0;
}

void subprocess::abort(void)
{
  if (pid && flag) kill(pid, SIGTERM);
}

void subprocess::operator << (char *instruction)
{
  int i, n, td, tf;
  char *program, *input_file, *argv[6];
  char cmd, *s = instruction;
  if (!instruction) return;
  cmd = *s++;
  switch (cmd)
    {
      case 's':
	dsp_device = speech_device;
	for (i = 0; i < 5; i++) argv[i] = new char[10];
	sscanf(s, "%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
	argv[5] = NULL;
	input_file = NULL;
	if (s = strchr(s, '\n')) s++;
	break;
      case 'l':
	dsp_device = speech_device;
	argv[0] = new char[10];
	sscanf(s, "%s", argv[0]);
	argv[1] = NULL;
	input_file = strchr(s, ' ') + 1;
	break;
      case 'a':
	dsp_device = sound_device;
	argv[0] = new char[strlen(audio_player) + 1];
	strcpy(argv[0], audio_player);
	argv[1] = NULL;
	input_file = s;
	break;
      case 't':
	dsp_device = tones_device;
	argv[0] = new char[strlen(tone_generator) + 1];
	argv[1] = new char[10];
	argv[2] = new char[10];
	strcpy(argv[0], tone_generator);
	sscanf(s, "%i %i\n", &td, &tf);
	n = (td * tf) % 500;
	if (n)
	  {
	    n = (n >= 250) ? 1 : -1;
	    for (i = 2; (i << 2) <= td; i++)
	      {
		if ((((td + n) * tf) % 500) == 0)
		  {
		    td += n;
		    break;
		  }
		if (n < 0) n += i;
		else n -= i;
	      }
	  }
	sprintf(argv[1], "%i\0", td);
	sprintf(argv[2], "%i\0", tf);
	argv[3] = NULL;
	input_file = NULL;
	s = NULL;
	break;
      case 'r':
	speech::rate = atoi(s);
	kill(getpid(), SIGCHLD);
      default:
	argv[0] = NULL;
    }
  if (argv[0])
    {
      program = new char[strlen(players_dir) + strlen(argv[0]) + 20];
      sprintf(program, "%s/%s", players_dir, argv[0]);
      run(program, input_file, argv);
      switch (cmd)
	{
	  case 'a':
	    flag = 0;
	    break;
	  case 's':
	    if (s)
	      {
		if (*s)
		  {
		    FILE *fp;
		    int player_fd = fd;
		    int player_pid = pid;
		    sprintf(program, "%s/%s", tts_dir, argv[0]);
		    run(program, input_file, argv);
		    pid = player_pid;
		    signal(SIGPIPE, SIG_IGN);
		    close(player_fd);
		    fp = fdopen(fd, "w");
		    fputs(s, fp);
		    fclose(fp);
		    signal(SIGPIPE, SIG_DFL);
		  }
		else
		  {
		    close(fd);
		    abort();
		  }
	      }
	    else
	      {
		close(fd);
		abort();
	      }
	    break;
	  case 't':
	    close(fd);
	}
      for (i = 0; argv[i]; i++) delete argv[i];
      delete program;
    }
  free(instruction);
}

void subprocess::operator << (queue &jobs)
{
  char *item;
  jobs>>item;
  (*this)<<item;
}
