/* siggen.h
 * header file for the Ncurses based signal generator
 * Jim Jackson  <jj@scs.leeds.ac.uk>  Jan 97
 */

#define VERSION "siggen  Ver. 1.40 (Jun 97)   Ncurses based Digital Signal Generator"

#define DEFAULT_BUFFERS 3
#define DAC_FILE "/dev/dsp"
#define SAMPLERATE 22050
#define EFAIL -1

extern int vflg,dflg;

extern int DAC;
extern unsigned int samplerate;         /* Samples/sec        */
extern unsigned int stereo;             /* stereo mono */
extern unsigned int afmt;               /* format for DSP  */
extern int Nfragbufs;                   /* number of driver buffers */
extern int fragsize;                    /* size of driver buffer fragments */
extern int fragsamplesize;              /* size of fragments in samples */

    /* channel 1  -  or mono..... */
extern char wf[32];                     /* waveform type */
extern unsigned int freq;               /* signal frequency */
extern int ratio;                       /* used in pulse, sweep etc */
extern int Gain;                        /* Amplification factor */

    /* channel 2 when in stereo mode ..... */
extern char wf2[32];                    /* waveform type */
extern unsigned int freq2;              /* signal frequency */
extern int ratio2;                      /* used in pulse, sweep etc */
extern int Gain2;                       /* Amplification factor */
extern int phase;                       /* phase diff with chan1 */

