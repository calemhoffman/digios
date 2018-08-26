#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

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
#include "gsang.h"

#define NGSGE 64
#if(0)
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
#endif

/* Gain Calibrtation */
float M=350.0;
float ehigain[NGE + 1];
float ehioffset[NGE + 1];
float ehibase[NGE + 1];
float ehiPZ[NGE+1];
extern void SetBeta ();

/* Other variables */
unsigned long long int  EvTimeStam0=0;

/* pointers to ROOT spectra */

TH1D *hEventCounter;
TH2F *hGeCounter,*hBGOCounter;
TH2F *hEhiRaw,*hEhiCln,*hEhiDrty;
TH2F *hGeBGO_DT;
TH2F *hTrB, *hFwB,*hE_TrB[NGE+1],*hE_TrBall;
TH2F *hE_Pre, *hE_Post, *hE_M2beg;

extern TH1D *ehi[MAXDETPOS + 1];

/* parameters */

extern PARS Pars;
int tlkup[NCHANNELS];
int tid[NCHANNELS];


/*-----------------------------------------------------*/

int
sup_dgs ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN], str[STRLEN];
  float pi;
  int i, i1, i2, i7, i8;
  FILE *fp;
  
  void getcal(char *);

  char file_name[]="./dgscal.dat";        // place this in sort directory

// functions for making root histograms 

  TH2F *make2D(const char*, int, int, int, int, int, int);
  TH1D *make1D(const char*, int ,int ,int);

// 2-D's for Rate

  hEventCounter = make1D("EvntCounter",14400,0,14400);   // Good for 4 hours if Counts/sec
  hGeCounter    = make2D("GeCounter",14400,0,14400,110,1,111);
  hBGOCounter   = make2D("BGOCounter",14400,0,14400,110,1,111);

// 2-D's for Energy

  hEhiRaw   = make2D("EhiRaw",16384,0,16384,64,1,65);
  hEhiCln   = make2D("EhiCln",16384,0,16384,110,1,111);
  hEhiDrty  = make2D("EhiDrty",16384,0,16384,110,1,111);

// 2-D's for Tacs

  hGeBGO_DT = make2D("GeBGO_DT",400,-200,200,64,1,65);

// 2-D's for PZ and Baseline

  hTrB = make2D("TrBase",4096,0,4096,64,1,65);
  hFwB = make2D("FwBase",4096,0,4096,64,1,65);
  
  for (i = 1; i < NGE+1; i++ ){
    sprintf(str, "E_TrB%i", i) ;
    hE_TrB[i] = make2D(str,2500,0,5000,1024,0,8192);
  }

   hE_TrBall = make2D("E_TrBall",8000,0,16000,1024,0,8192);
   hE_Pre    = make2D("E_Pre",8000,0,16000,2048,0,16384);
   hE_Post   = make2D("E_Post",8000,0,16000,2048,0,16384);
   hE_M2beg  = make2D("E_M2b3g",8000,0,16000,2048,0,16384);

/* list what we have */

  //printf (" we have define the following spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  //Pars.wlist->Print ();

  /* -------------------- */
  /* read in the map file */
  /* -------------------- */

  for (i = 0; i < NCHANNELS; i++)
    {
      tlkup[i] = NOTHING;
      tid[i] = NOTHING;
    };

  fp = fopen ("map.dat", "r");
  if (fp == NULL)
    {
      printf ("need a \"map.dat\" file to run\n");
      system("./mkMap > map.dat");
      printf("just made you one...\n");
      fp = fopen ("map.dat", "r");
      assert(fp != NULL);
     };

  printf ("\nmapping - started\n");

  i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
  printf ("%i %i %i %s\n", i1, i7, i8, str);
  while (i2 == 4) {
    tlkup[i1] = i7;
    tid[i1] = i8;
    i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
    //printf ("%i %i %i %s\n", i1, i7, i8, str);
  };
  fclose (fp);
  
  
  printf ("\nmapping - complete!!\n");

