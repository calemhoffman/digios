#define GeneralSort_cxx

#include "GeneralSort.h"
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>
#include <TBenchmark.h>

#define NUMPRINT 20 //>0
#define MAXNUMHITS 200 //Highest multiplicity
#define M -100 //M value for energy filter from digi setting, number of channel

ULong64_t MaxProcessedEntries=100000000;
ULong64_t NumEntries = 0;
int EffEntries;
ULong64_t ProcessedEntries = 0;

#include "../working/GeneralSortMapping.h"

//clock
TBenchmark gClock;
Bool_t shown = 0;

TFile *oFile;
TTree *gen_tree;

TH1F *hEvents;
TCanvas *cc0;

//PSD struct
typedef struct {
  Int_t runID;
  Float_t Energy[100];
  Float_t XF[100];
  Float_t XN[100];
  Float_t Ring[100];
  Float_t RDT[100];
  Float_t TAC[100];
  Float_t ELUM[32];
  Float_t EZERO[10];//0,1 - DE-E exit, 2,3 - DE-E atscat, 4 - ETOT
// 2,3 - DEX,EX

  ULong64_t EBISTimestamp; 
  ULong64_t T1Timestamp;   // proton pulse, not before run 33
  ULong64_t EnergyTimestamp[100];
  ULong64_t XFTimestamp[100];
  ULong64_t XNTimestamp[100];
  ULong64_t RingTimestamp[100];
  ULong64_t RDTTimestamp[100];
  ULong64_t TACTimestamp[100];
  ULong64_t ELUMTimestamp[32];
  ULong64_t EZEROTimestamp[10];
  
  Float_t x[100];

} PSD;

PSD psd; 

TString saveFileName = "gen.root";

int runIDPresent = 0;
TString fileNum = "";

