//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 10 21:31:47 2018 by ROOT version 6.10/02
// from TTree gen_tree/PSD Tree
// found on file: gen_run11.root
//////////////////////////////////////////////////////////

//#######################################################//
//   this processor is for gating before AutoCalibration
//---------------------------------------------------------
//   if correction_coinTime.dat exist => calculate coinTime
//                                  else gives uncorrected coinTime
//#######################################################//

#ifndef Cali_littleTree_trace_h
#define Cali_littleTree_trace_h

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


#include "../Armory/AnalysisLibrary.h"

// Headers needed by this particular selector

class Cali_littleTree_trace : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Float_t         e[100];
   ULong64_t       e_t[100];
   Float_t         ring[100];
   Float_t         xf[100];
   Float_t         xn[100];
   Float_t         rdt[100];
   ULong64_t       rdt_t[100];
   
   //Trace
   Float_t         te_t[30];
   Float_t         trdt[8];
   Float_t         trdt_t[8];

   // List of branches
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_Ring;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   
   TBranch        *b_Trace_Energy_Time;  //!   
   TBranch        *b_Trace_RDT;  //!
   TBranch        *b_Trace_RDT_Time;  //!
   
   bool isTraceDataExist; // if b_Trace_** exist

   Cali_littleTree_trace(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Cali_littleTree_trace() { }
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

   ClassDef(Cali_littleTree_trace,0);
   
   //=============================== 
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   int totnumEntry; // of original root
  
   //tree  
   Int_t eventID;
   double eTemp;
   double xTemp; // unadjusted position, range (-1,1)
   double zTemp; 
   int detIDTemp;
   int hitID; // is e, xf, xn are all fired.
   int multiHit; // multipicity of z
   int rdtdEMultiHit; // multipicity of recoil-dE
   
   Float_t coinTimeUC; 
   Float_t coinTime;
   
   TObjArray * fList; //!
   
   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   //========correction parameters
   int numDet;
   int iDet; // number of detector at different position
   int jDet; // number of detector at same position
   vector<double> pos;
   double Bfield;
   double a;
   double length;
   double firstPos;

   DetGeo detGeo;
   
   double xnCorr[30]; // xn correction for xn = xf
   double xfxneCorr[30][2]; //xf, xn correction for e = xf + xn
   double xScale[30]; //scale x to full range
   double cTCorr[30][9]; // coinTime correction
   bool isCoinTimeLoaded;
   TF1 ** f7 ; //!
   
   //============ RDT cut
   bool isRDTCutExist;
   TCutG ** cut = NULL;
   
};

#endif

