#define GeneralSortTrace_cxx

#include "GeneralSortTrace.h"
#include "TROOT.h"
#include <TH2.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TBenchmark.h>

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity

#include "../working/GeneralSortMapping.h"

//================== TODO to be a setup file
const int nDet=30; //number of PSD detector

TString saveFileName;
TFile *saveFile; //!
TTree *newTree; //!

ULong64_t MaxProcessedEntries=1000000000;
int EffEntries;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;

bool isTraceON = true;
bool isSaveTrace = true;
bool isSaveFitTrace = true;
int traceMethod = 1; //0 = no process, 1, fit, 2, Trapezoid (not implement yet) //TODO
int traceLength = 600;

bool isTACRF = true;
bool isRecoil = true;
bool isElum = false;
bool isEZero = false;

//trace
TClonesArray * arr ;//!
TGraph * gTrace; //!
TF1 * gFit; //!

float te[nDet];    // energy from trace
float te_r[nDet];  // rising time from frace
float te_t[nDet];  // time
float ttac[6];
float ttac_t[6];
float ttac_r[6];
float trdt[8];
float trdt_t[8];
float trdt_r[8];

//clock
TBenchmark gClock;
Bool_t shown = 0;

//runID
int runIDPresent = 0;
TString fileNum = "";


//PSD struct
typedef struct {
  Int_t   eventID;
  Int_t runID;
  Float_t Energy[NARRAY];
  Float_t XF[NARRAY];
  Float_t XN[NARRAY];
  Float_t Ring[NARRAY];
  Float_t RDT[NRDT];
  Float_t TAC[NTAC];
  Float_t ELUM[NELUM];
  Float_t EZERO[NEZERO];//0,1 - DE-E exit, 2,3 - DE-E atscat, 4 - ETOT
  // 2,3 - DEX,EX

  ULong64_t EnergyTimestamp[NARRAY];
  ULong64_t XFTimestamp[NARRAY];
  ULong64_t XNTimestamp[NARRAY];
  ULong64_t RingTimestamp[NARRAY];
  ULong64_t RDTTimestamp[NRDT];
  ULong64_t TACTimestamp[NTAC];
  ULong64_t ELUMTimestamp[NELUM];
  ULong64_t EZEROTimestamp[NEZERO];
  
  Float_t x[NARRAY];

} PSD;

PSD psd; 

void GeneralSortTrace::Begin(TTree * tree)
{
   
   TString option = GetOption();
   NumEntries = tree->GetEntries();
   EffEntries = TMath::Min(MaxProcessedEntries, NumEntries);

   printf( "=====================================================\n");
   printf( "===============  GeneralSortTrace.C =================\n");
   printf( "============  General Sort w/ Trace  ================\n");
   printf( "=====================================================\n");
   printf( "========== Make a new tree with trace, total Entry : %llu, use : %d [%4.1f%%]\n", NumEntries, EffEntries, EffEntries*100./NumEntries);
   printf( "  TAC/RF : %s \n", isTACRF ?  "On" : "Off");
   printf( "  Recoil : %s \n", isRecoil ? "On" : "Off");
   printf( "  Elum   : %s \n", isElum ?   "On" : "Off");
   printf( "  EZero  : %s \n", isEZero ?  "On" : "Off");
   printf( "  Trace  : %s , Method: %s, Save: %s \n", isTraceON ?  "On" : "Off", traceMethod==0 ? "no-Fit" : "Fit" , isSaveTrace? "Yes": "No:");

   //================= save file Name
   saveFileName = tree->GetDirectory()->GetName();
   int findslat = saveFileName.Last('/');
   saveFileName.Remove(0, findslat+1);
   saveFileName = "../root_data/trace_" + saveFileName;
   printf("Output Root File : %s \n", saveFileName.Data());

   saveFile = new TFile(saveFileName,"RECREATE");
   newTree = new TTree("gen_tree","PSD Tree w/ trace");

   newTree->Branch("eventID", &psd.eventID, "eventID/I");
   newTree->Branch("runID", &psd.runID,"runID/I");
   
   newTree->Branch("e",      psd.Energy,          Form("Energy[%d]/F", nDet));
   newTree->Branch("e_t",    psd.EnergyTimestamp, Form("EnergyTimestamp[%d]/l", nDet));
   newTree->Branch("xf",     psd.XF,              Form("XF[%d]/F", nDet));
   newTree->Branch("xf_t",   psd.XFTimestamp,     Form("XFTimestamp[%d]/l", nDet));
   newTree->Branch("xn",     psd.XN,              Form("XN[%d]/F", nDet));
   newTree->Branch("xn_t",   psd.XNTimestamp,     Form("XNTimestamp[%d]/l", nDet));
   newTree->Branch("ring",   psd.Ring,            Form("Ring[%d]/F", nDet));
   newTree->Branch("ring_t", psd.RingTimestamp,   Form("RingTimestamp[%d]/l", nDet));
   newTree->Branch("x",      psd.x,               Form("x[%d]/F", nDet)); 

   if( isRecoil){
      newTree->Branch("rdt",   psd.RDT,          Form("RDT[%d]/F", nDet));
      newTree->Branch("rdt_t", psd.RDTTimestamp, Form("RDTTimestamp[%d]/l", nDet)); 
   }
   
   if( isTACRF ){
      newTree->Branch("tac",   psd.TAC,          Form("TAC[%d]/F", nDet));
      newTree->Branch("tac_t", psd.TACTimestamp, Form("TACTimestamp[%d]/l", nDet)); 
   }
   
   if( isElum ) {
      newTree->Branch("elum",   psd.ELUM,          "ELUM[32]/F");
      newTree->Branch("elum_t", psd.ELUMTimestamp, "ELUMTimestamp[32]/l"); 
   }
   
   if( isEZero ){
      newTree->Branch("ezero",   psd.EZERO,          "EZERO[4]/F");
      newTree->Branch("ezero_t", psd.EZEROTimestamp, "EZEROTimestamp[4]/l"); 
   }
   if( isTraceON ){
      arr = new TClonesArray("TGraph");
         
      if( isSaveTrace){
         newTree->Branch("trace", arr, 256000);
         arr->BypassStreamer();
      }
      
      if( traceMethod > 0 ){
	      gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
         newTree->Branch("te",             te,  "Trace_Energy[nDet]/F");
         newTree->Branch("te_r",         te_r,  "Trace_Energy_RiseTime[nDet]/F");
         newTree->Branch("te_t",         te_t,  "Trace_Energy_Time[nDet]/F");
         newTree->Branch("trdt",         trdt,  "Trace_RDT[8]/F");
         newTree->Branch("trdt_t",     trdt_t,  "Trace_RDT_Time[8]/F");
         newTree->Branch("trdt_r",     trdt_r,  "Trace_RDT_RiseTime[8]/F");
      }
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
    if( 110 > idDetMap[i] && idDetMap[i] >= 100 ){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]);; // Cyan
    }else{ 
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
  }
  printf("\n");
  gClock.Reset();
  gClock.Start("timer");
   
  printf("====================== started \n");
}

