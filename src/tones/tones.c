/* tones.c
 * generate and play tones for given period....
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
 * 20Feb98  Added recognition of musical notes format e.g. G3 for G
 *          in 3rd octave, A2# for A# in second octave. Octave '0'
 *          runs from C at 33Hz to C at 65Hz
 *  9Sep97  Split out the tone spec. parsing and tone playing into 
 *          tonesgen.c
 *  8Sep97  Alter tones specification to be able to specify different
 *          durations. i.e. t1,t2:ms. 
 *          Also start the change of generating technique to do sampling
 *          1Hz sample space for requisite number of samples.
 * --Aug97  Re-organise some of DAC routines out into DAC.c
 * 18Feb97  -f flag missing, added it
 *          also added facility to be able to change wavform. A
 *          non-numeric parameter will cause a change in waveform
 *          for rest of tones.
 *          Also still occasional noise after tones played - altered DSPend again!
 * 16Feb97  altered it so you can specify several freqs to be played together
 *          by using punctuation of some sort to seperate freqs 
 *          e.g. 440,880   would play two freqs of 440Hz and 880Hz together
 * 13Feb97  Based on sgen, using generator.c - only mono version.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <math.h>


#define VERSION "tones  Ver. 1.3 (Sep 97)   tone generator"

#define DAC_FILE "/dev/dsp"
#define SAMPLERATE 22050
#define DEF_CHANNELS 4
#define EFAIL -1

#define MAXPRM 32
#define MAXTONES 64
#define chkword(a,b) ((n>=a)&&(strncmp(p,b,n)==0))

int aflg,fflg,vflg,dflg,wfmt;
int loopflg;

unsigned int stereo;             /* stereo or mono mode      */
unsigned int samplerate;         /* Samples/sec              */
unsigned int ratio;              /* used in pulse, sweep etc */
unsigned int afmt;               /* format for DSP  */
unsigned int DSPbytecount;       /* number of bytes sent to DSP  */
int frag_size;                   /* size of a DSP buffer  */
int channels;                    /* number of generating channels */

char *sys,*outfile,*infile,*dsp_device;

extern char *getGenErrStr();

help(e)
int e;
{  
   char **aa;
   
   fputs(VERSION,stdout);
   fputs("\nUsage: tones [-s samplerate] [-8/-16|-b 8/16] [waveform] T freq(s)\n",stdout);
   fprintf(stdout,"Defaults: SINE wave, %d channels, output to %s, %d samples/sec,\n",
	   DEF_CHANNELS,dsp_device,SAMPLERATE);
   fputs("          mono, 16 bit samples if possible, else 8 bit.\n\n",stdout);
   fputs("\nGenerates and plays for T millisecs, tones of given frequencies.\n",stdout);
   fputs("Several freqs can be played together by specifying a list of freqs\n",stdout);
   fputs("separated by ',' or '+', e.g. 150,200,300 or 100+200+300\n",stdout);
   fputs("The maximum number of frequencies which can be played together\n",stdout);
   fputs("is specified by the '-c channels' option. Each channel adds\n",stdout);
   fputs("processing overhead to the gneration process.\n",stdout);
   fputs("An optional duration maybe specified, that overrides the default,\n",stdout);
   fputs("by appending ':n' where n specifies number of millisecs,\n",stdout);
   fputs("e.g. 150,300:250 plays 150Hz and 300Hz together for 250 millisecs\n",stdout);
   fputs("     0:1000 plays one second (1000ms) of silence.\n",stdout);
   fputs("Valid waveforms are:",stdout);
   for ( aa=(char **)getWavNames(); *aa; aa++ ) fprintf(stdout," %s",*aa);
   fputs(".\nand can be specified anywhere and affect all later tones.\n",stdout);
   fputs("flags: -f,-a         force overwrite/append of/to file\n",stdout);
   fputs("       -o/-w file    write samples to raw/WAVE file ('-' is stdout)\n",stdout);
   fputs("       -i file       contents of file specify tones program ('-' is stdin)\n",stdout);
   fputs("       -s samples    generate with samplerate of samples/sec\n",stdout);
   fputs("       -c channels   number of channels or voices\n",stdout);
   fputs("       -loop         repeat play the tones ad nauseam.\n",stdout);
   fputs("       -v            be verbose.\n",stdout);
   fputs(" -8/-16 or -b 8|16   force 8 bit or 16 bit mode.\n",stdout);
   return(e);
}

