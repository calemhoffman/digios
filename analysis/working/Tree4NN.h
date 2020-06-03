//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun  1 16:00:16 2020 by ROOT version 6.18/04
// from TTree tree/tree
// found on file: A_trace_run055-057_059-085_087-099.root
//////////////////////////////////////////////////////////

#ifndef Tree4NN_h
#define Tree4NN_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TBenchmark.h>
#include <TF1.h>
#include <string>
#include <fstream>
#include <TObjArray.h>
#include <TCutG.h>
#include "TClonesArray.h"

// Header file for the classes stored in the TTree if any.

class Tree4NN : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eventID;
   Int_t           run;
   Float_t         e[30];
   Float_t         xf[30];
   Float_t         xn[30];
   Float_t         ring[30];
   Float_t         x[30];
   Float_t         z[30];
   Int_t           detID;
   Int_t           hitID[30];
   Int_t           multiHit;
   Float_t         Ex;
   Float_t         thetaCM;
   Float_t         thetaLab;
   ULong64_t       e_t[30];
   Float_t         rdt[8];
   ULong64_t       rdt_t[8];
   Int_t           rdtID[8];
   Int_t           rdtdEMultiHit;
   Float_t         elum[32];
   ULong64_t       elum_t[32];
   Float_t         ezero[10];
   ULong64_t       ezero_t[10];
   Int_t           coin_t;
   Float_t         tcoin_t;
   Float_t         coinTimeUC;
   Float_t         coinTime;
   Float_t         te[30];
   Float_t         te_r[30];
   Float_t         te_t[30];
   Float_t         trdt[8];
   Float_t         trdt_t[8];
   Float_t         trdt_r[8];

   // List of branches
   TBranch        *b_eventID;   //!
   TBranch        *b_run;   //!
   TBranch        *b_e;   //!
   TBranch        *b_xf;   //!
   TBranch        *b_xn;   //!
   TBranch        *b_x;   //!
   TBranch        *b_z;   //!
   TBranch        *b_det;   //!
   TBranch        *b_hitID;   //!
   TBranch        *b_multiHit;   //!
   TBranch        *b_Ex;   //!
   TBranch        *b_thetaCM;   //!
   TBranch        *b_thetaLab;   //!
   TBranch        *b_e_t;   //!
   TBranch        *b_rdtC;   //!
   TBranch        *b_rdtC_t;   //!
   TBranch        *b_rdtID;   //!
   TBranch        *b_rdtdEMultiHit;   //!
   TBranch        *b_elum;   //!
   TBranch        *b_elum_t;   //!
   TBranch        *b_ezero;   //!
   TBranch        *b_ezero_t;   //!
   TBranch        *b_coincident_time_from_digitizer;   //!
   TBranch        *b_tcoin_t;   //!
   TBranch        *b_coinTimeUnCalibrated_ns;   //!
   TBranch        *b_coinTime_ns;   //!
   TBranch        *b_Trace_Energy;   //!
   TBranch        *b_Trace_Energy_RiseTime;   //!
   TBranch        *b_Trace_Energy_Time;   //!
   TBranch        *b_Trace_RDT;   //!
   TBranch        *b_Trace_RDT_Time;   //!
   TBranch        *b_Trace_RDT_RiseTime;   //!

   Tree4NN(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Tree4NN() { }
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

   ClassDef(Tree4NN,0);
   
   //==========================
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   int totnumEntry; // of original root
   
   //tree  
   Float_t e_n;
   Float_t z_n;
   Float_t coinTime_n;
   Int_t detID_n;
   
   Float_t rdtdE;
   Float_t rdtE;
   Int_t rdtID_n;
   
   Float_t Ex_n;
   
   Int_t isGood;
   
   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t validEventCount;
   
   //========================== RDT cut
   bool isRDTCutExist;
   TCutG ** cut = NULL;
   
   double rdtScale[8];
   
};

#endif

