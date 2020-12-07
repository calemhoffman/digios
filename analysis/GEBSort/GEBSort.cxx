
/* sorter for data from the GEB tab (or a file) */
/* totally universal in its scope */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>
#include <stddef.h>
#include <zlib.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1.h"
#include "TStopwatch.h"

#include "TH2.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"

#include "TMapFile.h"

#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"

/* global variables */

#if(USEZLIB==0)
off_t inData;
#endif
#if(USEZLIB==1)
gzFile zinData;
#endif

/* buffer of data for sorting */

char *rbuf;

int nn1 = 0;
int nn2 = 0;
int nn3 = 0;

//NprintEvNo = 0;
int bufPos = 0;
int nBadTestPat = 0;
int egemin = 2;
double oldLEDTs = 0;
double oldCFDTs = 0;
time_t tdmp = 0, tdmplast;

int time_stamp ();

/* parameters */

PARS Pars;

static const int ConnectionRetryWait = 2 * 100; /* usec between con. retries. */
static const int ConnectionRetryCount = 150;    /* Times we retry connect. */

struct gretTap *pTap;
struct GEBData *pData;


TStopwatch StpWatch;


/*--------------------------------------------------------*/
void CheckNoArgs (int required, int actual, char *str){

  if (required < actual){
    printf ("argument problem with chat option\n");
    printf ("--> %s\n", str);
    printf ("required # arguments: %i\n", required - 1);
    printf ("actual   # arguments: %i\n", actual - 1);
    printf ("Please fix and try again, quitting...\n");
    exit (1);
  };
}

/*----------------------------------------------------------------------------*/
int GebTypeStr (int type, char str[]){
//   printf("got type %i, %i\n", type,GEB_TYPE_DECOMP);
  if (type == GEB_TYPE_DECOMP)
    sprintf (str, "GEB_TYPE_DECOMP      ");
  else if (type == GEB_TYPE_BGS)
    sprintf (str, "GEB_TYPE_BGS         ");
  else if (type == GEB_TYPE_RAW)
    sprintf (str, "GEB_TYPE_RAW         ");
  else if (type == GEB_TYPE_TRACK)
    sprintf (str, "GEB_TYPE_TRACK       ");
  else if (type == GEB_TYPE_S800_RAW)
    sprintf (str, "GEB_TYPE_S800_RAW    ");
  else if (type == GEB_TYPE_NSCLnonevent)
    sprintf (str, "GEB_TYPE_NSCLnonevent");
  else if (type == GEB_TYPE_GT_SCALER)
    sprintf (str, "GEB_TYPE_GT_SCALER   ");
  else if (type == GEB_TYPE_GT_MOD29)
    sprintf (str, "GEB_TYPE_GT_MOD29    ");
  else if (type == GEB_TYPE_S800PHYSDATA)
    sprintf (str, "GEB_TYPE_S800PHYSDATA");
  else if (type == GEB_TYPE_G4SIM)
    sprintf (str, "GEB_TYPE_G4SIM       ");
  else if (type == GEB_TYPE_CHICO)
    sprintf (str, "GEB_TYPE_CHICO       ");
  else if (type == GEB_TYPE_NSCLNONEVTS)
    sprintf (str, "GEB_TYPE_NSCLNONEVTS ");
  else if (type == GEB_TYPE_DGS)
    sprintf (str, "GEB_TYPE_DGS         ");
  else
    {
      sprintf (str, "%i is unknown, maybe update 'GebTypeStr' function?", type);
      return (1);
    };
//      printf("type: %s\n",str);

  return (0);

};

