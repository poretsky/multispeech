/* DAC.c
 * miscellaneous routines for handling sound driver /dev/dsp device
 * Jim Jackson
 * 
 * Covered by GNU Public license - see file COPYING
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include "siggen.h"

/* DACopen(device,mode,&samplerate,&fmt,&stereo)
 *    opens device for mode (r/w) at samplerate samples/sec in 8/16 bit fmt
 *    and in stereo. Sets these params to best match possible.
 * 
 * getfragsize(fd) 
 *    get fragment size of a DSP buffer
 * 
 * getfreeobufs(fd)
 *    return number of free output fragment buffers free at moment
 * 
 * setfragsize(fd,N,S,fmt,stereo)
 *    set number of buffers to N, each set to fragment size to hold aprox
 *    100 msec of mono/stereo, 8/16 bit samples.
 * 
 * is16bit(fd)
 *    returns true if file fd is a 16 sound device
 * 
 * isstereo(fd)
 *    returns true if file fd is a stereo sound device  (To Be Done)
 */

/* is16bit(fd)   return true if fd is a dsp device supporting 16 bit
 *               sampling. Return false if not.
 */

is16bit(fd)
int fd;
{
   int fmts;
   
   if (ioctl(fd, SNDCTL_DSP_GETFMTS , &fmts) < 0) {
      return(0);
   }
   return(fmts&AFMT_S16_LE);
}

/* getfragsize(fd)   return fragsize of DSP buufer for fd
 *    or return -1 on error.
 */

getfragsize(fd)
int fd;
{
   int n;
   
   if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &n)<0) return(-1);
   return(n);
}

/* getfreeobufs(fd)   return number of empty output fragment buffers 
 *     available or return -1 on error.
 */

getfreeobufs(fd)
int fd;
{
   int n;
   audio_buf_info osp;

   if (ioctl(fd,SNDCTL_DSP_GETOSPACE, &osp)<0) return(-1);
   return(osp.fragments);
}

/* setfragsize(fd,N,S,afmt,stereo)
 *                     set N bufs of appropriate sound driver buffer fragment
 *                     size to support a samplerate of S samples/sec
 *                     afmt is 8 or 16 bit samples, stereo is true if 2 chans
 *                     return -1 if error - see errno, else return fragsize
 *                     set.
 */

setfragsize(fd,N,S,afmt,stereo)
int fd,N,S,afmt,stereo;
{
   int fr,n;
   
   if (N<1) {
      errno=EINVAL; return(-1); 
   }
   
   fr=10;
   if (S>15000) {
      fr=11;
      if (S>30000) fr=12;
   }
   fr=(N<<16)+fr+((stereo)?1:0)+((afmt==AFMT_S16_LE)?1:0);
   if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT , &fr) < 0) return(-1);
   if (ioctl(fd, SNDCTL_DSP_GETBLKSIZE , &n) < 0) return(-1);
   return(n);
}

/*
 * DACopen(char *fnm, char *mode, int *samples, int *fmt, int *stereo)
 * 
 *   open dspfile for read "r" or write "w", and set samples per sec
 *    as sampling rate - note we get pointer for samples so we can 
 *    return the actual samplerate set. 
 *    If stereo mode is unspecified (-1) then set stereo if possible,
 *    if fmt is unspecified (AFMT_QUERY) then set 16 bit if possible
 *    otherwise we set mono and/or 8 bit.
 *    Actual settings are returned in fmt and stereo.
 *   return file descriptor or -1 on error.
 */

DACopen(fnm,mode,samples,fmt,stereo)
char *fnm;
char *mode;
int *samples,*fmt,*stereo;
{
   int fd;
   int m,i;
   
   if (*mode=='r') m=O_RDONLY;
   else if (*mode=='w') m=O_WRONLY;
   else {
      errno=EINVAL; return(-1);
   }
   if ((fd = open (fnm,m,0)) >= 0) {  /* params must be set in this order */
      if ((*fmt==AFMT_QUERY) && is16bit(fd)) *fmt=AFMT_S16_LE;
      if (ioctl(fd, SNDCTL_DSP_SETFMT, fmt)>=0) { 
	 if (*stereo==-1) {
	    *stereo=1;
	    if (ioctl(fd, SNDCTL_DSP_STEREO, stereo)<0) *stereo=0;
	 }
	 if (ioctl(fd, SNDCTL_DSP_STEREO, stereo)>=0) {
	    if (ioctl(fd, SNDCTL_DSP_SPEED, samples)>=0) { 
	       return(fd);
	    }
	 }
      }
   }
   return(-1);
}