void GeneralSort::Begin(TTree * tree)
{
   
  TString option = GetOption();
  NumEntries = tree->GetEntries();
  EffEntries = TMath::Min(MaxProcessedEntries, NumEntries);
  
  saveFileName = tree->GetDirectory()->GetName();
  int findslat = saveFileName.Last('/');
  saveFileName.Remove(0, findslat+1);
  saveFileName = "../root_data/gen_" + saveFileName;

  hEvents = new TH1F("hEvents","Number of events; Events;",NumEntries*1.2,0,NumEntries*1.2);

  oFile = new TFile(saveFileName,"RECREATE");

  gen_tree = new TTree("gen_tree","PSD Tree");

  gen_tree->Branch("runID", &psd.runID,"runID/I");

  gen_tree->Branch("e",psd.Energy,"Energy[100]/F");
  gen_tree->Branch("e_t",psd.EnergyTimestamp,"EnergyTimestamp[100]/l");
  
  gen_tree->Branch("EBIS",&psd.EBISTimestamp,"EBISTimestamp/l");
  gen_tree->Branch("T1",&psd.T1Timestamp,"T1Timestamp/l");
  
  gen_tree->Branch("ring",psd.Ring,"Ring[100]/F");
  gen_tree->Branch("ring_t",psd.RingTimestamp,"RingTimestamp[100]/l");

  gen_tree->Branch("xf",psd.XF,"XF[100]/F");
  gen_tree->Branch("xf_t",psd.XFTimestamp,"XFTimestamp[100]/l");
 
  gen_tree->Branch("xn",psd.XN,"XN[100]/F");
  gen_tree->Branch("xn_t",psd.XNTimestamp,"XNTimestamp[100]/l"); 
  
  gen_tree->Branch("x",psd.x,"x[100]/F");

  gen_tree->Branch("rdt",psd.RDT,"RDT[100]/F");
  gen_tree->Branch("rdt_t",psd.RDTTimestamp,"RDTTimestamp[100]/l"); 

  gen_tree->Branch("tac",psd.TAC,"TAC[100]/F");
  gen_tree->Branch("tac_t",psd.TACTimestamp,"TACTimestamp[100]/l"); 
  
  gen_tree->Branch("elum",psd.ELUM,"ELUM[32]/F");
  gen_tree->Branch("elum_t",psd.ELUMTimestamp,"ELUMTimestamp[32]/l"); 

  gen_tree->Branch("ezero",psd.EZERO,"EZERO[10]/F");
  gen_tree->Branch("ezero_t",psd.EZEROTimestamp,"EZEROTimestamp[10]/l"); 

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
    switch (idKindMap[i]) {
       case 0: printf("\033[31m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       case 1: printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       case 2: printf("\033[33m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       case 3: printf("\033[34m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       case 4: printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       case 5: printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
       default: printf("%3d(%2d)|", idDetMap[i], idKindMap[i]); break; // no color
     }
  }
  printf("\n");
  gClock.Reset();
  gClock.Start("timer");

  printf("====================== started \n");

}

void GeneralSort::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

Bool_t GeneralSort::Process(Long64_t entry)
{ 
  //=============================== Get Run Number
  if( entry == 0 ) {
      fileNum = fChain->GetDirectory()->GetName();
      
      printf("----------------------- openning  %s \n", fileNum.Data());
      
      int findslat = fileNum.Last('/');
      fileNum.Remove(0, findslat+1);
      int found = fileNum.First(".");
      fileNum.Remove(found);
      //the fileNum should be something like "xxx_run4563" now
      while( !fileNum.IsDigit() ){
         fileNum.Remove(0,1);
      }
      runIDPresent = fileNum.Atoi();
   } 
   psd.runID = runIDPresent; 
  
  
  
  ProcessedEntries++;
  if (ProcessedEntries<MaxProcessedEntries) {
    hEvents->Fill(ProcessedEntries);
    
    psd.EBISTimestamp=TMath::QuietNaN();
    psd.T1Timestamp=TMath::QuietNaN();

    //=============================== Zero struct
    for (Int_t i=0;i<100;i++) {//num dets
      psd.Energy[i]=TMath::QuietNaN();
      psd.XF[i]=TMath::QuietNaN();
      psd.XN[i]=TMath::QuietNaN();
      psd.x[i]=TMath::QuietNaN();
      psd.Ring[i]=0.;
      psd.RDT[i]=TMath::QuietNaN();
      psd.TAC[i]=TMath::QuietNaN();
      if (i<32) psd.ELUM[i]=TMath::QuietNaN();
      if (i<10) psd.EZERO[i]=TMath::QuietNaN();
    
      psd.EnergyTimestamp[i]=TMath::QuietNaN();
      psd.XFTimestamp[i]=TMath::QuietNaN();
      psd.XNTimestamp[i]=TMath::QuietNaN();
      psd.RingTimestamp[i]=TMath::QuietNaN();
      psd.RDTTimestamp[i]=TMath::QuietNaN();
      psd.TACTimestamp[i]=TMath::QuietNaN();
      if (i<32) psd.ELUMTimestamp[i]=TMath::QuietNaN();
      if (i<10) psd.EZEROTimestamp[i]=TMath::QuietNaN();	    
    }

    
    //=============================== Pull needed entries
    b_NumHits->GetEntry(entry);
    b_id->GetEntry(entry);
    b_pre_rise_energy->GetEntry(entry);
    b_post_rise_energy->GetEntry(entry);
    //   b_base_sample->GetEntry(entry);
    //    b_baseline->GetEntry(entry);
    b_event_timestamp->GetEntry(entry);

    //ID PSD Channels
    Int_t idKind = -1;
    Int_t idDet=-1; // Detector number
    Int_t idConst=1010; //Temp value to get idDet
    
    //==============================================================
    /* --------------------- Loop over NumHits ------------------ */
    //==============================================================
    for (Int_t i=0;i<NumHits;i++) {
      Int_t idTemp = id[i] - idConst;
      idDet = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
      
      
      //=============================== PSD
      if ( 0 <= idDet && idDet < 100 && 0 <= idKind && idKind <= 3 ) {         
        switch(idKind)
          {
          case 0: /* Energy signal */
            psd.Energy[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.EnergyTimestamp[idDet] = event_timestamp[i];
            break;
          case 1: // XF
            psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.XFTimestamp[idDet] = event_timestamp[i];
            break;
          case 2: // XN
            psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.XNTimestamp[idDet] = event_timestamp[i];
            break;
          case 3: // Ring
            psd.Ring[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.RingTimestamp[idDet] = event_timestamp[i];
            break;
          default:
            ;
            break;// 
          }
      }

      //=============================== TAC & RF TIMING
      //if ((id[i]>1000&&id[i]<2000)&&(idDet>=400&&idDet<=450)) {
      if ( idDet >= 400 && idDet <= 450 ) {   
        Int_t tacID = idDet - 400;
        psd.TAC[tacID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
        psd.TACTimestamp[tacID] = event_timestamp[i];
      }
       
      //=============================== RECOIL
      //if ((id[i]>1000&&id[i]<2000)&&(idDet>=100&&idDet<=110)) {
      if ( idDet >= 100 && idDet <= 110 ) {
        Int_t rdtID = idDet-100;
        psd.RDT[rdtID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
        psd.RDTTimestamp[rdtID] = event_timestamp[i];
      }

      //=============================== ELUM
      //if ((id[i]>=1000 && id[i]<1130)&&(idDet>=200&&idDet<=240)) {
      if ( idDet >= 200 && idDet <= 240 ) {
        Int_t elumID = idDet - 200;
        psd.ELUM[elumID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M * (-1);
        psd.ELUMTimestamp[elumID] = event_timestamp[i];
      }
      
      //=============================== EZERO
      //if ((id[i]>1000&&id[i]<2000)&&(idDet>=300&&idDet<310)) {
      if ( idDet >= 300 && idDet < 310 ) {
        Int_t ezeroID = idDet - 300;
        psd.EZERO[ezeroID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/M;
        psd.EZEROTimestamp[ezeroID] = event_timestamp[i];
      }
      
      //=============================== EBIS 
      if (id[i]==1010) {
        psd.EBISTimestamp = event_timestamp[i];
      }
         
      //=============================== T1 proton pulse
      if (id[i]==1013) {
        psd.T1Timestamp = event_timestamp[i];
      }
      
    } // End NumHits Loop
    
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  CalculateX
    for(int i = 0 ; i < 30; i++){
      psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
    }

    //Progress display
    /************************************************************************/
    oFile->cd(); //set focus on this file
    gen_tree->Fill();  

    gClock.Stop("timer");
    Double_t time = gClock.GetRealTime("timer");
    gClock.Start("timer");

    if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry+1)*100./EffEntries),
               TMath::Floor(time/60.), 
               time - TMath::Floor(time/60.)*60.,
               EffEntries*time/(entry+1.)/60.);
         shown = 1;
         gen_tree->Write();
      }
    }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
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
  oFile->Close();
  
  //  cc0 = new TCanvas("cc0","cc0",800,600);
  // cc0->Clear(); hEvents->Draw();  
  
  printf("=======================================================\n");
  printf(" Total processed entries : %3.1f k/%3.1f k [%4.1f%%] \n",EffEntries/1000.0, NumEntries/1000., EffEntries*100./NumEntries);
  gClock.Stop("timer");
  Double_t time = gClock.GetRealTime("timer");
  printf(" Total run time : %6.0f sec \n", time);
  printf(" Sorting rate   : %6.3f k/sec\n",EffEntries/time/1000.0);
  printf(" saved as \e[31m %s \e[m. events saved: %d\n", saveFileName.Data() , savedEntries); 
  printf("=======================================================\n");
  gROOT->ProcessLine(".q");
}
