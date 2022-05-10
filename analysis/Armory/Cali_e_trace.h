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

#include "../Armory/AnalysisLibrary.h"

// Headers needed by this particular selector

class Cali_e_trace : public TSelector {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain

   //********* IMPORTANT *********//
   // the array size MUST be larger or equal to the array size of source root file 
   // e.g. in the source root file has e[50], in here, e[50] or e[100] is OK, but not e[40]

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
   Float_t         tac[10];
   ULong64_t       tac_t[10];
   Float_t         elum[50];
   ULong64_t       elum_t[50];
   Float_t         ezero[50];
   ULong64_t       ezero_t[50];
   
   //Trace
   Float_t         te[100];
   Float_t         te_r[100];
   Float_t         te_t[100];
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
   //Float_t zpos;
   int det;    //
   int hitID[30]; // is e, xf, xn are all fired.
   int multiHit; // multipicity of z
   
   Float_t thetaCM;
   Float_t Ex;   
   Float_t thetaLab;
   
   Float_t rdtC[8];
   ULong64_t rdtC_t[8];
   int rdtID; // rdt hit ID
   int rdtdEMultiHit;

   Int_t   coin_t;
   Float_t tcoin_t;
   Float_t coinTimeUC; 
   Float_t coinTime;
   
   TObjArray * fList; //!
   
   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t validEventCount;
   
   //========correction parameters
   DetGeo detGeo;
   
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
   double eCorr2[30][2]; // e-correction
   double rdtCorr[8][2]; //rdt-correction
   
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
   if( numFile > 0 ) {
      int oldRunNum = -100;
      bool contFlag = false; // is runNumber continue;
      for( int i = 0; i < numFile ; i++){
         TString name = fChain->GetListOfFiles()->At(i)->GetTitle();
         int found = name.Last('/');
         name.Remove(0, found + 1 ); // this should give "XXX_run0XX.root"
         TString prefix = name;
         found = name.Last('.');
         name.Remove(found); // this should give "XXX_run0XX"
         found = name.Last('_');
         int runNum = name.Remove(0, found+4).Atoi(); // this should give the 3 digit run number 

         if( i == 0 ) {
            found = prefix.First("_");
            prefix.Remove(found);
            saveFileName = expName + "_" + prefix +  "_run";
         }

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
   }else{
      saveFileName.Form("%s_default.root", expName.Data());
   }
   printf("save file name : %s \n", saveFileName.Data());
   printf("---------------------------------------------\n");
   if( saveFileName == ".root" ) gROOT->ProcessLine(".q");
   
   saveFile = new TFile( saveFileName,"recreate");

   validEventCount = 0;
   
   //===================================================== loading parameter
   
   //========================================= detector Geometry
   printf("======================= loading parameters files .... \n");
   string detGeoFileName = "detectorGeo.txt";
   printf("loading detector geometery : %s.", detGeoFileName.c_str());

   TMacro * haha = new TMacro();
   if( haha->ReadFile(detGeoFileName.c_str()) > 0 ) {

      detGeo = LoadDetectorGeo(haha);

      PrintDetGeo(detGeo);

      Bfield = detGeo.Bfield;
      perpDist = detGeo.detPerpDist;
      firstPos = detGeo.firstPos;
      length = detGeo.detLength;
      jDet = detGeo.mDet;
      pos = detGeo.detPos;
      
      printf("... done.\n");
   }else{
      printf("... fail\n");
      Terminate();
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
   
      printf(" loading e correction second.");
   file.open("correction_e2.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr2[i][0] = a;  // 1/a1
         eCorr2[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf(".............. done.\n");
      
   }else{
      printf(".............. fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr2[i][0] = 1.;
         eCorr2[i][1] = 0.;
      }
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
      while( file >> a >> b){
         if( i >= 8) break;
         rdtCorr[i][0] = a;  // a1
         rdtCorr[i][1] = b;  // a2, e' = e * a1 + a0 
         i = i + 1;
      }
      printf("..................... done.\n");
      TMacro cali_rdt("correction_rdt.dat");
      cali_rdt.Write("correction_rdt");
   }else{
      printf("..................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         rdtCorr[i][0] = 1.;
         rdtCorr[i][1] = 0.;
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
   TFile * fileCut = new TFile("rdtCuts.root");   
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
      
      saveFile->cd();
      cutList->Write("rdtCutList");
   }

   TString ezCutFile = "EZCut.root";
   TFile * fileEZCut = new TFile(ezCutFile);
   TCutG * ezCut = (TCutG *) fileEZCut->FindObjectAny("cutez"); 

   printf("================================== numDet : %d \n", numDet);

   //====================== Create tree
   

   newTree =  new TTree("tree","tree");
   
   eventID = -1;
   run = 0;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   if( isRunIDExist )  newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,      eC, Form("e[%d]/F" , numDet) );
   newTree->Branch("xf",     xfC, Form("xf[%d]/F", numDet) );
   newTree->Branch("xn",     xnC, Form("xn[%d]/F", numDet) );
   newTree->Branch("ring",  ring, Form("xn[%d]/F", numDet) );
   newTree->Branch("x" ,       x, Form("x[%d]/F" , numDet) );
   newTree->Branch("z" ,       z, Form("z[%d]/F" , numDet) );
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", hitID, Form("hitID[%d]/I", numDet) );
   newTree->Branch("multiHit", &multiHit, "multiHit/I");
   
   newTree->Branch("Ex", &Ex, "Ex/F");
   newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
   newTree->Branch("thetaLab", &thetaLab, "thetaLab/F");
   
   newTree->Branch("e_t", eC_t, Form("e_t[%d]/l", numDet) );
   
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/l");
   newTree->Branch("rdtID", &rdtID, "rdtID/I");
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
      newTree->Branch("tac", tac, "tac[10]/F");
      newTree->Branch("tac_t", tac_t, "tac_t[10]/l");
   }
   
   newTree->Branch("coin_t", &coin_t, "coincident_time_from_digitizer/I");
   
   if( isTraceDataExist ){
      newTree->Branch("tcoin_t", &tcoin_t, "tcoin_t/F");
      newTree->Branch("coinTimeUC", &coinTimeUC, "coinTimeUnCalibrated_ns/F");
      newTree->Branch("coinTime", &coinTime, "coinTime_ns/F");
      newTree->Branch("te",             te,  Form("Trace_Energy[%d]/F", numDet) );
      newTree->Branch("te_r",         te_r,  Form("Trace_Energy_RiseTime[%d]/F", numDet));
      newTree->Branch("te_t",         te_t,  Form("Trace_Energy_Time[%d]/F", numDet));
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
