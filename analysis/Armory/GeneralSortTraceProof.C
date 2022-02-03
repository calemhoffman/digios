#define GeneralSortTraceProof_cxx

#include "GeneralSortTraceProof.h"

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity

//=================================== setting
bool isTraceON = true;
bool isSaveTrace = true;
bool isSaveFitTrace = true;
int traceMethod = 1; //0 = no process; 1 = fit; 2 = trapezoid
float delayChannel = 200.; //initial guess of the time

// Also go to line 146 to set the trace analysis gate

//=================================== end of setting

///------- if the number in GeneralSortMapping.h is zero, the corresponding items will disable.
bool isTACRF  = true;
bool isRecoil = true;
bool isElum   = true;
bool isEZero  = true;
bool isCRDT   = true;

TGraph * GeneralSortTraceProof::TrapezoidFilter(TGraph * trace){
   ///Trapezoid filter https://doi.org/10.1016/0168-9002(94)91652-7

   int baseLineEnd = 80;
   
   int riseTime = 20; //ch
   int flatTop = 20;
   float decayTime = 300;
   
   TGraph *  trapezoid = new TGraph();
   trapezoid->Clear();
   
   ///find baseline;
   double baseline = 0;
   for( int i = 0; i < baseLineEnd; i++){
      baseline += trace->Eval(i);
   }
   baseline = baseline*1./baseLineEnd;
   
   int length = trace->GetN();
   
   double pn = 0.;
   double sn = 0.;
   for( int i = 0; i < length ; i++){
   
      double dlk = trace->Eval(i) - baseline;
      if( i - riseTime >= 0            ) dlk -= trace->Eval(i - riseTime)             - baseline;
      if( i - flatTop - riseTime >= 0  ) dlk -= trace->Eval(i - flatTop - riseTime)   - baseline;
      if( i - flatTop - 2*riseTime >= 0) dlk += trace->Eval(i - flatTop - 2*riseTime) - baseline;
      
      if( i == 0 ){
         pn = dlk;
         sn = pn + dlk*decayTime;
      }else{
         pn = pn + dlk;
         sn = sn + pn + dlk*decayTime;
      }    
      
      trapezoid->SetPoint(i, i, sn / decayTime / riseTime);
    
   }
   
   return trapezoid;
   
}