void GeneralSortTrace::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

Bool_t GeneralSortTrace::Process(Long64_t entry)
{ 
   psd.eventID = entry;
   
   ProcessedEntries++;
   if(ProcessedEntries >= MaxProcessedEntries) return kTRUE;
   
   b_NumHits->GetEntry(entry);
   //if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac
   
/**///======================================== extract runID from fileName
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
   

/**///======================================= Zero struct
   for (Int_t i=0 ; i<nDet; i++) {//num dets
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = 0.0;
      psd.RDT[i]     = TMath::QuietNaN();
      psd.TAC[i]     = TMath::QuietNaN();
      if (i<32) psd.ELUM[i] = TMath::QuietNaN();
      if (i<4) psd.EZERO[i] = TMath::QuietNaN();

      psd.EnergyTimestamp[i] = TMath::QuietNaN();
      psd.XFTimestamp[i]     = TMath::QuietNaN();
      psd.XNTimestamp[i]     = TMath::QuietNaN();
      psd.RingTimestamp[i]   = TMath::QuietNaN();
      psd.RDTTimestamp[i]    = TMath::QuietNaN();
      psd.TACTimestamp[i]    = TMath::QuietNaN();
      if (i<32) psd.ELUMTimestamp[i] = TMath::QuietNaN();
      if (i<4) psd.EZEROTimestamp[i] = TMath::QuietNaN();	
      
      psd.x[i]       = TMath::QuietNaN();    
   }
   
   if( isTraceON ){
      for(int i = 0; i < nDet; i++){
         te[i]     = TMath::QuietNaN();
         te_r[i]   = TMath::QuietNaN();
         te_t[i]   = TMath::QuietNaN();
         
         if( i < 8 ) {
            trdt[i]   = TMath::QuietNaN();
            trdt_t[i] = TMath::QuietNaN();
            trdt_r[i] = TMath::QuietNaN();
         }
      }
      
      arr->Clear();
   }

/**///======================================= Pull needed entries
   
   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);
   if( isTraceON && isTraceDataExist ) b_trace->GetEntry(entry);

   //ID PSD Channels
   Int_t idKind  = -1;
   Int_t idDet   = -1; // Detector number
   
   /* -- Loop over NumHits -- */
   for(Int_t i=0;i<NumHits;i++) {    
      Int_t idTemp   = id[i] - idConst;
      idDet  = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
      
      //PSD
      /***********************************************************************/
      if( (id[i] > 1000 && id[i] < 2000) &&  nDet > idDet && idDet>-1 ) {
         
         switch(idKind){
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
         }
      }
      
      //TAC & RF TIMING
      /***********************************************************************/
      if( isTACRF && idDet >= 400 && idDet <= 450 ) {   
        Int_t tacID = idDet - 400;
        psd.TAC[tacID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
        psd.TACTimestamp[tacID] = event_timestamp[i];
      }

      //RECOIL
      /************************************************************************/
      if( isRecoil && (id[i]>1000&&id[i]<2000)&&(idDet>=100&&idDet<=110)) { 
         Int_t rdtTemp = idDet-101;
         psd.RDT[rdtTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
         psd.RDTTimestamp[rdtTemp] = event_timestamp[i];
      }
      
      //ELUM
      /************************************************************************/
      if( isElum && (id[i]>=1090 && id[i]<1130)&&(idDet>=200&&idDet<=240)) {
         Int_t elumTemp = idDet - 200;

         if (elumTemp<16) {
            psd.ELUM[elumTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
         } else {
            psd.ELUM[elumTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
         }
         
         psd.ELUMTimestamp[elumTemp] = event_timestamp[i];
      }

      //EZERO
      /************************************************************************/
      if( isEZero && (id[i]>1000&&id[i]<2000)&&(idDet>=300&&idDet<310)) {
         Int_t ezeroTemp = idDet - 300;
         if (ezeroTemp<4) {
            psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.EZEROTimestamp[ezeroTemp] = event_timestamp[i];
         }
      }
      
   }//end of NumHits
   
   for(int i = 0 ; i < nDet; i++){
      psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
   }
   
   //Trace
   /************************************************************************/
   if( isTraceON && isTraceDataExist) {
      int countTrace = 0;
      for(Int_t i = 0; i < NumHits ; i++) {
         Int_t idTemp   = id[i] - idConst;
         idDet  = idDetMap[idTemp];
         idKind = idKindMap[idTemp];
         
         bool isPSDe = (30 > idDet && idDet >= 0 && idKind == 0);
         bool isRDT  = (130 > idDet && idDet >= 100 );
         if( !isPSDe && !isRDT ) continue;
                  
         gTrace = (TGraph*) arr->ConstructedAt(countTrace);
         gTrace->Clear();
         countTrace ++;
         
         //Set gTrace
         
         if( traceMethod == 0 ){
            for ( int j = 0 ; j < traceLength; j++){
               gTrace->SetPoint(j, j, trace[i][j]);
            }
            
            gTrace->SetName(Form("det:%d,kind:%d", idDet, idKind));
            continue;
         }
         
         if( traceMethod == 1){
            double base = 0;
            for( int j = 0; j < traceLength; j++){ 
               if( trace[i][j] < 16000){
                  base = trace[i][j];
                  gTrace->SetPoint(j, j, trace[i][j]);
               }else{
                  gTrace->SetPoint(j, j, base);
               }
            }
            
            //Set gFit
            gFit->SetLineStyle(idDet);
            gFit->SetLineColor(idKind == 0 ? 4 : idKind);
            gFit->SetRange(0, traceLength);

            base = gTrace->Eval(1);
            double temp = gTrace->Eval(80) - base;

            gFit->SetParameter(0, temp); //energy
            gFit->SetParameter(1, 100); // time
            gFit->SetParameter(2, 10); //riseTime
            gFit->SetParameter(3, base);

            //if( gTrace->Eval(120) < base ) gFit->SetRange(0, 100); //sometimes, the trace will drop    
            //if( gTrace->Eval(20) < base) gFit->SetParameter(1, 5); //sometimes, the trace drop after 5 ch

            if( isSaveFitTrace ) {
               gTrace->Fit("gFit", "qR");
            }else{
               gTrace->Fit("gFit", "qR0");
            }
            
            if( 30 > idDet && idDet >= 0 && idKind == 0 ) {
               te[idDet]   = gFit->GetParameter(0);
               te_t[idDet] = gFit->GetParameter(1);
               te_r[idDet] = gFit->GetParameter(2);
            }
            
            if( 200 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-101;
               trdt[rdtTemp]   = TMath::Abs(gFit->GetParameter(0));
               trdt_t[rdtTemp] = gFit->GetParameter(1);
               trdt_r[rdtTemp] = gFit->GetParameter(2);
            }
            
         }
         
      } // End NumHits Loop
   }// end of trace

   //Clock
   /************************************************************************/
   saveFile->cd(); //set focus on this file
   newTree->Fill();  
   
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
         newTree->Write();
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }
   
   return kTRUE;
}

void GeneralSortTrace::SlaveTerminate()
{

}

void GeneralSortTrace::Terminate()
{
   newTree->Write();
   int savedEntries = newTree->GetEntries();
   saveFile->Close();

   printf("=======================================================\n");
   printf("----- Total processed entries : %3.1f k/%3.1f k [%4.1f%%] \n",EffEntries/1000.0, NumEntries/1000., EffEntries*100./NumEntries);
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   printf("----- Total run time : %6.0f sec \n", time);
   printf("----- Sorting Rate   : %6.3f k/sec\n",EffEntries/time/1000.0);
   printf("----- saved as \e[31m %s \e[m. events saved: %d\n", saveFileName.Data() , savedEntries); 
   
   gROOT->ProcessLine(".q");
}