#ifdef Cali_littleTree_trace_cxx
void Cali_littleTree_trace::Init(TTree *tree)
{   
   totnumEntry = tree->GetEntries();
   printf( "=========================================================================== \n");
   printf( "========================= Cali_littleTree_trace.C ========================= \n");
   printf( "=== Make a small tree with calibration xn-xf and gate, total Entry : %d \n", totnumEntry);
   printf( "=========================================================================== \n");
   
   // Set branch addresses and branch pointers
   ////999999999   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("ring", ring, &b_Ring);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);

   TBranch * br = NULL;
   int traceData = 0;
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("te_t");
   if( br == NULL ) {
      printf(" WARNING! cannot find Branch: te_t -> proceed without coincident-time calculation. \n");
   }else{
      traceData ++;
      fChain->SetBranchAddress("te_t", te_t, &b_Trace_Energy_Time);
   }

   br = (TBranch *) fChain->GetListOfBranches()->FindObject("trdt");
   if( br == NULL ) {
      printf(" WARNING! cannot find Branch: trdt -> proceed without coincident-time calculation. \n");
   }else {
      traceData ++;
      fChain->SetBranchAddress("trdt", trdt, &b_Trace_RDT);   
   }
   
   br = (TBranch *) fChain->GetListOfBranches()->FindObject("trdt_t");
   if( br == NULL ) {
      printf(" WARNING! cannot find Branch: trdt_t -> proceed without coincident-time calculation. \n");
   }else {
      traceData ++;
      fChain->SetBranchAddress("trdt_t", trdt_t, &b_Trace_RDT_Time);   
   }
   
   isTraceDataExist = false;
   if( traceData == 3 ) isTraceDataExist = true;
   
   //===================================================== loading parameter
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());

   TMacro * haha = new TMacro();
   if( haha->ReadFile(detGeoFileName.c_str()) > 0 ) {

      detGeo = LoadDetectorGeo(haha);

      PrintDetGeo(detGeo);

      iDet = detGeo.nDet;
      jDet = detGeo.mDet;

      Bfield = detGeo.Bfield;
      a = detGeo.detPerpDist;
      length = detGeo.detLength;
      firstPos = detGeo.firstPos;
	pos = detGeo.detPos;
      
      printf("... done.\n");
   }else{
      printf("... fail\n");
      Terminate();
   }

   numDet = iDet * jDet;
   
   //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   ifstream file;
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         //xnCorr[i] = 1;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for( int i = 0 ; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   
   //========================================= e = xf + xn correction
   printf("----- loading xf/xn-e correction.");
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
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for( int i = 0 ; i < numDet; i++){
         xfxneCorr[i][0] = 0.0;
         xfxneCorr[i][1] = 1.0;
      }
   }
   file.close();
   
   //========================================= xf = xn correction
   printf("----- loading scale-X correction.");
   file.open("correction_scaleX.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xScale[i] = a;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for( int i = 0 ; i < numDet; i++){
         xScale[i] = 1;
      }
   }
   file.close();

   //========================================= coinTime correction
   if( isTraceDataExist ) {
   
      printf("----- loading coin-Time correction parameters.");
      file.open("correction_coinTime.dat");
      
      
      if( file.is_open() ){
         isCoinTimeLoaded = true;
         double d, a0, a1, a2, a3, a4, a5, a6, a7, a8;
         int i = 0;
         while( file >> d >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8){
            if( i >= numDet) break;
            cTCorr[i][0] = a0;
            cTCorr[i][1] = a1;
            cTCorr[i][2] = a2;
            cTCorr[i][3] = a3;
            cTCorr[i][4] = a4;
            cTCorr[i][5] = a5;
            cTCorr[i][6] = a6;
            cTCorr[i][7] = a7;
            cTCorr[i][8] = a8;
            //printf("\n%2d, a0: %7.4f, a1: %7.4f .... a7: %7.4f", i, cTCorr[i][0], cTCorr[i][1], cTCorr[i][7]);
            i = i + 1;
         }
         printf("... done.\n");

      }else{
         isCoinTimeLoaded = false;
         printf("... fail.\n");
         printf(" ---> coincident time has x-dependent\n");
         for( int i = 0; i < numDet; i++){
            for( int j = 0 ; j < 9; j++){
               cTCorr[i][j] = 0.;
            } 
         }
      } 
      file.close();
   }
   
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
   }
   
   //====================================== make tree
   saveFileName = "temp.root"; 
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   eventID = -1;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   
   newTree->Branch("e" ,  &eTemp, "eTemp/D");
   newTree->Branch("x" ,  &xTemp, "xTemp/D");
   newTree->Branch("z" ,  &zTemp, "zTemp/D");
   newTree->Branch("detID", &detIDTemp, "detIDTemp/I");
   newTree->Branch("hitID", &hitID, "hitID/I");
   newTree->Branch("multiHit", &multiHit, "multiHit/I");
      
   if( isTraceDataExist ) {
   
      newTree->Branch("coinTimeUC", &coinTimeUC, "coinTimeUC/F");
      
      TString name;
      f7 = new TF1*[numDet];
      fList = new TObjArray();
      for(int i = 0; i < numDet; i++){
         name.Form("f%d", i);
         f7[i] = new TF1(name, "pol7", -2, 2);
         
         for(int j = 0 ; j < 8; j++){
            f7[i]->SetParameter(j, cTCorr[i][j]);
         }
         fList->Add(f7[i]);
      }
      
      if( isCoinTimeLoaded ){
         fList->Write("fList", TObject::kSingleKey);
         newTree->Branch("coinTime", &coinTime, "coinTime/F");
      }
   
   }
   
   //====== clock
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   
   printf("================================== numDet : %d \n", numDet);
   

}

Bool_t Cali_littleTree_trace::Notify()
{

   return kTRUE;
}


#endif // #ifdef Cali_littleTree_trace_cxx