/*----------------------------------------------------------------------------*/
int buf_read (off_t inData, char *buf, int nbytes) {

/* buffered read of data from disk or network */
/* 'rbuf' is the buffer we maintain, it is global */
/* 'buf' is the buffer of data we return */

/* declarations */

  ssize_t st;
  int partread, l, ntries;
  struct GEBData *tmpData, *tmpStore;
  static int bpos = RBUFSIZE, ActualBufSize;
  static int bleft = 0;
  char str[64];
  unsigned int *pi4;
  int i, bstart;

  if (Pars.CurEvNo <= Pars.NumToPrint) printf ("buf_read called, asked for %i byte, have %i bytes left in buffer, bpos=%i\n", nbytes, bleft, bpos);

  if (buf == NULL){
    printf ("buf_read: pointer to buf is NULL, cannot continue\n");
    return (0);
  };

  /*-----------------------*/
  /* get data via a buffer */
  /*-----------------------*/

//  printf("entered buffer read, asking for %i bytes\n",nbytes);

  /* do we need to read a new buffer in ? */

  if (nbytes > bleft){
    if (Pars.CurEvNo <= Pars.NumToPrint) printf ("__forced to read data because nbytes=%i > bleft=%i\n", nbytes, bleft);

    /* first read what is left in the buffer */
    partread = bleft;
    if (bleft > 0){
      memcpy (buf, rbuf + bpos, partread);
      bleft = 0;
      bpos = ActualBufSize;
      if (Pars.CurEvNo <= Pars.NumToPrint)  printf ("partial transfer of %i bytes, set bleft=%i, set bpos=%i\n", partread, bleft, bpos);
    }else{
      partread = 0;
    }
    /* then read in a new buffer from disk or GEB */
    if (Pars.InputSrc == DISK){
        ActualBufSize = read (inData, rbuf, RBUFSIZE);
        nn1 += ActualBufSize;
        bpos = 0;
        bleft = ActualBufSize;
        if (Pars.CurEvNo <= Pars.NumToPrint) printf ("read new buffer of size %i bytes, bpos=%i, bleft=%i\n", ActualBufSize, bpos, bleft);
        if (ActualBufSize != RBUFSIZE){
          printf ("could not read a full buffer of size %i, only got %i, bleft=%i\n", RBUFSIZE, ActualBufSize, bleft);
          printf ("read new buffer of size %i bytes, bpos=%i, bleft=%i\n", ActualBufSize, bpos, bleft);
          if (ActualBufSize <= 0)  return (0);
        };

    }

    /* then read the rest */

    if (Pars.CurEvNo <= Pars.NumToPrint){
      printf ("buf=0x%p ", buf);
      printf ("rbuf=0x%p ", rbuf);
      printf ("partread:%i ", partread);
      printf ("bpos:%i ", bpos);
      printf ("nbytes:%i ", nbytes);
      printf ("nbytes - partread:%i ", nbytes - partread);
      printf ("bleft:%i\n", bleft);
      fflush (stdout);
    }

    /* trap */

    if (bleft < (nbytes - partread)){
      printf ("GEBSort error: the buffer must be define big enough to hold the biggest event we have\n");
      printf ("#define RBUFSIZE %i (at least) in GEBSort.h\n", nbytes);
      printf ("buf=0x%p ", buf);
      printf ("rbuf=0x%p ", rbuf);
      printf ("partread:%i ", partread);
      printf ("bpos:%i ", bpos);
      printf ("nbytes:%i ", nbytes);
      printf ("nbytes - partread:%i ", nbytes - partread);
      printf ("bleft:%i\n", bleft);
      fflush (stdout);
      exit (1);
    };

    memcpy (buf + partread, rbuf + bpos, nbytes - partread);
    bpos += nbytes - partread;
    bleft -= (nbytes - partread);

  }else{

    /* just transfer what was aked for */

    if (Pars.CurEvNo <= Pars.NumToPrint) printf ("simple transfer: now: bleft=%i, bpos=%i\n", bleft, bpos);

    memcpy (buf, rbuf + bpos, nbytes);
    bpos += nbytes;
    bleft -= nbytes;
    if (Pars.CurEvNo <= Pars.NumToPrint) printf ("transfer of %i bytes, set bleft=%i, set bpos=%i\n", nbytes, bleft, bpos);

  };
  if (Pars.CurEvNo <= Pars.NumToPrint) printf ("buf_read done:bpos=%i,bleft=%i\n", bpos, bleft);

  assert (bleft >= 0);

  return (nbytes);

};

