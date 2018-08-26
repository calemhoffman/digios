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

#define MAXH 50
#define MAXK 40

/* pointers to ROOT spectra */


TH2F *SMAP_firsthits;
TH1D *fmsp;
TH1D *gmult;
TH1D *sumTrackE;
TH2F *fomXe;
TH2F *HK;
TH1D *rate_mode1;
TH2F *gg;
TH2F *ndet_e;
TH2F *rad_e;

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



/* parameters */

extern PARS Pars;

/*-----------------------------------------------------*/

int
print_tracked_gamma_rays (FILE * fp, TRACKED_GAMMA_HIT * grh)
{

/* declarations */

  int j;
  float r1;

  fprintf (fp, "number of gamma rays: %i\n", grh->ngam);
  for (j = 0; j < grh->ngam; j++)
    {
      fprintf (fp, "esum=%6.3f, ", grh->gr[j].esum);
      fprintf (fp, "ndet (#interactions)=%2i, ", grh->gr[j].ndet);
      fprintf (fp, "fom=%6.3f, ", grh->gr[j].fom);
      fprintf (fp, "tracked=%i\n", grh->gr[j].tracked);
      fprintf (fp, "1'th hit: (%6.3f, %6.3f, %6.3f), e= %6.1f keV\n", grh->gr[j].x0, grh->gr[j].y0, grh->gr[j].z0,
               grh->gr[j].e0);
      fprintf (fp, "2'nd hit: (%6.3f, %6.3f, %6.3f), e= %6.1f keV\n", grh->gr[j].x1, grh->gr[j].y1, grh->gr[j].z1,
               grh->gr[j].e1);
      r1 = grh->gr[j].x0 * grh->gr[j].x0 + grh->gr[j].y0 * grh->gr[j].y0 + grh->gr[j].z0 * grh->gr[j].z0;
      r1 = sqrtf (r1);
      fprintf (fp, "1'th interaction radius: %9.1f mm\n", r1);
    };

  /* done */

  return (0);

};

/*-----------------------------------------------------*/

int
sup_mode1 ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i;
  unsigned int seed;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);
  int get_a_seed (unsigned int *);

  /* define spectra */

  sprintf (str1, "SMAP_firsthits");
  sprintf (str2, "SMAP_firsthits");
  SMAP_firsthits = mkTH2F (str1, str2, 256, 0, 360, 256, 0, 180);
  sprintf (str1, "horizontal");
  SMAP_firsthits->SetXTitle (str1);
  sprintf (str1, "vertical");
  SMAP_firsthits->SetYTitle (str1);

  sprintf (str1, "fm");
  sprintf (str2, "figure of merit");
  fmsp = mkTH1D (str1, str2, 1024, 0, 2.1);
  fmsp->SetXTitle (str1);

  sprintf (str1, "gmult");
  sprintf (str2, "tracked gamma ray multiplicity");
  gmult = mkTH1D (str1, str2, 21, 0, 20);
  gmult->SetXTitle (str1);

  sprintf (str1, "sumTrackE");
  sumTrackE = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
  sumTrackE->SetXTitle (str1);

  sprintf (str1, "fomXe");
  sprintf (str2, "fomXe");
  fomXe = mkTH2F (str1, str2, LONGLEN, 1, LONGLEN, 200, 0, 2.0);
  sprintf (str1, "e");
  fomXe->SetXTitle (str1);
  sprintf (str1, "fom");
  fomXe->SetYTitle (str1);

  sprintf (str1, "HK");
  sprintf (str2, "HK");
  HK = mkTH2F (str1, str2, 1024, 0, MAXK, 1024, 0, MAXH);
  sprintf (str1, "K, # gamma rays");
  HK->SetXTitle (str1);
  sprintf (str1, "H, summed energy");
  HK->SetYTitle (str1);

  sprintf (str1, "rate_mode1");
  sprintf (str2, "rate_mode1");
  rate_mode1 = mkTH1D (str1, str2, RATELEN, 0, RATELEN);
  rate_mode1->SetXTitle (str1);

  sprintf (str1, "gg");
  sprintf (str2, "gg");
  gg = mkTH2F (str1, str2, Pars.GGMAX, 1, Pars.GGMAX, Pars.GGMAX, 1, Pars.GGMAX);
  sprintf (str1, "g1");
  gg->SetXTitle (str1);
  sprintf (str1, "g2");
  gg->SetYTitle (str1);

  sprintf (str1, "ndet_e");
  sprintf (str2, "ndet_e");
  ndet_e = mkTH2F (str1, str2, 9, 1, 10, Pars.GGMAX, 1, (double) Pars.GGMAX);
  sprintf (str1, "ndet");
  ndet_e->SetXTitle (str1);
  sprintf (str1, "e");
  ndet_e->SetYTitle (str1);

  sprintf (str1, "rad_e");
  sprintf (str2, "rad_e");
  rad_e = mkTH2F (str1, str2, 2048, 0, 300, Pars.GGMAX, 1, Pars.GGMAX);
  sprintf (str1, "radius");
  rad_e->SetXTitle (str1);
  sprintf (str1, "E (keV)");
  rad_e->SetYTitle (str1);

  /* list what we have */

