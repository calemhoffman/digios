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
#include "TBranch.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TNamed.h"

#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"
#include "bin_rcnp.h"

/* parameters */
unsigned long long int EvTimeStamp0=0;
extern PARS Pars;
int TypeIDLookup[NCHANNELS];
int DetIDLookup[NCHANNELS];
Float_t AngleDegree[NCHANNELS];

/* pointers to ROOT spectra */
TTree *tree;
Int_t EvtMult;
UInt_t EvtCount;
UInt_t HitCount;

TNamed *timeWindowInfo,*maxEventsInfo;
TNamed *numEventsInfo,*numHitsInfo;

EvtList* EL = new EvtList(100);

int
sup_rcnp ()
{
  /* declarations */
  
  char str1[STRLEN], str2[STRLEN], TypeName[STRLEN];
  float pi;
  int i,ChannelID, TypeID, DetID;
  float DetAng;
  FILE *fp;
  EvtCount=0; HitCount=0;

  /* TTree create */
  tree = new TTree("tree","tree");
  //Branches from EvtList Class \todo/ simple branch w/ leafs
  tree->Branch("RunNum",&EL->RunNumber,"RunNum/I");
  tree->Branch("NumHits",&EL->NumHits,"NumHits/I");
  tree->Branch("id",EL->id,"id[NumHits]/S");
  tree->Branch("tpe",EL->tpe,"tpe[NumHits]/s");
  tree->Branch("tid",EL->tid,"tid[NumHits]/s");
  tree->Branch("chan_id",EL->chan_id,"chan_id[NumHits]/s");
  tree->Branch("board_id",EL->board_id,"board_id[NumHits]/s");
  tree->Branch("geo_addr",EL->geo_addr,"geo_addr[NumHits]/s");
  tree->Branch("packet_length",EL->packet_length,"packet_length[NumHits]/s");
  tree->Branch("header_type",EL->header_type,"header_type[NumHits]/s");
  tree->Branch("event_type",EL->event_type,"event_type[NumHits]/s");
  tree->Branch("header_length",EL->header_length,"header_length[NumHits]/s");
  tree->Branch("event_timestamp",EL->event_timestamp,"event_timestamp[NumHits]/l");
  tree->Branch("last_disc_timestamp",EL->last_disc_timestamp,"last_disc_timestamp[NumHits]/l");
  tree->Branch("peak_timestamp",EL->peak_timestamp,"peak_timestamp[NumHits]/l");
  tree->Branch("timestamp_match_flag",EL->timestamp_match_flag,"timestamp_match_flag[NumHits]/s");
  tree->Branch("external_disc_flag",EL->external_disc_flag,"external_disc_flag[NumHits]/s");
  tree->Branch("cfd_valid_flag",EL->cfd_valid_flag,"cfd_valid_flag[NumHits]/s");
  tree->Branch("pileup_only_flag",EL->pileup_only_flag,"pileup_only_flag[NumHits]/s");
  tree->Branch("offset_flag",EL->offset_flag,"offset_flag[NumHits]/s");
  tree->Branch("sync_error_flag",EL->sync_error_flag,"sync_error_flag[NumHits]/s");
  tree->Branch("general_error_flag",EL->general_error_flag,"general_error_flag[NumHits]/s");
  tree->Branch("peak_valid_flag",EL->peak_valid_flag,"peak_valid_flag[NumHits]/s");
  tree->Branch("pileup_flag",EL->pileup_flag,"pileup_flag[NumHits]/s");
  tree->Branch("sampled_baseline",EL->sampled_baseline,"sampled_baseline[NumHits]/I");
  tree->Branch("cfd_sample_0",EL->cfd_sample_0,"cfd_sample_0[NumHits]/I");
  tree->Branch("cfd_sample_1",EL->cfd_sample_1,"cfd_sample_1[NumHits]/I");
  tree->Branch("cfd_sample_2",EL->cfd_sample_2,"cfd_sample_2[NumHits]/I");
  tree->Branch("pre_rise_energy",EL->pre_rise_energy,"pre_rise_energy[NumHits]/I");
  tree->Branch("post_rise_energy",EL->post_rise_energy,"post_rise_energy[NumHits]/I");
  tree->Branch("m1_begin_sample",EL->m1_begin_sample,"m1_begin_sample[NumHits]/s");
  tree->Branch("m1_end_sample",EL->m1_end_sample,"m1_end_sample[NumHits]/s");
  tree->Branch("m2_begin_sample",EL->m2_begin_sample,"m2_begin_sample[NumHits]/s");
  tree->Branch("m2_end_sample",EL->m2_end_sample,"m2_end_sample[NumHits]/s");
  tree->Branch("peak_sample",EL->peak_sample,"peak_sample[NumHits]/s");
  tree->Branch("base_sample",EL->base_sample,"base_sample[NumHits]/s");
  tree->Branch("baseline",EL->baseline,"baseline[NumHits]/I");
  tree->Branch("trace_length",EL->trace_length,"trace_length[NumHits]/s");
  //tree->Branch("trace",EL->trace,"trace[NumHits][1024]/S");
  //  tree->GetBranch("trace")->SetCompressionSettings(205);

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* read in channel mapping file */

  for (i = 0; i < NCHANNELS; i++)
    {
      TypeIDLookup[i] = NOTHING;
      DetIDLookup[i] = NOTHING;
    };

  fp = fopen ("map.dat", "r");
  if (fp == NULL)
    {
      printf ("\n\nneed a \"map.dat\" file to run\n");
      exit (1);
     };

  i=0;
  printf ("\nmapping - started\n"); // new map needs 5 inputs now!!
  i = fscanf (fp, "\n%i %i %i %s %f", &ChannelID, &TypeID, &DetID, TypeName, &DetAng);
  printf ("%i %i %i %s %f\n", ChannelID, TypeID, DetID, TypeName, DetAng);
  while (i == 5) {
    TypeIDLookup[ChannelID] = TypeID;
    DetIDLookup[ChannelID] = DetID;
    AngleDegree[ChannelID] = DetAng;
    i = fscanf (fp, "\n%i %i %i %s %f", &ChannelID, &TypeID, &DetID, TypeName, &DetAng);
  };
  fclose (fp);  
  printf ("mapping - complete!!\n");
  
  return (0);
};

