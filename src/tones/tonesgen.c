/* tonesgen.c
 * Parsing and playing of tones commands for "tones" program
 * Jim Jackson     Linux Version
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

/* History...................
 * 
 * 23Feb98  made all samples be 16 bit, with 8 bit conversion when writing
 *          to final audio buffer.
 * 20Feb98  Added recognition of musical notes format e.g. G3 for G
 *          in 3rd octave, A#2 for A# in second octave. Octave '0'
 *          runs from C at 33Hz to C at 65Hz
 *  8Sep97  split tones into 2 parts - this is the tones generation stuff
 *          tones.c is all the preamble etc.
 */
 
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <math.h>

#define CHAN_SEPERATOR(X) (X==',')

/* defines the frequencies for basic musical notes - these are 
 * integers storing the bottom octave freqs x 1000
 */
/*              A      A#     B      B#     C      C#     D      D#    */
int Notes[]={ 55000, 58270, 61735, 32704, 32704, 34649, 36709, 38892,
              41204, 43654, 43654, 46250, 49000, 51913, };
/*              E      E#     F      F#     G      G#  */
/* YEAH I know there is no E# and B# but by equiv'ing them to
 * frequencies for F and C it makes decoding from the table simpler.
 * Index into table is  twice the letter-'A' and add one if a '#'
 * This means that E# gets decoded but produces F
 */

/* We hold data for a number of channels (voices or generators) in arrays
 * indexed by the channel number. This is done so that we can preserve 
 * waveform integrity when we have to generate long tones over several
 * fragment buffers! (see the doTomeCmd() function)
 * Each generator has an assigned waveform, names and 1Hz samples
 * and a pointer into the sample space of the next sample to play.
 * 
 * The arrays get initialised by init_samples(), and updated when waveforms
 * get changed. New waveforms can be specified for all channels,
 * or on channel by channel basis.
 */

char **WaveFormName=NULL;   /* array of Current waveform name          */
char **WaveForm=NULL;       /* array of Current waveform samples       */
int  *CurSample=NULL;       /* array of indices of next samples in WaveForm */

char **WavForms;            /* Array of valid names for waveforms   */
char **WavBufs=NULL;        /* 1 Hz samples of all valid waveforms  */
char *Silence;              /* Silence waveform samples       */

/* These values are setup by a call to init_samples() - see below. */

int samplerate=0,                /* Samples/sec        */
    afmt=-1,                     /* Format of samples, 8bit or 16bit */
    channels=0,                  /* number of generators */
    stereo=-1;                   /* Mono or stereo     */

int BuffSize=0;                  /* size of buffer to use          */

int FD;                          /* File Descriptor for writing samples to */

/* --------------------------------------------------------- */

short int *Buff;                 /* Actual sample buffer but in ints
				    even for 8 bit */
int Buffsamples=0;               /* number of samples per buffer   */

unsigned int samples=0;          /* count of number of samples written
				  * since init_samples() called 
				  * returned by sample_count()   */

unsigned int Tms;                /* period in millisecs for each tone */
unsigned int Nt;                 /* number of samples for Tms         */

/* --------------------------------------------------------- */

int GenErr=0;             /* like errno, records status of last tonegen 
			   *  operation */

#define ERR_NOMEM  1       /* No memory */
#define ERR_NOWAV  2       /* Unknown Waveform */
#define ERR_BADPAR 3       /* Bad parameter */

char GenErrStr[130];      /*  Explanatory message for GenErr    */

#define SetErrStr(S) strncpy(GenErrStr,S,128)

/* --------------------------------------------------------- */

extern char **getWavNames(),*maketone();
extern int vflg;

#define VERBOSE (vflg>1)

#define chkword(a,b) ((n>=a)&&(strncmp(p,b,n)==0))

#define IS16BIT (afmt==AFMT_S16_LE)

/* getGenErrStr() return the error string for the last recorded error
 */

char *getGenErrStr()
{
   return(GenErrStr);
}

/* init_samples(fo,N,S,F,St,Ch)  initialise the generation variables etc.
 *   So that we don't need to specify these again - store them in local
 *   static variables, and initialise the 1Hz sample buffers.
 *   We always generate 16 bit samples and convert to 8 bit on output if nec.
 * 
 *   fo is output file number,
 *   N  is buffer size to use in bytes,
 *   S  is sample rate
 *   F  is sample format (8 or 16 bit)
 *   St is 0 for mono, 1 for stereo
 *   Ch is number of generating channels to allow
 */

