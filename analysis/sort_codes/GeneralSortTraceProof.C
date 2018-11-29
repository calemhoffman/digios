#define GeneralSortTraceProof_cxx

#include "GeneralSortTraceProof.h"

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity
#define M 100 //M value for energy filter from digi setting

#include "GeneralSortMapping.h"

//===================== setting
TString saveFileName = "sortedTrace.root"; //TODO add suffix to original file

bool isTraceON = true;
bool isSaveTrace = true;
bool isSaveFitTrace = true;
int traceMethod = 1; //0 = no process; 1 = fit;
int traceLength = 200;
float delayChannel = 100.; //initial guess of the time

bool isTACRF = false;
bool isRecoil = true;
bool isElum = false;
bool isEZero = false;

void GeneralSortTraceProof::Begin(TTree * /*tree*/)
{

   TString option = GetOption();

   printf( "=====================================================\n");
   printf( "==========  GeneralSortTraceProof.C ================= \n");
   printf( "============  General Sort w/ Trace  ================\n");
   printf( "=====================================================\n");
   printf( "  TAC/RF : %s \n", isTACRF ?  "On" : "Off");
   printf( "  Recoil : %s \n", isRecoil ? "On" : "Off");
   printf( "  Elum   : %s \n", isElum ?   "On" : "Off");
   printf( "  EZero  : %s \n", isEZero ?  "On" : "Off");
   TString traceMethodName;
   switch(traceMethod){
   case 0: traceMethodName = "simply copy"; break;
   case 1: traceMethodName = "fit"; break;
   }
   printf( "  Trace  : %s , Method: %s, Save: %s \n", isTraceON ?  "On" : "Off", traceMethodName.Data(), isSaveTrace? "Yes": "No:");
   
   printf("====================== Begin. \n");
}

void GeneralSortTraceProof::SlaveBegin(TTree * /*tree*/)
{
   printf("========================= Slave Begin.\n");   
   TString option = GetOption();

   //create tree in slave
   proofFile = new TProofOutputFile(saveFileName);
   saveFile = proofFile->OpenFile("RECREATE");
   
   newTree = new TTree("tree","PSD Tree w/ trace");
   newTree->SetDirectory(saveFile);
   newTree->AutoSave();

   newTree->Branch("eventID", &psd.eventID, "eventID/I");
   newTree->Branch("runID", &psd.runID, "runID/I");

   newTree->Branch("e",    psd.Energy,          "Energy[24]/F");
   newTree->Branch("e_t",  psd.EnergyTimestamp, "EnergyTimestamp[24]/l");
   newTree->Branch("xf",   psd.XF,              "XF[24]/F");
   newTree->Branch("xf_t", psd.XFTimestamp,     "XFTimestamp[24]/l");
   newTree->Branch("xn",   psd.XN,              "XN[24]/F");
   newTree->Branch("xn_t", psd.XNTimestamp,     "XNTimestamp[24]/l");
   newTree->Branch("x",    psd.x,               "x[24]/F"); 

   if( isRecoil){
      newTree->Branch("rdt",   psd.RDT,          "RDT[24]/F");
      newTree->Branch("rdt_t", psd.RDTTimestamp, "RDTTimestamp[24]/l"); 
   }
   
   if( isTACRF ){
      newTree->Branch("tac",   psd.TAC,          "TAC[24]/F");
      newTree->Branch("tac_t", psd.TACTimestamp, "TACTimestamp[24]/l"); 
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
         newTree->Branch("te",             te,  "Trace_Energy[24]/F");
         newTree->Branch("te_r",         te_r,  "Trace_Energy_RiseTime[24]/F");
         newTree->Branch("te_t",         te_t,  "Trace_Energy_Time[24]/F");
         newTree->Branch("trdt",         trdt,  "Trace_RDT[8]/F");
         newTree->Branch("trdt_t",     trdt_t,  "Trace_RDT_Time[8]/F");
         newTree->Branch("trdt_r",     trdt_r,  "Trace_RDT_RiseTime[8]/F");
      }
   }
  
}

Bool_t GeneralSortTraceProof::Process(Long64_t entry)
{ 
   psd.eventID = entry;
   psd.runID = runIDLast;

   b_NumHits->GetEntry(entry);
   if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac

/**///======================================= Zero struct
   for (Int_t i=0 ; i<24; i++) {//num dets
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = TMath::QuietNaN();
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
      for(int i = 0; i < 24; i++){
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
   if( isTraceON ) b_trace->GetEntry(entry);

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
      if( (id[i] > 1000 && id[i] < 2000) &&  30> idDet && idDet>-1 ) {
         
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
         }
      }
      
      //TAC & RF TIMING
      /***********************************************************************/
      if( isTACRF && id[i] > 1160 && id[i] < 1171) { //RF TIMING SWITCH
         switch(id[i]){
            case 1163: //
               psd.TAC[0] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[0] = event_timestamp[i];
               break;
            case 1164: // 
               psd.TAC[1] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[1] = event_timestamp[i];
               break;
            case 1165: // 
               psd.TAC[2] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[2] = event_timestamp[i];
            case 1167: // 
               psd.TAC[3] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[3] = event_timestamp[i];
            case 1168: //
               psd.TAC[4] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[4] = event_timestamp[i];
            case 1169: //
               psd.TAC[5] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.TACTimestamp[5] = event_timestamp[i];
            break;
         }
      }

      //RECOIL
      /************************************************************************/
      if( isRecoil && (id[i]>1000&&id[i]<2000)&&(idDet>=100&&idDet<=110)) { //recOILS
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
      }//end ELUM

      //EZERO
      /************************************************************************/
      if( isEZero && (id[i]>1000&&id[i]<2000)&&(idDet>=300&&idDet<310)) {
         Int_t ezeroTemp = idDet - 300;
         if (ezeroTemp<4) {
            psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.EZEROTimestamp[ezeroTemp] = event_timestamp[i];
         }
      }//end EZERO
      
   }//end of NumHits
   
   for(int i = 0 ; i < 24; i++){
      psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
   }
   
   //Trace
   /************************************************************************/
   if( isTraceON ) {
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
            
            int lineColor = 1;
            switch(idKind){
               case  0: lineColor = 3; break;
               case  1: lineColor = 1; break;
               case  2: lineColor = 2; break;
               case -1: lineColor = 4; break;
            }
            
            gFit->SetLineColor(lineColor);
            gFit->SetRange(0, traceLength);

            base = gTrace->Eval(1);
            double fileNameTemp = gTrace->Eval(delayChannel*1.5) - base;

            gFit->SetParameter(0, fileNameTemp); //energy
            gFit->SetParameter(1, delayChannel); // time
            gFit->SetParameter(2, 1); //riseTime
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

   //Fill
   /************************************************************************/
   newTree->Fill();  
   
   return kTRUE;
}

void GeneralSortTraceProof::SlaveTerminate()
{
   printf("========================= Slave Terminate.\n");
   
   // i don't know why it works...
   saveFile->cd();
   newTree->Write();
   fOutput->Add(proofFile);
   saveFile->Close();
   
}

void GeneralSortTraceProof::Terminate()
{

   printf("========================= Terminate.\n");
   
   // i don't know why it works...
   proofFile = dynamic_cast<TProofOutputFile*>(fOutput->FindObject(saveFileName));
   saveFile = TFile::Open(saveFileName);
   
   //get entries
   TTree * tree = (TTree*) saveFile->FindObjectAny("tree");
   int validCount = tree->GetEntries();
   
   saveFile->Close();
   
   printf("=======================================================\n");
   printf("----- saved as %s. valid event: %d\n", saveFileName.Data() , validCount); 
   
}