/* ------------------------------------------------------------- */
/* ---------------- Unpacker ----------------------------------- */
int
DGSEvDecompose_v4 (unsigned int *ev, int len, DGSEVENT * DGSEvent)
{

  /* firmware circa 9/2013 */

  /* declarations */
  int i, k;
  unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
  unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
  int rawE;
  unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
  unsigned long long int ulli1;


  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered DGSEvDecompose_v4:\n");

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

  DGSEvent->tpe = TypeIDLookup[DGSEvent->id];
  DGSEvent->tid = DetIDLookup[DGSEvent->id];
  DGSEvent->flag = 0;

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
                
    case 1:                // LED Header
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
      
    case 3:                // 2015/16 LED Header
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
      DGSEvent->peak_timestamp         = (((unsigned long long int)(*(ev + 8) & 0xFFFF0000)) >> 16);    // Word 8: 31..16  
      //DGSEvent->last_post_enter_sample =                                 // Word 9: 13..0  // /* need to understand these... */
      //DGSEvent->trigger_timestamp =                                      // Word 9: 31..16  //
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

  /* done */
  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit DGSEvDecompose_v4 :\n");
  
  return (0);
  
};

/* ----------------------------------------------------------------- */

int
bin_rcnp (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
//  int RelEvT=0;
  float Energy=0;

  DGSEVENT DGSEvent[MAXCOINEV];

  /* prototypes */

  int GebTypeStr (int type, char str[]);

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_rcnp:\n");

  /* loop through the coincidence event and fish out GEB_TYPE_DGS data */
  EvtMult=0;
  EvtCount++;
  for (int i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_DGS)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_rcnp, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }
	  DGSEvDecompose_v4 ((unsigned int *) GEB_event->ptinp[i], 
			     GEB_event->ptgd[i]->length / sizeof (unsigned int),
			     &DGSEvent[EvtMult]);
	  EvtMult++; HitCount++;
        }

    }

  // TTree work starts //  
  EL->Reset(); //Reset variables
  
  //Pass from DGSEvent -> EvtList
  EL->RunNumber = Pars.RunNumber;
  EL->NumHits = EvtMult;
  for (Int_t i=0;i<EvtMult;i++) {
    EL->id[i] = DGSEvent[i].id;
    EL->tpe[i] = DGSEvent[i].tpe;
    EL->tid[i] = DGSEvent[i].tid;
    EL->chan_id[i] = DGSEvent[i].chan_id;
    EL->board_id[i] = DGSEvent[i].board_id;
    EL->geo_addr[i] = DGSEvent[i].geo_addr;
    EL->packet_length[i] = DGSEvent[i].packet_length;
    EL->header_type[i] = DGSEvent[i].header_type;
    EL->event_type[i] = DGSEvent[i].event_type;
    EL->header_length[i] = DGSEvent[i].header_length;
    EL->event_timestamp[i] = DGSEvent[i].event_timestamp;
    EL->last_disc_timestamp[i] = DGSEvent[i].last_disc_timestamp;
    EL->peak_timestamp[i] = DGSEvent[i].peak_timestamp;
    EL->timestamp_match_flag[i] = DGSEvent[i].timestamp_match_flag;
    EL->external_disc_flag[i] = DGSEvent[i].external_disc_flag;
    EL->cfd_valid_flag[i] = DGSEvent[i].cfd_valid_flag;
    EL->pileup_only_flag[i] = DGSEvent[i].pileup_only_flag;
    EL->offset_flag[i] = DGSEvent[i].offset_flag;
    EL->sync_error_flag[i] = DGSEvent[i].sync_error_flag;
    EL->general_error_flag[i] = DGSEvent[i].general_error_flag;
    EL->peak_valid_flag[i] = DGSEvent[i].peak_valid_flag;
    EL->pileup_flag[i] = DGSEvent[i].pileup_flag;
    EL->sampled_baseline[i] = DGSEvent[i].sampled_baseline;
    EL->cfd_sample_0[i] = DGSEvent[i].cfd_sample_0;
    EL->cfd_sample_1[i] = DGSEvent[i].cfd_sample_1;
    EL->cfd_sample_2[i] = DGSEvent[i].cfd_sample_2;
    EL->pre_rise_energy[i] = DGSEvent[i].pre_rise_energy;
    EL->post_rise_energy[i] = DGSEvent[i].post_rise_energy;
    EL->m1_begin_sample[i] = DGSEvent[i].m1_begin_sample;
    EL->m1_end_sample[i] = DGSEvent[i].m1_end_sample;
    EL->m2_begin_sample[i] = DGSEvent[i].m2_begin_sample;
    EL->m2_end_sample[i] = DGSEvent[i].m2_end_sample;
    EL->peak_sample[i] = DGSEvent[i].peak_sample;
    EL->base_sample[i] = DGSEvent[i].base_sample;
    EL->baseline[i] = DGSEvent[i].baseline;
    EL->trace_length[i] = DGSEvent[i].traceLen;

    for(Int_t j=0; (j<EL->trace_length[i]&&j<1024); j++){
       EL->trace[i][j] = DGSEvent[i].trace[j];
    }
  }

  //* ---__^ TTREE FILL ^__ ---*//
  //gDebug=2;
  //printf("Bytes comitted to tree[%i]: %i\n", Pars.CurEvNo, tree->Fill());


  /* done */
  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_rcnp\n");

  return (0);

};


int
exit_rcnp ()
{
 //TTree UserInformation
  char tmp[200];
  sprintf(tmp,"%lld",Pars.dTS);
  timeWindowInfo = new TNamed("Coincidence time window used:    ",tmp);
  tree->GetUserInfo()->Add(timeWindowInfo);
  sprintf(tmp,"%i",Pars.nEvents);
   maxEventsInfo = new TNamed("Maximum number of events to sort:",tmp);
  tree->GetUserInfo()->Add(maxEventsInfo);
  sprintf(tmp,"%u",EvtCount);
   numEventsInfo = new TNamed("Number of Events sorted:         ",tmp);
  tree->GetUserInfo()->Add(numEventsInfo);
  sprintf(tmp,"%u",HitCount);
     numHitsInfo = new TNamed("Number of Hits sorted:           ",tmp);
  tree->GetUserInfo()->Add(numHitsInfo);

  printf("...Finished Run %d ...\n",Pars.RunNumber);

  return (0);
};