//  printf (" we have define the following spectra:\n");

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();

  get_a_seed (&seed);
  srand (seed);

};

/* ----------------------------------------------------------------- */

int
bin_mode1 (GEB_EVENT * GEB_event)
{

  /* declarations */

  int k, l, i, j, crystalno, moduleno, detno, ngrh;
  float sX, sY, polAng, aziAng, rr, xx, yy, zz, dp, rn;
  double d1, d2;
  float RAD2DEG = 0.01745329;
  char str[128];
  float detDopFac;
  TRACKED_GAMMA_HIT *grh;
  PAYLOAD *ptinp;
  GEBDATA *ptgd;
  static long long int t0;
  static int firsttime = 1;
  float polang[MAX_GAMMA_RAYS];
  float doppler_factor[MAX_GAMMA_RAYS];

  /* prototypes */

  int GebTypeStr (int type, char str[]);
  float findAzimuthFromCartesian (float, float, float);
  float findPolarFromCartesian (float, float, float, float *);


  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("--------------------\nentered bin_mode1:\n");

  /* find the multiplicity  */

  ngrh = 0;
  for (i = 0; i < GEB_event->mult; i++)
    if (GEB_event->ptgd[i]->type == GEB_TYPE_TRACK)
      ngrh++;
  gmult->Fill(ngrh,1);

  /* quit if multiplicity is not OK */

  if (ngrh < Pars.multlo || ngrh > Pars.multhi)
    return (0);
  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf("mult= %i is OK\n", ngrh);

  /* process mode 1 data */

  ngrh = 0;
  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_TRACK)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_mode1, %2i> %2i, %s, TS=%lli;\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }

          /* mode 1 rate spectrum, x=minute, y=Hz */

          if (firsttime)
            {
              firsttime = 0;
              t0 = GEB_event->ptgd[i]->timestamp;
            };
          d1 = (double) (GEB_event->ptgd[i]->timestamp - t0);
          d1 /= 100000000;
          d1 /= 60;
          if (d1 > 0 && d1 < (double) RATELEN)
            rate_mode1->Fill (d1, 1 / 60.0);
          ngrh++;

          grh = (TRACKED_GAMMA_HIT *) GEB_event->ptinp[i];

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("ngam=%i\n", grh->ngam);
              for (j = 0; j < grh->ngam; j++)
                {
                  printf ("%2i: esum=%8.0f, fom=%4.2f, tracked %i\n", j, grh->gr[j].esum, grh->gr[j].fom,
                          grh->gr[j].tracked);
                };
            };

          /* find doppler correction factors */

          if (Pars.beta > 0.0001)
            {
              for (j = 0; j < grh->ngam; j++)
                if (grh->gr[j].tracked)
                  {

                    rr = grh->gr[j].x0 * grh->gr[j].x0 + grh->gr[j].y0 * grh->gr[j].y0 + grh->gr[j].z0 * grh->gr[j].z0;
                    rr = sqrtf (rr);

                    dp = (grh->gr[j].x0 * Pars.beamdir[0] +
                          grh->gr[j].y0 * Pars.beamdir[1] + grh->gr[j].z0 * Pars.beamdir[2]) / rr;

                    if (dp < -1.0)
                      dp = -1.0;
                    if (dp > 1.0)
                      dp = 1.0;
                    polang[j] = acosf (dp);

                    if (grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi)
                      {
                        d1 = (double) (grh->gr[j].esum);
                        if (d1 < 0)
                          d1 = 0;
                        if (d1 > Pars.GGMAX)
                          d1 = Pars.GGMAX - 1;

                        rad_e->Fill (rr, d1, 1.0);
                      };

                    rr = 1.0 - Pars.beta * Pars.beta;
                    doppler_factor[j] = sqrt (rr) / (1.0 - Pars.beta * cos (polang[j]));

                    if (Pars.CurEvNo <= Pars.NumToPrint)
                      {
                        printf ("doppler_factor[%i]=%f\n", j, doppler_factor[j]);
                      }
                  };

              /* apply doppler corrections */

              for (j = 0; j < grh->ngam; j++)
                if (grh->gr[j].tracked)
                  {

                    if (Pars.CurEvNo <= Pars.NumToPrint)
                      printf ("dopler correction: esum %f e0 %f e1 %f -> ", grh->gr[j].esum, grh->gr[j].e0,
                              grh->gr[j].e1);
                    grh->gr[j].esum /= doppler_factor[j];
                    grh->gr[j].e0 /= doppler_factor[j];
                    grh->gr[j].e1 /= doppler_factor[j];
                    if (Pars.CurEvNo <= Pars.NumToPrint)
                      printf ("%f %f %f\n", grh->gr[j].esum, grh->gr[j].e0, grh->gr[j].e1);

                  };
            };

          if (Pars.CurEvNo <= Pars.NumToPrint)
            print_tracked_gamma_rays (stdout, grh);

          /* energy spectra vs ndet */

          for (j = 0; j < grh->ngam; j++)
            if (grh->gr[j].tracked)
              if (grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi)
                {
                  if (grh->gr[j].ndet < 10)
                    if (grh->gr[j].esum < Pars.GGMAX)
                      ndet_e->Fill ((double) grh->gr[j].ndet, (double) grh->gr[j].esum, 1);
                };


          for (j = 0; j < grh->ngam; j++)
            if (grh->gr[j].tracked)
              {
                polAng = findPolarFromCartesian (grh->gr[j].x0, grh->gr[j].y0, grh->gr[j].z0, &rr);
                aziAng = findAzimuthFromCartesian (grh->gr[j].x0, grh->gr[j].y0, grh->gr[j].z0);


                /* SMAP coordinates */

                sX = aziAng * sinf (polAng) / RAD2DEG + 180;
                sY = polAng / RAD2DEG;  /* + 1.5; */

                /* update */

                if (Pars.CurEvNo <= Pars.NumToPrint)
                  {
                    printf ("aziAng=%6.2f\n", aziAng / M_PI * 180);
                    printf ("polAng=%6.2f\n", polAng / M_PI * 180);
                    printf (" sX,sY=%6.2f,%6.2f\n", sX, sY);
                    fflush (stdout);
                  };

                if (sX > 0 && sX < 360)
                  if (sY > 0 && sY < 180)
                    SMAP_firsthits->Fill (sX, sY, 1);
              };



          /* esum vs fom matrix */

          for (j = 0; j < grh->ngam; j++)
            if (grh->gr[j].tracked)
              {
                d1 = (double) (grh->gr[j].esum);
                if (grh->gr[j].fom < 2.0 && grh->gr[j].fom >= 0)
                  if (d1 > 0 && d1 < LONGLEN)
                    if (grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi)
                      {

                        fomXe->Fill (d1, grh->gr[j].fom, 1);
//                        printf("*** %f %f\n", (float)d1, (float)grh->gr[j].fom);

                        if (grh->gr[j].fom >= Pars.fomlo[grh->gr[j].ndet])
                          if (grh->gr[j].fom <= Pars.fomhi[grh->gr[j].ndet])
                            sumTrackE->Fill (d1, 1);
                      };
              };

          /* fom spectrum */

          for (j = 0; j < grh->ngam; j++)
            if (grh->gr[j].tracked)
              if (grh->gr[j].fom >= 0 && grh->gr[j].fom < 2.0)
                if (grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi)
                  fmsp->Fill (grh->gr[j].fom, 1);

          /* HK matrix */

          d2 = 0;
          for (j = 0; j < grh->ngam; j++)
            if (grh->gr[j].tracked)
              d2 += (double) (grh->gr[j].esum);
          d2 /= 1000;
          rn = (float) rand () / RAND_MAX;
          d1 = (grh->ngam + (rn - 0.5));
          if (d1 > (double) 0 && d1 < (double) MAXK)
            if (d2 > (double) 0 && d2 < (double) MAXH)
              {
//                printf ("HK: ngam=%i, %f %f\n", grh->ngam, (float) d1, (float) d2);
//                fflush (stdout);
                HK->Fill (d1, d2, 1);
              };


          /* gamma gamma matrix */

          if (grh->ngam >= 2)
            for (k = 0; k < grh->ngam; k++)
              if (grh->gr[k].tracked)
                if (grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi)
                  if (grh->gr[k].fom >= Pars.fomlo[grh->gr[k].ndet] && grh->gr[k].fom <= Pars.fomhi[grh->gr[k].ndet])
                    if (grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi)
                      for (l = k + 1; l < grh->ngam; l++)
                        if (grh->gr[l].tracked)
                          if (grh->gr[l].ndet >= Pars.ndetlimlo && grh->gr[l].ndet <= Pars.ndetlimhi)
                            if (grh->gr[l].fom >= Pars.fomlo[grh->gr[l].ndet]
                                && grh->gr[l].fom <= Pars.fomhi[grh->gr[l].ndet])
                              if (grh->gr[l].ndet >= Pars.ndetlimlo && grh->gr[l].ndet <= Pars.ndetlimhi)
                                {

                                  d1 = (double) grh->gr[k].esum;
                                  if (d1 < 0)
                                    d1 = 0;
                                  if (d1 > Pars.GGMAX)
                                    d1 = Pars.GGMAX;

                                  d2 = (double) grh->gr[l].esum;
                                  if (d2 < 0)
                                    d2 = 0;
                                  if (d2 > Pars.GGMAX)
                                    d2 = Pars.GGMAX;

                                  gg->Fill (d1, d2, 1.0);
                                  gg->Fill (d2, d1, 1.0);

                                };

        };

    };



  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_mode1\n");

  return (0);

}