/* main
 *
 */
 
main(argc,argv)
int argc;
char **argv;
{
   unsigned int i,j,k,l,m,n,N;
   FILE *f,*fi;
   char *p,*fnm,bf[260],*omode;
   int fd,st;
   unsigned int t;

   dsp_device = getenv("DSP_DEVICE");
   if (!dsp_device) dsp_device = DAC_FILE;
   if ((p=strrchr(sys=*argv++,'/'))!=NULL) sys=++p;
   argc--;
   
   outfile=infile=NULL;
   samplerate=SAMPLERATE; afmt=AFMT_QUERY;
   channels=DEF_CHANNELS;
   loopflg=stereo=wfmt=frag_size=0;                 /* force mono mode */
   dflg=vflg=aflg=fflg=0;
   
   while (argc && **argv=='-') {          /* all flags and options must come */
      n=strlen(p=(*argv++)+1); argc--;    /* before paramters                */
      if (chkword(1,"samplerate")) {
	 if (argc && isdigit(**argv)) { samplerate=atoi(*argv++); argc--; }
      }
      else if (chkword(1,"output")) {     /* specify output file       */
	 if (argc) { outfile=*argv++; argc--; } /* output file name          */
      }
      else if (chkword(1,"input")) {     /* specify input file       */
	 if (argc) { infile=*argv++; argc--; } /* input file name          */
      }
      else if (chkword(1,"loop")) {       /*  specify loop option   */
         loopflg=1;
      }
      else if (chkword(1,"wave")) {     /* e.g. write WAVE format file  */
	 if (argc) { outfile=*argv++; argc--; wfmt=1; } /* output file name  */
      }
      else if (chkword(2,"16")) { afmt=AFMT_S16_LE; }
      else if (chkword(1,"bits")) {
	 i=0;
	 if (argc) { 
	    i=atoi(*argv++); argc--;
	 }
	 if (i==8) afmt=AFMT_U8;
	 else if (i==16) afmt=AFMT_S16_LE;
	 else exit(err_rpt(EINVAL,"must be '-b 8' or '-b 16'."));
      }
      else if (chkword(1,"channels")) {
	 i=0;
	 if (argc) { 
	    i=atoi(*argv++); argc--;
	 }
	 if (i<1) exit(err_rpt(EINVAL,"Must have 1 or more channels."));
	 channels=i;
      }
      else {                             /* check for single char. flags    */
	 for (; *p; p++) {
	    if (*p=='h') exit(help(EFAIL));
	    else if (*p=='8') afmt=AFMT_U8;
	    else if (*p=='a') aflg=1;
	    else if (*p=='f') fflg=1;
	    else if (*p=='d') dflg=1;
	    else if (*p=='l') loopflg=1;
	    else if (*p=='v') vflg++;
	    else {
	       *bf='-'; *(bf+1)=*p; *(bf+2)=0;
	       exit(help(err_rpt(EINVAL,bf)));
	    }
	 }
      }
   }
   
   if (vflg) puts(VERSION);

   fi=NULL;            /* fi!=NULL if a command file is to be read */
   if (infile!=NULL) { /* check for valid infile if specified */
      if (strcmp(infile,"-")==0) {
	 fi=stdin; infile="stdin";
      } else if ((fi=fopen(infile,"r"))==NULL) {
	 exit(err_rpt(errno,infile));
      }
   }
   
   if (outfile==NULL) {           /* if no outfile then write direct to DAC */
                                  /* if no format specified then try 16 bit */
      i=(afmt==AFMT_QUERY)?AFMT_S16_LE:afmt ; 
      if ((fd=DACopen(fnm=dsp_device,"w",&samplerate,&i,&stereo))<0) {
	 if (afmt==AFMT_QUERY) {  /* if no format specified try for 8 bit.. */
	    i=AFMT_U8;
	    fd=DACopen(fnm=dsp_device,"w",&samplerate,&i,&stereo);
	 }
	 if (fd<0) exit(err_rpt(errno,"Opening DSP for output."));
      }
      if ((frag_size=getfragsize(fd))<0)
	exit(err_rpt(errno,"Problem getting DSP Buffer size."));

      if (vflg) {
	 printf("%s : DAC Opened for output\n",fnm);
         printf("%d bytes per DSP buffer.\n",frag_size);
      }
      if ((afmt!=AFMT_QUERY && i!=afmt) || stereo!=0) {
	 if (i!=afmt) 
	   err_rpt(EINVAL,"Sound card doesn't support requested sample format.");
	 if (stereo!=0)
	   err_rpt(EINVAL,"Card doesn't do MONO - nah, don't believe it.");
	 exit(EINVAL);
      }
      afmt=i;
   }
   else if (strcmp(outfile,"-")==0) {
      if (aflg || wfmt)
         exit(err_rpt(EINVAL,"Cannot write append, or write a WAVE file, to stdout."));
      f=stdout; fnm="stdout";
   }
   else {
      if (aflg && wfmt) {
         exit(err_rpt(EINVAL,"Cannot write append to a WAVE file."));
      }
      afmt=(afmt==AFMT_QUERY)?AFMT_S16_LE:afmt ; /* set 16 bit mode unless
						  * some format is forced */
      omode=(aflg)?"a":"w";
      if ((f=fopen(fnm=outfile,"r"))!=NULL) {
	 fclose(f);
	 if ( (!aflg) && (!fflg) ) exit(err_rpt(EEXIST,fnm));
      }
      if ((f=fopen(fnm,omode))==NULL) { exit(err_rpt(errno,fnm)); }
      frag_size=32768;
      if (vflg) { 
	 printf("%s samples to %s file %s\n",
		(*omode=='a')?"Appending":"Writing",
		(wfmt)?"WAVE":"RAW", fnm);	       
         printf("Buffer size is %d bytes.\n",frag_size);
      }
      fd=fileno(f);
      loopflg=0;    /* cannot loop if writing to a file! */
      if (wfmt && (writewavhdr(fd,0,samplerate,afmt,stereo)<0)) {
	 exit(err_rpt(errno,"Write WAVE file header."));
      }
   }

   if (!(afmt==AFMT_S16_LE || afmt==AFMT_U8)) {
      exit(err_rpt(EINVAL,"Only unsigned 8 and signed 16 bit samples supported."));
   }
 	
   if (vflg) {
      printf("%d %s, %s, samples/sec\n",samplerate,
	     (stereo)?"stereo":"mono",
	     (afmt==AFMT_U8)?"unsigned 8 bit":
	     ((afmt==AFMT_S16_LE)?"signed 16 bit, little endian":
	      "Unknown audio format"));
      printf("Upto %d Frequencies can be played together.\n",channels);
   }
   
   
   /* initialise base samples etc */
   init_samples(fd,frag_size,samplerate,afmt,stereo,channels);
   
   do {
      for ( i=0; i<argc; i++) {
	 if (vflg) printf("%s\n",argv[i]);
	 if (st=doToneCmd(argv[i])) {
	    fprintf(stderr,"%s -> %s\n",argv[i],getGenErrStr());
	 }
      }
      if (fi!=NULL) {
	 for (i=1,rewind(fi); (p=fgets(bf,260,fi))!=NULL; i++) {
	    for ( ; isspace(*p); p++) {}
	    if (*p && *p!='#') {
	       if (vflg) printf("%s\n",p);
	       if (st=doToneCmd(p)) {
		  fprintf(stderr,"[%3d] %s -> %s\n",i,p,getGenErrStr());
	       }
	    }
	 }
      }
   } while (loopflg);
   
   if (outfile==NULL) {
      flush_samples();
   } else if (wfmt && 
	      (writewavhdr(fd,getsamples(),samplerate,afmt,stereo)<0)) {
      err_rpt(errno,"Rewriting WAVE file header.");
   }
   
   close(fd);
   exit(0);
}  

doline(s)
char *s;

{
   int i,n;
   char **aa[50];
   
   for ( ; isspace(*s); s++) {}
   if (*s && *s!='#') {
      n=parse(s,aa,'\0');
      for (i=0; i<n; i++) doToneCmd(aa[i]);
   }
}
