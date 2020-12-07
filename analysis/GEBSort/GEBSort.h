#include "TFile.h"
#include "gdecomp.h"


#define TAPE 0
#define NET  1
#define DISK 2
#define GEB  3

#define LONGLEN 8000
#define SHORTLEN 1024
#define RBUFSIZE 5000000
#define MAXPAYLOADSIZE 10000
#define STRLEN 256
#define MAXDETPOS 120
#define MAXCRYSTALNO 3
#define RATELEN 16384
#define DTBTEVLEN 1000
#define MAXNOSEG 9
#define MAXDETNO 200
#define NUMSEGS 36
#define MAXSEGNO MAXDETNO*NUMSEGS

#define MAXGEBS 200

#define MAX_GAMMA_RAYS 40
#define GEB_BITS GEB_HEADER_BYTES*8

/* max values for # of bits */

#define M14BITS 0x3fff 
#define M13BITS 0x1fff
#define M12BITS 0x0fff
#define M11BITS 0x07ff
#define M10BITS 0x03ff

/* basic spectra lengths */

#define L14BITS  M14BITS+1
#define L13BITS  M13BITS+1
#define L12BITS  M12BITS+1
#define L11BITS  M11BITS+1
#define L10BITS  M10BITS+1

typedef struct GEB_event
{
  int mult;
  CRYS_INTPTS *ptinp[MAXGEBS];
  GEBDATA *ptgd[MAXGEBS];
} GEB_EVENT;

typedef struct PARS
{
  int RunNumber;
  char ROOTFile[STRLEN];
  int nEvents;
  char GTSortInputFile[STRLEN];
  unsigned int StartMapAddress;
  int InputSrc;
  int WeWereSignalled;
  int firstEvent;
  int GSudpPort;
  int NumToPrint;
  int DumpEvery;
  TFile *f1;
  TList *wlist;
  long long int curTS;
  long long int dTS;
  long long int nbytes;
  int CurEvNo;
  char pHost[16];
  int grouping;
  int type;
  float timeout;
  int modwrite;
  int tsnumwrites;

} PARS;


/* structure for the tracked gamma rays */
/* written to the output with geb ID GEB_TYPE_TRACK==3 */

//typedef struct TRACKED_GAMMA_RAY {
//    float esum;              /* gamma ray energy */
//    int ndet;                /* number of interactions */
//    float fom;               /* figure of merit */
//    int   tracked;           /* 1==if tracked */
//    long long int timestamp; /* timestap of first interaction point */
//    float x0, y0, z0, e0;    /* first interaction point */
//    float x1, y1, z1, e1;    /* second interaction point */
//    } TRACKED_GAMMA_RAY;
//
//typedef struct TRACKED_GAMMA_HIT {
//      int ngam;
//      int pad;
//      TRACKED_GAMMA_RAY gr[MAX_GAMMA_RAYS];
//    } TRACKED_GAMMA_HIT;
