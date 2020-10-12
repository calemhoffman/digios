//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 10 21:31:47 2018 by ROOT version 6.10/02
// from TTree gen_tree/PSD Tree
// found on file: gen_run11.root
//////////////////////////////////////////////////////////

#ifndef Cali_e_trace_h
#define Cali_e_trace_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TF1.h>
#include <string>
#include <fstream>
#include <TObjArray.h>
#include <TCutG.h>
#include "TMacro.h"
#include "TClonesArray.h"

// Headers needed by this particular selector


class Cali_e_trace : public TSelector {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           runID;
   Float_t         e[100];
   ULong64_t       e_t[100];
   ULong64_t       EBIS_t;
   Float_t         xf[100];
   ULong64_t       xf_t[100];
   Float_t         xn[100];
   ULong64_t       xn_t[100];
   Float_t         ring[100];
   ULong64_t       ring_t[100];
   Float_t         rdt[8];
   ULong64_t       rdt_t[8];
   Float_t         tac[100];
   ULong64_t       tac_t[100];
   Float_t         elum[32];
   ULong64_t       elum_t[32];
   Float_t         ezero[10];
   ULong64_t       ezero_t[10];
   
   //Trace
   Float_t         te[30];
   Float_t         te_r[30];
   Float_t         te_t[30];
   Float_t         trdt[8];
   Float_t         trdt_t[8];
   Float_t         trdt_r[8];

   // List of branches
   TBranch        *b_runID; //!
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_EBISTimestamp;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_XNTimestamp;   //!
   TBranch        *b_RING;   //!
   TBranch        *b_RINGTimestamp;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   TBranch        *b_TAC;   //!
   TBranch        *b_TACTimestamp;   //!
   TBranch        *b_ELUM;   //!
   TBranch        *b_ELUMTimestamp;   //!
   TBranch        *b_EZERO;   //!
   TBranch        *b_EZEROTimestamp;   //!
   
   TBranch        *b_Trace_Energy;  //!
   TBranch        *b_Trace_Energy_Time;  //!
   TBranch        *b_Trace_Energy_RiseTime;  //!
   
   TBranch        *b_Trace_RDT;  //!
   TBranch        *b_Trace_RDT_Time;  //!
   TBranch        *b_Trace_RDT_RiseTime;  //!
   
   bool isRunIDExist;
   bool isEBISExist;
   bool isTACExist;
   bool isELUMExist;
   bool isEZEROExist;
   bool isTraceDataExist; // if b_Trace_** exist