void GeneralSortTraceProof::Begin(TTree */*tree*/)
{

   TString option = GetOption();

   printf( "=====================================================\n");
   printf( "==========  GeneralSortTraceProof.C =================\n");
   printf( "============  General Sort w/ Trace  ================\n");
   printf( "=====================================================\n");


   if ( isTACRF  && NTAC   == 0 ) isTACRF  = false ;
   if ( isRecoil && NRDT   == 0 ) isRecoil = false ;
   if ( isElum   && NELUM  == 0 ) isElum   = false ;
   if ( isEZero  && NEZERO == 0 ) isEZero  = false ;
   if ( isCRDT   && NCRDT  == 0 ) isCRDT   = false ;

   
   printf( "  TAC/RF   : %s , %d \n", isTACRF  ? "On" : "Off", NTAC);
   printf( "  Recoil   : %s , %d \n", isRecoil ? "On" : "Off", NRDT);
   printf( "  Elum     : %s , %d \n", isElum   ? "On" : "Off", NELUM);
   printf( "  EZero    : %s , %d \n", isEZero  ? "On" : "Off", NEZERO);
   printf( "  C-Recoil : %s , %d \n", isCRDT   ? "On" : "Off", NCRDT);
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
    if( 100 + NRDT >= idDetMap[i] && idDetMap[i] >= 100){
      printf("\033[36m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // Recoil, Cyan
    }else if( 200+NELUM >= idDetMap[i] && idDetMap[i] >= 200){
      printf("\033[91m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // Elum, 
    }else if( 300+NEZERO >= idDetMap[i] && idDetMap[i] >= 300){
      printf("\033[35m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // EZERO, 
    }else if( 400+NTAC >= idDetMap[i] && idDetMap[i] >= 400){
      printf("\033[93m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // TAC,
    }else if( 400+NCRDT >= idDetMap[i] && idDetMap[i] >= 500){
      printf("\033[37m%3d(%2d)\033[0m|", idDetMap[i], idKindMap[i]); // Circular Recoil
    }else if(  NARRAY > idDetMap[i] && idDetMap[i] >= 0){    
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
   
   if ( isTACRF  && NTAC   == 0 ) isTACRF  = false ;
   if ( isRecoil && NRDT   == 0 ) isRecoil = false ;
   if ( isElum   && NELUM  == 0 ) isElum   = false ;
   if ( isEZero  && NEZERO == 0 ) isEZero  = false ;
   if ( isCRDT   && NCRDT  == 0 ) isCRDT   = false ;

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

   newTree->Branch("e",     psd.Energy,          Form("Energy[%d]/F", NARRAY));
   newTree->Branch("e_t",   psd.EnergyTimestamp, Form("EnergyTimestamp[%d]/l",NARRAY));
   newTree->Branch("ring",  psd.Ring,            Form("Ring[%d]/F", NARRAY));
   newTree->Branch("ring_t",psd.RingTimestamp,   Form("RingTimestamp[%d]/l",NARRAY));
   newTree->Branch("xf",    psd.XF,              Form("XF[%d]/F", NARRAY));
   newTree->Branch("xf_t",  psd.XFTimestamp,     Form("XFTimestamp[%d]/l", NARRAY));
   newTree->Branch("xn",    psd.XN,              Form("XN[%d]/F", NARRAY));
   newTree->Branch("xn_t",  psd.XNTimestamp,     Form("XNTimestamp[%d]/l", NARRAY)); 
   newTree->Branch("x",     psd.x,               Form("x[%d]/F", NARRAY));

   if( isRecoil){
      newTree->Branch("rdt"  ,psd.RDT,          Form("RDT[%d]/F",NRDT));
      newTree->Branch("rdt_t",psd.RDTTimestamp, Form("RDTTimestamp[%d]/l", NRDT)); 
   }else{
      printf(" -----  no recoil.\n"); 
   }
   
   if( isTACRF ){
      newTree->Branch("tac"  ,psd.TAC,         Form("TAC[%d]/F", NTAC));
      newTree->Branch("tac_t",psd.TACTimestamp,Form("TACTimestamp[%d]/l", NTAC)); 
   }else{
      printf(" -----  no TAC.\n"); 
   }
   
   if( isElum ) {
      newTree->Branch("elum"  ,psd.ELUM,         Form("ELUM[%d]/F", NELUM));
      newTree->Branch("elum_t",psd.ELUMTimestamp,Form("ELUMTimestamp[%d]/l",NELUM)); 
   }else{
      printf(" -----  no elum\n"); 
   }
   
   if( isEZero ){
      newTree->Branch("ezero"  ,psd.EZERO,         Form("EZERO[%d]/F", NEZERO));
      newTree->Branch("ezero_t",psd.EZEROTimestamp,Form("EZEROTimestamp[%d]/l", NEZERO)); 
   }else{
      printf(" -----  no ezero.\n"); 
   }

   if( isCRDT ){
      newTree->Branch("crdt"  ,psd.CRDT,         Form("CRDT[%d]/F", NCRDT));
      newTree->Branch("crdt_t",psd.CRDTTimestamp,Form("CRDTTimestamp[%d]/l", NCRDT)); 
   }else{
      printf(" -----  no crdt.\n"); 
   }
   
   if( isTraceON ){
      arr = new TClonesArray("TGraph");
         
      if( isSaveTrace){
         newTree->Branch("trace", arr, 256000);
         arr->BypassStreamer();
      }
      
      arrTrapezoid = new TClonesArray("TGraph");
      if( traceMethod  == 2 ) {
         newTree->Branch("trapezoid", arrTrapezoid, 256000);
         arrTrapezoid->BypassStreamer();
      }
      
      if( traceMethod > 0 ){
	      gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
	      gFitLin = new TF1("gFitLin", "[0] + [1]*(x-250)", 0, 140);

         newTree->Branch("te",             te,  Form("Trace_Energy[%d]/F",          NARRAY));
         newTree->Branch("te_r",         te_r,  Form("Trace_Energy_RiseTime[%d]/F", NARRAY));
         newTree->Branch("te_t",         te_t,  Form("Trace_Energy_Time[%d]/F",     NARRAY));
         newTree->Branch("txf",             txf,  Form("Trace_XF[%d]/F",          NARRAY));
         newTree->Branch("txf_r",         txf_r,  Form("Trace_XF_RiseTime[%d]/F", NARRAY));
         newTree->Branch("txf_t",         txf_t,  Form("Trace_XF_Time[%d]/F",     NARRAY));
         newTree->Branch("txn",             txn,  Form("Trace_XN[%d]/F",          NARRAY));
         newTree->Branch("txn_r",         txn_r,  Form("Trace_XN_RiseTime[%d]/F", NARRAY));
         newTree->Branch("txn_t",         txn_t,  Form("Trace_XN_Time[%d]/F",     NARRAY));

         //lin slope fit result
         newTree->Branch("te_m",         te_m,  Form("Trace_Energy_Slope[%d]/F",     NARRAY));
         newTree->Branch("te_b",         te_b,  Form("Trace_Energy_Offset[%d]/F",     NARRAY));
         newTree->Branch("txf_m",         txf_m,  Form("Trace_XF_Slope[%d]/F",     NARRAY));
         newTree->Branch("txf_b",         txf_b,  Form("Trace_XF_Offset[%d]/F",     NARRAY));
         newTree->Branch("txn_m",         txn_m,  Form("Trace_XN_Slope[%d]/F",     NARRAY));
         newTree->Branch("txn_b",         txn_b,  Form("Trace_XN_Offset[%d]/F",     NARRAY));
         
         if( isRecoil ){
            newTree->Branch("trdt",         trdt,  Form("Trace_RDT[%d]/F",          NRDT));
            newTree->Branch("trdt_t",     trdt_t,  Form("Trace_RDT_Time[%d]/F",     NRDT));
            newTree->Branch("trdt_r",     trdt_r,  Form("Trace_RDT_RiseTime[%d]/F", NRDT));
            newTree->Branch("trdt_m",     trdt_m,  Form("Trace_RDT_Slope[%d]/F", NRDT));
            newTree->Branch("trdt_b",     trdt_b,  Form("Trace_RDT_Offset[%d]/F", NRDT));
         }
         if ( isEZero ) {
            newTree->Branch("tezero",       tezero,    Form("Trace_ezero[%d]/F",         NEZERO));
            newTree->Branch("tezero_t",     tezero_t,  Form("Trace_ezero_Time[%d]/F",    NEZERO));
            newTree->Branch("tezero_r",     tezero_r,  Form("Trace_ezero_RiseTime[%d]/F",NEZERO));         
         }
         if ( isElum ) {
            newTree->Branch("telum",       telum,    Form("Trace_elum[%d]/F",         NELUM));
            newTree->Branch("telum_t",     telum_t,  Form("Trace_elum_Time[%d]/F",    NELUM));
            newTree->Branch("telum_r",     telum_r,  Form("Trace_elum_RiseTime[%d]/F",NELUM));         
         }
         if ( isCRDT ){
            newTree->Branch("tcrdt"  , tcrdt,    Form("Trace_CRDT[%d]/F", NCRDT));
            newTree->Branch("tcrdt_t", tcrdt_t,  Form("Trace_CRDT_Time[%d]/l", NCRDT)); 
            newTree->Branch("tcrdt_r", tcrdt_r,  Form("Trace_CRDT_RiseTime[%d]/l", NCRDT)); 
         }
         
      }
   }
  
}

Bool_t GeneralSortTraceProof::Process(Long64_t entry)
{ 
   psd.eventID = entry;
   psd.runID = runIDLast;

   NumHits = 0;
   b_NumHits->GetEntry(entry);
   
   //if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac, etc

/**////======================================= Zero struct
   for (Int_t i=0 ; i< NARRAY; i++) { 
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = 0.0;
      
      psd.EnergyTimestamp[i] = 0;
      psd.XFTimestamp[i]     = 0;
      psd.XNTimestamp[i]     = 0;
      psd.RingTimestamp[i]   = 0;
      
      psd.x[i]       = TMath::QuietNaN();    
   
      if ( i < NRDT ){
         psd.RDT[i]     = TMath::QuietNaN();
         psd.RDTTimestamp[i]    = 0;
      }
      if ( i < NTAC ){
         psd.TAC[i]     = TMath::QuietNaN();
         psd.TACTimestamp[i]    = 0;
      }
      if ( i < NELUM ) {
         psd.ELUM[i] = TMath::QuietNaN();
         psd.ELUMTimestamp[i]   = 0;
      }
      if ( i < NEZERO ) {
         psd.EZERO[i] = TMath::QuietNaN();
         psd.EZEROTimestamp[i]  = 0;
      }
      
   }
   
   if( isTraceON ){
      for(int i = 0; i < NARRAY; i++){
         te[i]     = TMath::QuietNaN();
         te_r[i]   = TMath::QuietNaN();
         te_t[i]   = TMath::QuietNaN();
         txf[i]     = TMath::QuietNaN();
         txf_r[i]   = TMath::QuietNaN();
         txf_t[i]   = TMath::QuietNaN();
         txn[i]     = TMath::QuietNaN();
         txn_r[i]   = TMath::QuietNaN();
         txn_t[i]   = TMath::QuietNaN();

         te_m[i]   = TMath::QuietNaN(); 
         te_b[i]   = TMath::QuietNaN(); 
         txf_m[i]   = TMath::QuietNaN(); 
         txf_b[i]   = TMath::QuietNaN(); 
         txn_m[i]   = TMath::QuietNaN();
         txn_b[i]   = TMath::QuietNaN();
         
         if( isRecoil &&  i < NRDT ) {
            trdt[i]   = TMath::QuietNaN();
            trdt_t[i] = TMath::QuietNaN();
            trdt_r[i] = TMath::QuietNaN();
            trdt_m[i] = TMath::QuietNaN();
            trdt_b[i] = TMath::QuietNaN();
         }
         
         if( isEZero &&  i < NEZERO ) {
            tezero[i]   = TMath::QuietNaN();
            tezero_t[i] = TMath::QuietNaN();
            tezero_r[i] = TMath::QuietNaN();
         }
         
         if( isElum &&  i < NELUM ) {
            telum[i]   = TMath::QuietNaN();
            telum_t[i] = TMath::QuietNaN();
            telum_r[i] = TMath::QuietNaN();
         }
         
         if( isCRDT &&  i < NCRDT ) {
            tcrdt[i]   = TMath::QuietNaN();
            tcrdt_t[i] = TMath::QuietNaN();
            tcrdt_r[i] = TMath::QuietNaN();
         }
      }
      
      arr->Clear("C");
   }

/**////======================================= Pull needed entries
   
   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);
   if( isTraceON ) {
      b_trace->GetEntry(entry);
      b_trace_length->GetEntry(entry);
   }

   ///ID PSD Channels
   Int_t idKind  = -1;
   Int_t idDet   = -1; /// Detector number
   
   /** -- Loop over NumHits -- */
   for(Int_t i=0;i<NumHits;i++) {    
      Int_t idTemp   = id[i] - idConst;
      idDet  = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
      
      ///PSD
      /***********************************************************************/
      if( NARRAY > idDet && idDet >= 0 && 3 >= idKind && idKind >= 0 ) {
         
         switch(idKind){
            case 0: /** Energy signal */
               psd.Energy[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
               psd.EnergyTimestamp[idDet] = event_timestamp[i];
               break;
            case 1: /// XF
               psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
               psd.XFTimestamp[idDet] = event_timestamp[i];
               break;
            case 2: /// XN
               psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
               psd.XNTimestamp[idDet] = event_timestamp[i];
               break;
            case 3: /// Ring
               psd.Ring[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
               psd.RingTimestamp[idDet] = event_timestamp[i];
               break;
         }
      }

      ///RECOIL
      /************************************************************************/
      if( isRecoil && 100 <= idDet && idDet <= 100 + NRDT ) { 
         Int_t rdtTemp = idDet-100;
         psd.RDT[rdtTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/MWIN ;
         psd.RDTTimestamp[rdtTemp] = event_timestamp[i];
      }
      
      ///ELUM
      /************************************************************************/
      if( isElum && 200 <= idDet && idDet <= 200 + NELUM ) {
         Int_t elumTemp = idDet - 200;
         psd.ELUM[elumTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
         psd.ELUMTimestamp[elumTemp] = event_timestamp[i];
      }

      ///EZERO
      /************************************************************************/
      if( isEZero &&  300 <= idDet && idDet < 300 + NEZERO ) {
         Int_t ezeroTemp = idDet - 300;
         psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
         psd.EZEROTimestamp[ezeroTemp] = event_timestamp[i];
      }

      ///TAC & RF TIMING
      /***********************************************************************/
      if( isTACRF && 400 <= idDet && idDet <= 400 + NTAC ) {   
        Int_t tacID = idDet - 400;
        psd.TAC[tacID] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN;
        psd.TACTimestamp[tacID] = event_timestamp[i];
      }
      
      ///Circular Recoil
      /************************************************************************/
      if( isCRDT && 500 <= idDet && idDet < 500 + NCRDT ) {
        Int_t crdtID = idDet - 500;
        psd.CRDT[crdtID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN ;
        psd.CRDTTimestamp[crdtID] = event_timestamp[i];
      }

   }///end of NumHits
   
   for(int i = 0 ; i < 24; i++){
      psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
   } //CRH NEED TO CHANGE once calibrated
   
   ///Trace
   /************************************************************************/
   if( isTraceON ) {
      int countTrace = 0;
      
      arr->Clear("C");
      
      for(Int_t i = 0; i < NumHits ; i++) {
         Int_t idTemp   = id[i] - idConst;
         idDet  = idDetMap[idTemp];
         idKind = idKindMap[idTemp];
         
         ///bool isPSDe = (30 > idDet && idDet >= 0 && idKind == 0);
         bool isPSD = (NARRAY > idDet && idDet >= 0);
         bool isRDT  = (NRDT + 100 > idDet && idDet >= 100 );
         bool isezero  = (NEZERO + 300 > idDet && idDet >= 300 );
         bool iselum  = (NELUM + 200 > idDet && idDet >= 200 );
         bool iscrdt  = (NCRDT + 500 > idDet && idDet >= 500 );

         //================ Set Gate
         ///if( !isPSD && !isRDT && !isezero) continue;
         if( !isPSD && !isRDT) continue;         
         //if( isPSD && (psd.Energy[idDet]<200 || psd.Energy[idDet]>16000) ) continue;
         //above Does do something but not exactly what expected

         int traceLength = trace_length[i];
         gTrace = (TGraph*) arr->ConstructedAt(countTrace, "C");
         gTrace->Clear();         
         gTrace->Set(traceLength);
         gTrace->SetTitle("");
         countTrace ++;

         ///printf("------- ev : %lld, %d /%d, countTrace : %d, length : %d, idDet : %d \n", entry, i, NumHits, countTrace, traceLength, idDet);
         
         if( isPSD ) {
            delayChannel = 100;
         }
         
         if( isRDT ) {
            delayChannel = 300;
         }
         ///==================  Set gTrace
         if( traceMethod == 0 ){
            for ( long long int j = 0 ; j < traceLength; j++){
               gTrace->SetPoint(j, j, trace[i][j]);
            }
         }
         
         ///=================== regulate the trace
         double base = 0;
         if ( traceMethod >= 1 ){
            for( int j = 0; j < traceLength; j++){ 
               if( trace[i][j] < 16000){
                  base = trace[i][j];
                  gTrace->SetPoint(j, j, trace[i][j]);
               }else{
                  gTrace->SetPoint(j, j, base);
               }
            }
         }
         
         gTrace->SetTitle(Form("ev=%d, id=%d, nHit=%d, length=%d", psd.eventID, idDet, i, traceLength ));
         gTraceLin->SetTitle(Form("Linear ev=%d, id=%d, nHit=%d, length=%d", psd.eventID, idDet, i, traceLength ));

         ///===================== fitting , find time
         if( traceMethod == 1){
            
            ///Set gFit
            gFit->SetLineStyle(idDet);
            gFitLin->SetLineStyle(idDet);
            
            int lineColor = 1;
            switch(idKind){
               case  0: lineColor = 3; break;
               case  1: lineColor = 1; break;
               case  2: lineColor = 2; break;
               case  3: lineColor = 4; break;
               case -1: lineColor = 6; break;
            }
            
            gFit->SetLineColor(lineColor);
            gFitLin->SetLineColor(lineColor);
            //gFit->SetRange(0, traceLength);
	         gFit->SetRange(0, 300);
            gFitLin->SetRange(200, traceLength);

            base = gTrace->Eval(1);
            double fileNameTemp = gTrace->Eval(delayChannel*1.5) - base;

            gFit->SetParameter(0, fileNameTemp); ///energy
            gFit->SetParameter(1, delayChannel); /// time
            gFit->SetParameter(2, 1);            ///riseTime
            gFit->SetParameter(3, base);

            gFitLin->SetParameter(0, fileNameTemp);
            gFitLin->SetParameter(1, -0.5);

            //if( gTrace->Eval(120) < base ) gFit->SetRange(0, 100); //sometimes, the trace will drop    
            //if( gTrace->Eval(20) < base) gFit->SetParameter(1, 5); //sometimes, the trace drop after 5 ch

            if( isSaveFitTrace ) {
               gTraceLin->Fit("gFitLin", "qR");
               gTrace->Fit("gFit","qR");
            }else{
               gTraceLin->Fit("gFitLin", "qR0");
               gTrace->Fit("gFit","qR0");
            }
            //CRH
            //printf(Form("%f %f",gFit->GetParameter(0),gFitLin->GetParameter(0)));

            if( NARRAY > idDet && idDet >= 0 /* && idKind == 0*/ ) {
               if ( idKind == 0 ) {
                  te[idDet]   = gFit->GetParameter(0);
                  te_t[idDet] = gFit->GetParameter(1);
                  te_r[idDet] = gFit->GetParameter(2);
                  te_m[idDet] = gFitLin->GetParameter(1);
                  te_b[idDet] = gFitLin->GetParameter(0);
               } else if ( idKind == 1 ) {
                  txf[idDet]   = gFit->GetParameter(0);
                  txf_t[idDet] = gFit->GetParameter(1);
                  txf_r[idDet] = gFit->GetParameter(2);
                  txf_m[idDet] = gFitLin->GetParameter(1);
                  txf_b[idDet] = gFitLin->GetParameter(0);
               } else if ( idKind == 2) {
                  txn[idDet]   = gFit->GetParameter(0);
                  txn_t[idDet] = gFit->GetParameter(1);
                  txn_r[idDet] = gFit->GetParameter(2);
                  txn_m[idDet] = gFitLin->GetParameter(1);
                  txn_b[idDet] = gFitLin->GetParameter(0);
               }
            }
            
            if( NRDT + 100 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-100;
               trdt[rdtTemp]   = TMath::Abs(gFit->GetParameter(0));
               trdt_t[rdtTemp] = gFit->GetParameter(1);
               trdt_r[rdtTemp] = gFit->GetParameter(2);
               trdt_m[rdtTemp] = gFit->GetParameter(1);
               trdt_b[rdtTemp] = gFit->GetParameter(0);
            }
            
            if( NELUM + 200 > idDet && idDet >= 200 ) {
               int elumTemp = idDet-200;
               telum[elumTemp]   = gFit->GetParameter(0);
               telum_t[elumTemp] = gFit->GetParameter(1);
               telum_r[elumTemp] = gFit->GetParameter(2);
            }
            
            if( NEZERO + 300 > idDet && idDet >= 300 ) {
               int ezeroTemp = idDet-300;
               tezero[ezeroTemp]   = gFit->GetParameter(0);
               tezero_t[ezeroTemp] = gFit->GetParameter(1);
               tezero_r[ezeroTemp] = gFit->GetParameter(2);
            }

            if( NCRDT + 500 > idDet && idDet >= 500 ) {
               int ecrdtTemp = idDet-500;
               tcrdt  [ecrdtTemp] = gFit->GetParameter(0);
               tcrdt_t[ecrdtTemp] = gFit->GetParameter(1);
               tcrdt_r[ecrdtTemp] = gFit->GetParameter(2);
            }
            
         }
         
         ///Trapezoid filter
         if( traceMethod == 2) {
            
            gTrapezoid  = (TGraph*) arr->ConstructedAt(countTrace);
            gTrapezoid->Clear();
            
            gTrapezoid = TrapezoidFilter(gTrace);
            
            
         }
         
      } /// End NumHits Loop
   }/// end of trace

   ///Fill
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
