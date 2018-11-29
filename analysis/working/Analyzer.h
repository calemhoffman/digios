//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 18 14:41:10 2018 by ROOT version 6.12/06
// from TTree tree/tree
// found on file: A_gen_run064.root
//////////////////////////////////////////////////////////

#ifndef Analyzer_h
#define Analyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

class Analyzer : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eventID;
   Int_t           run;
   Float_t         e[24];
   Float_t         x[24];
   Float_t         z[24];
   Int_t           detID;
   Int_t           hitID[24];
   Int_t           multiHit;
   Float_t         Ex;
   Float_t         thetaCM;
   Float_t         thetaLab;
   ULong64_t       e_t[24];
   Float_t         rdt[8];
   ULong64_t       rdt_t[8];
   Int_t           rdtID[8];
   Int_t           rdtMultiHit;
   Int_t           arrayRDT;
   ULong64_t       ebis_t;
   Float_t         elum[32];
   Float_t         ezero[10];
   Int_t           coin_t;

   // List of branches
   TBranch        *b_eventID;   //!
   TBranch        *b_run;   //!
   TBranch        *b_e;   //!
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
   TBranch        *b_rdtMultiHit;   //!
   TBranch        *b_arrayRDT;   //!
   TBranch        *b_EBIS_t;   //!
   TBranch        *b_elum;   //!
   TBranch        *b_ezero;   //!
   TBranch        *b_coin_t;   //!

   Analyzer(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Analyzer() { }
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

   ClassDef(Analyzer,0);
};

#endif

#ifdef Analyzer_cxx
void Analyzer::Init(TTree *tree)
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
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eventID", &eventID, &b_eventID);
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("e", e, &b_e);
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
   fChain->SetBranchAddress("rdtMultiHit", &rdtMultiHit, &b_rdtMultiHit);
   fChain->SetBranchAddress("arrayRDT", &arrayRDT, &b_arrayRDT);
   fChain->SetBranchAddress("ebis_t", &ebis_t, &b_EBIS_t);
   fChain->SetBranchAddress("elum", elum, &b_elum);
   fChain->SetBranchAddress("ezero", ezero, &b_ezero);
   fChain->SetBranchAddress("coin_t", &coin_t, &b_coin_t);
}

Bool_t Analyzer::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Analyzer_cxx