init_samples(fo,N,S,F,St,Ch)
int fo,N,S,F,St,Ch;
{
   int i;
   char *p;
   
   GenErr=samples=0; SetErrStr("OK");

   WavForms=getWavNames();
   if (WavBufs==NULL) {
      for (i=0; WavForms[i]!=NULL; i++) {}
      WavBufs=(char **)malloc((i+1)*sizeof(char *));
      for ( ; i-- ; WavBufs[i]=NULL) {}
   }
   
   for (i=0; (p=WavForms[i])!=NULL; i++) {
      if (WavBufs[i]!=NULL) free(WavBufs[i]);
      if (vflg==1)
	 printf("Initialising %s waveform...\n",p);
      WavBufs[i]=maketone(p,1,S,AFMT_S16_LE,St);
      if (WavBufs[i]==NULL) { 
	 sprintf(GenErrStr,"Could not generate waveform %s",p);
	 return(GenErr=ERR_NOWAV);
      }
   }
   if ((i=setwaveform("off",&Silence,NULL))) return(i);
   
   if (Buff!=NULL) free(Buff);
   /* if 8bit then double buffer size 'cos we do all stuff 16bit internally */
   if ((Buff=malloc(N<<(F==AFMT_U8)))==NULL) { 
      SetErrStr("Cannot allocate Play Buffer");
      return(GenErr=ERR_NOMEM);
   }
   if (WaveFormName!=NULL) free(WaveFormName);
   WaveFormName=(char **)malloc(Ch*sizeof(char *));
   if (WaveForm!=NULL) free(WaveForm);
   WaveForm=(char **)malloc(Ch*sizeof(char *));
   if (CurSample!=NULL) free(CurSample);
   CurSample=(int *)malloc(Ch*sizeof(int));
   if (WaveFormName==NULL || WaveForm==NULL || CurSample==NULL) {
      SetErrStr("Cannot allocate Generator Arrays");
      return(GenErr=ERR_NOMEM);
   }
   
   FD=fo; BuffSize=N; samplerate=S; afmt=F; stereo=St; channels=Ch;
   Tms=0;
   Buffsamples=(N>>stereo)>>(afmt==AFMT_S16_LE);  /* calc number of samples
						   * in buffer */
   for (i=0; i<channels; i++) {
      CurSample[i]=0;
      WaveForm[i]=WavBufs[0]; WaveFormName[i]=WavForms[0];
   }
   return(0);
}

/* setwaveform(s,WFs,WFN)  search for string s in list of waveform names
 *   and if found set up *WFs with sample space and *WFN as name. 
 *   Return 0 if all ok
 *   else return error number and set up GenErr and GenErrStr
 */

setwaveform(s,WFs,WFN)
char *s;
char **WFs,**WFN;
{
   int i,n;
   char *p;
   
   p=(s==NULL)?"":s;    /* Zero length string selects first waveform */
   n=strlen(p);
   for ( i=0; WavForms[i]!=NULL; i++) {
      if (chkword(2,WavForms[i])) {
	 if (WFs!=NULL) *WFs=WavBufs[i];
	 if (WFN!=NULL) *WFN=WavForms[i];
	 return(0);
      }
   }
   sprintf(GenErrStr,"No such waveform: \"%s\".",p);
   return(GenErr=ERR_NOWAV);
}

/* getsamples()   return number of samples written since init_samples 
 *                  called.
 */

unsigned int getsamples()
{
   return(samples);
}

/* flush_samples()  to write enough silence to fill the current
 *        kernel buffer fragment, so we don't get spurious sounds.
 */

flush_samples()
{
   int n;
   int cp=0;
   
   if (samplerate==0) return(0);
   if ((n=samples%Buffsamples)) {
      n=Buffsamples-(samples%Buffsamples);
      newsamples(Buff,Silence,&cp,n,1);
      writesamples(Buff,n);
   }
   samplerate=0;
   return(0);
}

/* newsamples(bf,wav,N,F,G)   create N new samples from waveform wav
 *                at freq F and store in buffer bf. *cp is the 'phase' pointer
 *                for selecting next sample from wav. Scale samples 
 *                by G/10000.   BEWARE of G>32000 because of overflow probs.
 */

