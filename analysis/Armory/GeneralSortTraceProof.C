#define GeneralSortTraceProof_cxx

#include "GeneralSortTraceProof.h"

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity

//=================================== setting
bool  isTraceON = true;
bool  isSaveTrace = true;
bool  isSaveFitTrace = true;
int   traceMethod = 1; //0 = no process; 1 = fit; 2 = trapezoid; 3 offset-different
float delayChannel = 100.; //initial guess of the time

// Also go to line 146 to set the trace analysis gate

//############# fit function definition
//=== Don't forget to set fit parameter at line ~ 550
const int numPara = 4;
float fitRange[2] = {0, 900};

double fitFunc(double * x, double * par){

   /// par[0] = A
   /// par[1] = t0
   /// par[2] = rise time
   /// par[3] = baseline
   /// par[4] = decay
   /// par[5] = pre-rise slope

   if( x[0] < par[1] ) return  par[3] + par[5] * (x[0]-par[1]);

   return par[3] + par[0] * (1 - TMath::Exp(- (x[0] - par[1]) / par[2]) ) * TMath::Exp(- (x[0] - par[1]) / par[4]);
}

double fitFuncSimple(double *x, double * par){

   /// par[0] = A
   /// par[1] = t0
   /// par[2] = rise time
   /// par[3] = baseline

   return par[0]/(1 + TMath::Exp(-(x[0] - par[1]) / par[2])) + par[3];

}


const int numPara2 = 10;
double fitFunc2(double * x, double * par){

   /// par[0] = A
   /// par[1] = t0
   /// par[2] = rise time
   /// par[3] = baseline
   /// par[4] = decay
   /// par[5] = pre-rise slope
   /// par[6] = A2
   /// par[7] = t0_2
   /// par[8] = rise time2
   /// par[9] = decay2

   if( x[0] < par[1] ) return  par[3] + par[5] * (x[0]-par[1]);

   double pulse1 = par[0] * (1 - TMath::Exp(- (x[0] - par[1]) / par[2]) ) * TMath::Exp(- (x[0] - par[1]) / par[4]);

   double pulse2 = 0 ;
   if( x[0] > par[7] ) pulse2 = par[6] * (1 - TMath::Exp(- (x[0] - par[7]) / par[8]) ) * TMath::Exp(- (x[0] - par[7]) / par[9]);

   return par[3] + pulse1 + pulse2;
}
//===================================================


//=================================== end of setting

///------- if the number in GeneralSortMapping.h is zero, the corresponding items will disable.
bool isTACRF  = true;
bool isRecoil = true;
bool isElum   = true;
bool isEZero  = true;
bool isCRDT   = true;
bool isAPOLLO = true;

