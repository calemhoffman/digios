
// This code is for the special calibration taks for GRETINA
// and the code is maintained by Shaofei Zhu

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


/* pointers to ROOT spectra */

//TH1D *ehi_sum_mode3;
//TH2F *SegE;

/* parameters */

extern PARS Pars;



/* ----------------------------------------------------------------- */

int
sup_gtcal ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i, i1;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* initialize */


  /* define spectra */

//  sprintf (str1, "ehi_sum_mode3");
//  sprintf (str2, "ehi_sum_mode3");
//  ehi_sum_mode3 = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
//  ehi_sum_mode3->SetXTitle (str1);

}

/* ----------------------------------------------------------------- */

int
bin_gtcal (GEB_EVENT * GEB_event)
{

  /* declarations */

  int i;
  char str[128];
  CRYS_INTPTS *ptinp;
  GEBDATA *ptgd;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

  /* prototypes */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_mode3:\n");

  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_RAW)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_gtcal, %2i> %2i, %s, TS=%lli, 0x%llx\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp, GEB_event->ptgd[i]->timestamp);
            }

          /* Shaofei, put your code here */

        };

    };

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_gtcal\n");

  return (0);

}
