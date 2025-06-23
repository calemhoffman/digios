#define GeneralSort_cxx
#include "GeneralSort.h"
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>
#include <TMacro.h>
#include <TBenchmark.h>

ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;

#include "../working/GeneralSortMapping.h"

//clock
TBenchmark gClock;
Bool_t shown = 0;

TFile *oFile;
TTree *gen_tree;

//PSD struct
typedef struct {
  Int_t runID;
  Float_t Energy[NARRAY];
  Float_t XF[NARRAY];
  Float_t XN[NARRAY];
  Float_t Ring[NARRAY];
  Float_t RDT[NRDT];
  Float_t TAC[NTAC];
  Float_t ELUM[NELUM];
  Float_t EZERO[NEZERO];
  Float_t CRDT[NCRDT];
  Float_t APOLLO[NAPOLLO];

  ULong64_t EnergyTimestamp[NARRAY];
  ULong64_t XFTimestamp[NARRAY];
  ULong64_t XNTimestamp[NARRAY];
  ULong64_t RingTimestamp[NARRAY];
  ULong64_t RDTTimestamp[NRDT];
  ULong64_t TACTimestamp[NTAC];
  ULong64_t ELUMTimestamp[NELUM];
  ULong64_t EZEROTimestamp[NEZERO];
  ULong64_t CRDTTimestamp[NCRDT];
  ULong64_t APOLLOTimestamp[NAPOLLO];
  
  Float_t x[NARRAY];

} PSD;

PSD psd; 

TString saveFileName = "gen.root";

int runIDPresent = 0;
TString fileNum = "";