/*----------------------------------------------------------------------------*/
int GEBGetEv (GEB_EVENT * GEV_event, int curEvNo) {

  /* declarations */

  static int nn = 0, ii = 0, nx = 0, nbadTS = 0, firsttime = 1;
  int siz, val, i1, i, stType;
  static int newbuf = 1, *pos;
  long long int TS, dTS;
  char str[256];

  /* prototypes */

  int bread (int, int *, int *, int *, int *);

  if (firsttime){
    firsttime = 0;

    /* get the initial header, into position 1, not 0 */

    ii = 1;
    siz = buf_read (inData, (char *) GEV_event->ptgd[ii], sizeof (GEBDATA));
    if (siz != sizeof (GEBDATA)){
      printf ("failed to read %lu bytes for header, got %i\n", sizeof (GEBDATA), siz);
      return (1);
    };
    Pars.nbytes += siz;
    nn2++;
    printf ("got initial header, TS=%lli\n", GEV_event->ptgd[ii]->timestamp);

    /* get the initial payload */

    i1 = GEV_event->ptgd[ii]->length;
    siz = buf_read (inData, (char *) GEV_event->ptinp[ii], i1);
    if (siz != i1){
      printf ("failed to read %i bytes for payload, got %i\n", i1, siz);
      return (2);
    };
    nn3++;
    Pars.nbytes += siz;
    printf ("read initial payload of siz=%i into event position %i\n", siz, ii);
    fflush (stdout);

    printf ("__ptgd[0]->type=%2i; ", GEV_event->ptgd[0]->type);
    printf ("ptgd[0]->length=%4i; ", GEV_event->ptgd[0]->length);
    printf ("ptgd[0]->timestamp=%lli\n", GEV_event->ptgd[0]->timestamp);
    fflush (stdout);

    ii = 1;
    nn = 1;
    printf ("initial ii=%i, nn=%i\n", ii, nn);

  };

  Pars.curTS = GEV_event->ptgd[0]->timestamp;
  TS = Pars.curTS;

  /* process leftovers from the last read */

  if (nn > 0){
    /* move the last (at pos nn) to the first position */
    memcpy ((char *) GEV_event->ptgd[0], (char *) GEV_event->ptgd[nn], sizeof (GEBDATA));
    memcpy ((char *) GEV_event->ptinp[0], (char *) GEV_event->ptinp[nn], GEV_event->ptgd[0]->length);
  }

  /* reset, ii=0 alwasy taken for the first leftover from last time */

  ii = 1;
  nn = 1;
  Pars.curTS = GEV_event->ptgd[0]->timestamp;

  GEV_event->mult = 0;

  while ((TS - Pars.curTS) < Pars.dTS){
    /*read geb header */

    /* trap for too long events */

    if (ii >= MAXGEBS){
      printf ("error: this event is too long > %i\n", ii);
      return (1);
    };

    siz = buf_read (inData, (char *) GEV_event->ptgd[ii], sizeof (GEBDATA));
    if (siz != sizeof (GEBDATA)){
      printf ("failed to read %lu bytes for header, got %i\n", sizeof (GEBDATA), siz);
      return (1);
    };
    nn2++;
    Pars.nbytes += siz;
    GEV_event->mult++;

    TS = GEV_event->ptgd[ii]->timestamp;

    if (Pars.CurEvNo <= Pars.NumToPrint || (Pars.CurEvNo % Pars.modwrite == 0)){
      printf ("ev# %5i ", Pars.CurEvNo);
      stType = GebTypeStr (GEV_event->ptgd[ii]->type, str);
      printf ("%s ", str);
      printf ("%4iBytes ", GEV_event->ptgd[ii]->length);
      printf ("TS=%lli ", GEV_event->ptgd[ii]->timestamp);
      printf ("curTS=%lli ", Pars.curTS);
      dTS = TS - Pars.curTS;
      printf ("dTS= %lli", dTS);
      printf(" time %5.2f s\n",StpWatch.RealTime());
      StpWatch.Start(kFALSE);
      fflush (stdout);  
    };

    /* trap for bad timestamps */

    if (TS < Pars.curTS){
      printf ("batflag:: TS<Pars.curTS, reset it at event # %i\n", Pars.CurEvNo);
      printf ("TS=%lli, Pars.curTS=%lli, DT=%lli\n", TS, Pars.curTS, TS - Pars.curTS);
      fflush (stdout);
      Pars.curTS = TS;
      printf ("new Pars.curTS=%lli\n", Pars.curTS);
      printf ("we have read %lli bytes so far\n", Pars.nbytes);
      nbadTS++;
      if (nbadTS > 1000){
        printf ("too many bad TS, quit with error code 3\n");
        fflush (stdout);
        return (3);
      };
    };

    /* read payload */

    i1 = GEV_event->ptgd[ii]->length;
    siz = buf_read (inData, (char *) GEV_event->ptinp[ii], i1);
    if (siz != i1){
      printf ("failed to read %i bytes for payload, got %i\n", i1, siz);
      return (2);
    };
    nn3++;
    Pars.nbytes += siz;

    ii++;
    nn++;

    if (Pars.CurEvNo <= Pars.NumToPrint || (Pars.CurEvNo % Pars.modwrite == 0)){
      if (stType != 0){
        printf ("data processing error, stType=%i\n", stType);
        exit (1);
      };
    }
  }

  ii--;
  nn--;

  /* return the mutiplicity */

  GEV_event->mult = ii;
  return (0);
}