TGraph * GeneralSortTraceProof::TrapezoidFilter(TGraph * trace){
   ///Trapezoid filter https://doi.org/10.1016/0168-9002(94)91652-7

   int baseLineEnd = 80;

   int riseTime = 10; //ch
   int flatTop = 20;
   float decayTime = 2000;

   TGraph *  trapezoid = new TGraph();
   //TH1F *  trapezoid = new TH1F("hTrap", "hTrap", fitRange[1] - fitRange[0], fitRange[0], fitRange[1]);
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

void GeneralSortTraceProof::Begin(TTree */*tree*/){

   TString option = GetOption();

   printf( "=====================================================\n");
   printf( "==========  GeneralSortTraceProof.C =================\n");
   printf( "============  General Sort w/ Trace  ================\n");
   printf( "=====================================================\n");

   if ( isTACRF  && NTAC    == 0 ) isTACRF  = false ;
   if ( isRecoil && NRDT    == 0 ) isRecoil = false ;
   if ( isElum   && NELUM   == 0 ) isElum   = false ;
   if ( isEZero  && NEZERO  == 0 ) isEZero  = false ;
   if ( isCRDT   && NCRDT   == 0 ) isCRDT   = false ;
   if ( isAPOLLO && NAPOLLO == 0 ) isAPOLLO = false ;

   printf( "  TAC/RF   : %s , %d \n", isTACRF  ? "On" : "Off", NTAC);
   printf( "  Recoil   : %s , %d \n", isRecoil ? "On" : "Off", NRDT);
   printf( "  Elum     : %s , %d \n", isElum   ? "On" : "Off", NELUM);
   printf( "  EZero    : %s , %d \n", isEZero  ? "On" : "Off", NEZERO);
   printf( "  C-Recoil : %s , %d \n", isCRDT   ? "On" : "Off", NCRDT);
   printf( "  APOLLO   : %s , %d \n", isAPOLLO ? "On" : "Off", NAPOLLO);
   TString traceMethodName;
   switch(traceMethod){
   case 0: traceMethodName = "copy"; break;
   case 1: traceMethodName = "fit"; break;
   case 2: traceMethodName = "trapezoid"; break;
   case 3: traceMethodName = "offset-diff."; break;
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

   if ( isTACRF  && NTAC    == 0 ) isTACRF  = false ;
   if ( isRecoil && NRDT    == 0 ) isRecoil = false ;
   if ( isElum   && NELUM   == 0 ) isElum   = false ;
   if ( isEZero  && NEZERO  == 0 ) isEZero  = false ;
   if ( isCRDT   && NCRDT   == 0 ) isCRDT   = false ;
   if ( isAPOLLO && NAPOLLO == 0 ) isAPOLLO = false ;

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

   newTree->Branch("evID", &psd.eventID, "evID/l");
   newTree->Branch("runID", &psd.runID, "runID/I");

   newTree->Branch("e",     psd.Energy,          Form("e[%d]/F", NARRAY));
   newTree->Branch("e_t",   psd.EnergyTimestamp, Form("e_t[%d]/l",NARRAY));
   newTree->Branch("ring",  psd.Ring,            Form("ring[%d]/F", NARRAY));
   newTree->Branch("ring_t",psd.RingTimestamp,   Form("ring_t[%d]/l",NARRAY));
   newTree->Branch("xf",    psd.XF,              Form("xf[%d]/F", NARRAY));
   newTree->Branch("xf_t",  psd.XFTimestamp,     Form("xf_t[%d]/l", NARRAY));
   newTree->Branch("xn",    psd.XN,              Form("xn[%d]/F", NARRAY));
   newTree->Branch("xn_t",  psd.XNTimestamp,     Form("xn_t[%d]/l", NARRAY));
   newTree->Branch("x",     psd.x,               Form("x[%d]/F", NARRAY));

   if( isRecoil){
      newTree->Branch("rdt"  ,psd.RDT,          Form("rdt[%d]/F",NRDT));
      newTree->Branch("rdt_t",psd.RDTTimestamp, Form("rdt_t[%d]/l", NRDT));
   }else{
      printf(" -----  no recoil.\n");
   }

   if( isTACRF ){
      newTree->Branch("tac"  ,psd.TAC,         Form("tac[%d]/F", NTAC));
      newTree->Branch("tac_t",psd.TACTimestamp,Form("tac_t[%d]/l", NTAC));
   }else{
      printf(" -----  no TAC.\n");
   }

   if( isElum ) {
      newTree->Branch("elum"  ,psd.ELUM,         Form("elum[%d]/F", NELUM));
      newTree->Branch("elum_t",psd.ELUMTimestamp,Form("elum_t[%d]/l",NELUM));
   }else{
      printf(" -----  no elum\n");
   }

   if( isEZero ){
      newTree->Branch("ezero"  ,psd.EZERO,         Form("ezero[%d]/F", NEZERO));
      newTree->Branch("ezero_t",psd.EZEROTimestamp,Form("ezero_t[%d]/l", NEZERO));
   }else{
      printf(" -----  no ezero.\n");
   }

   if( isCRDT ){
      newTree->Branch("crdt"  ,psd.CRDT,         Form("crdt[%d]/F", NCRDT));
      newTree->Branch("crdt_t",psd.CRDTTimestamp,Form("crdt_t[%d]/l", NCRDT));
   }else{
      printf(" -----  no crdt.\n");
   }

   if( isAPOLLO ){
      newTree->Branch("apollo"  ,psd.APOLLO,         Form("apollo[%d]/F", NAPOLLO));
      newTree->Branch("apollo_t",psd.APOLLOTimestamp,Form("apollo_t[%d]/l", NAPOLLO));
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
         // gFit  = new TF1("gFit", fitFunc, fitRange[0], fitRange[1], numPara);
         gFit  = new TF1("gFit", fitFuncSimple, fitRange[0], fitRange[1], numPara);
         newTree->Branch("te",             te,  Form("te[%d]/F",          NARRAY));
         newTree->Branch("te_r",         te_r,  Form("te_r[%d]/F", NARRAY));
         newTree->Branch("te_t",         te_t,  Form("te_t[%d]/F",     NARRAY));

         if( isRecoil ){
            newTree->Branch("trdt",         trdt,  Form("trdt[%d]/F",          NRDT));
            newTree->Branch("trdt_t",     trdt_t,  Form("trdt_t[%d]/F",     NRDT));
            newTree->Branch("trdt_r",     trdt_r,  Form("trdt_r[%d]/F", NRDT));
         }
         if ( isEZero ) {
            newTree->Branch("tezero",       tezero,    Form("tezero[%d]/F",         NEZERO));
            newTree->Branch("tezero_t",     tezero_t,  Form("tezero_t[%d]/F",    NEZERO));
            newTree->Branch("tezero_r",     tezero_r,  Form("tezero_r[%d]/F",NEZERO));
         }
         if ( isElum ) {
            newTree->Branch("telum",       telum,    Form("telum[%d]/F",         NELUM));
            newTree->Branch("telum_t",     telum_t,  Form("telum_t[%d]/F",    NELUM));
            newTree->Branch("telum_r",     telum_r,  Form("telum_r[%d]/F",NELUM));
         }
         if ( isCRDT ){
            newTree->Branch("tcrdt"  , tcrdt,    Form("tcrdt[%d]/F", NCRDT));
            newTree->Branch("tcrdt_t", tcrdt_t,  Form("tcrdt_t[%d]/l", NCRDT));
            newTree->Branch("tcrdt_r", tcrdt_r,  Form("tcrdt_r[%d]/l", NCRDT));
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

   if( isTraceON ){
      for(int i = 0; i < NARRAY; i++){
         te[i]     = TMath::QuietNaN();
         te_r[i]   = TMath::QuietNaN();
         te_t[i]   = TMath::QuietNaN();

         if( isRecoil &&  i < NRDT ) {
            trdt[i]   = TMath::QuietNaN();
            trdt_t[i] = TMath::QuietNaN();
            trdt_r[i] = TMath::QuietNaN();
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
               psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_XFXN;
               psd.XFTimestamp[idDet] = event_timestamp[i];
               break;
            case 2: /// XN
               psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_XFXN;
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
         psd.RDT[rdtTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_RDT ;
         psd.RDTTimestamp[rdtTemp] = event_timestamp[i];
      }

      ///ELUM
      /************************************************************************/
      if( isElum && 200 <= idDet && idDet <= 200 + NELUM ) {
         Int_t elumTemp = idDet - 200;
         psd.ELUM[elumTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_ELUM;
         psd.ELUMTimestamp[elumTemp] = event_timestamp[i];
      }

      ///EZERO
      /************************************************************************/
      if( isEZero &&  300 <= idDet && idDet < 300 + NEZERO ) {
         Int_t ezeroTemp = idDet - 300;
         psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/MWIN * POLARITY_EZERO;
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
        psd.CRDT[crdtID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN * POLARITY_CRDT;
        psd.CRDTTimestamp[crdtID] = event_timestamp[i];
      }

      ///APOLLO
      /************************************************************************/
      if ( NAPOLLO > 0 && idDet >= 600 && idDet <= 600 + NAPOLLO ) {
        Int_t apolloID = idDet - 600;
        psd.APOLLO[apolloID] = ((float)(post_rise_energy[i]) -(float)(pre_rise_energy[i]))/MWIN * POLARITY_APOLLO;
        psd.APOLLOTimestamp[apolloID] = event_timestamp[i];
      }

   }///end of NumHits

   for(int i = 0 ; i < NARRAY; i++){
      psd.x[i] = (psd.XF[i] - psd.XN[i])/(psd.XF[i] + psd.XN[i]);
   }

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
         bool isPSD    = (NARRAY > idDet && idDet >= 0);
         bool isRDT    = (NRDT + 100 > idDet && idDet >= 100 );
         bool isezero  = (NEZERO + 300 > idDet && idDet >= 300 );
         bool iselum   = (NELUM + 200 > idDet && idDet >= 200 );
         bool iscrdt   = (NCRDT + 500 > idDet && idDet >= 500 );
         bool isapollo = (NAPOLLO + 600 > idDet && idDet >= 600 );

         //================ Set Gate
         ///if( !isPSD && !isRDT && !isezero) continue;
         ///if( !isPSD && !isRDT) continue;

         if( isapollo) continue; /// becasue APOLLO trace branch is not set

         int traceLength = trace_length[i];
         gTrace = (TGraph*) arr->ConstructedAt(countTrace, "C");
         gTrace->Clear();
         gTrace->Set(traceLength);

         gTrace->SetTitle("");
         countTrace ++;

         ///printf("------- ev : %lld, %d /%d, countTrace : %d, length : %d, idDet : %d \n", entry, i, NumHits, countTrace, traceLength, idDet);

         delayChannel = 100;

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

         gTrace->SetTitle(Form("ev=%llu, id=%d, nHit=%d, length=%d", psd.eventID, idDet, i, traceLength ));

         ///===================== fitting , find time
         if( traceMethod == 1){

            ///Set gFit
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

            //base = gTrace->Eval(1);
            base = 8200;
            double energyEst = gTrace->Eval(delayChannel*1.5) - base;

            gFit->SetParameter(0, energyEst); ///energy
            gFit->SetParameter(1, delayChannel); /// time
            gFit->SetParameter(2, 1);            ///riseTime
            gFit->SetParameter(3, base);
            // gFit->SetParameter(4, 2000);   // decay
            // gFit->SetParameter(5, -1);

            gFit->SetParLimits(1, 85, 110);
            // gFit->SetParLimits(4, 800, 99999999999);
            // gFit->SetParLimits(5, -2, 0);

            //if( gTrace->Eval(120) < base ) gFit->SetRange(0, 100); //sometimes, the trace will drop
            //if( gTrace->Eval(20) < base) gFit->SetParameter(1, 5); //sometimes, the trace drop after 5 ch

            if( isPSD ) fitRange[1] = 300;
            if( isRDT ) fitRange[1] = 900;

            if( isSaveFitTrace ) {
               gTrace->Fit("gFit", "QR", "", fitRange[0], fitRange[1]);
            }else{
               gTrace->Fit("gFit", "QR0", "", fitRange[0], fitRange[1]);
            }

            if( NARRAY > idDet && idDet >= 0 && idKind == 0 ) {
               te[idDet]   = gFit->GetParameter(0);
               // double riseTime = gFit->GetParameter(2);
               // te_t[idDet] = gFit->GetParameter(1) + riseTime * 0.105360516; // 10% rise time
               te_t[idDet] = gFit->GetParameter(1);
               te_r[idDet] = gFit->GetParameter(2);
            }

            if( NRDT + 100 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-100;
               trdt[rdtTemp]   = TMath::Abs(gFit->GetParameter(0));
               // double riseTime = gFit->GetParameter(2);
               // trdt_t[rdtTemp] = gFit->GetParameter(1) + riseTime * 0.105360516; // 10% rise time
               trdt_t[rdtTemp] = gFit->GetParameter(1); 
               trdt_r[rdtTemp] = gFit->GetParameter(2);
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

            int len = gTrapezoid->GetN();

            TH1F h1 ("h1", "h1", len, 0, len);
            for( int i =0 ; i < len; i++){
               h1.Fill( gTrapezoid->GetPointX(i)-0.5, gTrapezoid->GetPointY(i));
            }

            TSpectrum peak(10);

            int nPeaksRaw = peak.Search(&h1, 4, "", 0.4);

            double * xpos = peak.GetPositionX();
            double * ypos = peak.GetPositionY();

            int inX[nPeaksRaw];
            TMath::Sort(nPeaksRaw, xpos, &inX[0], 0);

            double xxx = 0, yyy = 0;

            for( int k = 0; k < nPeaksRaw; k++){
               if( 280 < xpos[inX[k]] && xpos[inX[k]] < 340  ) {
                  xxx = xpos[inX[k]];
                  yyy = ypos[inX[k]];
                  break;
               }
            }

            if( NARRAY > idDet && idDet >= 0 && idKind == 0 ) {
               te[idDet]   = yyy;
               te_t[idDet] = xxx - 30;
               te_r[idDet] = 0;
            }

            if( NRDT + 100 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-100;
               trdt[rdtTemp]   = yyy;
               trdt_t[rdtTemp] = xxx - 30;
               trdt_r[rdtTemp] = 0;
            }

         }

         ///Offset-Difference
         if( traceMethod == 3){

            int offset = 30;

            TH1F * jaja = new TH1F();
            jaja->Clear();
            jaja->Reset();
            jaja->SetBins(gTrace->GetN()-offset, 0, gTrace->GetN() - offset);

            for( int k = 0; k < gTrace->GetN() - offset; k++){
               double a = gTrace->GetPointY(k+offset);
               double b = gTrace->GetPointY(k);
               jaja->Fill(k, (a -b) + offset);
               jaja->SetBinError(k, 0);
            }

            //========= moving average
            TH1F * specS = (TH1F*) jaja->Clone();
            specS->Rebin(2);

            int nPeaks = 0;
            std::vector<double> tPos, height;
            double t1 = 0, t2 = 0;

            {
               TSpectrum * peak = new TSpectrum(10);

               printf("============= estimating background...\n");
               TH1 * h1 = peak->Background(specS, 30);
               //h1->Draw("same");
               printf("============= substracting the linear background...\n");
               //specS->Sumw2();
               specS->Add(h1, -1.);

               delete peak;
               peak = new TSpectrum(10);
               int nPeaks = peak->Search(specS, 5, "", 0.2);
               double * xpos = peak->GetPositionX();
               double * ypos = peak->GetPositionY();

               int * inX = new int[nPeaks];
               TMath::Sort(nPeaks, xpos, inX, 0 );

               for( int j = 0; j < nPeaks; j++){
                  tPos.push_back(xpos[inX[j]]);
                  height.push_back(ypos[inX[j]]);
               }
               // for( int j = 0; j < nPeaks; j++){
               //    printf(" position : %f , %f \n", energy[j], height[j]);
               // }

               delete peak;

               //get the FWHM of the first peak
               int b0 = specS->FindBin(tPos[0]);
               int b = b0;
               do{
                  b = b - 1;
                  double y = specS->GetBinContent(b);
                  double t = specS->GetBinCenter(b);

                  if( y < height[0]/2. ) {

                     double yy = specS->GetBinContent(b+1);
                     double tt = specS->GetBinCenter(b+1);

                     t1 = t + (tt-t) * (height[0]/2 - y)/(yy-y);
                     break;
                  }
               }while(b > 0);
               b = b0;
               do{
                  b = b + 1;
                  double y = specS->GetBinContent(b);
                  double t = specS->GetBinCenter(b);

                  if( y < height[0]/2. ) {
                     double yy = specS->GetBinContent(b-1);
                     double tt = specS->GetBinCenter(b-1);

                     t2 = t + (tt-t) * (height[0]/2 - y)/(yy-y);
                     break;
                  }
               }while(b < 2*b0);

            }

            delete jaja;
            delete specS;

            if( NARRAY > idDet && idDet >= 0 && idKind == 0 ) {
               te[idDet]   = height[0];
               te_t[idDet] = tPos[0];
               te_r[idDet] = t2-t1;
            }

            if( NRDT + 100 > idDet && idDet >= 100 ) {
               int rdtTemp = idDet-100;
               trdt[rdtTemp]   = height[0];
               trdt_t[rdtTemp] = tPos[0];
               trdt_r[rdtTemp] = t2-t1;
            }

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