newsamples(bf,wav,cp,N,F,G)
short int *bf;
short int *wav;
int *cp,N,F,G;
{
   int i,n;
   short int *vbp;

   /* all sorts of things can go wrong in this code
    * it is assumed that the calling code calls with sensible
    * parameters
    */
   
   vbp=bf;
   for (i=0; i<N; i++) {
      *vbp++=(short int)((int)(wav[*cp])*G/10000);
      *cp=(*cp+F)%samplerate;
   }
}

/* addsamples(bf,wav,cp,N,F,G)   add N new samples from waveform wav
 *                at freq F into abuffer bf, *cp is the 'phase' pointer
 *                for selecting next sample from wav. scale new samples
 *                by G/10000
 */

addsamples(bf,wav,cp,N,F,G)
short int *bf;
short int *wav;
int *cp,N,F,G;
{
   int i,n,A;
   short int *vbp;

   /* all sorts of things can go wrong in this code
    * it is assumed that the calling code calls with sensible
    * parameters
    */
   
   vbp=bf;
   for (i=0; i<N; i++) {
      A=(int)(wav[*cp]);
      *vbp++=(short int)(((int)(*vbp)+A)/2);  /* ignoring pro tem the +/-1 correction factor */
      *cp=(*cp+F)%samplerate;
   }
}

/* doToneCmd(s)   interpret tone command in string s
 * 
 *  if *s is alphanumeric then it changes waveform, it can be a comma
 *    separated list of waveforms to setup the different channels.
 *  if *s is numeric and default tone period not set then sets tone period
 *  else it is a tone specification
 *    N    a single freq. to play for default time period
 *    N:t  a single freq. played for t millisecs (overrides def. time period)
 *    N1,N2,... mix freqs N1,N2 etc and play the mix for default time period
 *    N1,N2,...:t  as above but played for t millisecs
 *    any of the above N can be replaced with N(db)  where db is number
 *      of deciBells by which to amplify or attenuate that tone.
 * 
 *  e.g.  300,600(-20):500  play a mixed tone of 300Hz mixed with 600Hz
 *        attenuated by 20dB for 500 millisecs.
 * 
 *  :t   on its own sets the default time period to t millisecs
 */

doToneCmd(s)
char *s;

{
   char *p,*ss;
   int i,f,n,N,t,st,sCS;
   int Tt,T;
   
   GenErr=0; SetErrStr("OK");
   for ( ; isspace(*s); s++) {}
   
   if (isalpha(*s) && isalpha(*(s+1))) {
      if (strncmp("echo",s,4)==0) {
	 printf("%s\n",s+5);
      } else if (strncmp("reset",s,5)==0 || (strncmp("resync",s,6)==0)) {
	 for (i=0; i<channels; i++) CurSample[i]=0;
      }
      else { GenErr=docmdwaveform(s); }
      return(GenErr);
   }
/* check if initial or subsequent tone duration spec..... */
   if ((*s==':') || ((Tms==0) && isdigit(*s))) {
      if (*s==':') s++;
      return(doToneDur(s,&t));
   }
/* now set up duration for this tone(s) spec..... */
   t=Tms;
   for ( p=s; *p; p++) {
      if (*p==':') {
	 *p++=0;
	 if (st=doToneDur(p,&t)) return(st);
	 break;
      }
   }
   return(GenErr=docmdtones(s,t));
}

/* doToneDur(s,t)  convert string s and set *t to resultant integer time
 *   if Tms is zero set it up and also Nt
 */

doToneDur(s,t)
char *s;
unsigned int *t;
{
   char *ss;
   
   *t=strtol(s,&ss,10);
   for ( s=ss; isspace(*s); s++) {}
   if (*s || (*t==0)) {
      SetErrStr("Bad Tone duration: 0 or unexpected character found");
      return(GenErr=ERR_BADPAR);
   }
   if (Tms==0)
     Nt=((Tms=*t)*samplerate+500)/1000;    /* calc. number of samples for
					   * Tms (millisecs)         */
   return(0);
}
/* docmdtones(s)  run thru. freq(s) specified and build up waveform..... 
 *     and play it for t millisecs.
 */