/*----------------------------------------------------------------------------*/
int main (int argc, char **argv){

  /*--------------*/
  /* declarations */
  /*--------------*/

  int j, i, HaveChatFile = 0;
  char *p;
  char ChatFileName[STRLEN];
  int GEBacq (char *);
  int time_stamp ();
  char str2[STRLEN], str3[STRLEN], str4[STRLEN];

  StpWatch.Start();

  /* inform of version of root used */

  printf ("from %s on %s %s\n", __FILE__, __DATE__, __TIME__);
  printf ("RCS version $Id: GEBSort.cxx,v 1.1 2014/02/12 16:32:11 tl Exp tl $\n");

  /*------*/
  /* help */
  /*------*/

  if (argc < 2){
    printf ("\n");
    printf ("use: %s -chat file [-help] [-version] .... TBD\n", argv[0]);
    printf ("\n");
    return (0);
  };

  /* initialize */

  Pars.InputSrc = NOTDEF;
  Pars.StartMapAddress = 0;

  /*--------------------*/
  /* Parse command line */
  /* and call GEBacq     */
  /*--------------------*/

  j = 1;                        /* current command line arg position */

  printf ("we have %i arguments\n", argc); fflush (stdout);

  if (argc == 1){
    printf ("help: see go file for examples of use of GEBSort");
    exit (0);
  };

  if (argc > 1){
    while (j < argc){
      printf ("%i... %s\n", j, argv[j]); fflush (stdout);

      if ((p = strstr (argv[j], "-input")) != NULL){
        // check that user specified enough arguments 
        j++;
        // determine input source 
        strcpy (str2, argv[j++]);
        
        if (strcmp ("disk", str2) == 0){
          strcpy (str3, argv[j++]);
          printf ("will take input from disk\n");
          strcpy (Pars.GTSortInputFile, str3);
          /* --- Very specific run number extraction for RCNP data ---*/
          /* --- Need to make this more general                    ---*/
          int run_num[100];
          int RunNum=0;
          int check=0;
          for (int j=0;j<100;j++) run_num[j]=0;
          for (int ii=0;ii<100;ii++) {
            if (isdigit(Pars.GTSortInputFile[ii])){ 
              run_num[ii]=(int)Pars.GTSortInputFile[ii]-'0';
              //std::cout << "HERE " << Pars.GTSortInputFile[ii] << std::endl;    
              std::cout << "HERE " << run_num[ii] << std::endl;    
            }
          }
          for (int j=0;j<100;j++) {
            if (run_num[j]==5) {
              RunNum=1000*run_num[j]+100*run_num[j+1]+10*run_num[j+2]+run_num[j+3];
              j=100;
            }
          }
          printf("RunNum %d\n",RunNum);
          Pars.RunNumber = RunNum;
          Pars.InputSrc = DISK;
          fflush (stdout);
        }else{
          printf ("unknown input option: %s\n", str2);
          printf ("aborting\n");
          fflush (stdout);
          exit (0);
        };
        printf ("\n");
      }else if ((p = strstr (argv[j], "-chat")) != NULL){
        j++;
        strcpy (ChatFileName, argv[j++]);
        printf ("will read sorting instructions from chatfile: %s\n", ChatFileName);
        system ("ulimit -a");
        HaveChatFile = 1;
      }else if ((p = strstr (argv[j], "-rootfile")) != NULL){
        j++;
        strcpy (Pars.ROOTFile, argv[j++]);
        printf ("rootfile name specified on command line\n");
        printf ("__will store spectra in rootfile: %s\n", Pars.ROOTFile);
        printf ("will recreate root file\n");
      }else{
        printf ("command line argument not understood!\n");
        printf ("%s: I was called as: \n--->[", argv[0]);
        for (i = 0; i < argc; i++){
          printf ("%s ", argv[i]);
          fflush (stdout);
        }
        printf ("]\non ");
        time_stamp ();
        exit (0);
      }
    };
  }
  /* now start the sorter */
  if (HaveChatFile){
    GEBacq (ChatFileName);
  }else{
    printf ("you must specify a chat script\n");
    exit (0);
  }

}

/*--------------------------------------------------------------------------*/
void signal_catcher (int sigval) {
  int time_stamp ();
  printf ("\nGSSort/GEBacq received signal <%i> on ", sigval);
  time_stamp ();
  Pars.WeWereSignalled = TRUE;
  fflush (stdout);
}

