#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"
#include "gdecomp.h"

#include "GEBSort.h"
#include "GTMerge.h"

#define MAXINT   2147483647

/* pointers to ROOT spectra */

TH1D *ehi_sum_mode3;
TH2F *SegE;

/* parameters */

extern PARS Pars;



/* ----------------------------------------------------------------- */

int
sup_mode3 ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i, i1;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* initialize */


  /* define spectra */

  sprintf (str1, "ehi_sum_mode3");
  sprintf (str2, "ehi_sum_mode3");
  ehi_sum_mode3 = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  ehi_sum_mode3->SetXTitle (str1);

  sprintf (str1, "SegE");
  sprintf (str2, "SegE");
  SegE = mkTH2F (str1, str2, MAXSEGNO, 1, MAXSEGNO, SHORTLEN, 1, LONGLEN);
  SegE->SetXTitle ("Crystal#*36+seg#");
  SegE->SetYTitle ("segment energy");

  /* list what we have */

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();


}

/* ----------------------------------------------------------------- */

int
bin_mode3 (GEB_EVENT * GEB_event)
{

  /* declarations */

  int i, j, i1, crystalno, moduleno, detno, pos, nseghits = 0;
  float sX, sY, polAng, aziAng, rr, xx, yy, zz;
  double d1;
  char str[128];
  int GebTypeStr (int type, char str[]);
  float detDopFac, dp;
  float RAD2DEG = 0.0174532925;
  static int firsttime = 1;
  static long long int t0;
  float polang[MAX_GAMMA_RAYS];
  float doppler_factor[MAX_GAMMA_RAYS];
  unsigned int *testPattern;
  static int nBadTestPat = 0;
  unsigned short int *i2;
  GTEVENT Event;
  unsigned short int t1, t2;

  CRYS_INTPTS *ptinp;
  GEBDATA *ptgd;

  /* prototypes */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_mode3 @ event number: %i\n", Pars.CurEvNo);

  nseghits = 0;
  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_RAW)
        {
          nseghits++;

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_mode3, %2i> %2i, %s, TS=%lli, 0x%llx\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp, GEB_event->ptgd[i]->timestamp);
            }

          testPattern = (unsigned int *) GEB_event->ptinp[i];
          if (*testPattern != EOE)
            {
              nBadTestPat++;

              if (nBadTestPat == 10)
                printf ("** suspending warnings about bad EOE markers...\n");

              if (nBadTestPat < 10)
                {
                  printf ("ooops: testPattern=%8.8x after event # %lld ", *testPattern, Pars.CurEvNo);
                  fflush (stdout);
                  exit (1);
                };

            }

          /* read the header */

          i2 = (unsigned short int *) (testPattern + 1);
          for (i = 0; i < HDRLENWORDS; i++)
            Event.hdr[i] = *(i2 + i);

          /* swap bytes */

          for (i = 0; i < HDRLENWORDS; i++)
            {
              t1 = (Event.hdr[i] & 0x00ff) << 8;
              t2 = (Event.hdr[i] & 0xff00) >> 8;
              Event.hdr[i] = t1 + t2;
            };

          if (Pars.CurEvNo <= Pars.NumToPrint)
            for (i = 0; i < HDRLENWORDS; i++)
              printf ("Event.hdr[%2i]=0x%4.4x, %6i\n", i, Event.hdr[i], Event.hdr[i]);

          Event.len = 4 * (Event.hdr[0] & 0x7ff) + 4;
          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("Event.len=%i\n", Event.len);

          Event.traceLen = Event.len - HDRLENBYTES - 4;
          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("Event.len=%i, Event.traceLen=%i\n", Event.len, Event.traceLen);

          /* copy trace */

          for (i = 0; i < (Event.traceLen / 2); i++)
            Event.trace[i] = *(i2 + i + HDRLENWORDS);

          /* swap bytes */

          for (i = 0; i < Event.traceLen / 2; i++)
            {
              t1 = (Event.trace[i] & 0x00ff) << 8;
              t2 = (Event.trace[i] & 0xff00) >> 8;
              Event.trace[i] = t1 + t2;
            };

          /* extract IDs */

          Event.chan_id = (Event.hdr[1] & 0xf);
          Event.board_id = (Event.hdr[1] >> 4);
          Event.id = (Event.board_id - (37 - 1) * 4) * 10 + Event.chan_id;

          /* extract high energy, per documentation */
          /* I do not understand what happens here!? */

          bool sign = 0;
          sign = (Event.hdr[7] & 0x0100);

          Event.rawE = (int) ((Event.hdr[7] & 0x00ff) << 16) + (int) Event.hdr[4];

          if (Event.rawE >= 0 && Event.rawE <= MAXINT)
            {
              if (sign)
                {
                  if ((((Event.id == CC_ID1)) || ((Event.id == CC_ID2)))
                      || ((((Event.id - CC_ID1) % 40) == 0) || (((Event.id - CC_ID2) % 40) == 0)))
                    {
                      Event.rawE = (int) ((Event.rawE) - (int) 0x01000000);
                      //      cout << Event.rawE << endl;
                    }
                  else
                    {
                      Event.rawE = (int) ((Event.rawE) - (int) 0x01000000);
                      Event.rawE = -(int) (Event.rawE);
                      //      cout << "segment: " << Event.id << endl;
                    }
                }
              else
                {
                  if ((((Event.id == CC_ID1) || (Event.id == CC_ID2)))
                      || ((((Event.id - CC_ID1) % 40) == 0) || (((Event.id - CC_ID2) % 40) == 0)))
                    {
                      Event.rawE = (int) (Event.rawE);
                      //       cout << Event.rawE << endl;
                    }
                  else
                    {
                      Event.rawE = -(int) (Event.rawE);
                      //      cout  << Event.id << "\t" << Event.rawE << endl;
                    }
                }
            }
          else
            Event.rawE = MAXINT;