   Cali_e_trace(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Cali_e_trace() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(Cali_e_trace,0);
   
   //=============================== 
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   int totnumEntry; // of original root
  
   //tree  
   Int_t eventID;
   Int_t run;
   Float_t eC[30];
   Float_t xfC[30];
   Float_t xnC[30];
   ULong64_t eC_t[30];
   Float_t x[30]; // unadjusted position, range (-1,1)
   Float_t z[30]; 
   int det;    //
   int hitID[30]; // is e, xf, xn are all fired.
   int multiHit; // multipicity of z
   
   Float_t thetaCM;
   Float_t Ex;   
   Float_t thetaLab;
   
   Float_t rdtC[8];
   ULong64_t rdtC_t[8];
   int rdtID[8]; // rdt hit ID
   int rdtdEMultiHit;

   Int_t   coin_t;
   Float_t tcoin_t;
   Float_t coinTimeUC; 
   Float_t coinTime;
   
   Float_t teS;      //for selected time 
   Float_t te_tS;
   Float_t te_rS;
   Float_t trdtS;
   Float_t trdt_tS;
   Float_t trdt_rS;
   
   TObjArray * fList; //!
   
   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t validEventCount;
   
   //========correction parameters
   int numDet;
   int iDet; // number of detector at different position
   int jDet; // number of detector at same position
   vector<double> pos;
   double Bfield;
   double perpDist;
   double length;
   double firstPos;
   double xnCorr[30]; // xn correction for xn = xf
   double xfxneCorr[30][2]; //xf, xn correction for e = xf + xn
   double xCorr[30]; // correction of x, scale to (-1,1)
   
   double eCorr[30][2]; // e-correction
   double rdtCorr[8]; //rdt-correction
   
   double cTCorr[30][9]; // coinTime correction
   TF1 ** f7 ; //!
   
   bool isReaction;
   double G, Z, H; // for excitation calcualtion
   double mass, q;
   double beta, gamma;
   double alpha ;
   double Et, massB;

   //======== RDT cut
   bool isRDTCutExist;
   TCutG ** cut = NULL;


};

#endif

#ifdef Cali_e_trace_cxx
void Cali_e_trace::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).
   
   // Set branch addresses and branch pointers
   if (!tree) return;
   
   //======================================   
   totnumEntry = tree->GetEntries();
   printf( "=========================================================================== \n");
   printf( "========================== Cali_e_trace.h ================================= \n");
   printf( "====== Make a new tree with all calibrations, total Entry : %d \n", totnumEntry);
   printf( "=========================================================================== \n");

   
   fChain = (TChain *)tree;
   //printf("========== number of tree loaded : %d \n", fChain->GetNTree());
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   fChain->SetBranchAddress("ring", ring, &b_RING);
 
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);   
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   //fChain->SetBranchAddress("ring_t", ring_t, &b_RingTimestamp);
   
   isRunIDExist = false;
   TBranch * br = (TBranch *) fChain->GetListOfBranches()->FindObject("runID");
   
   if( br == NULL ){
      printf(" ++++++++ no runID.\n");
   }else{
      isRunIDExist = true;
      fChain->SetBranchAddress("runID", &runID, &b_runID);
   }
   
   isEBISExist = false;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("EBIS");
   if( br == NULL ){
      printf(" ++++++++ no EBIS data.\n");
   }else{
      isEBISExist = true;
      fChain->SetBranchAddress("EBIS", &EBIS_t, &b_EBISTimestamp);
   }
   
   isTACExist = false;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("tac");
   if( br == NULL ){
      printf(" ++++++++ no tac data.\n");
   }else{
      isTACExist = true;
      fChain->SetBranchAddress("tac", tac, &b_TAC);
      fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   }
   
   isELUMExist = false;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("elum");
   if( br == NULL ){
      printf(" ++++++++ no elum data.\n");
   }else{
      isELUMExist = true;
      fChain->SetBranchAddress("elum", elum, &b_ELUM);
      fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   }
   
   isEZEROExist = false;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("ezero");
   if( br == NULL ){
      printf(" ++++++++ no ezero data.\n");
   }else{
      isEZEROExist = true;
      fChain->SetBranchAddress("ezero", ezero, &b_EZERO);
      fChain->SetBranchAddress("ezero_t", ezero_t, &b_EZEROTimestamp);
   }
   
   isTraceDataExist = false;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("te_t");
   if( br == NULL ){
      printf(" WARNING! cannot find trace data -> proceed without trace. \n");
   }else{
      printf(" =========== find trace data -> proceed trace. \n");
      isTraceDataExist = true;
      fChain->SetBranchAddress("te",   te,   &b_Trace_Energy);
      fChain->SetBranchAddress("te_t", te_t, &b_Trace_Energy_Time);
      fChain->SetBranchAddress("te_r", te_r, &b_Trace_Energy_RiseTime);

      fChain->SetBranchAddress("trdt",   trdt,   &b_Trace_RDT);
      fChain->SetBranchAddress("trdt_t", trdt_t, &b_Trace_RDT_Time);
      fChain->SetBranchAddress("trdt_r", trdt_r, &b_Trace_RDT_RiseTime);
   }
   
   //================================ load expName
   string expNameFile = "../../expName.sh";
   printf("======================= loading expName files : %s.", expNameFile.c_str());
   ifstream file;
   file.open(expNameFile.c_str());
   TString expName = "";
   int i = 0;
   if( file.is_open()){
      string x;
      while( file >> x){
         if( x.substr(0,1) == "#" )  continue;
         if( i == 1  )  expName = x;
         i = i + 1;
      }
      printf("... done.\n");
      
      expName.Remove(0,8);
      printf("expName = %s \n", expName.Data());
      
   }else{
      printf("... fail.\n");
   }
   
   file.close();

   //================= Formation of file name
   int numFile = fChain->GetListOfFiles()->GetLast() + 1;   
   int oldRunNum = -100;
   bool contFlag = false; // is runNumber continue;
   for( int i = 0; i < numFile ; i++){
      TString name = fChain->GetListOfFiles()->At(i)->GetTitle();
      int found = name.Last('/');
      name.Remove(0, found + 1 ); // this should give "XXX_run0XX.root"
      found = name.Last('.');
      name.Remove(found); // this should give "XXX_run0XX"
      if( i == 0 ) {
         saveFileName = name;
         int kk = saveFileName.Sizeof();
         saveFileName.Remove(kk-4); // this should give "XXX_run"
         saveFileName = expName + "_" + saveFileName;
      }
      found = name.Last('_');
      int runNum = name.Remove(0, found+4).Atoi(); // this should give the 3 digit run number 

      if( runNum == oldRunNum + 1 ){
         int kk = saveFileName.Sizeof();
         if( contFlag == false ){
            saveFileName.Remove(kk-2); //remove the "-"
            saveFileName += "-";
         }else{
            saveFileName.Remove(kk-5); //remove the runNum and "-"
         }
         contFlag = true;
      }
      if( runNum > oldRunNum + 1) contFlag = false;
      
      saveFileName += Form("%03d_", runNum);
      oldRunNum = runNum;
   }
   int kk = saveFileName.Sizeof();
   saveFileName.Remove(kk-2); // remove the last "-"
   saveFileName += ".root";
   
   //printf("Output File  %s \n", saveFileName.Data());
   
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   eventID = -1;
   run = 0;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   if( isRunIDExist )  newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,   eC, "e[30]/F");
   newTree->Branch("xf",  xfC, "xf[30]/F");
   newTree->Branch("xn",  xnC, "xn[30]/F");
   newTree->Branch("ring",  ring, "xn[30]/F");
   newTree->Branch("x" ,    x, "x[30]/F");
   newTree->Branch("z" ,    z, "z[30]/F");
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", hitID, "hitID[30]/I");
   newTree->Branch("multiHit", &multiHit, "multiHit/I");
   
   newTree->Branch("Ex", &Ex, "Ex/F");
   newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
   newTree->Branch("thetaLab", &thetaLab, "thetaLab/F");
   
   newTree->Branch("e_t", eC_t, "e_t[30]/l");
   
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/l");
   newTree->Branch("rdtID", rdtID, "rdtID[8]/I");
   newTree->Branch("rdtdEMultiHit", &rdtdEMultiHit, "rdtdEMultiHit/I");
   
   if( isEBISExist ) newTree->Branch("ebis_t", &EBIS_t, "EBIS_t/l");
   
   if( isELUMExist ) {
     newTree->Branch("elum", elum, "elum[32]/F");
     newTree->Branch("elum_t", elum_t, "elum_t[32]/l");
   }
   
   if( isEZEROExist ) {
      newTree->Branch("ezero", ezero, "ezero[10]/F");
      newTree->Branch("ezero_t", ezero_t, "ezero_t[10]/l");
   }
   
   if( isTACExist ){
      newTree->Branch("tac", tac, "tac[100]/F");
      newTree->Branch("tac_t", tac_t, "tac_t[100]/l");
   }
   
   newTree->Branch("coin_t", &coin_t, "coincident_time_from_digitizer/I");
   
   if( isTraceDataExist ){
      newTree->Branch("tcoin_t", &tcoin_t, "tcoin_t/F");
      newTree->Branch("coinTimeUC", &coinTimeUC, "coinTimeUnCalibrated_ns/F");
      newTree->Branch("coinTime", &coinTime, "coinTime_ns/F");
   
      newTree->Branch("te",             te,  "Trace_Energy[30]/F");
      newTree->Branch("te_r",         te_r,  "Trace_Energy_RiseTime[30]/F");
      newTree->Branch("te_t",         te_t,  "Trace_Energy_Time[30]/F");
      newTree->Branch("trdt",         trdt,  "Trace_RDT[8]/F");
      newTree->Branch("trdt_t",     trdt_t,  "Trace_RDT_Time[8]/F");
      newTree->Branch("trdt_r",     trdt_r,  "Trace_RDT_RiseTime[8]/F");
   }

   printf("Is EBIS  exist : %d\n", isEBISExist);
   printf("Is ELUM  exist : %d\n", isELUMExist);
   printf("Is EZero exist : %d\n", isEZEROExist);
   printf("Is TAC   exist : %d\n", isTACExist);
   printf("Is Trace exist : %d\n", isTraceDataExist);
   //=== clock
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   validEventCount = 0;
   
   //===================================================== loading parameter
   
   //========================================= detector Geometry
   printf("======================= loading parameters files .... \n");
   string detGeoFileName = "detectorGeo.txt";
   printf("loading detector geometery : %s.", detGeoFileName.c_str());
   file.open(detGeoFileName.c_str());
   if( file.is_open() ){
      i = 0;
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( x.substr(0,1) == "#" )  break;
         if( i == 0  )          Bfield = abs(atof(x.c_str()));
         if( i == 3  )        perpDist = atof(x.c_str());
         if( i == 5  )          length = atof(x.c_str());
         if( i == 14 )        firstPos = atof(x.c_str());
         if( i == 17 )            jDet = atoi(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      vector<double> posTemp = pos;
      for(int id = 0; id < iDet; id++){
        if( firstPos > 0 ) pos[id] = firstPos + posTemp[id];
        if( firstPos < 0 ) pos[id] = firstPos - posTemp[iDet-1-id];
      }
      
      printf(" Bfield       : %6.2f T\n", Bfield);
      printf(" PSD prepDist : %6.3f mm\n", perpDist);
      printf(" PSD length   : %6.3f mm\n", length);
      printf(" # row Det    : %6d \n", jDet);
      printf(" # col Det    : %6d \n", iDet);
      printf("----------- list of detector position\n");
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("==================================\n");

      TMacro detGeo("detectorGeo.txt");
      detGeo.Write("detGeo");
      
   }else{
       printf("... fail\n");
       Bfield = 2.5;
       firstPos =  0;
       perpDist = 9;
       length = 50.5;
       jDet = 4;
       iDet = 6;
       pos[0] = 0;
       pos[1] = 58.6;
       pos[2] = 117.9;
       pos[3] = 176.8;
       pos[4] = 235.8;
       pos[5] = 294.8;
   }
   
   numDet = iDet * jDet;

   //========================================= xf = xn correction
   printf("loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("................... done.\n");
      TMacro cali_xf_xn("correction_xf_xn.dat");
      cali_xf_xn.Write("correction_xf_xn");
   }else{
      printf("................... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   printf("loading xf/xn-e correction.");
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("................. done.\n");
      TMacro cali_xfxn_e("correction_xfxn_e.dat");
      cali_xfxn_e.Write("correction_xfxn_e");
   }else{
      printf("................. fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   printf("loading e correction.");
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;  // 1/a1
         eCorr[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf("....................... done.\n");
      TMacro cali_e("correction_e.dat");
      cali_e.Write("correction_e");      
   }else{
      printf("....................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
      //return;
   }
   file.close();
   
   
   //========================================= x-scale correction
   printf("loading x correction.");
   file.open("correction_scaleX.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a){
         if( i >= numDet) break;
         xCorr[i] = a;  
         i = i + 1;
      }
      printf("....................... done.\n");
      TMacro cali_scaleX("correction_scaleX.dat");
      cali_scaleX.Write("correction_scaleX");
   }else{
      printf("....................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         xCorr[i] = 1.;
      }
      //return;
   }
   file.close();
   
   
   //========================================= rdt correction
   printf("loading rdt correction.");
   file.open("correction_rdt.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a){
         if( i >= 8) break;
         rdtCorr[i] = a;  //
         i = i + 1;
      }
      printf("..................... done.\n");
      TMacro cali_rdt("correction_rdt.dat");
      cali_rdt.Write("correction_rdt");
   }else{
      printf("..................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         rdtCorr[i] = 1.;
      }
   }
   file.close();
   
   //========================================= coinTime correction
   if( isTraceDataExist ){
      printf("loading coin-Time correction parameters.");
      file.open("correction_coinTime.dat");
      
      f7 = new TF1*[numDet];
      fList = new TObjArray();
      
      if( file.is_open() ){
         int d;
         double a0, a1, a2, a3, a4, a5, a6, a7, a8;
         int i = 0;
         while( file >>  d >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8){
            if( i >= numDet) break;
            cTCorr[i][0] = a0;
            cTCorr[i][1] = a1;
            cTCorr[i][2] = a2;
            cTCorr[i][3] = a3;
            cTCorr[i][4] = a4;
            cTCorr[i][5] = a5;
            cTCorr[i][6] = a6;
            cTCorr[i][7] = a7;
            cTCorr[i][8] = a8; // this is the offset find by fitting the 1-D plot
            //printf("\n%2d, a0: %6.2f, a1: %6.2f .... a7: %6.2f", i, cTCorr[i][0], cTCorr[i][1], cTCorr[i][7]);
            i = i + 1;
         }
         printf(".... done.\n");
         TMacro cali_coinTime("correction_coinTime.dat");
         cali_coinTime.Write("correction_coinTime");
         
      }else{
         printf(".... fail.\n");
         for( int i = 0; i < numDet; i++){
            for( int j = 0 ; j < 9; j++) cTCorr[i][j] = 0.;
         }
      }

      TString name;
      for(int i = 0; i < numDet; i++){
         name.Form("f%d", i);
         f7[i] = new TF1(name, "pol7", -2, 2);
            
         for(int j = 0 ; j < 8; j++){
            f7[i]->SetParameter(j, cTCorr[i][j]);
         }

         if(file.is_open()) fList->Add(f7[i]);
      }

      if( file.is_open()){
         fList->Write("fList", TObject::kSingleKey);
      }

      file.close();
   }
   
   //========================================= reaction parameters
   printf("loading reaction parameter.");
   file.open("reaction.dat");
   isReaction = false;
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x ){
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) mass = atof(x.c_str());
         if( i == 1 ) q    = atof(x.c_str());
         if( i == 2 ) beta = atof(x.c_str()); 
         if( i == 3 ) Et   = atof(x.c_str()); 
         if( i == 4 ) massB = atof(x.c_str()); 
         i = i + 1;
      }
      printf("................. done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.;
      gamma = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamma * beta * perpDist ;
      printf("============\n");
      printf("mass-b   : %f MeV/c2 \n", mass);
      printf("charge-b : %f \n", q);
      printf("E-total  : %f MeV \n", Et);
      printf("mass-B   : %f MeV/c2 \n", massB);      
      printf("beta     : %f \n", beta);
      printf("B-field  : %f T \n", Bfield);
      printf("alpha    : %f MeV/mm \n", alpha);
      printf("perpDist : %f mm \n", perpDist);
      printf("G        : %f MeV \n", G);
      
      TMacro reactionData("reaction.dat");
      reactionData.Write("reactionPara");
   }else{
      printf("................. fail.\n");
      isReaction = false;
   }
   file.close();

   //====================================== load RDT cut
   TFile * fileCut = new TFile(""); //"rdtCuts.root");   
   TObjArray * cutList = NULL;
   isRDTCutExist = false;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      if( cutList != NULL){
         isRDTCutExist = true;
         const int numCut = cutList->GetEntries();
         cut = new TCutG * [numCut];
         printf("=========== found %d cuts in %s \n", numCut, fileCut->GetName());
         for( int i = 0 ; i < numCut; i++){
            cut[i] = (TCutG* ) cutList->At(i);
            printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
         }
      }

      cutList->Write("rdtCutList");
   }

   printf("================================== numDet : %d \n", numDet);
   

}

Bool_t Cali_e_trace::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Cali_e_trace_cxx
