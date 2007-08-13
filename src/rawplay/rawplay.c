/*
 * Name: 	rawplay
 * Version:	2.0
 * Description:	rawplay accepts .raw format sound input on standart input
 *		and outputs it's to /dev/dsp. The .raw format is a header­
 *		less .au format. If either the .raw format is
 *		broken or /dev/dsp can not be opened rawplay outputs a more
 *		or less sensible error message.
 *
 * Copyright:	(C) 1998 Vincent Pagel <pagel@hal.fpms.ac.be>
 *		rawplay is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; version 2
 *
 * History:
 * 1998		V.Pagel <pagel@hal.fpms.ac.be>	* created
 * 10-07-1998	T.Pospisek <tpo@spin.ch>	* licenced with authors
 * 						  acceptance under GPL
 * 						* Added a bit better error
 * 						  messages
 * 						* made fit for and released
 * 21-07-1998	T.Pospisek <tpo@spin.ch>	* adjusted header
 * 19-08-1998	T.Pospisek <tpo@spin.ch>	* "Allright" removed
 * 						* stupid bug fixed, I hope
 * 						  nobody was using this
 * 						  until now...
 *
 * 13-11-2000  modified by Igor B. Poretsky <master@goga.energo.ru>
 * to deal with the multilingual speech server for Emacspeak.
 * Added some command line options
 * and various input formats.
 * Added signal handler for SIGTERM.
 *
 * 12-08-2007  modified by Igor B. Poretsky <master@goga.energo.ru>
 * to take in account the cases when only stereo output is possible.
 * Added option to specify sound output device.
 * Added version information.
 */ 

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

static char *version_info = "Rawplay v2.1";
static char *default_device = "/dev/dsp";

static int fd;

static void terminator(int s)
{
  ioctl(fd, SNDCTL_DSP_RESET);
  close(fd);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int option, samplerate, sampling = 16;
  char *sound_device = default_device;
  int size = 2;
  int frequency = 16000;
  int signedness = 1;
  int stereo = 0;
  double volume = 0.8;
  union
  {
    int8_t byte[2];
    int16_t word[2];
  } sample;

  /* Parse command line options */
  while ((option = getopt(argc, argv, "d:f:v:Vsubwh")) != -1)
    switch (option)
      {
	case 'V':
	  fprintf(stderr, "%s\n", version_info);
	  return EXIT_SUCCESS;
	case 'd':
	  sound_device = optarg;
	  break;
	case 'f':
	  frequency = atoi(optarg);
	  break;
	case 'v':
	  volume = atof(optarg);
	  break;
	case 's':
	  signedness = 1;
	  break;
	case 'u':
	  signedness = 0;
	case 'b':
	  size = 1;
	  break;
	case 'w':
	  size = 2;
	  break;
	case 'h':
	default:
	  fprintf(stderr, "Valid options are as follows:\n");
	  fprintf(stderr, "-d <device> -- use <device> for sound output instead of /dev/dsp\n");
	  fprintf(stderr, "-f <freq> -- sampling frequency = <freq>\n");
	  fprintf(stderr, "-v <vol>  -- play with volume <vol>, volume ranges from 0 through 1\n");
	  fprintf(stderr, "-s -- play as signed\n");
	  fprintf(stderr, "-u -- play as unsigned\n");
	  fprintf(stderr, "-b -- play as 8-bit\n");
	  fprintf(stderr, "-w -- play as 16-bit\n");
	  fprintf(stderr, "defaults are: 16-bit signed 16000 Hz\n");
	  fprintf(stderr, "-V -- print version information\n");
	  fprintf(stderr, "-h -- print this help\n");
	  return EXIT_SUCCESS;
      }

  /* Assign signal for playback interruption */
  signal(SIGTERM, terminator);

  /* Open sound device for output */
  fd = open(sound_device, O_WRONLY | O_SYNC);
  if (fd < 0)
    {
      fprintf(stderr, "Error opening %s\n", sound_device);
      return EXIT_FAILURE;
    }

  /* Adjust sound device */
  ioctl(fd, SNDCTL_DSP_RESET);
  if ((ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &sampling) < 0) || (sampling != 16))
    {
      fprintf(stderr, "Error setting 16-bit output sampling\n");
      return EXIT_FAILURE;
    }
  if (ioctl(fd, SNDCTL_DSP_STEREO, &stereo) < 0)
    {
      fprintf(stderr, "Error setting mono/stereo mode\n");
      return EXIT_FAILURE;
    }
  samplerate = frequency;
  if ((ioctl(fd, SNDCTL_DSP_SPEED, &samplerate) < 0) || (samplerate != frequency))
    {
      fprintf(stderr, "Error setting sample rate\n");
      return EXIT_FAILURE;
    }

  /* Playback */
  while (read(0, &sample, size) > 0)
    {
      switch (size)
	{
	  case 1:
	    if (signedness) sample.word[0] = (int16_t)rint((double)sample.byte[0] * volume * 250.0);
	    else sample.word[0] = (int16_t)rint((double)(sample.byte[0] - 128) * volume * 250.0);
	    break;
	  case 2:
	    if (signedness) sample.word[0] = (int16_t)rint((double)sample.word[0] * volume);
	    else sample.word[0] = (int16_t)rint((double)(sample.word[0] - 32768) * volume);
	    break;
	  default:
	    break;
	}
      if (stereo)
	sample.word[1] = sample.word[0];
      if (write(fd, &sample, stereo ? 4 : 2) < 0)
	{
	  fprintf(stderr, "Error writing to %s\n", sound_device);
	  return EXIT_FAILURE;
	}
    }

  /* Sync playback and finish */
  ioctl(fd, SNDCTL_DSP_SYNC); 
  ioctl(fd, SNDCTL_DSP_RESET);
  close(fd);
  return EXIT_SUCCESS;
}
