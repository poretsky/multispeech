/* file.c
 * File handling routines for reading/writing samples from buffers etc.
 * Jim Jackson    Aug 97
 */

/*
 * Copyright (C) 1997 Jim Jackson                    jj@scs.leeds.ac.uk
 *                    School of Computer Studies,
 *                    The University of Leeds,
 *                    Leeds, LS2 9JT, UK
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program - see the file COPYING; if not, write to 
 *  the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
 *  MA 02139, USA.
 */

#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/soundcard.h>
#include "fmtheaders.h"

extern int afmt,stereo,samplerate;

/* writewavhdr(fd,Ns,samplerate,afmt,stereo)  write out wave format header
 *     to fd, for Ns samples, samplerate is samples/sec,
 *     afmt defines format, stereo whether stereo/mono
 */

writewavhdr(fd,Ns,samplerate,afmt,stereo)
int fd,Ns,samplerate,afmt,stereo;
{
   WaveHeader wh;
   int Nb;
   
   Nb=((Ns<<stereo)<<(afmt==AFMT_S16_LE));
   if (lseek(fd,(off_t) 0, SEEK_SET)<0) return(-1);
   
   wh.main_chunk = RIFF;
   wh.length     = Nb + sizeof(WaveHeader) - 8; 
   wh.chunk_type = WAVE;
   wh.sub_chunk  = FMT;
   wh.sc_len     = 16;
   wh.format     = PCM_CODE;
   wh.modus      = (stereo) ? 2 : 1;
   wh.sample_fq  = samplerate;
   wh.byte_p_spl = (afmt==AFMT_U8) ? 1 : 2;
   wh.byte_p_sec = samplerate * wh.modus * wh.byte_p_spl;
   wh.bit_p_spl  = (afmt==AFMT_U8) ? 8 : 16;
   wh.data_chunk = DATA;
   wh.data_length= Nb;
   return(write(fd, &wh, sizeof(WaveHeader)));
}

/*
 *  writecyclic(fd,bf,bfn,Ns)   bf is a sample buffer of size bfn bytes.
 *      writecyclic writes Ns bytes from this buffer
 *      to file descr. fd, treating the buffer as circular.
 */

writecyclic(fd,bf,bfn,Ns)
int fd;
unsigned char *bf;
int bfn,Ns;
{
   int i;
   
   for (;;) {
      if (Ns<=bfn) { return(write(fd,bf,Ns)); }
      Ns-=bfn;
      if (write(fd,bf,bfn) < 0) return(-1);
   }
}

