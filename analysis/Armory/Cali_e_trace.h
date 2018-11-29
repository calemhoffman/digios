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
#include "TClonesArray.h"

// Headers needed by this particular selector


class Cali_e_trace : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           runID;
   Float_t         e[100];
   ULong64_t       e_t[100];
   ULong64_t       EBIS_t;
   Float_t         xf[100];
   ULong64_t       xf_t[100];
   Float_t         xn[100];
   ULong64_t       xn_t[100];
   Float_t         rdt[100];
   ULong64_t       rdt_t[100];
   Float_t         tac[100];
   ULong64_t       tac_t[100];
   Float_t         elum[32];
   ULong64_t       elum_t[32];
   Float_t         ezero[10];
   ULong64_t       ezero_t[10];
   
   //Trace
   Float_t         te[24];
   Float_t         te_r[24];
   Float_t         te_t[24];
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
   Float_t eC[24];
   Float_t xfC[24];
   Float_t xnC[24];
   ULong64_t eC_t[24];
   Float_t x[24]; // unadjusted position, range (-1,1)
   Float_t z[24]; 
   int det;    //TODO, when multiHit, which is detID?
   int hitID[24]; // is e, xf, xn are all fired.
   int multiHit; // multipicity of z
   
   Float_t thetaCM;
   Float_t Ex;   
   Float_t thetaLab;
   
   Float_t rdtC[8];
   ULong64_t rdtC_t[8];
   int rdtID[8]; // rdt hit ID
   int rdtMultiHit;
   
   int arrayRDT; // det(0-5)  | 0 = rdt(3,7), 1 = rdt(0,4), 2 = rdt(1,5), 3 = rdt(2,6) 
                 // det(6-11) | 3 = rdt(3,7), 0 = rdt(0,4), 1 = rdt(1,5), 2 = rdt(2,6) 
                 // det(12-17)| 2 = rdt(3,7), 3 = rdt(0,4), 0 = rdt(1,5), 1 = rdt(2,6) 
                 // det(18-23)| 1 = rdt(3,7), 2 = rdt(0,4), 3 = rdt(1,5), 0 = rdt(2,6) 
   
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
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   double eCorr[24][2]; // e-correction
   double rdtCorr[8]; //rdt-correction
   
   double cTCorr[24][9]; // coinTime correction
   TF1 ** f7 ; //!
   
   bool isReaction;
   double G, Z, H; // for excitation calcualtion
   double mass, q;
   double beta, gamma;
   double alpha ;
   double Et, massB;


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

   
   fChain = tree;
   //printf("========== number of tree loaded : %d \n", fChain->GetNTree());
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runID", &runID, &b_runID);
   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);   
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   
   isEBISExist = false;
   TBranch * br = (TBranch *) fChain->GetListOfBranches()->FindObject("EBIS");
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
      printf(" ++++++++ no zero data.\n");
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
   

   //=================
   saveFileName = fChain->GetDirectory()->GetName();
   //remove any folder path to get the name;
   int found;
   do{
      found = saveFileName.First("/");
      saveFileName.Remove(0,found+1);
   }while( found >= 0 );
   saveFileName = "A_" + saveFileName; 
   
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   eventID = 0;
   run = 0;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,   eC, "e[24]/F");
   //newTree->Branch("xf",  xfC, "xf[24]/F");
   //newTree->Branch("xn",  xnC, "xn[24]/F");
   newTree->Branch("x" ,    x, "x[24]/F");
   newTree->Branch("z" ,    z, "z[24]/F");
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", hitID, "hitID[24]/I");
   newTree->Branch("multiHit", &multiHit, "multiHit/I");
   
   newTree->Branch("Ex", &Ex, "Ex/F");
   newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
   newTree->Branch("thetaLab", &thetaLab, "thetaLab/F");
   
   newTree->Branch("e_t", eC_t, "e_t[24]/l");
   
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/l");
   newTree->Branch("rdtID", rdtID, "rdtID[8]/I");
   newTree->Branch("rdtMultiHit", &rdtMultiHit, "rdtMultiHit/I");
   
   newTree->Branch("arrayRDT", &arrayRDT, "arrayRDT/I");
   
   if( isEBISExist ) newTree->Branch("ebis_t", &EBIS_t, "EBIS_t/l");
   if( isELUMExist ) newTree->Branch("elum", elum, "elum[32]/F");
   if( isEZEROExist ) newTree->Branch("ezero", ezero, "ezero[10]/F");
   
   newTree->Branch("coin_t", &coin_t, "coin_t/I");
   
   if( isTraceDataExist ){
      newTree->Branch("tcoin_t", &tcoin_t, "tcoin_t/F");
      newTree->Branch("coinTimeUC", &coinTimeUC, "coinTimeUC/F");
      newTree->Branch("coinTime", &coinTime, "coinTime/F");
       
      newTree->Branch("te",     &teS,     "teS/F");
      newTree->Branch("te_t",   &te_tS,   "te_tS/F");
      newTree->Branch("te_r",   &te_rS,   "te_rS/F");
      newTree->Branch("trdt",   &trdtS,   "trdtS/F");
      newTree->Branch("trdt_t", &trdt_tS, "trdt_tS/F");
      newTree->Branch("trdt_r", &trdt_rS, "trdt_rS/F");
   }
   
   //=== clock
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   
   //===================================================== loading parameter
   
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) Bfield   = atof(x.c_str());
         if( i == 2 ) a        = atof(x.c_str());         
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       Bfield = 2.5;
       firstPos =  0;
       a = 9;
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
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      
      for(int i = 0; i < numDet; i++){
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
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   
   printf("----- loading e correction.");
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
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < 24 ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
      //return;
   }
   file.close();
   
   //========================================= rdt correction
   
   printf("----- loading rdt correction.");
   file.open("correction_rdt.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a){
         if( i >= 8) break;
         rdtCorr[i] = a;  //
         i = i + 1;
      }
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < 24 ; i++){
         rdtCorr[i] = 1.;
      }
   }
   file.close();
   
   //========================================= coinTime correction
   if( isTraceDataExist ){
      printf("----- loading coin-Time correction parameters.");
      file.open("correction_coinTime.dat");
      
      f7 = new TF1*[numDet];
      fList = new TObjArray();
      
      if( file.is_open() ){
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
            printf("\n%2d, a0: %f, a1: %f .... a7: %f", i, cTCorr[i][0], cTCorr[i][1], cTCorr[i][7]);
            i = i + 1;
         }
         printf("... done.\n");
         
      }else{
         printf("... fail.\n");
         for( int i = 0; i < numDet; i++){
            for( int j = 0 ; j < 9; j++){
               cTCorr[i][j] = 0.;
            } 
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
   printf("----- loading reaction parameter.");
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
      printf("... done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.;
      gamma = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamma * beta * a ;
      printf("============\n");
      printf("mass-b  : %f MeV/c2 \n", mass);
      printf("charge-b: %f \n", q);
      printf("E-total : %f MeV \n", Et);
      printf("mass-B  : %f MeV/c2 \n", massB);      
      printf("beta    : %f \n", beta);
      printf("B-field : %f T \n", Bfield);
      printf("alpha   : %f MeV/mm \n", alpha);
      printf("a       : %f mm \n", a);
      printf("G       : %f MeV \n", G);


   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();

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