void GeneralSort::Begin(TTree * tree)
{
   
  TString option = GetOption();
  NumEntries = tree->GetEntries();

  saveFileName = tree->GetDirectory()->GetName();
  int findslat = saveFileName.Last('/');
  saveFileName.Remove(0, findslat+1);
  saveFileName = "../root_data/gen_" + saveFileName;

  printf("=============================================================\n");
  printf("=====================  GeneralSort.C  =======================\n");
  printf("=============================================================\n");
  printf("                    file : %s \n", tree->GetDirectory()->GetName());
  printf("          Number of Event: %llu\n", NumEntries);

  oFile = new TFile(saveFileName,"RECREATE");

  gen_tree = new TTree("gen_tree","PSD Tree");

  // gen_tree->Branch("runID", &psd.runID,"runID/I");

  gen_tree->Branch("e",psd.Energy,           Form("e[%d]/F", NARRAY));
  gen_tree->Branch("e_t",psd.EnergyTimestamp,Form("e_t[%d]/l",NARRAY));
  
  gen_tree->Branch("ring",psd.Ring,           Form("ring[%d]/F", NARRAY));
  gen_tree->Branch("ring_t",psd.RingTimestamp,Form("ring_t[%d]/l",NARRAY));

  gen_tree->Branch("xf",psd.XF,           Form("xf[%d]/F", NARRAY));
  gen_tree->Branch("xf_t",psd.XFTimestamp,Form("xf_t[%d]/l", NARRAY));
 
  gen_tree->Branch("xn",psd.XN,           Form("xn[%d]/F", NARRAY));
  gen_tree->Branch("xn_t",psd.XNTimestamp,Form("xn_t[%d]/l", NARRAY)); 
  
  gen_tree->Branch("x",psd.x, Form("x[%d]/F", NARRAY));

  if( NRDT > 0 ){
    gen_tree->Branch("rdt"  ,psd.RDT,          Form("rdt[%d]/F",NRDT));
    gen_tree->Branch("rdt_t",psd.RDTTimestamp, Form("rdt_t[%d]/l", NRDT)); 
  }else{
    printf(" -----  no recoil.\n"); 
  }
  
  if( NTAC > 0 ){
    gen_tree->Branch("tac"  ,psd.TAC,         Form("tac[%d]/F", NTAC));
    gen_tree->Branch("tac_t",psd.TACTimestamp,Form("tac_t[%d]/l", NTAC)); 
  }else{
    printf(" -----  no TAC.\n"); 
  }
  
  if( NELUM > 0 ){
    gen_tree->Branch("elum"  ,psd.ELUM,         Form("elum[%d]/F", NELUM));
    gen_tree->Branch("elum_t",psd.ELUMTimestamp,Form("elum_t[%d]/l",NELUM)); 
  }else{
    printf(" -----  no elum\n"); 
  }
  
  if( NEZERO > 0 ){
    gen_tree->Branch("ezero"  ,psd.EZERO,         Form("ezero[%d]/F", NEZERO));
    gen_tree->Branch("ezero_t",psd.EZEROTimestamp,Form("ezero_t[%d]/l", NEZERO)); 
  }else{
    printf(" -----  no ezero.\n"); 
  }

  if( NCRDT > 0 ){
    gen_tree->Branch("crdt"  ,psd.CRDT,         Form("crdt[%d]/F", NCRDT));
    gen_tree->Branch("crdt_t",psd.CRDTTimestamp,Form("crdt_t[%d]/l", NCRDT)); 
  }else{
    printf(" -----  no crdt.\n"); 
  }
  
  if( NAPOLLO > 0 ){
    gen_tree->Branch("apollo"  ,psd.APOLLO,         Form("apollo[%d]/F", NAPOLLO));
    gen_tree->Branch("apollo_t",psd.APOLLOTimestamp,Form("apollo_t[%d]/l", NAPOLLO)); 
  }else{
    printf(" -----  no APOLLO.\n"); 
  }
  
  printf("======= ID-MAP: \n");
  printf("%11s|", ""); 
  for(int i = 0 ; i < 10; i++ ) printf("%7d|", i);
  printf("\n");
  for(int i = 0 ; i < 96; i++ ) printf("-");
  for(int i = 0 ; i < 160; i ++){
    if( (i) % 10 == 0 ) {
       printf("\n");
       if(((i+1)/10)/4+1 < 5) printf("%11s|", Form("VME%d-Dig%d", ((i+1)/10)/4+1, ((i+1)/10)%4+1)); 
    }
    if( 100 + NRDT >= idDetMap[i] && idDetMap[i] >= 100){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Recoil, Cyan
    }else if( 200+NELUM >= idDetMap[i] && idDetMap[i] >= 200){
      printf("\033[91m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Elum, 
    }else if( 300+NEZERO >= idDetMap[i] && idDetMap[i] >= 300){
      printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// EZERO, 
    }else if( 400+NTAC >= idDetMap[i] && idDetMap[i] >= 400){
      printf("\033[93m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// TAC, 
    }else if( 500+NCRDT >= idDetMap[i] && idDetMap[i] >= 500){
      printf("\033[100m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// Circular Recoil
    }else if( 600+NAPOLLO >= idDetMap[i] && idDetMap[i] >= 600){
      printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); /// APOLLO
    }else if(  NARRAY > idDetMap[i] && idDetMap[i] >= 0){    
      switch (idKindMap[i]) {
         case -1: printf("%7s|", ""); break;
         case  0: printf("\033[31m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// RED
         case  1: printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Green
         case  2: printf("\033[33m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Yellow
         case  3: printf("\033[34m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Blue
         case  4: printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; /// Magenta
         default: printf("%3d(%2d)|", idDetMap[i], idKindMap[i]); break; /// no color
       }
     }else{
       printf("%7s|", "");
     }
  }
  printf("\n");
  gClock.Reset();
  gClock.Start("timer");
  
  timeBegin = 0;
  timeEnd = 0;

  printf("====================== started \n");

}

void GeneralSort::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

void GeneralSort::CheckTime(ULong64_t timeStamp, Long64_t entry){
  
  if( timeStamp == 0 ) return;
  ///if( timeStamp > 7.88e+15) return; // this is too big to be the right timestamp
  if( timeBegin == 0 ) timeBegin = timeStamp;
  
  float frac = 0.01;
  if( timeStamp < timeBegin  && entry < NumEntries*frac ) timeBegin = timeStamp; /// 1st 1 % of event
  if( timeStamp > timeEnd  && entry > NumEntries*(1.0-frac) ) timeEnd = timeStamp; /// last 1 % of event
  
}

Bool_t GeneralSort::Process(Long64_t entry)
{ 
  ///=============================== Get Run Number
  if( entry == 0 ) {
      fileNum = fChain->GetDirectory()->GetName();
      
      ///printf("----------------------- openning  %s \n", fileNum.Data());
      
      int findslat = fileNum.Last('/');
      fileNum.Remove(0, findslat+1);
      int found = fileNum.First(".");
      fileNum.Remove(found);
      ///the fileNum should be something like "xxx_run" now
      while( !fileNum.IsDigit() ){
         fileNum.Remove(0,1);
      }
      runIDPresent = fileNum.Atoi();
   } 
  //  psd.runID = runIDPresent; 

  
  ProcessedEntries++;

  ///=============================== Zero struct
  for (Int_t i=0; i < NARRAY; i++) {
    psd.Energy[i]=TMath::QuietNaN();
    psd.XF[i]=TMath::QuietNaN();
    psd.XN[i]=TMath::QuietNaN();
    psd.x[i]=TMath::QuietNaN();
    psd.Ring[i]=0.;
    
    psd.EnergyTimestamp[i] = 0;
    psd.XFTimestamp[i]     = 0; 
    psd.XNTimestamp[i]     = 0; 
    psd.RingTimestamp[i]   = 0;
  }
  
  for (Int_t i=0; i < NRDT; i++) {
    psd.RDT[i]=TMath::QuietNaN();
    psd.RDTTimestamp[i]	 = 0; 
  }
  
  for (Int_t i=0; i < NTAC; i++) {
    psd.TAC[i]=TMath::QuietNaN();
    psd.TACTimestamp[i]	 = 0; 
  }
  
  for (Int_t i=0; i < NELUM; i++) {
    psd.ELUM[i]=TMath::QuietNaN();
    psd.ELUMTimestamp[i] = 0;
  }
  
  for (Int_t i=0; i < NEZERO; i++) {
    psd.EZERO[i]=TMath::QuietNaN();
    psd.EZEROTimestamp[i]= 0;
  }
  
  for (Int_t i=0; i < NCRDT; i++) {
    psd.CRDT[i]=TMath::QuietNaN();
    psd.CRDTTimestamp[i]= 0;
  }
  
  for (Int_t i=0; i < NAPOLLO; i++) {    
    psd.APOLLO[i]=TMath::QuietNaN();
    psd.APOLLOTimestamp[i]= 0;
  }

  ///=============================== Pull needed entries
  b_NumHits->GetEntry(entry);
  b_id->GetEntry(entry);
  b_pre_rise_energy->GetEntry(entry);
  b_post_rise_energy->GetEntry(entry);
  //   b_base_sample->GetEntry(entry);
  //   b_baseline->GetEntry(entry);
  b_event_timestamp->GetEntry(entry);

  //ID PSD Channels
  Int_t idKind = -1;
  Int_t idDet  = -1; // Detector number
    
  ///==============================================================
  /** --------------------- Loop over NumHits ------------------ */
  ///==============================================================
  for (Int_t i=0;i<NumHits;i++) {
    Int_t idTemp = id[i] - idConst;
    idDet = idDetMap[idTemp];
    idKind = idKindMap[idTemp];
    
    CheckTime(event_timestamp[i], entry);
    
    ///=============================== PSD
    if ( 0 <= idDet && idDet < NARRAY && 0 <= idKind && idKind <= 3 ) {         
      switch(idKind)
        {
        case 0: /* Energy signal */
          psd.Energy[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
          psd.EnergyTimestamp[idDet] = event_timestamp[i];
          break;
        case 1: // XF
          psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN* POLARITY_XFXN;
          psd.XFTimestamp[idDet] = event_timestamp[i];
          break;
        case 2: // XN
          psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN* POLARITY_XFXN;
          psd.XNTimestamp[idDet] = event_timestamp[i];
          break;
        case 3: // Ring
          psd.Ring[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
          psd.RingTimestamp[idDet] = event_timestamp[i];
          break;
        default:
          ;
          break;// 
        }

    }
      
    ///=============================== RECOIL
    if ( idDet >= 100 && idDet <= 100 + NRDT ) {
      Int_t rdtID = idDet-100;
      psd.RDT[rdtID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN* POLARITY_RDT ;
      psd.RDTTimestamp[rdtID] = event_timestamp[i];
    }

    ///=============================== ELUM
    if ( NELUM > 0 && idDet >= 200 && idDet <= 200 + NELUM ) {
      Int_t elumID = idDet - 200;
      psd.ELUM[elumID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_ELUM;
      psd.ELUMTimestamp[elumID] = event_timestamp[i];
    }
    
    ///=============================== EZERO
    if ( NEZERO > 0 && idDet >= 300 && idDet <= 300 + NEZERO ) {
      Int_t ezeroID = idDet - 300;
      psd.EZERO[ezeroID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN * POLARITY_EZERO;
      psd.EZEROTimestamp[ezeroID] = event_timestamp[i];
    }

    ///=============================== TAC & RF TIMING
    if ( NTAC > 0 && idDet >= 400 && idDet <= 400 + NTAC ) {   
      Int_t tacID = idDet - 400;
      psd.TAC[tacID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
      psd.TACTimestamp[tacID] = event_timestamp[i];
      if( event_timestamp[i] < timeBegin ) timeBegin = event_timestamp[i];
    }
    
    ///=============================== Circular-Recoil
    if ( NCRDT > 0 && idDet >= 500 && idDet <= 500 + NCRDT ) {
      Int_t crdtID = idDet - 500;
      psd.CRDT[crdtID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN * POLARITY_CRDT;
      psd.CRDTTimestamp[crdtID] = event_timestamp[i];
    }

    ///=============================== APOLLO
    if ( NAPOLLO > 0 && idDet >= 600 && idDet <= 600 + NAPOLLO ) {
      Int_t apolloID = idDet - 600;
      psd.APOLLO[apolloID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN * POLARITY_APOLLO;
      psd.APOLLOTimestamp[apolloID] = event_timestamp[i];
    }
    
  } // End NumHits Loop
    
  ///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  CalculateX
  for(int i = 0 ; i < NARRAY; i++){
    psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
  }

  ///Progress display
  /************************************************************************/
  oFile->cd(); ///set focus on this file
  gen_tree->Fill();  

  gClock.Stop("timer");
  Double_t time = gClock.GetRealTime("timer");
  gClock.Start("timer");

  if ( !shown ) {
    if (fmod(time, 10) < 1 ){
        printf( "%10lld[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
              entry, 
              TMath::Nint((entry+1)*100./NumEntries),
              TMath::Floor(time/60.), 
              time - TMath::Floor(time/60.)*60.,
              NumEntries*time/(entry+1.)/60.);
        shown = 1;
        gen_tree->Write();
    }
  }else{
    if (fmod(time, 10) > 9 ){
        shown = 0;
    }
  }

  return kTRUE;
}

void GeneralSort::SlaveTerminate()
{

}

void GeneralSort::Terminate()
{
  gen_tree->Write();
  int savedEntries = gen_tree->GetEntries();
  
  TMacro info;
  info.AddLine(Form("%llu", timeBegin));
  info.AddLine(Form("%llu", timeEnd));
  info.AddLine(Form("%d", savedEntries));
  info.AddLine(Form("%d", runIDPresent));
  info.Write("info");
  
  oFile->Close();
  
  printf("=======================================================\n");
  printf(" Total processed entries : %3.1f k/%3.1f k [%4.1f%%] \n",NumEntries/1000.0, NumEntries/1000., NumEntries*100./NumEntries);
  gClock.Stop("timer");
  Double_t time = gClock.GetRealTime("timer");
  printf(" Total run time  : %6.0f sec \n", time);
  printf(" Sorting rate    : %6.3f k/sec\n",NumEntries/time/1000.0);
  printf(" Start TimeStamp : %llu = %.3f sec \n", timeBegin, timeBegin/1e8);
  printf(" End   TimeStamp : %llu = %.3f sec \n", timeEnd, timeEnd/1e8);
  printf(" Run duration    : %.3f sec = %.3f min \n", (timeEnd - timeBegin)/1e8, (timeEnd - timeBegin)/1e8/60.);
  printf(" saved as \e[31m %s \e[m. events saved: %d\n", saveFileName.Data() , savedEntries); 
  printf("=======================================================\n");
  gROOT->ProcessLine(".q");
}
