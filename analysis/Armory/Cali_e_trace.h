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
   //======== coin cut   
   TCutG * cutcoin0 = NULL;
    TCutG * cutcoin1 = NULL;
     TCutG * cutcoin2 = NULL;
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
        if( firstPos > 0 ) pos[id] = firstPos + posTemp[iDet-1-id];
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
   
    //====================================== load coin cut

      cutcoin0 = new TCutG("cutcoin0",30);
   cutcoin0->SetVarX("z");
   cutcoin0->SetVarY("coinTimeUC");
   cutcoin0->SetTitle("Graph");
   cutcoin0->SetFillStyle(1000);
   cutcoin0->SetPoint(0,516.709,-1986.4);
   cutcoin0->SetPoint(1,522.236,-1944.34);
   cutcoin0->SetPoint(2,526.945,-1902.74);
   cutcoin0->SetPoint(3,532.472,-1870.02);
   cutcoin0->SetPoint(4,540.456,-1852.26);
   cutcoin0->SetPoint(5,548.44,-1862.55);
   cutcoin0->SetPoint(6,555.196,-1901.8);
   cutcoin0->SetPoint(7,561.747,-1963.03);
   cutcoin0->SetPoint(8,568.503,-2007.9);
   cutcoin0->SetPoint(9,576.487,-1994.35);
   cutcoin0->SetPoint(10,593.069,-1963.5);
   cutcoin0->SetPoint(11,605.352,-1956.02);
   cutcoin0->SetPoint(12,616.816,-1965.84);
   cutcoin0->SetPoint(13,629.918,-1996.21);
   cutcoin0->SetPoint(14,636.265,-2010.7);
   cutcoin0->SetPoint(15,655.304,-1987.33);
   cutcoin0->SetPoint(16,671.067,-1987.33);
   cutcoin0->SetPoint(17,685.602,-2007.9);
   cutcoin0->SetPoint(18,697.271,-1999.02);
   cutcoin0->SetPoint(19,714.058,-1985.93);
   cutcoin0->SetPoint(20,735.144,-1994.35);
   cutcoin0->SetPoint(21,746.199,-2007.43);
   cutcoin0->SetPoint(22,749.065,-2009.77);
   cutcoin0->SetPoint(23,760.325,-1991.54);
   cutcoin0->SetPoint(24,782.844,-1983.6);
   cutcoin0->SetPoint(25,797.788,-1990.61);
   cutcoin0->SetPoint(26,810.686,-2014.91);
   cutcoin0->SetPoint(27,810.276,-2035.01);
   cutcoin0->SetPoint(28,516.709,-2025.19);
   cutcoin0->SetPoint(29,516.709,-1986.4);

   cutcoin1 = new TCutG("cutcoin1",53);
   cutcoin1->SetVarX("z");
   cutcoin1->SetVarY("coinTimeUC");
   cutcoin1->SetTitle("Graph");
   cutcoin1->SetFillStyle(1000);
   cutcoin1->SetPoint(0,451.053,-2066.04);
   cutcoin1->SetPoint(1,461.833,-2030.82);
   cutcoin1->SetPoint(2,469.117,-1995.1);
   cutcoin1->SetPoint(3,480.188,-1966.43);
   cutcoin1->SetPoint(4,488.346,-1964.41);
   cutcoin1->SetPoint(5,498.834,-1994.6);
   cutcoin1->SetPoint(6,506.701,-2040.38);
   cutcoin1->SetPoint(7,510.197,-2061.01);
   cutcoin1->SetPoint(8,517.772,-2051.95);
   cutcoin1->SetPoint(9,524.182,-2014.72);
   cutcoin1->SetPoint(10,529.717,-1974.98);
   cutcoin1->SetPoint(11,538.166,-1947.31);
   cutcoin1->SetPoint(12,546.033,-1944.79);
   cutcoin1->SetPoint(13,555.938,-1964.41);
   cutcoin1->SetPoint(14,564.096,-2032.33);
   cutcoin1->SetPoint(15,571.671,-2061.51);
   cutcoin1->SetPoint(16,576.916,-2061.01);
   cutcoin1->SetPoint(17,583.325,-2030.32);
   cutcoin1->SetPoint(18,592.648,-1995.1);
   cutcoin1->SetPoint(19,603.137,-1980.51);
   cutcoin1->SetPoint(20,615.373,-1997.12);
   cutcoin1->SetPoint(21,621.783,-2028.31);
   cutcoin1->SetPoint(22,630.524,-2065.54);
   cutcoin1->SetPoint(23,636.642,-2064.53);
   cutcoin1->SetPoint(24,644.508,-2025.79);
   cutcoin1->SetPoint(25,647.13,-1997.12);
   cutcoin1->SetPoint(26,652.083,-1965.92);
   cutcoin1->SetPoint(27,659.658,-1950.33);
   cutcoin1->SetPoint(28,666.359,-1951.33);
   cutcoin1->SetPoint(29,671.604,-1966.93);
   cutcoin1->SetPoint(30,679.179,-2011.2);
   cutcoin1->SetPoint(31,686.171,-2049.44);
   cutcoin1->SetPoint(32,689.959,-2059.5);
   cutcoin1->SetPoint(33,693.163,-2058.49);
   cutcoin1->SetPoint(34,698.408,-2031.33);
   cutcoin1->SetPoint(35,701.904,-2005.17);
   cutcoin1->SetPoint(36,707.439,-1979);
   cutcoin1->SetPoint(37,712.392,-1954.86);
   cutcoin1->SetPoint(38,721.715,-1939.26);
   cutcoin1->SetPoint(39,730.165,-1954.86);
   cutcoin1->SetPoint(40,740.653,-2014.22);
   cutcoin1->SetPoint(41,748.228,-2054.97);
   cutcoin1->SetPoint(42,756.386,-2034.85);
   cutcoin1->SetPoint(43,764.835,-1988.56);
   cutcoin1->SetPoint(44,777.946,-1965.42);
   cutcoin1->SetPoint(45,790.182,-1984.04);
   cutcoin1->SetPoint(46,798.049,-2014.22);
   cutcoin1->SetPoint(47,805.041,-2068.05);
   cutcoin1->SetPoint(48,807.08,-2082.64);
   cutcoin1->SetPoint(49,799.797,-2095.72);
   cutcoin1->SetPoint(50,453.093,-2087.67);
   cutcoin1->SetPoint(51,453.093,-2087.67);
   cutcoin1->SetPoint(52,451.053,-2066.04);
   
   cutcoin2 = new TCutG("cutcoin2",32);
   cutcoin2->SetVarX("z");
   cutcoin2->SetVarY("coinTimeUC");
   cutcoin2->SetTitle("Graph");
   cutcoin2->SetFillStyle(1000);
   cutcoin2->SetPoint(0,517.218,-1978.97);
   cutcoin2->SetPoint(1,523.888,-1925.04);
   cutcoin2->SetPoint(2,527.48,-1884.98);
   cutcoin2->SetPoint(3,531.585,-1844.14);
   cutcoin2->SetPoint(4,536.972,-1815.64);
   cutcoin2->SetPoint(5,542.873,-1804.85);
   cutcoin2->SetPoint(6,548.004,-1808.71);
   cutcoin2->SetPoint(7,552.622,-1832.59);
   cutcoin2->SetPoint(8,560.832,-1914.25);
   cutcoin2->SetPoint(9,566.989,-1965.87);
   cutcoin2->SetPoint(10,570.324,-1981.28);
   cutcoin2->SetPoint(11,583.665,-1966.64);
   cutcoin2->SetPoint(12,597.006,-1938.14);
   cutcoin2->SetPoint(13,613.425,-1945.84);
   cutcoin2->SetPoint(14,626.766,-1974.35);
   cutcoin2->SetPoint(15,634.976,-1991.29);
   cutcoin2->SetPoint(16,652.934,-1970.49);
   cutcoin2->SetPoint(17,669.61,-1973.57);
   cutcoin2->SetPoint(18,689.878,-1992.06);
   cutcoin2->SetPoint(19,706.554,-1975.12);
   cutcoin2->SetPoint(20,729.131,-1967.41);
   cutcoin2->SetPoint(21,746.833,-1991.29);
   cutcoin2->SetPoint(22,754.53,-1988.21);
   cutcoin2->SetPoint(23,772.745,-1968.18);
   cutcoin2->SetPoint(24,796.348,-1971.26);
   cutcoin2->SetPoint(25,809.432,-1994.38);
   cutcoin2->SetPoint(26,808.919,-2016.72);
   cutcoin2->SetPoint(27,808.919,-2016.72);
   cutcoin2->SetPoint(28,533.637,-2019.03);
   cutcoin2->SetPoint(29,513.113,-1997.46);
   cutcoin2->SetPoint(30,513.113,-1997.46);
   cutcoin2->SetPoint(31,517.218,-1978.97);
   
   
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
