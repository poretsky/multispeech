
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "regexp.h"
#include "regmagic.h"

#define export

#define NDIPHS 3000
#define NFRAMES 23000

#define FW 1
#define CW 2
#define PUNCT 3

#define DEF_F0 125

#define _SPN_ 6
#define _MBROLA_ 7

/* the following four should really come from the data header */
/* #define SR 10000	sample rate  */
/* #define FR_SZ 132	standard frame size  */
/* #define FR_DATA 16   shorts per frame, coeffs + assorted  */
/* #define NCOEFFS 12   ...  */

/* malloc defaults  */
#define DEF_BUFFER 1024
#define DEF_LING_LIST 100
#define DEF_SENT_LIST 100
#define DEF_SENT_SIL 100
#define DEF_SPL 100
#define DEF_PHONS 100
#define DEF_TARGS 100
#define DEF_FRAMES 100
#define DEF_PM 100

#define PHON_SZ 5
#define DIPH_SZ 10

#define OUT_BUF 2048

#define VOWEL 1	/* for syllabification essentially  */
#define CONST 2
#define BOUND 3
#define STRESS 4

/* various typedefs  */

typedef struct {
  char *input_file;
  char *output_file;
  char *diphone_file;
  char *hash_file;
  char *format;
  int type;	/* format by any other name  */
  FILE *ifd;
  FILE *ofd;
  FILE *xfd;
  FILE *dfd;
  void *db;
  int fw_num;
  int broad_cats_num;
  int dur0_num;
  int edin2sampa0_num;
  char prog[40];
  int sr;
  int fr_sz;
  int fr_data;
  int ncoeffs;
  int norm;		/* used for normalising output amplitude  */
  int ft_endian_loc;	/* for telling byte order  */
} CONFIG;

typedef struct {
  int size;
  int fill;
  char *text;
} BUFFER;

typedef struct {
  char *word;
  int type;
  char *transcription;
} LING;

typedef struct {
  int max;
  int sz;
  LING **text;
} LING_LIST;

typedef struct key {
  char *keyword;
  int keycount;
} KEY;

typedef struct {
  char *keyword;
  char *keyvalue;
} PKEY;

typedef struct {
  char *shape;
  float factor;
  int durn;
} P_ELEM;

typedef struct {
  int sil_sz;
  int sil_max;
  P_ELEM *sil;
  BUFFER list;
} SENT;

typedef struct {
  char phoneme[5];
  int syll;
  int dur;
  char *sprosod1;
  char *sprosod2;
  float strength1;
  float strength2;	/* for combined elements  */
} SPROSOD;

typedef struct {
  int max;
  int sz;
  SPROSOD **phoneme;
} SPROSOD_LIST;

typedef struct {
  char diph[10];
  int beg;
  int mid;
  int end;
} ENTRY;

typedef struct {
  short *frame; 
} FRAME;

typedef struct {
  int p_sz;
  int p_max;
  int t_sz;
  int t_max;
  char **phons;
  int *duration;
  int *cum_dur;
  int *pc_targs;
  int *targ_phon;
  int *targ_freq;
  int *abs_targ; /* maybe in samples  */
  int *pb;
  float *scale;
  char **diphs;
} SPN;

typedef struct {
  int f_sz;
  int p_sz;
  int f_max;
  int p_max;
  FRAME **mcebuf;
  short *duration; /* since variants may be required  */
  short *pitch;
} ACOUSTIC;


typedef struct {
  char *left_context;
  regexp *lc;
  char *target;
  char *right_context;
  regexp *rc;
  char *output;
} RULE;

#define SAME 0x0001

typedef char *Rule[4];

extern KEY fw[];
extern KEY broad_cats[];
extern KEY dur0[];
extern PKEY edin2sampa0[];

/* now definitions of global data  */

extern ENTRY indx[NDIPHS]; 
extern FRAME dico[NFRAMES];
extern int nindex;
extern char *dbName;	/* ???  */

/* Endian and byte swaping */
extern int ft_endian_loc;
/* Sun, HP, SGI Mips, M68000 */
#define FT_BIG_ENDIAN (((char *)&ft_endian_loc)[0] == 0)
/* Intel, Alpha, DEC Mips, Vax */
#define FT_LITTLE_ENDIAN (((char *)&ft_endian_loc)[0] != 0)
 
#define SWAPINT(x) (((x) & 0xff) << 24 | ((x) & 0xff00) << 8 | \
                    ((x) & 0xff0000) >> 8 | ((x) & 0xff000000) >> 24)
#define SWAPSHORT(x) (((x) & 0xff) << 8 | ((x) & 0xff00) >> 8)

#include "prototypes.h"
