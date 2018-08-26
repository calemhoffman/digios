
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

#define MAXFLOAT 3.40282347e+38
#define MAXINT   2147483647


#if(HAVE_VXWORKS)
#include <GEBClient.h>
#include <gretTapClient.h>
#endif

/* global variables */

#if(USEZLIB==0)
off_t inData;
#endif
#if(USEZLIB==1)
gzFile zinData;
#endif

/* buffer of data for sorting */

char *rbuf;

extern TH1D *sumTrackE;

int nn2 = 0;
int nn3 = 0;

//NprintEvNo = 0;
int bufPos = 0;
int nBadTestPat = 0;
int egemin = 2;
double oldLEDTs = 0;
double oldCFDTs = 0;
time_t tdmp = 0, tdmplast;
float ehiGeOffset[NCHANNELS];
float ehiGeGain[NCHANNELS];
int ehiDoGainCor = 0;
int minid = MAXINT, maxid = -MAXINT;

char CommandFileName[STRLEN] = "GEBSort.command";

int time_stamp ();
TH1D *mkTH1D (char *, char *, int, double, double);
TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

TH1D *ehi[MAXDETPOS + 1];

/* user */


#include "UserInclude.h"

#define DEBUG1 0
#define DEBUG2 0

/* Doppler correction and Ge calibrations */

#define NGSGE 110

float cal_off[NGSGE], cal_gain[NGSGE];
double angle[NGSGE], anglephi[NGSGE];

double DopCorFac[NGSGE], ACFac[NGSGE];

void SetBeta ();

/* parameters */

PARS Pars;

static const int ConnectionRetryWait = 2 * 100; /* usec between con. retries. */
static const int ConnectionRetryCount = 150;    /* Times we retry connect. */

struct gretTap *pTap;
struct GEBData *pData;

int nn1 = 0;

TStopwatch StpWatch;

/*----------------------------------------------------*/

TH1D *
mkTH1D (char *str1, char *str2, int nn, double lo, double hi)
{
  TH1D *tmppt;

  if (!Pars.UpdateRootFile)
    {
      tmppt = new TH1D (str1, str2, nn, lo, hi);
      printf ("Created Object \"%s\", %p\n, \"%s\"", str1, tmppt, str2);
    }
  else
    {
      tmppt = (TH1D *) gROOT->FindObject (str1);
      printf ("Found Object \"%s\", %p\n", str1, tmppt);
    }

  return (tmppt);

}

/*----------------------------------------------------*/

TH2F *
mkTH2F (char *str1, char *str2, int nn1, double lo1, double hi1, int nn2, double lo2, double hi2)
{
  TH2F *tmppt;

  if (!Pars.UpdateRootFile)
    {
      tmppt = new TH2F (str1, str2, nn1, lo1, hi1, nn2, lo2, hi2);
      printf ("Created Object \"%s\", %p\n", str1, tmppt);
    }
  else
    {
      tmppt = (TH2F *) gROOT->FindObject (str1);
      printf ("Found Object \"%s\", %p\n", str1, tmppt);
    };

  return (tmppt);

}



/*--------------------------------------------------------*/

void
CheckNoArgs (int required, int actual, char *str)
{

  if (required < actual)
    {
      printf ("argument problem with chat option\n");
      printf ("--> %s\n", str);
      printf ("required # arguments: %i\n", required - 1);
      printf ("actual   # arguments: %i\n", actual - 1);
      printf ("Please fix and try again, quitting...\n");
      exit (1);
    };

}

/*--------------------------------------------------------*/
void
SetBeta ()
{

  /* delarations */

  int i;
  double d1;

  /*-------------------------------------*/
  /* find Doppler and aberration factors */
  /*-------------------------------------*/

  for (i = 0; i < NGSGE; i++)
    {
      //printf("det %3.3i-> ", i);
      d1 = angle[i] / 57.29577951;
      DopCorFac[i] = (1 - Pars.beta * cos (d1)) / sqrt (1 - Pars.beta * Pars.beta);
      //printf("dop cor fac: %6.4f; ", DopCorFac[i]);
      ACFac[i] = DopCorFac[i] * DopCorFac[i];
      //printf("aberration cor fac: %6.4f\n", ACFac[i]);

    };
  fflush (stdout);

  /* done */

}

/*-----------------------------------------------------------*/

float
findPolarFromCartesian (float xx, float yy, float zz, float *rr)
{
  float d1;

  *rr = sqrtf (xx * xx + yy * yy + zz * zz);
  d1 = acosf (zz / *rr);

  return (d1);
}

/*-----------------------------------------------------------*/

float
findAzimuthFromCartesian (float xx, float yy, float zz)
{

  float d1;

#if(0)
  if (xx > 0 && yy >= 0)
    d1 = atanf (yy / xx);
  else if (xx > 0 && yy < 0)
    d1 = atanf (yy / xx) + 2. * M_PI;
  else if (xx < 0)
    d1 = atanf (yy / xx) + M_PI;
  else if (xx == 0 && yy > 0)
    d1 = M_PI / 2.;
  else if (xx == 0 && yy < 0)
    d1 = 3. * M_PI / 2.;
  else
    d1 = 0.0;
#endif
  d1 = atan2f (yy, xx);


  return (d1);

}



/*----------------------------------------------------------------------------*/

