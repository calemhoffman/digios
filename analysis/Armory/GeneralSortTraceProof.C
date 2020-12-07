#define GeneralSortTraceProof_cxx

#include "GeneralSortTraceProof.h"

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity
#define M -100 //M value for energy filter from digi setting

//by copy the GeneralSortMapping.h in to Armory does not work
//relative path does not work
#ifdef __linux__
   //#include "/lcrc/project/HELIOS/digios/analysis/working/GeneralSortMapping.h"
   //#include "/home/ttang/digios/analysis/working/GeneralSortMapping.h"   
#elif __APPLE__
   #include "/Users/heliosdigios/digios/analysis/working/GeneralSortMapping.h"
  //#include "/Users/mobileryan/digios/analysis/working/GeneralSortMapping.h"
#endif

//=================================== setting
bool isTraceON = true;
bool isSaveTrace = true;
bool isSaveFitTrace = true;
int traceMethod = 1; //0 = no process; 1 = fit;
float delayChannel = 150.; //initial guess of the time

bool isTACRF = false;
bool isRecoil = true;
bool isElum = false;
bool isEZero = true;
//=================================== end of setting

void GeneralSortTraceProof::Begin(TTree */*tree*/)
{

   TString option = GetOption();

   printf( "=====================================================\n");
   printf( "==========  GeneralSortTraceProof.C =================\n");
   printf( "============  General Sort w/ Trace  ================\n");
   printf( "=====================================================\n");
   printf( "  TAC/RF : %s \n", isTACRF ?  "On" : "Off");
   printf( "  Recoil : %s \n", isRecoil ? "On" : "Off");
   printf( "  Elum   : %s \n", isElum ?   "On" : "Off");
   printf( "  EZero  : %s \n", isEZero ?  "On" : "Off");
   TString traceMethodName;
   switch(traceMethod){
   case 0: traceMethodName = "copy"; break;
   case 1: traceMethodName = "fit"; break;
   }
   printf( "  Trace  : %s , Method: %s, Save: %s \n", 
               isTraceON ?  "On" : "Off", 
               traceMethodName.Data(), 
               isSaveTrace? "Yes": "No:");
   printf( "=====================================================\n");   
   //printf("                    file : %s \n", tree->GetDirectory()->GetName());
   //printf("          Number of Event: %llu \n", tree->GetEntries());
   
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
    if( 110 >= idDetMap[i] && idDetMap[i] >= 100){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // Recoil, Cyan
    }else if( 240 >= idDetMap[i] && idDetMap[i] >= 200){
      printf("\033[91m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // Elum, 
    }else if( 310 >= idDetMap[i] && idDetMap[i] >= 300){
      printf("\033[92m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // EZERO, 
    }else if( 450 >= idDetMap[i] && idDetMap[i] >= 400){
      printf("\033[93m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // EZERO, 
    }else if(  99 >= idDetMap[i] && idDetMap[i] >= 0){    
      switch (idKindMap[i]) {
         case -1: printf("%7s|", ""); break;
         case  0: printf("\033[31m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // RED
         case  1: printf("\033[32m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // Green
         case  2: printf("\033[33m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // Yellow
         case  3: printf("\033[34m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // Blue
         case  4: printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); break; // Magenta
         default: printf("%3d(%2d)|", idDetMap[i], idKindMap[i]); break; // no color
       }
     }else{
       printf("%7s|", "");
     }
   }
   printf("\n");
   printf("\n==================== \n");
   
   saveFileName = option;
   int findslat = saveFileName.Last('/');
   saveFileName.Remove(0, findslat+1);
   saveFileName = "../root_data/trace_" + saveFileName;
   
   printf("Save file Name : %s \n", saveFileName.Data());
   
   SetOption(saveFileName.Data());
   
   printf("====================== Begin. \n");
}

void GeneralSortTraceProof::SlaveBegin(TTree * /*tree*/)
{
   printf("========================= Slave Begin.\n");   
   TString option = GetOption();

   //create tree in slave
   
   saveFileName = option;
   int findslat = saveFileName.Last('/');
   saveFileName.Remove(0, findslat+1);
   saveFileName = "../root_data/trace_" + saveFileName;
   
   proofFile = new TProofOutputFile(saveFileName, "M");
   saveFile = proofFile->OpenFile("RECREATE");
   
   newTree = new TTree("gen_tree","PSD Tree w/ trace");
   newTree->SetDirectory(saveFile);
   newTree->AutoSave();

   newTree->Branch("eventID", &psd.eventID, "eventID/I");
   newTree->Branch("runID", &psd.runID, "runID/I");

   newTree->Branch("e",    psd.Energy,          "Energy[30]/F");
   newTree->Branch("e_t",  psd.EnergyTimestamp, "EnergyTimestamp[30]/l");
   newTree->Branch("ring",    psd.Ring,          "Ring[30]/F");
   newTree->Branch("ring_t",  psd.RingTimestamp, "RingTimestamp[30]/l");
   newTree->Branch("xf",   psd.XF,              "XF[30]/F");
   newTree->Branch("xf_t", psd.XFTimestamp,     "XFTimestamp[30]/l");
   newTree->Branch("xn",   psd.XN,              "XN[30]/F");
   newTree->Branch("xn_t", psd.XNTimestamp,     "XNTimestamp[30]/l");
   newTree->Branch("x",    psd.x,               "x[30]/F"); 

   if( isRecoil){
      newTree->Branch("rdt",   psd.RDT,          "RDT[8]/F");
      newTree->Branch("rdt_t", psd.RDTTimestamp, "RDTTimestamp[8]/l"); 
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
         newTree->Branch("te",             te,  "Trace_Energy[30]/F");
         newTree->Branch("te_r",         te_r,  "Trace_Energy_RiseTime[30]/F");
         newTree->Branch("te_t",         te_t,  "Trace_Energy_Time[30]/F");
         
         if( isRecoil ){
            newTree->Branch("trdt",         trdt,  "Trace_RDT[8]/F");
            newTree->Branch("trdt_t",     trdt_t,  "Trace_RDT_Time[8]/F");
            newTree->Branch("trdt_r",     trdt_r,  "Trace_RDT_RiseTime[8]/F");
         }
         if ( isEZero ) {
            newTree->Branch("tezero",       tezero,    "Trace_ezero[8]/F");
            newTree->Branch("tezero_t",     tezero_t,  "Trace_ezero_Time[8]/F");
            newTree->Branch("tezero_r",     tezero_r,  "Trace_ezero_RiseTime[8]/F");         
         }
      }
   }
  
}

Bool_t GeneralSortTraceProof::Process(Long64_t entry)
{ 
   psd.eventID = entry;
   psd.runID = runIDLast;

   b_NumHits->GetEntry(entry);
   //if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac

/**///======================================= Zero struct
   for (Int_t i=0 ; i< 100; i++) {//num dets
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = 0.0;
      psd.RDT[i]     = TMath::QuietNaN();
      psd.TAC[i]     = TMath::QuietNaN();
      if ( i < 32 ) psd.ELUM[i] = TMath::QuietNaN();
      if ( i < 10 ) psd.EZERO[i] = TMath::QuietNaN();

      psd.EnergyTimestamp[i] = 0;
      psd.XFTimestamp[i]     = 0;
      psd.XNTimestamp[i]     = 0;
      psd.RingTimestamp[i]   = 0;
      psd.RDTTimestamp[i]    = 0;
      psd.TACTimestamp[i]    = 0;
      if (i < 32) psd.ELUMTimestamp[i]   = 0;
      if (i < 10) psd.EZEROTimestamp[i]  = 0;	
      
      psd.x[i]       = TMath::QuietNaN();    
   }
   
   if( isTraceON ){
      for(int i = 0; i < 30; i++){
         te[i]     = TMath::QuietNaN();
         te_r[i]   = TMath::QuietNaN();
         te_t[i]   = TMath::QuietNaN();
         
         if( isRecoil &&  i < 8 ) {
            trdt[i]   = TMath::QuietNaN();
            trdt_t[i] = TMath::QuietNaN();
            trdt_r[i] = TMath::QuietNaN();
         }
         
         if( isEZero &&  i < 8 ) {
            tezero[i]   = TMath::QuietNaN();
            tezero_t[i] = TMath::QuietNaN();
            tezero_r[i] = TMath::QuietNaN();
         }
      }
      
      arr->Clear();
   }

/**///======================================= Pull needed entries
   
   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);
   if( isTraceON ) {
      b_trace->GetEntry(entry);
      b_trace_length->GetEntry(entry);
   }

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
      if( 100 > idDet && idDet >= 0 && 3 >= idKind && idKind >= 0 ) {
         
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
      if( isRecoil && idDet >= 100 && idDet <= 110 ) { 
         Int_t rdtTemp = idDet-100;
         psd.RDT[rdtTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M * (-1);
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
      if( isEZero && ( 300 <= idDet && idDet < 310 )) {
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
         
         //bool isPSDe = (30 > idDet && idDet >= 0 && idKind == 0);
         bool isPSD = (30 > idDet && idDet >= 0);
         bool isRDT  = (130 > idDet && idDet >= 100 );
         bool isezero  = (310 > idDet && idDet >= 300 );
         //if( !isPSD && !isRDT && !isezero) continue;
         
         //if( !isRDT) continue;
         
                  
         gTrace = (TGraph*) arr->ConstructedAt(countTrace);
         gTrace->Clear();
         countTrace ++;
         
         int traceLength = trace_length[i];
         
         //Set gTrace
         if( traceMethod == 0 ){
            for ( long long int j = 0 ; j < traceLength; j++){
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
               case  3: lineColor = 4; break;
               case -1: lineColor = 6; break;
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
               te[idDet]   = gFit->GetParameter(0) * (-1);
               te_t[idDet] = gFit->GetParameter(1);
               te_r[idDet] = gFit->GetParameter(2);
            }
            
            if( 200 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-100;
               trdt[rdtTemp]   = TMath::Abs(gFit->GetParameter(0));
               trdt_t[rdtTemp] = gFit->GetParameter(1);
               trdt_r[rdtTemp] = gFit->GetParameter(2);
            }
            
            if( 310 > idDet && idDet >= 300 ) {
               int ezeroTemp = idDet-300;
               tezero[ezeroTemp]   = TMath::Abs(gFit->GetParameter(0));
               tezero_t[ezeroTemp] = gFit->GetParameter(1);
               tezero_r[ezeroTemp] = gFit->GetParameter(2);
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
   if( saveFile->IsOpen() ){
     TTree * tree = (TTree*) saveFile->FindObjectAny("gen_tree");
     int validCount = tree->GetEntries();
   
     saveFile->Close();
   
     printf("=======================================================\n");
     printf("----- saved as %s. valid event: %d\n", saveFileName.Data() , validCount); 
   }
   
   
}
