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
 * 13-11-2000  modified by Igor B. Poretsky <root@goga.energo.ru>
 * to deal with the multilingual speech server for Emacspeak.
 * Added some command line options
 * and various input formats.
 * Added signal handler for SIGTERM.
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

int fd;

void terminator(int s)
{
  ioctl(fd, SNDCTL_DSP_RESET);
  close(fd);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int tmp;
  int size = 2;
  int frequency = 16000;
  int signedness = 1;
  double volume = 0.8;
  union
  {
    signed char byte;
    signed short int word;
    int value;
  } sample;

  while ((tmp = getopt(argc, argv, "f:v:subwh")) != -1)
    switch (tmp)
      {
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
	  fprintf(stderr, "-f <freq> -- sampling frequency = <freq>\n");
	  fprintf(stderr, "-v <vol>  -- play with volume <vol>, volume ranges from 0 through 1\n");
	  fprintf(stderr, "-s -- play as signed\n");
	  fprintf(stderr, "-u -- play as unsigned\n");
	  fprintf(stderr, "-b -- play as 8-bit\n");
	  fprintf(stderr, "-w -- play as 16-bit\n");
	  fprintf(stderr, "defaults are: 16-bit signed 16000 Hz\n");
	  fprintf(stderr, "-h -- this help\n");
	  return EXIT_SUCCESS;
      }

  signal(SIGTERM, terminator);

  fd = open("/dev/dsp", O_WRONLY | O_SYNC);
  if (fd < 0)
    {
      fprintf(stderr,"Error opening /dev/dsp \n");
      return EXIT_FAILURE;
    }
  tmp = 16;
  if ((ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &tmp) < 0) || (tmp != 16))
    {
      fprintf(stderr,"Error setting SNDCTL_DSP_SAMPLESIZE \n");
      return EXIT_FAILURE;
    }

  tmp = 0;			/* Mono */
  if ((ioctl(fd, SNDCTL_DSP_STEREO, &tmp) < 0) || (tmp != 0))
    {
      fprintf(stderr,"Error setting SNDCTL_DSP_STEREO\n");
      return EXIT_FAILURE;
    }

  tmp = frequency;	/* Sample Rate */
  if ((ioctl(fd, SNDCTL_DSP_SPEED, &tmp) < 0) || (tmp != frequency))
    {
      fprintf(stderr,"Error setting SNDCTL_DSP_SPEED\n");
      return EXIT_FAILURE;
    }

  while (read(0, &sample, size) > 0)
    {
      switch (size)
	{
	  case 1:
	    if (signedness) sample.word = (short int)rint((double)sample.byte * volume * 250.0);
	    else sample.word = (short int)rint((double)(sample.byte - 128) * volume * 250.0);
	    break;
	  case 2:
	    if (signedness) sample.word = (short int)rint((double)sample.word * volume);
	    else sample.word = (short int)rint((double)(sample.word - 32768) * volume);
	    break;
	}
      if (write(fd, &sample, 2) < 0)
	{
	  fprintf(stderr,"Error writing to /dev/dsp\n");
	  return EXIT_FAILURE;
	}
      sample.value = 0;
    }

  ioctl(fd, SNDCTL_DSP_SYNC); 
  ioctl(fd, SNDCTL_DSP_RESET);
  close(fd);
}