/*---------------------------------------------------------------------------*/
int GEBSort_read_chat (char *name) {

  /* declarations */

  FILE *fp, *fp1;
  char *pc, *pc1, str[STRLEN] = { '0' }, str1[STRLEN] = {'0'}, str2[STRLEN] = {'0'};
  char str3[STRLEN], str4[STRLEN], str5[STRLEN], str6[STRLEN];
  int nn = 0, nni = 0, st, PType;
  char *p;
  int i, k, i1, i2, i3, i4, i5, i6;
  int j1, j2, j3, j4, j5, j6, j7;
  float f1, f2, f3, f4, pi, r1, r2, rr;
  int echo = 0, nret;
  double d1;

  /* open chat file */
  if ((fp = fopen (name, "r")) == NULL){
    printf ("error: could not open chat file: <%s>\n", name);
    exit (0);
  };
  printf ("chat file: <%s> open\n", name);
  printf ("\n");
  fflush (stdout);

  /* read content and act */
  pc = fgets (str, STRLEN, fp);

  while (pc != NULL){
    if (echo) printf ("chat->%s", str); fflush (stdout);
    /* attemp to interpret the line */

    if ((p = strstr (str, "nevents")) != NULL) {
      nret = sscanf (str, "%s %i", str1, &Pars.nEvents);
      CheckNoArgs (nret, 2, str);
      printf ("will sort a max of %i events\n", Pars.nEvents);
      fflush (stdout);

    }
    else if (str[0] == 35){
      /* '#' comment line, do nothing */
      nni--;                /* don't count as instruction */

    }
    else if (str[0] == 59){
      /* ';' comment line, do nothing */
      nni--;                /* don't count as instruction */

    }
    else if (str[0] == 10){
      /* empty line, do nothing */
      nni--;                /* don't count as instruction */

    }
    else if ((p = strstr (str, "firstEvent")) != NULL)
    {
      nret = sscanf (str, "%s %i", str1, &Pars.firstEvent);
      CheckNoArgs (nret, 2, str);
      printf ("will start sorting at event %d\n", Pars.firstEvent);
      fflush (stdout);

    }
    else if ((p = strstr (str, "timewin")) != NULL)
    {
      nret = sscanf (str, "%s %lli", str1, &Pars.dTS);
      CheckNoArgs (nret, 2, str);
      printf ("will sort using a time window of %lli\n", Pars.dTS);
      fflush (stdout);
    }
    else if ((p = strstr (str, "modwrite")) != NULL)
    {
      nret = sscanf (str, "%s %i", str1, &Pars.modwrite);
      CheckNoArgs (nret, 2, str);

    }
    else if ((p = strstr (str, "tsnumwrites")) != NULL)
    {
      nret = sscanf (str, "%s %i", str1, &Pars.tsnumwrites);
      CheckNoArgs (nret, 2, str);

    }
    else if ((p = strstr (str, "abort")) != NULL)
    {

      printf ("will abort\n");
      fclose (fp);
      printf ("\n");
      printf ("chat file: <%s> closed\n", name);
      printf ("processed %i sort instructions and %i lines\n", nni, nn);
      printf ("\n");
      fflush (stdout);
      exit (0);

    }
    else if ((p = strstr (str, "printevents")) != NULL)
      {
        nret = sscanf (str, "%s %i", str1, &Pars.NumToPrint);
        CheckNoArgs (nret, 2, str);
        printf ("will print details of the first %i events\n", Pars.NumToPrint);
        fflush (stdout);

      }
    else if ((p = strstr (str, "DumpEvery")) != NULL)
      {
        nret = sscanf (str, "%s %i", str1, &Pars.DumpEvery);
        CheckNoArgs (nret, 2, str);
        printf ("will dump to output file every %i minutes\n", Pars.DumpEvery);
        fflush (stdout);

      }
    else if ((p = strstr (str, "exit")) != NULL)
      {

        printf ("will skip rest of chat file\n");
        fclose (fp);
        printf ("\n");
        printf ("chat file: <%s> closed\n", name);
        printf ("processed %i sort instructions and %i lines\n", nni, nn);
        printf ("\n");
        fflush (stdout);
        return (0);
      }
    else
    {
      /*-----------------------------*/
      /* chatscript read error point */
      /*-----------------------------*/

      printf ("line %2.2i in chat script, option :%s \n__not understood\n", nn, str);
      printf ("%i\n", str[0]);
      printf ("aborting\n");
      fflush (stdout);
      exit (0);
    };

    /* read next line in chat script */
    nn++;                     /* line counter */
    nni++;                    /* instruction counter */
    pc = fgets (str, STRLEN, fp);

  };

  /* done */

  fclose (fp);
  printf ("\n");
  printf ("chat file: <%s> closed\n", name);
  printf ("processed %i sort instructions and %i lines\n", nni, nn);
  printf ("\n");
  fflush (stdout);
  return (0);

};