#ifdef Tree4NN_cxx
void Tree4NN::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   
   //======================================   
   totnumEntry = tree->GetEntries();
   printf( "=========================================================================== \n");
   printf( "==========================   Tree4NN.h    ================================= \n");
   printf( "====== Make a new tree for nerual network, total Entry : %d \n", totnumEntry);
   printf( "=========================================================================== \n");

   
   fChain = (TChain *)tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eventID", &eventID, &b_eventID);
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("e", e, &b_e);
   fChain->SetBranchAddress("xf", xf, &b_xf);
   fChain->SetBranchAddress("xn", xn, &b_xn);
   fChain->SetBranchAddress("ring", ring, &b_xn);
   fChain->SetBranchAddress("x", x, &b_x);
   fChain->SetBranchAddress("z", z, &b_z);
   fChain->SetBranchAddress("detID", &detID, &b_det);
   fChain->SetBranchAddress("hitID", hitID, &b_hitID);
   fChain->SetBranchAddress("multiHit", &multiHit, &b_multiHit);
   fChain->SetBranchAddress("Ex", &Ex, &b_Ex);
   fChain->SetBranchAddress("thetaCM", &thetaCM, &b_thetaCM);
   fChain->SetBranchAddress("thetaLab", &thetaLab, &b_thetaLab);
   fChain->SetBranchAddress("e_t", e_t, &b_e_t);
   fChain->SetBranchAddress("rdt", rdt, &b_rdtC);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_rdtC_t);
   fChain->SetBranchAddress("rdtID", rdtID, &b_rdtID);
   fChain->SetBranchAddress("rdtdEMultiHit", &rdtdEMultiHit, &b_rdtdEMultiHit);
   fChain->SetBranchAddress("elum", elum, &b_elum);
   fChain->SetBranchAddress("elum_t", elum_t, &b_elum_t);
   fChain->SetBranchAddress("ezero", ezero, &b_ezero);
   fChain->SetBranchAddress("ezero_t", ezero_t, &b_ezero_t);
   fChain->SetBranchAddress("coin_t", &coin_t, &b_coincident_time_from_digitizer);
   fChain->SetBranchAddress("tcoin_t", &tcoin_t, &b_tcoin_t);
   fChain->SetBranchAddress("coinTimeUC", &coinTimeUC, &b_coinTimeUnCalibrated_ns);
   fChain->SetBranchAddress("coinTime", &coinTime, &b_coinTime_ns);
   fChain->SetBranchAddress("te", te, &b_Trace_Energy);
   fChain->SetBranchAddress("te_r", te_r, &b_Trace_Energy_RiseTime);
   fChain->SetBranchAddress("te_t", te_t, &b_Trace_Energy_Time);
   fChain->SetBranchAddress("trdt", trdt, &b_Trace_RDT);
   fChain->SetBranchAddress("trdt_t", trdt_t, &b_Trace_RDT_Time);
   fChain->SetBranchAddress("trdt_r", trdt_r, &b_Trace_RDT_RiseTime);


   //===========================
   saveFileName = "nntree.root";
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   newTree->Branch("e" ,   &e_n, "e/F");
   newTree->Branch("z",    &z_n, "z/F");
   newTree->Branch("coinTime",  &coinTime_n, "coinTime/F");
   newTree->Branch("detID",  &detID_n, "detID/I");
   newTree->Branch("rdtdE",  &rdtdE, "rdtdE/F");
   newTree->Branch("rdtE",  &rdtE, "rdtE/F");
   newTree->Branch("rdtID",  &rdtID_n, "rdtID/I");
   
   newTree->Branch("Ex",  &Ex_n, "Ex/F");
   
   newTree->Branch("isGood",  &isGood, "isGood/I");
   
   rdtScale[0] = 1.10;
   rdtScale[1] = 0.54;
   rdtScale[2] = 1.00;
   rdtScale[3] = 1.00;
   rdtScale[4] = 1.00;
   rdtScale[5] = 1.00;
   rdtScale[6] = 1.05;
   rdtScale[7] = 0.50;
   
   //===========================
   TFile * fileCut = new TFile("rdtCuts_15C_tight.root");   
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
   
   
   //=== clock
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   validEventCount = 0;
   
   printf("===========================================\n");


}

Bool_t Tree4NN::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Tree4NN_cxx