//  Event.ehi = abs(Event.rawE)/64;
          Event.ehi = abs (Event.rawE / 320);   // slower but cannot bit-shift an int

          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("Event.ehi=%i\n", Event.ehi);

          /* bin some data */

          if (Event.ehi > 10 && Event.ehi < LONGLEN)
            {

            ehi_sum_mode3->Fill ((double) Event.ehi, 1);

            /* unsuppressed segment energy spectra */

            if (Event.id > 0 && Event.id < MAXSEGNO)
              SegE->Fill ((double) Event.id, (double) Event.ehi);

            };


          /* extract LED external time, per documentation */

          Event.LEDts = (unsigned long long int) Event.hdr[3] +
            ((unsigned long long int) Event.hdr[2] << 16) + ((unsigned long long int) Event.hdr[5] << 32);
#if(0)
          printf ("\n");
          printf ("hdr[ 3]=0x%4.4x, ", Event.hdr[3]);
          printf ("hdr[ 2]=0x%4.4x ", Event.hdr[2]);
          printf ("hdr[ 5]=0x%4.4x ", Event.hdr[5]);
#endif

          /* extract CFD external time, per documentation */

          Event.CFDts = (unsigned long long int) Event.hdr[6] +
            ((unsigned long long int) Event.hdr[9] << 16) + ((unsigned long long int) Event.hdr[8] << 32);
#if(0)
          printf ("hdr[ 8]=0x%4.4x ", Event.hdr[8]);
          printf ("hdr[ 9]=0x%4.4x ", Event.hdr[9]);
          printf ("hdr[ 6]=0x%4.4x, ", Event.hdr[6]);
#endif

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("ev %8i> ", Pars.CurEvNo);
              printf ("board= %8i; ", Event.board_id);
              printf ("chan= %8i; ", Event.chan_id);
              printf ("id= %8i; ", Event.id);
              printf ("e= %6i; ", Event.ehi);
              printf ("\n");
              /* print a little of the trace */

              printf ("tr: ");
              for (i = 0; i < 3; i++)
                printf ("%6i ", Event.trace[i]);
              printf ("...");
              for (i = Event.traceLen / 2 - 3; i < Event.traceLen / 2; i++)
                printf ("%6i ", Event.trace[i]);
              printf (";\n");

              /* time stamps */

              printf ("__LEDts=0x%8.8x|0x%8.8x; ", (unsigned int) (Event.LEDts >> 32),
                      (unsigned int) (Event.LEDts & 0xffffffff));
              printf ("CFDts=0x%8.8x|%8.8x\n", (unsigned int) (Event.CFDts >> 32),
                      (unsigned int) (Event.CFDts & 0xffffffff));

              /* bits */

//  printf("__pu=%i, ", Event.pu);
//  printf("CFDvalid=%i, ", Event.CFDvalid);
//  printf("ExtTrig=%i, ", Event.ExtTrig);
//  printf("LEDsign=%i\n", Event.LEDsign);

            };

        };


    };

#if(0)
  if (nseghits == 1)
    {

      /* update suppressed segment energy matrix */

      /* only update if the events multiplicity is one so  */
      /* that neighboring crystals suppess. Also only update  */
      /* if just one segment fired, so the other segments  */
      /* effectively suppress. */

      if (Event.ehi > 10 && Event.ehi < LONGLEN)
        if (Event.id > 0 && Event.id < MAXSEGNO)
          SegE->Fill ((double) Event.id, (double) Event.ehi);
    };
#endif

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_mode3\n");

//  if (1) exit (0);

  return (0);

}