// Set Default Calibration parameters

  for (i = 0; i <= NGE+1; i++) {
    //printf ("\nsup_dgs %i \n",i);
    ehigain[i] = 1.0;
    ehioffset[i] = 0.0;
    ehiPZ[i]=1.0;
    ehibase[i]=0.0;
  };

 // This is the DGS calibration file

    getcal(file_name);


 // Reset calibration parameters to turn off
 
   for (i = 0; i <= NGE+1; i++) {
    //ehigain[i]   = 1.0;
    //ehioffset[i] = 0.0;
    //ehiPZ[i]=1.0;
    //ehibase[i]=0.0;
  };  
    
    printf ("\nsup_dgs done!! M=%f\n",M);
    
    return(0);

};

int
DGSEvDecompose_v3 (unsigned int *ev, int len, DGSEVENT * DGSEvent)
{

  /* firmware circa 9/2013 */

  /* declarations */

  int i, k, i1;
  unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
  unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
  int rawE;
  unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
  unsigned long long int ulli1;


  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered DGSEvDecompose_v3:\n");

  /* swap the bytes */

  i = 0;
  while (i < len)
    {

      /* before 4 3 2 1 */
      /*        | | | | */
      /* after  1 2 3 4 */

      t1 = (*(ev + i) & 0x000000ff) << 24;
      t2 = (*(ev + i) & 0x0000ff00) << 8;
      t3 = (*(ev + i) & 0x00ff0000) >> 8;
      t4 = (*(ev + i) & 0xff000000) >> 24;
      *(ev + i) = t1 + t2 + t3 + t4;

      i++;
    }

  /* debug print */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("event len=%i (%i bytes) >\n", len, len * sizeof (unsigned int));
      for (i = 0; i < len; i++)
        {
          printf ("%3i[doc: %3i]: %12u, 0x%8.8x; ", i, i + 1, *(ev + i), *(ev + i));
          ui0 = 0x80000000;
          printf ("|");
          for (k = 0; k <= 31; k++)
            {
              if ((*(ev + i) & ui0) == ui0)
                printf ("1");
              else
                printf ("0");
              ui0 = ui0 / 2;
              if ((k + 1) % 4 == 0)
                printf ("|");
            };
          printf ("\n");
        };
    };

  // Decode the generic part of the header.

  DGSEvent->chan_id = (*(ev + 0) & 0x0000000f);
  DGSEvent->board_id = ((*(ev + 0) & 0x0000fff0) >> 4);        // USER_DEF
  DGSEvent->id = DGSEvent->board_id * 10 + DGSEvent->chan_id;  
  DGSEvent->packet_length = ((*(ev + 0) & 0x07ff0000) >> 16);
  DGSEvent->geo_addr = ((*(ev + 0) & 0xf8000000) >> 27);

  DGSEvent->header_type = ((*(ev + 2) & 0x000f0000)  >> 16); 
  DGSEvent->event_type = ((*(ev + 2) & 0x03800000)  >> 23);     // hope this is right.
  DGSEvent->header_length = ((*(ev + 2) & 0xFC000000)  >> 26);  // hope this is right.
    
  /* extract the LED time stamp */

  DGSEvent->event_timestamp = 0;
  DGSEvent->event_timestamp = (unsigned long long int) *(ev + 1);
  ulli1 = (unsigned long long int) (*(ev + 2) & 0x0000ffff);
  ulli1 = (ulli1 << 32);
  DGSEvent->event_timestamp += ulli1;

  /* store the type and type ID */

  DGSEvent->tpe = tlkup[DGSEvent->id];
  DGSEvent->tid = tid[DGSEvent->id];
  DGSEvent->flag = 0;

  /*
  
  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("chan_id = %i, board_id=%i, id=%i\n", DGSEvent->chan_id, DGSEvent->board_id, DGSEvent->id);
    }

  /* extract the energy */

  switch(DGSEvent->header_type)
  {
    case 0:                // Original LED header (Compatibility mode)
      DGSEvent->timestamp_match_flag   = 0;
      DGSEvent->external_disc_flag     = 0;
      DGSEvent->cfd_valid_flag         = 0;
      DGSEvent->pileup_only_flag       = 0;                                                  
      DGSEvent->cfd_sample_0           = 0; 
      DGSEvent->cfd_sample_1           = 0; 
      DGSEvent->cfd_sample_2           = 0;

      DGSEvent->peak_valid_flag        = ((*(ev + 3) & 0x00000200) >> 9);                               // Word 3: 9
      DGSEvent->offset_flag            = ((*(ev + 3) & 0x00000400) >> 10);                              // Word 3: 10
      DGSEvent->sync_error_flag        = ((*(ev + 3) & 0x00001000) >> 12);                              // Word 3: 12
      DGSEvent->general_error_flag     = ((*(ev + 3) & 0x00002000) >> 13);                              // Word 3: 13
      DGSEvent->pileup_flag            = ((*(ev + 3) & 0x00008000) >> 15);                              // Word 3: 15
      DGSEvent->last_disc_timestamp    = (((unsigned long long int)(*(ev + 3) & 0xFFFF0000)) >> 16 )|   // Word 3: 31..16 & 
                                         (((unsigned long long int)(*(ev + 4) & 0xFFFFFFFF)) << 16);    // Word 4 :31..0  
      DGSEvent->sampled_baseline       = ((*(ev + 5) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      DGSEvent->pre_rise_energy        = ((*(ev + 7) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      DGSEvent->post_rise_energy       = ((*(ev + 7) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                         ((*(ev + 8) & 0x0000FFFF) << 8);                               // Word 8: 15..0 
      DGSEvent->peak_timestamp         = (((unsigned long long int)(*(ev + 8) & 0xFFFF0000)) >> 16 )|   // Word 8: 31..16 & 
                                         (((unsigned long long int)(*(ev + 9) & 0xFFFFFFFF)) << 16);    // Word 9 :31..0  
      DGSEvent->m1_begin_sample        = ((*(ev + 10) & 0x00003FFF) >> 0);                              // Word 10:13..0
      DGSEvent->m1_end_sample          = ((*(ev + 10) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      DGSEvent->m2_begin_sample        = ((*(ev + 11) & 0x00003FFF) >> 0);                              // Word 11:13..0
      DGSEvent->m2_end_sample          = ((*(ev + 11) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      DGSEvent->peak_sample            = ((*(ev + 12) & 0x00003FFF) >> 0);                              // Word 12:13..0
      DGSEvent->base_sample            = ((*(ev + 12) & 0x3FFF0000) >> 16);                             // Word 12:29..16
    break;
                
    case 1:                // New LED Header
      DGSEvent->timestamp_match_flag   = 0;
      DGSEvent->cfd_valid_flag         = 0;
      DGSEvent->cfd_sample_0           = 0; 
      DGSEvent->cfd_sample_1           = 0; 
      DGSEvent->cfd_sample_2           = 0;
                        
      DGSEvent->external_disc_flag     = ((*(ev + 3) & 0x00000100) >> 8);                               // Word 3: 8
      DGSEvent->peak_valid_flag        = ((*(ev + 3) & 0x00000200) >> 9);                               // Word 3: 9
      DGSEvent->offset_flag            = ((*(ev + 3) & 0x00000400) >> 10);                              // Word 3: 10
      DGSEvent->sync_error_flag        = ((*(ev + 3) & 0x00001000) >> 12);                              // Word 3: 12
      DGSEvent->general_error_flag     = ((*(ev + 3) & 0x00002000) >> 13);                              // Word 3: 13
      DGSEvent->pileup_only_flag       = ((*(ev + 3) & 0x00004000) >> 14);                              // Word 3: 14
      DGSEvent->pileup_flag            = ((*(ev + 3) & 0x00008000) >> 15);                              // Word 3: 15  
      DGSEvent->last_disc_timestamp    = (((unsigned long long int)(*(ev + 3) & 0xFFFF0000)) >> 16 )|   // Word 3: 31..16 & 
                                         (((unsigned long long int)(*(ev + 4) & 0xFFFFFFFF)) << 16);    // Word 4 :31..0  
      DGSEvent->sampled_baseline       = ((*(ev + 5) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      DGSEvent->pre_rise_energy        = ((*(ev + 7) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      DGSEvent->post_rise_energy       = ((*(ev + 7) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                         ((*(ev + 8) & 0x0000FFFF) << 8);                               // Word 8: 15..0 
      DGSEvent->peak_timestamp         = (((unsigned long long int)(*(ev + 8) & 0xFFFF0000)) >> 16) |   // Word 8: 31..16 & 
                                         (((unsigned long long int)(*(ev + 9) & 0xFFFFFFFF)) << 16);    // Word 9: 31..0  
      DGSEvent->m1_begin_sample        = ((*(ev + 10) & 0x00003FFF) >> 0);                              // Word 10:13..0
      DGSEvent->m1_end_sample          = ((*(ev + 10) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      DGSEvent->m2_begin_sample        = ((*(ev + 11) & 0x00003FFF) >> 0);                              // Word 11:13..0
      DGSEvent->m2_end_sample          = ((*(ev + 11) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      DGSEvent->peak_sample            = ((*(ev + 12) & 0x00003FFF) >> 0);                              // Word 12:13..0
      DGSEvent->base_sample            = ((*(ev + 12) & 0x3FFF0000) >> 16);                             // Word 12:29..16
    break;

    case 2:                // CFD Header
      DGSEvent->timestamp_match_flag   = ((*(ev + 3) & 0x00000080) >> 7);                               // Word 3: 7
      DGSEvent->external_disc_flag     = ((*(ev + 3) & 0x00000100) >> 8);                               // Word 3: 8
      DGSEvent->peak_valid_flag        = ((*(ev + 3) & 0x00000200) >> 9);                               // Word 3: 9
      DGSEvent->offset_flag            = ((*(ev + 3) & 0x00000400) >> 10);                              // Word 3: 10
      DGSEvent->cfd_valid_flag         = ((*(ev + 3) & 0x00000800) >> 11);                              // Word 3: 11
      DGSEvent->sync_error_flag        = ((*(ev + 3) & 0x00001000) >> 12);                              // Word 3: 12
      DGSEvent->general_error_flag     = ((*(ev + 3) & 0x00002000) >> 13);                              // Word 3: 13
      DGSEvent->pileup_only_flag       = ((*(ev + 3) & 0x00004000) >> 14);                              // Word 3: 14
      DGSEvent->pileup_flag            = ((*(ev + 3) & 0x00008000) >> 15);                              // Word 3: 15
      DGSEvent->last_disc_timestamp    = (((unsigned long long int)(*(ev + 3) & 0xFFFF0000)) >> 16 )|   // Word 3: 31..16 &
                                         (((unsigned long long int)(*(ev + 4) & 0xFFFFFFFF)) << 16);    // Word 4 :31..0
      DGSEvent->cfd_sample_0           = ((*(ev + 4) & 0x3FFF0000) >> 16);                              // Word 4: 29..16
      DGSEvent->sampled_baseline       = ((*(ev + 5) & 0x00FFFFFF) >> 0);                               // Word 5: 23..0
      DGSEvent->cfd_sample_1           = ((*(ev + 6) & 0x00003FFF) >> 0);                               // Word 6: 13..0
      DGSEvent->cfd_sample_2           = ((*(ev + 6) & 0x3FFF0000) >> 16);                              // Word 6: 29..16
      DGSEvent->pre_rise_energy        = ((*(ev + 7) & 0x00FFFFFF) >> 0);                               // Word 7: 23..0
      DGSEvent->post_rise_energy       = ((*(ev + 7) & 0xFF000000) >> 28) |                             // Word 7: 31..24 & 
                                         ((*(ev + 8) & 0x0000FFFF) << 8);                               // Word 8: 15..0 
      DGSEvent->peak_timestamp         = (((unsigned long long int)(*(ev + 8) & 0xFFFF0000)) >> 16) |   // Word 8: 31..16 & 
                                         (((unsigned long long int)(*(ev + 9) & 0xFFFFFFFF)) << 16);    // Word 9: 31..0  
      DGSEvent->m1_begin_sample        = ((*(ev + 10) & 0x00003FFF) >> 0);                              // Word 10:13..0
      DGSEvent->m1_end_sample          = ((*(ev + 10) & 0x3FFF0000) >> 16);                             // Word 10:29..16
      DGSEvent->m2_begin_sample        = ((*(ev + 11) & 0x00003FFF) >> 0);                              // Word 11:13..0
      DGSEvent->m2_end_sample          = ((*(ev + 11) & 0x3FFF0000) >> 16);                             // Word 11:29..16
      DGSEvent->peak_sample            = ((*(ev + 12) & 0x00003FFF) >> 0);                              // Word 12:13..0
      DGSEvent->base_sample            = ((*(ev + 12) & 0x3FFF0000) >> 16);                             // Word 12:29..16
    break;
  }

  DGSEvent->baseline = (DGSEvent->m1_begin_sample + DGSEvent->m1_end_sample)/2 ;
  

  /* Now load Trace into DGSEvent Structure */

  DGSEvent->traceLen = 0;
  for(i = 13; i < len-1; i++){
    if(i<1037){
      DGSEvent->trace[2*(i-13)] = (unsigned short int) (*(ev + i) & 0xffff);
      DGSEvent->trace[2*(i-13)+1] = (unsigned short int) ((*(ev +i) >> 16) & 0xffff);
      DGSEvent->traceLen += 2;
    }
  }

  //rawE = (int) POST_RISE_SUM - (int) PRE_RISE_SUM;
  //DGSEvent->ehi = rawE / 800;

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("rawE = 0x%8.8x %i, DGSEvent->ehi= %i\n", rawE, rawE, DGSEvent->ehi);

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit DGSEvDecompose_v3:\n");

  return (0);

}

/* ----------------------------------------------------------------- */

int
bin_dgs (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int i, j, i1, ng, gsid;
  unsigned int ui1;
  DGSEVENT DGSEvent[MAXCOINEV];
  
  int RelEvT=0,DEvT=0,DTrT=0,tdiff=0;
  float Energy;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

  /* Print debug */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_dgs:\n");

  /* loop through the coincidence event and fish out DGS data */
  /* (gamma rays) count in ng */

  ng = 0;
  for (i = 0; i < GEB_event->mult; i++) 
  {
    if (GEB_event->ptgd[i]->type == GEB_TYPE_DGS)
    {
      if (Pars.CurEvNo <= Pars.NumToPrint)
      {
        GebTypeStr (GEB_event->ptgd[i]->type, str);
        printf ("bin_dgs, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                GEB_event->ptgd[i]->timestamp);
      }

      DGSEvDecompose_v3 ((unsigned int *) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof (unsigned int),
                          &DGSEvent[ng]);
      ng++;
    }
  }

  // Initialize EvTimeStam0
  
  //printf("Stat 1 \n");

  if(EvTimeStam0==0) EvTimeStam0=DGSEvent[0].event_timestamp;
  RelEvT = (int)((DGSEvent[0].event_timestamp-EvTimeStam0)/100000000); // overflow?
  hEventCounter->Fill(RelEvT);


// GS ENERGY CALIBRATION 

  double d1;
  int e;
  float postdiff, asymBase;
  
  /* Compton Suppression Loop + raw energy loop*/
  //printf ( "M = %f\n",M);

  for (i = 0; i < ng; i++){
    if ( (DGSEvent[i].tpe==GE1) || (DGSEvent[i].tpe==GE2) || (DGSEvent[i].tpe==GE3) || (DGSEvent[i].tpe==GE4))  {
      gsid = (DGSEvent[i].tid-1)*4+DGSEvent[i].tpe-10;  //Clover ids are 11 12 13 14 (clover 1 = 1,2,3,4) clover 2 = 2,3,4,5 etc
      hGeCounter->Fill( (int)((DGSEvent[0].event_timestamp-EvTimeStam0)/100000000), gsid);

      /***************** Calculate GE Energy  ***************************/

      /* PZ correction */
      Energy = ((float)(DGSEvent[i].post_rise_energy)-(float)(DGSEvent[i].pre_rise_energy)*ehiPZ[gsid])/M*ehigain[gsid];

      /* Asymtotic Baseline correction */

      asymBase = ehibase[gsid];
      //asymBase = float(DGSEvent[i].base_sample);

      Energy = Energy - asymBase*(1.-ehiPZ[gsid])*ehigain[gsid] + ehioffset[gsid];

      if(Pars.beta!=0) {
       d1 = angtheta[gsid-1]/57.29577951;
       Energy = Energy*(1 - Pars.beta * cos(d1) ) / sqrt (1 - Pars.beta * Pars.beta);
      }
      /* Check for Post_sum anomoly */
      postdiff = (float)(DGSEvent[i].post_rise_energy)/M - (float)(DGSEvent[i].m2_begin_sample+DGSEvent[i].m2_end_sample)/2.0;
      /* if post diff to large, mark energy as negative */
      //printf("postdiff = %f\n",postdiff);
      if(postdiff<100){     
        DGSEvent[i].ehi = Energy;
      } else {
        if(Energy>0) DGSEvent[i].ehi = Energy*-1.0; // could already have some negative energies
      }

      for(j=0; j< ng; j++){
        if (DGSEvent[j].tpe==BGO && DGSEvent[j].tid == gsid) {      // BGO & GE in coincidence
          tdiff = (int)(DGSEvent[i].event_timestamp - DGSEvent[j].event_timestamp);
          hGeBGO_DT->Fill(tdiff,gsid);
          if(abs(tdiff) <= 50) DGSEvent[i].flag = 1;  // Mark as Dirty Ge
        }
      }//end suppression loop
             
    }//end ge loop

    if (DGSEvent[i].tpe==BGO)  {
      hBGOCounter->Fill( (int)((DGSEvent[0].event_timestamp-EvTimeStam0)/100000000), DGSEvent[i].tid);
      DGSEvent[i].ehi = ((float)(DGSEvent[i].post_rise_energy)-(float)(DGSEvent[i].pre_rise_energy))/M;
    }
  }

  /* Energy Histogram loop */

  for (i = 0; i < ng; i++){
    if ( (DGSEvent[i].tpe==GE1) || (DGSEvent[i].tpe==GE2) || (DGSEvent[i].tpe==GE3) || (DGSEvent[i].tpe==GE4)){
      gsid = (DGSEvent[i].tid-1)*4+DGSEvent[i].tpe-10;
      e = (int)DGSEvent[i].ehi;
      //printf("ehi = %i\n",e);
      hEhiRaw->Fill(e,gsid);
      if(DGSEvent[i].flag == 0) hEhiCln->Fill(e,gsid);
      if(DGSEvent[i].flag == 1) hEhiDrty->Fill(e,gsid);
    }
  }
   
  /* Histogram's for PZ and baseline correction */
  
  for (i = 0; i < ng; i++){
   if ( (DGSEvent[i].tpe==GE1) || (DGSEvent[i].tpe==GE2) || (DGSEvent[i].tpe==GE3) || (DGSEvent[i].tpe==GE4))  {
      gsid = (DGSEvent[i].tid-1)*4+DGSEvent[i].tpe-10;
      hTrB->Fill(DGSEvent[i].baseline,gsid);
      hFwB->Fill(DGSEvent[i].base_sample,gsid);
      hE_TrB[gsid]->Fill(DGSEvent[i].ehi,DGSEvent[i].baseline);
      if(gsid==7) {
         hE_TrBall->Fill(DGSEvent[i].ehi,DGSEvent[i].baseline);
         hE_Pre->Fill(DGSEvent[i].ehi,DGSEvent[i].pre_rise_energy/M);
         hE_Post->Fill(DGSEvent[i].ehi,DGSEvent[i].post_rise_energy/M);
         hE_M2beg->Fill(DGSEvent[i].ehi,DGSEvent[i].m2_begin_sample);
      }
    }
  }
  
/* debug list the gamma rays we found */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    for (i = 0; i < ng; i++) {
      printf ("we have %i gamma rays\n", ng);
      printf ("%2i> ", i);
      printf ("chan_id=%i; ", DGSEvent[i].chan_id);
      printf ("board_id=%i; ", DGSEvent[i].board_id);
      printf ("id =%i; ", DGSEvent[i].id);
      printf ("tpe=%i; ", DGSEvent[i].tpe);
      printf ("tid=%i; ", DGSEvent[i].tid);
      printf ("EventTS=%llu; ", DGSEvent[i].event_timestamp);
      printf ("ehi=%8i ", DGSEvent[i].ehi);
      printf ("\n");
      fflush (stdout);
    };

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_dgs\n");
  
  //printf("Stat 2 \n");
  return (0);

}
//////////////////////////////////////////////////////////////////////////////
TH2F *make2D (const char* txt, int xln,int xlo,int xhi,int yln,int ylo,int yhi)
{
char str[STRLEN];
char fn[STRLEN];

TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);


TH2F *h2D;

  sprintf(str,txt);
  h2D = mkTH2F(str,str,xln,xlo,xhi,yln,ylo,yhi);
  
  return h2D;
}
//////////////////////////////////////////////////////////////////////////////
TH1D *make1D (const char* txt, int xln,int xlo,int xhi)
{
char str[STRLEN];
char fn[STRLEN];
double xlod,xhid;
TH1D *mkTH1D (char *, char *, int, double, double);
TH1D *h1D;

  xlod = xlo;
  xhid = xhi;
  
  sprintf(str,txt);
  h1D = mkTH1D(str,str,xln,xlod,xhid);
  return h1D;
}
//////////////////////////////////////////////////////////////////////////////
void getcal(char *file)
{
  int i,ii,j,k,l,ret=0;
  float a,b,c,d;
  char mystring [1000];
  FILE *fp;

  fp = fopen(file,"r"); // read mode

  // if Error opening file - end. 

  if( fp == NULL )
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // read file and parse

  while(fgets(mystring , 100 , fp)!=NULL){
    //printf("i= %i %s",i,mystring);
    ret = strncmp(mystring,"#",1);
    if(ret == 0) {
      //printf("%s",mystring);
    } else {
      ret = sscanf(mystring,"%i %f %f %f %f",&i,&a,&b,&c,&d);
//      if(ret == 1) {
//        M = i; 
//        printf("M = %f\n",M);
//      }
      if(ret == 5){
        printf("%i %f %f %f %f \n",i,a,b,c,d);
        ehiPZ[i]=b; 
        ehibase[i]=a;
        ehigain[i]=d;
        ehioffset[i]=c;
        if(i == 0) M = a;
      }
    }
  } 
  printf("M = %f\n",M);
  fclose(fp);
}