int
GebTypeStr (int type, char str[])
{
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
int
buf_read (off_t inData, char *buf, int nbytes)
{

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

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("buf_read called, asked for %i byte, have %i bytes left in buffer, bpos=%i\n", nbytes, bleft, bpos);

  if (buf == NULL)
    {
      printf ("buf_read: pointer to buf is NULL, cannot continue\n");
      return (0);
    };

#if(0)

  /*---------------------------------*/
  /* get the data with a simple read */
  /*---------------------------------*/
  /* we never want to do that, too slow */

  st = read (inData, buf, nbytes);

#else

  /*-----------------------*/
  /* get data via a buffer */
  /*-----------------------*/

//  printf("entered buffer read, asking for %i bytes\n",nbytes);

  /* do we need to read a new buffer in ? */

  if (nbytes > bleft)
    {
      if (Pars.CurEvNo <= Pars.NumToPrint)
        printf ("__forced to read data because nbytes=%i > bleft=%i\n", nbytes, bleft);

      /* first read what is left in the buffer */

      partread = bleft;
      if (bleft > 0)
        {
          memcpy (buf, rbuf + bpos, partread);

          bleft = 0;
          bpos = ActualBufSize;
          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("partial transfer of %i bytes, set bleft=%i, set bpos=%i\n", partread, bleft, bpos);
        }
      else
        partread = 0;

      /* then read in a new buffer from disk or GEB */

      if (Pars.InputSrc == DISK)
        {

          ActualBufSize = read (inData, rbuf, RBUFSIZE);
          nn1 += ActualBufSize;
          bpos = 0;
          bleft = ActualBufSize;
          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("read new buffer of size %i bytes, bpos=%i, bleft=%i\n", ActualBufSize, bpos, bleft);
          if (ActualBufSize != RBUFSIZE)
            {
              printf ("could not read a full buffer of size %i, only got %i, bleft=%i\n", RBUFSIZE, ActualBufSize,
                      bleft);
              printf ("read new buffer of size %i bytes, bpos=%i, bleft=%i\n", ActualBufSize, bpos, bleft);
              if (ActualBufSize <= 0)
                return (0);
            };

        }
      else if (Pars.InputSrc == GEB)
        {
#if (HAVE_VXWORKS)
          bzero (rbuf, RBUFSIZE + 1);

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("attempting to get %i gebheader/payloads from GEB\n", Pars.grouping);
              fflush (stdout);
            }

          ntries = 0;
          pData = gretTapData (pTap, Pars.grouping, Pars.timeout);

          /* failed to get data */

          if (pData == NULL)
            {
              printf ("failed to get more data from GEBTap, return 0 bytes\n");
              return (0);
            }



          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("... got them, we think\n");

          /* what comes back is a linked list of events */
          /* which we quietly read and stuff into the  */
          /* buffer. It would probably be better to use  */
          /* the list directly, but the buffer method  */
          /* exists and we are not as clever as Carl, by far */

          /* FYI: struct GEBData *pData; GEBData defined in GEBLink.h */
          /*      dont know what to do with refCount and refIndex yet */


          bpos = 0;
          bleft = st;
          tmpData = pData;
          bstart = 0;
          for (l = 0; l < Pars.grouping; l++)
            {
              if (tmpData->length < MAXPAYLOADSIZE)
              if (tmpData->payload != NULL)
                {

                  bstart = bpos;
                  if (Pars.CurEvNo <= Pars.NumToPrint)
                    {
                      printf ("processing link # %i\n", l);
                      fflush (stdout);
                    }

                  /* copy GEB header */

                  if (Pars.CurEvNo <= Pars.NumToPrint)
                    {
                      printf ("will copy header; len %i, ", GEB_HEADER_BYTES);
                      printf ("TS=%lli;", tmpData->timestamp);
                      printf ("type=%i", tmpData->type);
                      GebTypeStr (tmpData->type, str);
                      printf ("==%s", str);
                      printf ("to bpos=%i(Byte) ", bpos);
                      printf ("... ");
                    };
                  memcpy (rbuf + bpos, (char *) tmpData, GEB_HEADER_BYTES);
                  bpos += GEB_HEADER_BYTES;
                  assert (bpos < RBUFSIZE);
                  if (Pars.CurEvNo <= Pars.NumToPrint)
                    {
                      printf ("done\n");
                    };

                  /* copy payload */

#if(0)
                  printf("tmpData->length=%i, ",tmpData->length);
                  printf("bpos=%i, ",bpos);
                  printf("RBUFSIZE=%i, ",RBUFSIZE);
                  printf("tmpData->payload=0x%p\n",tmpData->payload);
                  fflush(stdout);
#endif
                  memcpy (rbuf + bpos, (char *) tmpData->payload, tmpData->length);
                  bpos += tmpData->length;
                  assert (bpos < RBUFSIZE);

                  if (Pars.CurEvNo <= Pars.NumToPrint)
                    printf ("copied payload of length %i\n", tmpData->length);

                }
              else
                {
                  if (Pars.CurEvNo <= Pars.NumToPrint && 0)
                    {
                      printf ("skipped data of type ");
                      GebTypeStr (tmpData->type, str);
                      printf ("%s", str);
                      printf ("with` ll=ength of %i\n", tmpData->length);
                      fflush (stdout);

                    }
                };

              /* next link and free the previous one */

              tmpStore = tmpData;
              tmpData = tmpData->next;
              free (tmpStore);

            };

          /* done transferring, set counters */
          /* for reading again */

          bleft = bpos;
          bpos = 0;
#endif
        };

      /* then read the rest */

      if (Pars.CurEvNo <= Pars.NumToPrint)
        {
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

      if (bleft < (nbytes - partread))
        {
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

    }
  else
    {

      /* just transfer what was aked for */

//    printf("%p, %p - %p, %i\n", buf, rbuf, rbuf+bpos,nbytes);
      if (Pars.CurEvNo <= Pars.NumToPrint)
        printf ("simple transfer: now: bleft=%i, bpos=%i\n", bleft, bpos);

      memcpy (buf, rbuf + bpos, nbytes);
      bpos += nbytes;
      bleft -= nbytes;
      if (Pars.CurEvNo <= Pars.NumToPrint)
        printf ("transfer of %i bytes, set bleft=%i, set bpos=%i\n", nbytes, bleft, bpos);

    };
  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("buf_read done:bpos=%i,bleft=%i\n", bpos, bleft);

  assert (bleft >= 0);


#endif

  /* done */

  return (nbytes);

};

/*----------------------------------------------------------------------------*/

int
GEBGetEv (GEB_EVENT * GEV_event, int curEvNo)
{

  /* declarations */

  static int nn = 0, ii = 0, nx = 0, nbadTS = 0, firsttime = 1;
  int siz, val, i1, i, stType;
  static int newbuf = 1, *pos;
  long long int TS, dTS;
  char str[256];

  /* prototypes */

  int bread (int, int *, int *, int *, int *);

#if(DEBUG2)
  printf ("GEBGetEv: called, nx=%i\n", nx);
  fflush (stdout);
#endif

  if (firsttime)
    {
      firsttime = 0;

      /* get the initial header, into position 1, not 0 */

      ii = 1;
      siz = buf_read (inData, (char *) GEV_event->ptgd[ii], sizeof (GEBDATA));
      if (siz != sizeof (GEBDATA))
        {
          printf ("failed to read %i bytes for header, got %i\n", sizeof (GEBDATA), siz);
          return (1);
        };
      Pars.nbytes += siz;
      nn2++;
      printf ("got initial header, TS=%lli\n", GEV_event->ptgd[ii]->timestamp);

      /* get the initial payload */

      i1 = GEV_event->ptgd[ii]->length;
      siz = buf_read (inData, (char *) GEV_event->ptinp[ii], i1);
      if (siz != i1)
        {
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

  if (nn > 0)
    {

#if(DEBUG2)
      printf ("we have old geb/payload left over at position %i: \n", nn);
      fflush (stdout);
      printf ("__ptgd[nn]->type=%2i; ", GEV_event->ptgd[nn]->type);
      printf ("ptgd[nn]->length=%4i; ", GEV_event->ptgd[nn]->length);
      printf ("ptgd[nn]->timestamp=%lli\n", GEV_event->ptgd[nn]->timestamp);
      fflush (stdout);
#endif
      /* move the last (at pos nn) to the first position */

      memcpy ((char *) GEV_event->ptgd[0], (char *) GEV_event->ptgd[nn], sizeof (GEBDATA));
      memcpy ((char *) GEV_event->ptinp[0], (char *) GEV_event->ptinp[nn], GEV_event->ptgd[0]->length);

    }

  /* reset, ii=0 alwasy taken for the first leftover from last time */

  ii = 1;
  nn = 1;
  Pars.curTS = GEV_event->ptgd[0]->timestamp;

  GEV_event->mult = 0;
#if(DEBUG2)
  printf ("ii=%i, nn=%i\n", ii, nn);
  printf ("Pars.curTS=%lli, TS=%lli\n", Pars.curTS, TS);
#endif

  while ((TS - Pars.curTS) < Pars.dTS)
    {
      /*read geb header */

//      assert (i < MAXGEBS);
//      assert (GEV_event->ptgd[i] != NULL);

#if (0)
      printf ("for ii=%i:: trying to get a geb header of size %i\n", ii, sizeof (GEBDATA));
      printf ("GEV_event->ptgd[%i]=0x%p\n", ii, GEV_event->ptgd[ii]);
      fflush (stdout);
#endif

      /* trap for too long events */

      if (ii >= MAXGEBS)
        {
          printf ("error: this event is too long > %i\n", ii);
          return (1);
        };

      siz = buf_read (inData, (char *) GEV_event->ptgd[ii], sizeof (GEBDATA));
      if (siz != sizeof (GEBDATA))
        {
          printf ("failed to read %i bytes for header, got %i\n", sizeof (GEBDATA), siz);
          return (1);
        };
      nn2++;
      Pars.nbytes += siz;
      GEV_event->mult++;


#if (0)
      printf ("ii=%i, found header with TS=%lli, payload length=%i\n", ii, GEV_event->ptgd[ii]->timestamp,
              GEV_event->ptgd[ii]->length);
      fflush (stdout);
#endif
      TS = GEV_event->ptgd[ii]->timestamp;
#if(DEBUG2)
      printf ("ii=%i, nn=%i\n", ii, nn);
      printf ("Pars.curTS=%lli, TS=%lli\n", Pars.curTS, TS);
#endif


      if (Pars.CurEvNo <= Pars.NumToPrint || (Pars.CurEvNo % Pars.modwrite == 0))
        {
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

      if (TS < Pars.curTS)
        {
          printf ("batflag:: TS<Pars.curTS, reset it at event # %i\n", Pars.CurEvNo);
          printf ("TS=%lli, Pars.curTS=%lli, DT=%lli\n", TS, Pars.curTS, TS - Pars.curTS);
          fflush (stdout);
          Pars.curTS = TS;
          printf ("new Pars.curTS=%lli\n", Pars.curTS);
          printf ("we have read %lli bytes so far\n", Pars.nbytes);
          nbadTS++;
          if (nbadTS > 1000)
            {
              printf ("too many bad TS, quit with error code 3\n");
              fflush (stdout);
              return (3);
            };
        };


      /* read payload */

      i1 = GEV_event->ptgd[ii]->length;
      siz = buf_read (inData, (char *) GEV_event->ptinp[ii], i1);
      if (siz != i1)
        {
          printf ("failed to read %i bytes for payload, got %i\n", i1, siz);
          return (2);
        };
      nn3++;
      Pars.nbytes += siz;
#if (DEBUG2)
      printf ("__read payload of siz=%i into event position %i\n", siz, ii);
      fflush (stdout);
#endif

      ii++;
      nn++;
#if (DEBUG2)
      printf ("1: ii=%i, nn=%i\n", ii, nn);
      fflush (stdout);
#endif

      if (Pars.CurEvNo <= Pars.NumToPrint || (Pars.CurEvNo % Pars.modwrite == 0))
        if (stType != 0)
          {
            printf ("data processing error, stType=%i\n", stType);
            exit (1);
          };
    }

  ii--;
  nn--;
#if (DEBUG2)
  printf ("2: ii=%i, nn=%i\n", ii, nn);
  fflush (stdout);
#endif

  /* return the mutiplicity */

  GEV_event->mult = ii;

#if(DEBUG2)
  printf ("complete event, next TS is %lli or  %lli out\n", TS, TS - Pars.curTS);
  printf ("we found %i events in coincidence, timestamps are\n", GEV_event->mult);
  for (i = 0; i < GEV_event->mult; i++)
    printf ("[%i] TS=%lli\n", i, GEV_event->ptgd[i]->timestamp);
  printf ("we have read %lli bytes so far\n", Pars.nbytes);
  printf ("GEV_event->mult=%i\n", GEV_event->mult);
  fflush (stdout);
  nx++;
  if (nx > 3)
    exit (0);
#endif

#if(DEBUG2)
  printf ("GEBGetEv: done %i\n");
  fflush (stdout);
#endif

  return (0);
}


/*----------------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

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

  if (argc < 2)
    {
      printf ("\n");
      printf ("use: %s -chat file [-help] [-version] .... TBD\n", argv[0]);
      printf ("\n");
      return (0);
    };

  /* initialize */

  Pars.InputSrc = NOTDEF;
  Pars.HaveRootFileName = 0;
  sprintf (Pars.ROOTFileOption, "UNDEFINED");
  Pars.GGMAX = 2000;
  Pars.ndetlimlo = 1;
  Pars.ndetlimhi = 8;
  Pars.fomlo[1] = 0;
  Pars.fomhi[1] = 2.0;
  Pars.UpdateRootFile = 0;
  Pars.UseShareMemFile = FALSE;
  Pars.StartMapAddress = 0;
  sprintf (Pars.ShareMemFile, "GTSort.map");

  for (i = 0; i < MAXDETNO; i++)
    {
      Pars.CCcal_offset[i] = 0;
      Pars.CCcal_gain[i] = 1.0;
      for (j = 0; j <= MAXCRYSTALNO; j++)
        {
          Pars.SEGcal_gain[i][j] = 1.0;
          Pars.SEGcal_offset[i][j] = 0.0;
        };
    }

  /*--------------------*/
  /* Parse command line */
  /* and call GEBacq     */
  /*--------------------*/

  j = 1;                        /* current command line arg position */

  printf ("we have %i arguments\n", argc);
  fflush (stdout);

  if (argc == 1)
    {
      printf ("help: see go file for examples of use of GEBSort");
      exit (0);
    };

  if (argc > 1)
    while (j < argc)
      {
        printf ("%i... %s\n", j, argv[j]);
        fflush (stdout);

        if ((p = strstr (argv[j], "-version")) != NULL)
          {
            printf ("$Id: GEBSort.cxx,v 1.1 2014/02/12 16:32:11 tl Exp tl $\n");
            exit (0);
          }
        else if ((p = strstr (argv[j], "-help")) != NULL)
          {
            printf ("\n");
            printf ("GEBSort is documented on the WWW, URL:\n");
            printf ("\n");
            printf ("http://wiki.anl.gov/gretina_at_anl \n");
            printf ("\n");
            exit (0);

          }
        else if ((p = strstr (argv[j], "-input")) != NULL)
          {

            /* check that user specified enough arguments */

            j++;

            /* determine input source */

            strcpy (str2, argv[j++]);


            if (strcmp ("disk", str2) == 0)
              {
                strcpy (str3, argv[j++]);
//                strcpy (Pars.ROOTFileOption, argv[j++]);
                printf ("will take input from disk\n");
                strcpy (Pars.GTSortInputFile, str3);
		/* --- Very specific run number extraction for RCNP data ---*/
		/* --- Need to make this more general                    ---*/
		int run_num[100];
		int RunNum=0;
		int check=0;
		for (int j=0;j<100;j++) run_num[j]=0;
		for (int ii=0;ii<100;ii++) {
		  if (isdigit(Pars.GTSortInputFile[ii]))
		    { 
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
              }
            else if (strcmp ("geb", str2) == 0)
              {
                strcpy (Pars.pHost, argv[j++]);
                Pars.grouping = atol (argv[j++]);
                Pars.type = atol (argv[j++]);
                Pars.timeout = (float) atof (argv[j++]);
                printf ("Pars.pHost=%s\n", Pars.pHost);
                printf ("Pars.grouping=%i\n", Pars.grouping);
                printf ("Pars.type=%i\n", Pars.type);
                printf ("Pars.timeout=%f\n", Pars.timeout);
                Pars.InputSrc = GEB;
//                strcpy (Pars.ROOTFileOption, argv[j++]);
                printf ("root file option: %s\n", Pars.ROOTFileOption);
#if(HAVE_VXWORKS==0)
                printf ("oppsie... you cannot specify this option unless\n");
                printf ("you have #define HAVE_VXWORKS 1 in GEBSort.h\n");
                printf ("and ahev a VxWorks license, quit\n");
                exit (0);
#endif
              }
            else
              {
                printf ("unknown input option: %s\n", str2);
                printf ("aborting\n");
                fflush (stdout);
                exit (0);
              };

            printf ("\n");

          }
        else if ((p = strstr (argv[j], "-chat")) != NULL)
          {
            j++;
            strcpy (ChatFileName, argv[j++]);
            printf ("will read sorting instructions from chatfile: %s\n", ChatFileName);
            system ("ulimit -a");
            HaveChatFile = 1;
          }
        else if ((p = strstr (argv[j], "-rootfile")) != NULL)
          {
            j++;
            strcpy (Pars.ROOTFile, argv[j++]);
            printf ("rootfile name specified on command line\n");
            printf ("__will store spectra in rootfile: %s\n", Pars.ROOTFile);
            Pars.HaveRootFileName = 1;
            Pars.UseRootFile = 1;
            if ((p = strstr (argv[j], "RECREATE")) != NULL)
              {
                Pars.UpdateRootFile = FALSE;
                printf ("will recreate root file\n");
                sprintf (Pars.ROOTFileOption, "RECREATE");
                j++;
              }
            else if ((p = strstr (argv[j], "UPDATE")) != NULL)
              {
                Pars.UpdateRootFile = TRUE;
                printf ("will update root file\n");
                sprintf (Pars.ROOTFileOption, "UPDATE");
                j++;
              }
            else
              {
                printf (" you must specify RECREATE or UPDATE after -rootfile\n");
                exit (0);
              }
          }
        else if ((p = strstr (argv[j], "-mapfile")) != NULL)
          {
            j++;
            Pars.UseRootFile = 0;
            strcpy (Pars.ShareMemFile, argv[j++]);
            sscanf (argv[j++], "%i", &Pars.SizeShareMemFile);
            printf ("will use shared memory file: %s\n", Pars.ShareMemFile);
            printf ("__of max size: %i bytes\n", Pars.SizeShareMemFile);
            Pars.UseShareMemFile = 1;
            sscanf (argv[j++], "0x%x", &Pars.StartMapAddress);
            printf ("will start shared mem at address: 0x%8.8x\n", Pars.StartMapAddress);
//if(1)exit(0);
          }
        else
          {
            printf ("command line argument not understood!\n");

            printf ("%s: I was called as: \n--->[", argv[0]);
            for (i = 0; i < argc; i++)
              {
                printf ("%s ", argv[i]);
                fflush (stdout);
              }
            printf ("]\non ");
            time_stamp ();
            exit (0);

          }
      };

  /* now start the sorter */
  if (HaveChatFile)
    GEBacq (ChatFileName);
  else
    {
      printf ("you must specify a chat script\n");
      exit (0);
    }

}

/*--------------------------------------------------------------------------*/

void
signal_catcher (int sigval)
{
  int time_stamp ();
  printf ("\nGSSort/GEBacq received signal <%i> on ", sigval);
  time_stamp ();
  Pars.WeWereSignalled = TRUE;
  fflush (stdout);
}

/*---------------------------------------------------------------------------*/

void
sdummyload (Long_t size)
{

  /* dummy load a shared memory to find out what */
  /* start address it chooses................... */

  TMapFile *m;
  m = TMapFile::Create ("dummy.map", "recreate", size);
#if(MAC==1)
  Pars.StartMapAddress = (unsigned long long int) m->GetMmallocDesc ();
#else
  Pars.StartMapAddress = (unsigned long long int) m->GetMmallocDesc ();
#endif
  m->Print ();

  /* close and remove dummy map file */

  m->Close ();
  gSystem->Exec ("\\rm dummy.map");
}

/*---------------------------------------------------------------------------*/

int
GEBSort_read_chat (char *name)
{

  /* declarations */

  FILE *fp, *fp1;
  char *pc, *pc1, str[STRLEN] = { '0' }, str1[STRLEN] =
  {
  '0'}, str2[STRLEN] =
  {
  '0'};
  char str3[STRLEN], str4[STRLEN], str5[STRLEN], str6[STRLEN];
  int nn = 0, nni = 0, st, PType;
  char *p;
  int i, k, i1, i2, i3, i4, i5, i6;
  int j1, j2, j3, j4, j5, j6, j7;
  float f1, f2, f3, f4, pi, r1, r2, rr;
  int echo = 0, nret;
  double d1;

  /* prototypes */

  TCutG *rd2dwin (Char_t *);
  int FindPEvMatNo (char *, int *);
  void FindCondNo (char *, int *);
  int SetFERAVSN (char *, int);
  void InitializeFERAvsntable ();
  void ZeroFERAtypeVSN ();
  void PrintFERATypes ();
  void SetNPosWarn (int);
  void SetRecordVer_tape (int);
  void SetRecordVer_disk (int);
  void SetRecordVer_net (int);
  int str_decomp (char *, int, int *);
  void FindTimeMaskNo (char *, int *);
  int RdOffFile (char *, int *);
  int RdGeCalFile (char *, float *, float *);
  void CheckNoArgs (int, int, char *);
  void SetSpecial (char *str);
  void SetExportModNWords (char *, int);
  void SetlongRaNCHANNELSTDCNWords (char *, int);
  void setIsomerIDs (int);
  void SetClockPar (int, float, float);
  void SetFERAvsntable (int, int);
  void SetnFeraDebug (int);


  /* open chat file */

  if ((fp = fopen (name, "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", name);
      exit (0);
    };
  printf ("chat file: <%s> open\n", name);
  printf ("\n");
  fflush (stdout);

  /* read content and act */

  pc = fgets (str, STRLEN, fp);

  /* rmEndComment(str, STRLEN); */

  while (pc != NULL)
    {
      if (echo)
        printf ("chat->%s", str);
      fflush (stdout);

      /* attemp to interpret the line */

      if ((p = strstr (str, "nevents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &Pars.nEvents);
          CheckNoArgs (nret, 2, str);
          printf ("will sort a max of %i events\n", Pars.nEvents);
          fflush (stdout);

        }
      else if (str[0] == 35)
        {
          /* '#' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 59)
        {
          /* ';' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 10)
        {
          /* empty line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if ((p = strstr (str, "nocrystaltoworldrot")) != NULL)
        {
          nret = sscanf (str, "%s", str1);
          Pars.nocrystaltoworldrot = 1;
          CheckNoArgs (nret, 1, str);
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
      else if ((p = strstr (str, "beamdir")) != NULL)
        {
          nret = sscanf (str, "%s %f %f %f", str1, &Pars.beamdir[0], &Pars.beamdir[1], &Pars.beamdir[2]);
          CheckNoArgs (nret, 4, str);
          rr =
            Pars.beamdir[0] * Pars.beamdir[0] + Pars.beamdir[1] * Pars.beamdir[1] + Pars.beamdir[2] * Pars.beamdir[2];
          rr = sqrtf (rr);
          printf ("will use beam direction %f %f %f\n", Pars.beamdir[0], Pars.beamdir[1], Pars.beamdir[2]);
          printf ("vector length= %f, normalizing\n", rr);
          Pars.beamdir[0] /= rr;
          Pars.beamdir[1] /= rr;
          Pars.beamdir[2] /= rr;
          printf ("will use beam direction %f %f %f\n", Pars.beamdir[0], Pars.beamdir[1], Pars.beamdir[2]);
        }
      else if ((p = strstr (str, "ndetlimits")) != NULL)
        {
          nret = sscanf (str, "%s %i %i", str1, &Pars.ndetlimlo, &Pars.ndetlimhi);
          CheckNoArgs (nret, 3, str);
          printf ("Pars.ndetlimlo=%i,", Pars.ndetlimlo);
          printf ("Pars.ndetlimhi=%i\n", Pars.ndetlimhi);
        }
      else if ((p = strstr (str, "fomlimits")) != NULL)
        {
          nret = sscanf (str, "%s %i %f %f", str1, &i1, &r1, &r2);
          CheckNoArgs (nret, 3, str);
          Pars.fomlo[i1] = r1;
          Pars.fomhi[i1] = r2;
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
      else if ((p = strstr (str, "CCcal")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, str2);
          CheckNoArgs (nret, 2, str);
          fp1 = fopen (str2, "r");
          if (fp1 == NULL)
            {
              printf ("WARNING: could not open CCcal file \"%s\", quit\n", str2);
//              exit (1);
            }
          else
            {
              printf ("reading cal file \"%s\"\n", str2);
              nn = 0;
              st = fscanf (fp1, "%i %f %f", &i1, &r1, &r2);
              while (st == 3)
                {
                  nn++;
                  Pars.CCcal_offset[i1] = r1;
                  Pars.CCcal_gain[i1] = r2;
                  printf ("CC det/offset/gain %3i %6.3f %9.6f\n", i1, Pars.CCcal_offset[i1], Pars.CCcal_gain[i1]);
                  st = fscanf (fp1, "%i %f %f", &i1, &r1, &r2);
                };
              printf ("done, read %i calibration \n", nn);
              fclose (fp1);
            };
        }
      else if ((p = strstr (str, "SEGcal")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, str2);
          CheckNoArgs (nret, 2, str);
          fp1 = fopen (str2, "r");
          if (fp1 == NULL)
            {
              printf ("WARNING: could not open SEGcal file \"%s\", quit\n", str2);
            }
          else
            {
              printf ("reading cal file \"%s\"\n", str2);
              nn = 0;
              st = fscanf (fp1, "%i %i %f %f", &i1, &i2, &r1, &r2);
              while (st == 4)
                {
                  nn++;
                  Pars.SEGcal_offset[i1][i2] = r1;
                  Pars.SEGcal_gain[i1][i2] = r2;
                  printf ("SEG det/seg/offset/gain %3i %2i %6.3f %9.6f\n", i1, i2,
                          Pars.SEGcal_offset[i1][i2], Pars.SEGcal_gain[i1][i2]);
                  st = fscanf (fp1, "%i %i %f %f", &i1, &i2, &r1, &r2);
                };
              printf ("done, read %i calibrations \n", nn);
              fclose (fp1);
            };
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
      else if ((p = strstr (str, "include")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, str2);
          CheckNoArgs (nret, 2, str);
          printf ("will now include chatscript %s\n", str2);
          fflush (stdout);
          GEBSort_read_chat (str2);
          printf ("done including chatscript %s\n", str2);
          fflush (stdout);

        }
      else if ((p = strstr (str, "printevents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &Pars.NumToPrint);
          CheckNoArgs (nret, 2, str);
          printf ("will print details of the first %i events\n", Pars.NumToPrint);
          fflush (stdout);

        }
      else if ((p = strstr (str, "multlims")) != NULL)
        {
          nret = sscanf (str, "%s %i %i", str1, &Pars.multlo, &Pars.multhi);
          CheckNoArgs (nret, 2, str);
          printf ("will require mult bt: %i %i\n", Pars.multlo, Pars.multhi);
          fflush (stdout);
        }
      else if ((p = strstr (str, "beta")) != NULL)
        {
          nret = sscanf (str, "%s %f", str1, &Pars.beta);
          CheckNoArgs (nret, 2, str);
          printf ("will use Pars.Beta (v/c) correction of %f\n", Pars.beta);
          fflush (stdout);
        }
      else if ((p = strstr (str, "DumpEvery")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &Pars.DumpEvery);
          CheckNoArgs (nret, 2, str);
          printf ("will dump to output file every %i minutes\n", Pars.DumpEvery);
          fflush (stdout);

        }
      else if ((p = strstr (str, "egemin")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &egemin);
          CheckNoArgs (nret, 2, str);
          printf ("will require %i minimum germanium signal\n", egemin);
          fflush (stdout);

        }
      else if ((p = strstr (str, "gglen")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &Pars.GGMAX);
          CheckNoArgs (nret, 2, str);
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

int
showStatus ()
{
  printf ("read %i events; ", (Pars.CurEvNo - Pars.firstEvent));
  printf ("Pars.beta=%6.4f; ", (float) Pars.beta);
  printf ("time since last update %i minutes\n", (int) tdmp);
  printf ("CommandFileName=\"%s\"\n", CommandFileName);

#include "UserStat.h"

  /* done */

  fflush (stdout);

  return (0);

};

/*----------------------------------------------------------------------------*/

/* allow for user functions here */

#include "UserFunctions.h"


/*----------------------------------------------------------------------------*/

int
GEBacq (char *ChatFileName)
{

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
  double rn, dtmpehi, d1, nsec;
  DGSHEADER dgsHeader;
  long long int tac, tFP;
  int nFP;
  float r1;
  static int firsttime = 1;
  static long long int t0, TSprev = 0;
  long long int tcur;
  unsigned int typehit[MAX_GEB_TYPE];
  FILE *TSfile;
  TH2F *dtbtev;
  long long int firtsTSinEvent, dTS;
  int dim;
  float rr[LONGLEN + 1];

//  ConnectionRetryCount = 10;


  /* root spectra pointers */

  TH1 *hhtemp;

  TList *zlist;
  TIterator *hiterator;

  TMapFile *mfile;

  TH1D *tmpTH1D = NULL;
  TH2F *tmpTH2F = NULL;



  /* prototypes */

  int GEBSort_read_chat (char *);
  int wr_spe (char *, int *, float *);

  /* data type binners sup==setup, bin_==binner */

  int sup_mode2 ();
  int sup_mode1 ();
  int sup_mode3 ();
  int sup_gtcal ();
  int sup_dgs ();
  int sup_rcnp ();
  int bin_mode1 (GEB_EVENT *);
  int bin_mode2 (GEB_EVENT *);
  int bin_mode3 (GEB_EVENT *);
  int bin_gtcal (GEB_EVENT *);
  int bin_dgs (GEB_EVENT *);
  int bin_rcnp (GEB_EVENT *);
  int exit_rcnp ();

  /* allow user to declare variables here */

  printf ("\n");
  printf ("executing UserDeclare.h code\n");
  printf ("\n");

#include "UserDeclare.h"

  /*-------*/
  /* Hello */
  /*-------*/

  printf ("\n");
  printf ("GEBsort running on: ");
  time_stamp ();
  printf ("$Id: GEBSort.cxx,v 1.1 2014/02/12 16:32:11 tl Exp tl $\n");
  printf ("\n");

  /*------------*/
  /* initialize */
  /*------------*/

  for (i = 0; i < MAX_GEB_TYPE; i++)
    typehit[i] = 0;

  printf ("\n");
  printf ("initializing\n");
  printf ("\n");
  fflush (stdout);

  Pars.nEvents = 2000000000;
  Pars.WeWereSignalled = FALSE; /* signal  */
  Pars.UseRootFile = FALSE;
  Pars.SizeShareMemFile = FALSE;
  Pars.spname[STRLEN];
  Pars.firstEvent = 0;
  Pars.GSudpPort = 1101;
  Pars.NumToPrint = 25;
  Pars.DumpEvery = 10;
  Pars.dTS = 500;
  Pars.nbytes = 0;
  Pars.beta = 0;
  Pars.modwrite = 1000;
  Pars.tsnumwrites = 100;
  Pars.nocrystaltoworldrot = 0;
  Pars.multlo = 1;
  Pars.multhi = 20;

  for (i = 0; i < MAXGEBS; i++)
    {
      GEB_event.ptgd[i] = (GEBDATA *) calloc (2 * sizeof (GEBDATA), 1);
      GEB_event.ptinp[i] = (CRYS_INTPTS *) calloc (MAXPAYLOADSIZE, 1);
    };

  rbuf = (char *) calloc (RBUFSIZE + 1, 1);

  /* get the rotation matrices */

  sprintf (str, "crmat.LINUX");
  in = open (str, O_RDONLY, 0);
  if (in > 0)
    printf ("%s is open (input) binary format\n", str);
  else
    {
      printf ("could not open %s\n", str);
      //  exit (1);
    };
  siz = read (in, (char *) Pars.crmat, sizeof (Pars.crmat));
  printf ("read %i bytes into Pars.crmat\n", siz);
  close (in);

  /*------------------*/
  /* read chat script */
  /*------------------*/

  GEBSort_read_chat (ChatFileName);

  printf ("checking proper input of chat file...\n");
  if (Pars.InputSrc == NOTDEF)
    {
      printf ("you must specify an input source\n");
      exit (1);
    }
  else if (Pars.InputSrc == DISK)
    {

#if(USEZLIB==0)
      /* attempt to open input file */

      inData = open (Pars.GTSortInputFile, O_RDONLY, 0);
      if (inData == -1)
        {
          printf ("could not open\"%s\"; quit\n", Pars.GTSortInputFile);
          exit (1);
        }
      else
        printf ("input file \"%s\" is open, inData=%i\n", Pars.GTSortInputFile, inData);

      /* find the very first GEB header to find start TS */

      siz = read (inData, (char *) GEB_event.ptgd[0], sizeof (GEBDATA));
#if (1)
      printf ("siz=%i;", siz);
      printf ("ptgd[i]->type=%2i; ", GEB_event.ptgd[0]->type);
      printf ("ptgd[i]->length=%4i; ", GEB_event.ptgd[0]->length);
      printf ("ptgd[i]->timestamp=%lli\n", GEB_event.ptgd[0]->timestamp);
      fflush (stdout);
#endif
      Pars.curTS = GEB_event.ptgd[0]->timestamp;
      printf ("start TS is %lli\n", Pars.curTS);

      /* reopen */

      close (inData);
      inData = open (Pars.GTSortInputFile, O_RDONLY, 0);
      printf ("reopened input file, inData=%i \n", inData);

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


    }
  else if (Pars.InputSrc == GEB)
    {

#if(HAVE_VXWORKS)
      printf ("will take input from GEB with these parameters:\n");
      printf ("Pars.pHost=%s; ", Pars.pHost);
      printf ("Pars.grouping=%i; ", Pars.grouping);
      printf ("Pars.type=%i; ", Pars.type);
      printf ("Pars.timeout=%f\n", Pars.timeout);

      printf ("connecting to tap\n");

      for (i = 0; i < ConnectionRetryCount; i++)
        {
#if(1)
          printf ("connecting, %i/%i, %i\n", i, ConnectionRetryCount, ConnectionRetryWait / 1000);
          fflush (stdout);
#endif
          pTap = gretTapConnect (Pars.pHost, GRETTAP_GEB, Pars.type);
          if (pTap || (gretTapClientError != GTC_TAPCONN))
            {
              /* Either success or a failure other than connection failure */
              printf ("got here, that is good, break out and go on\n");

              break;
            }
          usleep (ConnectionRetryWait);

#if(DEBUG)
          fprintf (stderr, "Retry number %d\n", i);
          fflush (stderr);
#endif

        }
      fprintf (stderr, "Retries: %d\n", i);
      fflush (stderr);
      if (!pTap)
        {
          fprintf (stderr, "Unable to connect to tap server at %s : %s\n", Pars.pHost,
                   gretTapClientErrorStrings[gretTapClientError]);
          exit (1);
        };
#endif
    }
  else
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


  if (Pars.UseShareMemFile && Pars.UseRootFile)
    {
      printf ("you cannot use shared memory and a root disk\n");
      printf ("at the same time!\n");
      exit (1);
    };

  /* force user to declare intension with root file */
  /* so I can't be blamed for any overwrites!!      */

  if (!Pars.UseShareMemFile)
    if ((p = strstr (Pars.ROOTFileOption, "UNDEFINED")) != NULL)
      {
        printf ("for root files you must specify either:\n");
        printf ("\n");
        printf ("    ROOTFileOption RECREATE\n");
        printf ("or\n");
        printf ("    ROOTFileOption UPDATE\n");
        printf ("\n");
        printf ("please modify your chat file and try again\n");
        exit (-1);
      };

  /*------------------------*/
  /* execute user init code */
  /*------------------------*/

  printf ("\n");
  printf ("executing ShareMemFileUserInit.h code\n");
  printf ("\n");

#include "UserInit.h"

  /*-----------------------------------*/
  /* if we are going to use the        */
  /* shared map file, create it!       */
  /* be careful about the map address! */
  /*-----------------------------------*/

  if (Pars.UseShareMemFile)
    {
      printf ("\n");

      if (Pars.StartMapAddress != 0)
        {
          TMapFile::SetMapAddress ((Long_t) Pars.StartMapAddress);
          printf ("shared mem start address set to 0x%8.8x\n", Pars.StartMapAddress);
        }
      else
        printf ("will use system default for shared mem start address\n");

      mfile = TMapFile::Create (Pars.ShareMemFile, "RECREATE", Pars.SizeShareMemFile, "GS shared mem");
      if (mfile == NULL)
        {
          printf ("failed to create TMapFile\n");
          exit (-1);
        };

      printf ("shared memory [%s] created, size: %i bytes\n", Pars.ShareMemFile, Pars.ShareMemFile);
      fflush (stdout);
      mfile->Print ();
      printf ("\n");

    };


  NprintEvNo = 0;
  Pars.CurEvNo = 0;

  if (!Pars.ShareMemFile)
    {
      Pars.DumpEvery = 2000000000;
      printf ("\n");
      printf ("_since rootfile: setting `Pars.DumpEvery` to infinity..!\n");
      printf ("\n");
    };

  /* delete any command file */

  sprintf (str, "\\rm -f %s", CommandFileName);
  system (str);
  printf ("deleted %s\n", str);

  /*------------------------------------------*/
  /* if we are using root file, then either   */
  /* read in old rootfile or create a nev one */
  /*------------------------------------------*/


  if (!Pars.UseShareMemFile)
    if (Pars.UpdateRootFile)
      {

        /* check here whether the old root file exists */

        fp = fopen (Pars.ROOTFile, "r");
        if (fp == NULL)
          {
            printf ("could not open old rootfile: %s\n", Pars.ROOTFile);
            printf ("the old rootfile must exist if you  \n");
            printf ("want to use the UPDATE option\n");
            printf ("aborting...\n");
            exit (0);
          };
        fclose (fp);

        /* read in old root file */

        Pars.f1 = NULL;
        Pars.f1 = new TFile (Pars.ROOTFile, "UPDATE");
        printf ("read old root file <%s>\n", Pars.ROOTFile);
        if (!Pars.f1->IsOpen ())
          {
            printf ("could not open file....\n\n");
            exit (-1);
          };
        printf ("base=<%s>\n", Pars.f1->GetPath ());
        Pars.f1->Print ();

      }
    else
      {
        /* create the rootfile */

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
      };

  printf ("\n");
  printf ("executing UserInit.h code\n");
  printf ("\n");
#include "UserInit.h"

  /* update shared mem with minimal info       */
  /* so it is not empty before the first update */
  /* shared memory wellness checkpoint          */

  if (Pars.UseShareMemFile)
    {
      printf ("\n");
      printf ("Note: if the command below fails,\n");
      printf ("increase the shared memory size!\n");
      printf ("\n");
      printf ("updating empty shared mem file... ");
      fflush (stdout);
//      mfile->Update ();
      UPDSSHMEM;
      printf ("Done!\n");
      printf ("\n");
      fflush (stdout);
    };

  TSfile = fopen ("TS.list", "w");

  /*--------------------------------*/
  /* setup the root spectra we need */
  /*--------------------------------*/

  /* spectra that are always there */

  sprintf (str1, "dtbtev");
  sprintf (str2, "dtbtev");
  dtbtev = mkTH2F (str1, str2, DTBTEVLEN / 2, 0, DTBTEVLEN, MAX_GEB_TYPE, 1, MAX_GEB_TYPE);
  sprintf (str1, "delta t");
  dtbtev->SetXTitle (str1);
  sprintf (str1, "type");
  dtbtev->SetYTitle (str1);

  /* spectra for different types of data */

  //sup_mode2 ();
  //sup_mode1 ();
  //sup_mode3 ();
  //sup_gtcal ();
  //sup_dgs ();
  sup_rcnp ();

  printf ("we have defined the following ROOT spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  // Pars.wlist->Print ();

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
  if (Pars.InputSrc == DISK)
    printf ("from disk...\n");
  else if (Pars.InputSrc == GEB)
    {
      printf ("from GEB...\n");
    }
  else if (Pars.InputSrc == NET)
    {
      printf ("from net... SHOULD NOT HAPPEN\n");
      exit (1);
    };
  printf ("\n");
  fflush (stdout);

  tdmplast = time (NULL);
  /* start of LOOP */
  while (st >= 0 && (Pars.CurEvNo - Pars.firstEvent) < Pars.nEvents && eov == 0)
    {

      /* zap [this may be too safe and slow...; yes it is] */

      //memset ((char *) &CEvent, 0, sizeof (COINEV));

      /*----------------*/
      /* get next event */
      /*----------------*/

#if(DEBUG2)
      printf ("calling GEBGetEv, Pars.CurEvNo=%i\n", Pars.CurEvNo);
#endif
      st = GEBGetEv (&GEB_event, Pars.CurEvNo);

      if (st == 0 && Pars.CurEvNo < Pars.tsnumwrites)
        {
          for (i = 0; i < GEB_event.mult; i++)
            {
              if (i == 0)
                fprintf (TSfile, "\n");
              GebTypeStr (GEB_event.ptgd[i]->type, str);
           

	      //   fprintf (TSfile, "%4i/%2i: (%2i,%s) TS=%20lli; ",
	      //Pars.CurEvNo, i, GEB_event.ptgd[i]->type, str,
              //         GEB_event.ptgd[i]->timestamp);
	  // fprintf (TSfile, "dT=%lli\n", GEB_event.ptgd[i]->timestamp - TSprev);
              TSprev = GEB_event.ptgd[i]->timestamp;
            }

        };

#if(DEBUG2)
      printf ("st=%i\n", st);
      printf ("GEB_event.mult=%i\n", GEB_event.mult);
      fflush (stdout);
      if (1)
        exit (0);
#endif

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
          for (i = 0; i < GEB_event.mult; i++)
            if (GEB_event.ptgd[i]->timestamp < firtsTSinEvent)
              firtsTSinEvent = GEB_event.ptgd[i]->timestamp;

          for (i = 0; i < GEB_event.mult; i++)
            {
              dTS = GEB_event.ptgd[i]->timestamp - firtsTSinEvent;
              d1 = (double) dTS;
              if (d1 >= (double) 0 && d1 < RATELEN)
                dtbtev->Fill (d1, GEB_event.ptgd[i]->type, 1);
            };
        };

      /*----------------------------------------*/
      /* allow user to manipulate raw data here */
      /*----------------------------------------*/

      if (st != 0)
        {
          printf (" GEBGetEv returned %i\n", st);
          printf ("we have read %i bytes; ", Pars.nbytes);
          printf ("CurEvNo=%i\n", Pars.CurEvNo);
          fflush (stdout);

          /* terminate sort */

          eov = 1;

          /* note: */
          /* we might want to wait and try GEBGetEv */
          /* later to give the impresssion of interactivity */
          /* here in some future version... */

        }

#include "UserRawEv.h"

//    assert (Pars.InputSrc == DISK);

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
              printf ("we have read %i bytes; ", Pars.nbytes);
              printf ("CurEvNo=%i\n", Pars.CurEvNo);
              fflush (stdout);
            };

#include "UserGoodEv.h"


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

          if (0)
            {
              printf ("debug quit\n");
              exit (0);
            };

#include "UserPreCond.h"

          /* bin GT mode 3 data  (== raw data with traces) */

          //bin_mode3 (&GEB_event);

          /* bin GT mode 2 data  (== decomposed data) */

          //bin_mode2 (&GEB_event);

          /* bin mode 1 data (==tracked data) */

          //bin_mode1 (&GEB_event);

          /* bin DGS data */

	  //          bin_dgs (&GEB_event);

          /* bin GT data for calibration */

          //bin_gtcal (&GEB_event);

          /* bin other stuff in rcnp */

          bin_rcnp (&GEB_event);
extern TTree *tree;
	  tree->Fill();
      //printf("Bytes comitted to tree[%i]: %i\n", Pars.CurEvNo, tree->Fill());
	/*-------------------------*/
          /* execute user event code */
      /*-------------------------*/
#include "UserEv.h"

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

    /*-----------------------------------------------------------*/
      /* dump all spectra on signal or dump every Pars.DumpEvery events */
      /* or respond to 'interactive' command...................... */
    /*-----------------------------------------------------------*/

      if (Pars.WeWereSignalled || (int) tdmp >= Pars.DumpEvery)
        {

          /* disarm signal */

          Pars.WeWereSignalled = FALSE;

          /* check for command file */

          fp = fopen (CommandFileName, "r");
          if (fp != NULL)
            {

              printf ("found command file: %s\n", CommandFileName);
              fgets (str, STRLEN, fp);
              printf ("with command: %s\n", str);

              if ((p = strstr (str, "DumpEvery")) != NULL)
                {

                  sscanf (str, "%s %i", str1, &Pars.DumpEvery);
                  printf ("will dump to output file every %i minutes\n", Pars.DumpEvery);
                  fflush (stdout);

                }
              else if ((p = strstr (str, "printevents")) != NULL)
                {
                  /* reset print event counter */

                  nret = sscanf (str, "%s %i", str1, &i1);
                  if (nret == 2)
                    Pars.NumToPrint = i1;
                  printf ("will print %i events\n", Pars.NumToPrint);
                  NprintEvNo = 0;

                }
              else if ((p = strstr (str, "status")) != NULL)
                {

                  showStatus ();


                }
              else if ((p = strstr (str, "stopsort")) != NULL)
                {
                  /* simulate end of event to stop sort */

                  eov = 1;

                }
              else if ((p = strstr (str, "zapall")) != NULL)
                {

                  /* zap spectra */
                  if (Pars.UseShareMemFile)
                    {
                      zlist = mfile->GetDirectory ()->GetList ();
                      hiterator = zlist->MakeIterator ();
                      while (hhtemp = (TH1 *) hiterator->Next ())
                        {
                          hhtemp->Reset ();
                        }
                      printf ("all spectra were zapped @ ");
                      time_stamp ();
                      fflush (stdout);

                      /* update */

                      printf ("updating shared mem... ");
                      UPDSSHMEM;
                    }
                  else
                    {
                      /* do nothing */
                    }
                }
              else if ((p = strstr (str, "zap")) != NULL)
                {
                  /* extract spectrum name */

                  sscanf (str, "%s %s", str1, Pars.spname);
                  hhtemp = (TH1D *) gROOT->FindObject (Pars.spname);
                  if (Pars.UseShareMemFile)
                    {
                      hhtemp = (TH1 *) mfile->Remove (Pars.spname);
                      if (hhtemp != NULL)
                        {
                          hhtemp->Print ();
                          hhtemp->Reset ();
                          mfile->Add (hhtemp, Pars.spname);
                          mfile->Update (hhtemp);
                        }
                      printf ("spectrum %s zapped @ ", Pars.spname);
                      time_stamp ();
                      fflush (stdout);
                      /* update */
                    }
                  else
                    {
                      /* do nothing */
                    };

                }
              else
                printf ("command not understood\n");

              /* delete command file */

              fclose (fp);
              sprintf (str, "\\rm %s", CommandFileName);
              system (str);
              printf ("%s\n", str);

            }
          else
            {

              printf ("\"%s\" was not found\n", CommandFileName);

              /* update sh mem or writeout root file */

              printf ("time since last dump: %i minute(s)\n", (int) tdmp);
              tdmp = 0;
              if (!Pars.UseShareMemFile)
                {

                  printf ("*---------------------------------\n");
                  printf ("* you cannot update a disk file.  \n");
                  printf ("  you must wait for sort to finish\n");
                  printf ("  or stop the sort! Ignoring you...\n");
                  printf ("*---------------------------------\n");

                }
              else
                {
                  printf ("updating shared mem... ");

                  UPDSSHMEM;
                  showStatus ();
                };

              tdmplast = time (NULL);

            };

          printf ("continuing the sort...\n");
          fflush (stdout);

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

  if (Pars.InputSrc == DISK)
    {
      close (inData);
    }
  else if (Pars.InputSrc == GEB)
    {
#if(HAVE_VXWORKS)
      gretTapClose (pTap);
#endif
    };
  printf ("\n");
  fflush (stdout);

 printf ("stat 1 \n");

  /* write the tracked spectrum in spe format as well */

#if(0)
  dim = LONGLEN;
  for (i = 0; i < dim; i++)
    {
      rr[i] = (float) sumTrackE->GetBinContent (i);;
    };
  sprintf (str, "sumTrackE.spe");
  wr_spe (str, &dim, rr);
#endif
 printf ("stat 2 \n");
  /* if we were using shared memory */

  if (Pars.UseShareMemFile)
    {
      UPDSSHMEM mfile->Print ();
      printf ("\n");
      mfile->ls ();
      printf ("\n");
    };

  /* if we were using rootfile */

  if (!Pars.UseShareMemFile)
    {
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
    }
  printf ("\n");


  /*-------------------------*/
  /* print simple statistics */
  /*-------------------------*/

  showStatus ();

#include "UserExit.h"

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