/*----------------------------------------------------------------------------*/

int showStatus (){
  printf ("read %i events; ", (Pars.CurEvNo - Pars.firstEvent));
  printf ("time since last update %i minutes\n", (int) tdmp);
  fflush (stdout);
  return (0);
};

/*----------------------------------------------------------------------------*/
int GEBacq (char *ChatFileName){

  /* declarations */

  int NprintEvNo = 0, in;
  GEB_EVENT GEB_event;
  int st = 0, eov = 0, nWords = 0, i1, i2, i, j, nret, siz;
  int ii, jj;
  char str[256], str1[256], str2[246];
  FILE *fp;
  time_t t1, t2;
  Int_t ComPressLevel = NOTDEF;
  char *p;
  double rn, d1, nsec;
  DGSHEADER dgsHeader;
  static int firsttime = 1;
  static long long int t0, TSprev = 0;
  long long int tcur;
  unsigned int typehit[MAX_GEB_TYPE];
  long long int firtsTSinEvent, dTS;

  /* prototypes */
  int GEBSort_read_chat (char *);
  int sup_rcnp ();
  int bin_rcnp (GEB_EVENT *);
  int exit_rcnp ();

  /*-------*/
  /* Hello */
  /*-------*/

  printf ("\nGEBsort running on: ");
  time_stamp ();
  printf ("$Id: GEBSort.cxx,v 1.1 2014/02/12 16:32:11 tl Exp tl $\n");
  printf ("\n");

  /*------------*/
  /* initialize */
  /*------------*/

  for (i = 0; i < MAX_GEB_TYPE; i++) typehit[i] = 0;

  printf ("\ninitializing\n\n");
  fflush (stdout);

  Pars.nEvents = 2000000000;
  Pars.WeWereSignalled = FALSE; /* signal  */
  Pars.firstEvent = 0;
  Pars.GSudpPort = 1101;
  Pars.NumToPrint = 25;
  Pars.DumpEvery = 2000000000;
  Pars.dTS = 500;
  Pars.nbytes = 0;
  Pars.modwrite = 1000;
  Pars.tsnumwrites = 100;

  for (i = 0; i < MAXGEBS; i++)  {
    GEB_event.ptgd[i] = (GEBDATA *) calloc (2 * sizeof (GEBDATA), 1);
    GEB_event.ptinp[i] = (CRYS_INTPTS *) calloc (MAXPAYLOADSIZE, 1);
  };

  rbuf = (char *) calloc (RBUFSIZE + 1, 1);

  /*------------------*/
  /* read chat script */
  /*------------------*/

  GEBSort_read_chat (ChatFileName);

  printf ("checking proper input of chat file...\n");
  if (Pars.InputSrc == NOTDEF){
    printf ("you must specify an input source\n");
    exit (1);
  }else if (Pars.InputSrc == DISK) {

#if(USEZLIB==0)

    /* attempt to open input file */

    inData = open (Pars.GTSortInputFile, O_RDONLY, 0);
    if (inData == -1)
      {
        printf ("could not open\"%s\"; quit\n", Pars.GTSortInputFile);
        exit (1);
      }
    else
      printf ("input file \"%s\" is open, inData=%lli\n", Pars.GTSortInputFile, inData);

    /* find the very first GEB header to find start TS */

    siz = read (inData, (char *) GEB_event.ptgd[0], sizeof (GEBDATA));

    printf ("siz=%i;", siz);
    printf ("ptgd[i]->type=%2i; ", GEB_event.ptgd[0]->type);
    printf ("ptgd[i]->length=%4i; ", GEB_event.ptgd[0]->length);
    printf ("ptgd[i]->timestamp=%lli\n", GEB_event.ptgd[0]->timestamp);
    fflush (stdout);

    Pars.curTS = GEB_event.ptgd[0]->timestamp;
    printf ("start TS is %lli\n", Pars.curTS);

    /* reopen */

    close (inData);
    inData = open (Pars.GTSortInputFile, O_RDONLY, 0);
    printf ("reopened input file, inData=%lli \n", inData);

#endif

#if(USEZLIB==1)
      /* attempt to open input file */

      zinData = gzopen (Pars.GTSortInputFile, "r");
      if (zinData == NULL)
        {
          printf ("could not open zipped \"%s\"; quit\n", Pars.GTSortInputFile);
          exit (1);
        }
      else
        printf ("zipped input file \"%s\" is open\n", Pars.GTSortInputFile);
#endif


    }else
    {
      printf ("input source not recognized, quit\n");
      exit (1);
    };

  printf ("input source is set up, supposedly\n");

  /*---------------------*/
  /* other sanety checks */
  /*---------------------*/

  if (Pars.InputSrc == NOTDEF)
    {
      printf ("you must specify an input source!\n");
      printf ("quitting...\n");
      exit (1);
    };


  /*-----------------------------------*/
  /* if we are going to use the        */
  /* shared map file, create it!       */
  /* be careful about the map address! */
  /*-----------------------------------*/

  NprintEvNo = 0;
  Pars.CurEvNo = 0;

  /*------------------------------------------*/
  /* if we are using root file, then either   */
  /* read in old rootfile or create a nev one */
  /*------------------------------------------*/

  Pars.f1 = NULL;
  Pars.f1 = new TFile (Pars.ROOTFile, "RECREATE");
  printf ("root file <%s>\n", Pars.ROOTFile);
  if (!Pars.f1->IsOpen ())
    {
      printf ("could not open file....\n\n");
      exit (-1);
    };
  printf ("base=<%s>\n", Pars.f1->GetPath ());
  Pars.f1->Print ();

  sup_rcnp ();

  printf ("we have defined the following ROOT spectra:\n");

  Pars.wlist = gDirectory->GetList ();

  /*----------------------*/
  /* setup signal catcher */
  /*----------------------*/

#ifdef LINUX
  signal (SIGHUP, signal_catcher);
#endif
#ifdef SOLARIS
  sigset (SIGHUP, signal_catcher);
#endif
  printf ("setup signal catcher\n");

  /*---------------*/
  /* start sorting */
  /*---------------*/

  printf ("started sorting... ");
  if (Pars.InputSrc == DISK) printf ("from disk...\n");
  printf ("\n");
  fflush (stdout);

  tdmplast = time (NULL);
  /* start of LOOP */
  while (st >= 0 && (Pars.CurEvNo - Pars.firstEvent) < Pars.nEvents && eov == 0)
    {
      /*----------------*/
      /* get next event */
      /*----------------*/

      st = GEBGetEv (&GEB_event, Pars.CurEvNo);

      if (st == 0 && Pars.CurEvNo < Pars.tsnumwrites)
        {
          for (i = 0; i < GEB_event.mult; i++)
            {
              if (i == 0)
              GebTypeStr (GEB_event.ptgd[i]->type, str);
           
              TSprev = GEB_event.ptgd[i]->timestamp;
            }

        };

      if (st == 0)
        {
          if (firsttime)
            {
              firsttime = 0;
              t0 = GEB_event.ptgd[0]->timestamp;
              printf ("t0=%lli\n", t0);
              printf ("first event: GEB_event.mult=%i\n", GEB_event.mult);
              for (i = 0; i < GEB_event.mult; i++)
                {
                  GebTypeStr (GEB_event.ptgd[i]->type, str);
                  printf ("%4i/%2i: (%2i,%s) TS=%20lli; ", Pars.CurEvNo, i, GEB_event.ptgd[i]->type, str,
                          GEB_event.ptgd[i]->timestamp);
                  printf ("dT=%lli\n", GEB_event.ptgd[i]->timestamp - TSprev);
                };
            }

          tcur = GEB_event.ptgd[0]->timestamp;

          /* count data types */

          for (i = 0; i < GEB_event.mult; i++)
            {
              if (GEB_event.ptgd[i]->type > 0 && GEB_event.ptgd[i]->type < MAX_GEB_TYPE)
                typehit[GEB_event.ptgd[i]->type]++;
            };
          /* fill dtbtev spectrum */
          firtsTSinEvent = LLONG_MAX;
          for (i = 0; i < GEB_event.mult; i++){
            if (GEB_event.ptgd[i]->timestamp < firtsTSinEvent){
              firtsTSinEvent = GEB_event.ptgd[i]->timestamp;
            }
          }
        };

      /*----------------------------------------*/
      /* allow user to manipulate raw data here */
      /*----------------------------------------*/

      if (st != 0)
        {
          printf (" GEBGetEv returned %i\n", st);
          printf ("we have read %lli bytes; ", Pars.nbytes);
          printf ("CurEvNo=%i\n", Pars.CurEvNo);
          fflush (stdout);

          /* terminate sort */

          eov = 1;

          /* note: */
          /* we might want to wait and try GEBGetEv */
          /* later to give the impresssion of interactivity */
          /* here in some future version... */

        }

      if (st == 0)
        {

          /*----------------------------*/
          /* good event, now process it */
          /*----------------------------*/

          /* statistics */

          Pars.CurEvNo++;
          NprintEvNo++;

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("GEBGetEv returned st=%i\n", st);
              printf ("we have read %lli bytes; ", Pars.nbytes);
              printf ("CurEvNo=%i\n", Pars.CurEvNo);
              fflush (stdout);
            };


          /* debug print some events */

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("\n+++++++++++++++++++++++++++++++\n");
              printf ("*start event # %i with multiplicity %i looks like this:\n", Pars.CurEvNo, GEB_event.mult);
              for (i = 0; i < GEB_event.mult; i++)
                {
                  GebTypeStr (GEB_event.ptgd[i]->type, str);
                  printf ("%2i> %2i, %s, TS=%lli\n", i, GEB_event.ptgd[i]->type, str, GEB_event.ptgd[i]->timestamp);
                };
            };


          bin_rcnp (&GEB_event);
          extern TTree *tree;
          tree->Fill();
          //printf("Bytes comitted to tree[%i]: %i\n", Pars.CurEvNo, tree->Fill());

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("*end of event # %i\n", Pars.CurEvNo);
              printf ("+++++++++++++++++++++++++++++++\n");
            };


        };


      /*---------------------*/
      /* house keeping...... */
      /* done every so often */
      /*---------------------*/

      if (Pars.CurEvNo % 100 == 0)
        {

          /* calc time since last dump */

          tdmp = time (NULL);
          tdmp -= tdmplast;
          tdmp /= 60;           /* now minutes */

        };


    };

  /*-----------------------*/
  /* we are done sorting!! */
  /* save all ROOT spectra */
  /*-----------------------*/
  exit_rcnp();

  printf ("\n");
  printf ("Sorting is done\n");
  printf ("attempting to save root or map file\n");
  printf ("\n");
  fflush (stdout);

  if (Pars.InputSrc == DISK){
    close (inData);
  }

  printf ("\n");
  fflush (stdout);