docmdtones(s,t)
char *s;
int t;
{
   int i,f,g,st,n,N;
   char *p,*ss;
   
   g=10000;
   ss=s;                      /* save pointer for freq. specifier string */
   for ( N=((t*samplerate)+500)/1000 ; N; N-=n) {
      n=(N<=Buffsamples)?N:Buffsamples;
      for ( i=st=0,s=ss; *s && i<channels; i++) {
	 for (; isspace(*s); s++) {}
	 if (CHAN_SEPERATOR(*s)) { s++; continue; }
	 if (isdigit(*s)) { f=strtol(s,&p,10); s=p; }
	 else if ((f=ConvNote(&s))==0) {
	    SetErrStr("Bad Tone Specifier: Digit or Note expected");
	    return(GenErr=ERR_BADPAR);
	 }
	 for ( ; isspace(*s); s++) {}
	 if (VERBOSE) {
	    printf("%s Wave, Freq %d Hz for %d millisecs (%d samples)\n",
		   WaveFormName[i],f,(1000*n+samplerate/2)/samplerate,n);
	 }
	 if (st)
	    addsamples(Buff,WaveForm[i],&CurSample[i],n,f,g);
	 else {
	    newsamples(Buff,WaveForm[i],&CurSample[i],n,f,g);
	    st=1;
	 }
	 if (*s && !CHAN_SEPERATOR(*s)) {
	    SetErrStr("Bad Tone Specifier: ',' expected");
	    return(GenErr=ERR_BADPAR);
	 }
	 if (*s) s++;
      }
      writesamples(Buff,n);
   }
   return(0);
}

/* ConvNote(**s)  check if *s defines a note [A-G][|#][0-9]
 * if so convert to equivalent frequency. C3 is middle C
 */

ConvNote(s)
char **s;
{
   int n;

   n=**s;
   if (islower(n)) n=toupper(n);
   if ( n>='A' && n<='G') {
      n=(n - 'A')*2; (*s)++;
      if (**s == '#') {
	 (*s)++; n++;
      }
      n=Notes[n];
      if (isdigit(**s)) {
	 n=((n<<((**s) - '0'))+500)/1000;
	 (*s)++;
	 if (vflg==1) printf("  (%dHz)\n",n);
	 return(n);
      }
   }
   return(0);
}

/* docmdwaveform(s)   parse string s as a waveform selection string
 * 
 *  either  "waveform"   selects all generators                 done
 *   or     "waveform,waveform,etc" sets waveforms sequentially done
 */

docmdwaveform(s)
char *s;
{
   char *p,c;
   unsigned char *wf,*wfn;
   int st;
   
   for ( st=0; *s && st<channels; st++) {
      for ( ; isspace(*s); s++) { }
      if (CHAN_SEPERATOR(*s)) { s++; continue; }
      for (p=s; isalnum(*s); s++) { }
      for ( ; isspace(*s); s++) { *s=0; }
      if (c=*s) *s++=0;
      if (GenErr=setwaveform(p,&wf,&wfn)) {
	 SetErrStr("Unrecognised Waveform.");
	 return(GenErr);
      }
      WaveForm[st]=wf;
      WaveFormName[st]=wfn;
      if (c==0 && st==0) {
	 if (VERBOSE) {
	    printf("Setting all %d Channels to %s waveform.\n",channels,wfn);
	 }
	 for (; st<channels; st++) {
	    WaveForm[st]=wf; WaveFormName[st]=wfn;
	 }
	 return(0);
      }
      if (VERBOSE) {
	 printf("Channel %d waveform set to %s\n",st+1,wfn);
      }
      if (c && !CHAN_SEPERATOR(c)) {
	 SetErrStr("Bad Char in Waveform Specification: ',' expected.");
	 return(ERR_BADPAR);
      }
   }
   return(0);
}

/* Write samples away to opened file/DAC 
 */

writesamples(bf,N)
short int *bf;
int N;
{
   int i,n;
   char *p;
   short int *s;

   n=N*2;
   if (afmt!=AFMT_S16_LE) {
      n=N;
      for (p=(char *)(s=bf), i=0; i<n ; i++) { 
	 *p++=(char)((((int)(*s++))/256)+128); 
      }
   }
   write(FD,(char *)bf,n);
   samples+=N;
   if (VERBOSE) {
      printf("played %6d buffers %6d samples\n",samples/Buffsamples,samples%Buffsamples);
   }
}

