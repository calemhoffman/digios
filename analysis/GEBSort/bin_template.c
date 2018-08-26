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


typedef struct PAYLOAD
{
  char p[MAXDATASIZE];
} PAYLOAD;

typedef struct TRACK_STRUCT
{
  int n;
  GEBDATA *gd;
  PAYLOAD *payload;
} TRACK_STRUCT;


/* pointers to ROOT spectra */


/* parameters */

extern PARS Pars;


/*-----------------------------------------------------*/

int
sup_template ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* list what we have */

  printf (" we have define the following spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  Pars.wlist->Print ();

};

/* ----------------------------------------------------------------- */

int
bin_template (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int i;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

if(1)return(0);

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_template:\n");


  /* loop through the coincidence event and fish out GEB_TYPE_DGS data */

  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_DGS)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }

        };

    };

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_template\n");

  return (0);

}