//  if (!Pars.UseShareMemFile)
//    {
      printf ("attempting to close root file...\n");
      fflush (stdout);

      printf ("Pars.f1->Write();\n");
      fflush (stdout);
      Pars.f1->Write ();
      printf ("Pars.f1->Print();\n");
      fflush (stdout);
      Pars.f1->Print ();
      printf ("Pars.f1->Close();\n");
      fflush (stdout);
      Pars.f1->Close ();
      printf ("done saving rootfile \"%s\n\n", Pars.ROOTFile);
      fflush (stdout);
//    }
  printf ("\n");


  /*-------------------------*/
  /* print simple statistics */
  /*-------------------------*/

  showStatus ();

  /* done */

  printf ("\n");
  printf ("sorted timestamp range %lli-->%lli: %lli\n", t0, tcur, tcur - t0);
  d1 = (double) (tcur - t0);
  d1 /= 100000000;
  printf ("that is %.1f seconds or ", (float) d1);
  nsec = d1;
  i1 = (unsigned int) d1 / 3600;
  d1 -= i1 * 3600;
  i2 = (unsigned int) d1 / 60;
  d1 -= i2 * 60;
  printf ("%ih%im%is\n", i1, i2, (int) d1);
  printf ("^^^^^ any timestamp jumps will upset this accounting\n");
  printf ("\n");


  printf ("hit statistics per type\n");
  i1 = 0;
  for (i = 1; i < MAX_GEB_TYPE; i++)
    if (typehit[i] > 0)
      {
        GebTypeStr (i, str);
        printf ("%2i %s %10i ;", i, str, typehit[i]);
        i1 += typehit[i];
        d1 = (double) typehit[i] / nsec;
        printf (" %9.2f Hz ", (float) d1);
        printf ("\n");
      };
  printf ("read a total of              %i ; header/payloads\n", i1);
  printf ("\n");

  //  printf ("\n");
  //  printf ("nn1=%i\n", nn1);
  //  printf ("nn2=%i\n", nn2);
  //  printf ("nn3=%i\n", nn3);
  //  printf ("\n");
  printf("Duration of sort in real time %5.2f s\n",StpWatch.RealTime());
  StpWatch.Start(kFALSE);
  printf("Sort time ratio (real t / dTS) SMALLER IS BETTER: %10.5f\n\n",StpWatch.RealTime()/nsec);
 
  printf ("boniva sancta! ");
  printf ("...GEBSort (unexpectedly) did not crash!\n");
  printf ("\n ** GEBSort is done at ");
  time_stamp ();
  printf ("\n");
  return (0);

}

/*----------------------------------------------------*/
