//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct  8 15:01:45 2020 by ROOT version 6.14/00
// from TTree gen_tree/PSD Tree
// found on file: ../root_data/gen_run332.root
//////////////////////////////////////////////////////////

#ifndef MonGamma_h
#define MonGamma_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

class MonGamma : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           runID;
   //Float_t         e[100];
   //ULong64_t       e_t[100];
   //ULong64_t       EBIS;
   //ULong64_t       T1;
   //Float_t         ring[100];
   //ULong64_t       ring_t[100];
   //Float_t         xf[100];
   //ULong64_t       xf_t[100];
   //Float_t         xn[100];
   //ULong64_t       xn_t[100];
   //Float_t         x[100];
   Float_t         rdt[100];
   ULong64_t       rdt_t[100];
   //Float_t         tac[100];
   //ULong64_t       tac_t[100];
   //Float_t         elum[32];
   //ULong64_t       elum_t[32];
   //Float_t         ezero[10];
   //ULong64_t       ezero_t[10];

   // List of branches
   TBranch        *b_runID;   //!
   //TBranch        *b_Energy;   //!
   //TBranch        *b_EnergyTimestamp;   //!
   //TBranch        *b_EBISTimestamp;   //!
   //TBranch        *b_T1Timestamp;   //!
   //TBranch        *b_Ring;   //!
   //TBranch        *b_RingTimestamp;   //!
   //TBranch        *b_XF;   //!
   //TBranch        *b_XFTimestamp;   //!
   //TBranch        *b_XN;   //!
   //TBranch        *b_XNTimestamp;   //!
   //TBranch        *b_x;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   //TBranch        *b_TAC;   //!
   //TBranch        *b_TACTimestamp;   //!
   //TBranch        *b_ELUM;   //!
   //TBranch        *b_ELUMTimestamp;   //!
   //TBranch        *b_EZERO;   //!
   //TBranch        *b_EZEROTimestamp;   //!

   MonGamma(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~MonGamma() { }
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

   ClassDef(MonGamma,0);
};

#endif

#ifdef MonGamma_cxx
void MonGamma::Init(TTree *tree)
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

   fChain->SetBranchAddress("runID", &runID, &b_runID);
   //fChain->SetBranchAddress("e", e, &b_Energy);
   //fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   //fChain->SetBranchAddress("EBIS", &EBIS, &b_EBISTimestamp);
   //fChain->SetBranchAddress("T1", &T1, &b_T1Timestamp);
   //fChain->SetBranchAddress("ring", ring, &b_Ring);
   //fChain->SetBranchAddress("ring_t", ring_t, &b_RingTimestamp);
   //fChain->SetBranchAddress("xf", xf, &b_XF);
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   //fChain->SetBranchAddress("xn", xn, &b_XN);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   //fChain->SetBranchAddress("x", x, &b_x);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);
   //fChain->SetBranchAddress("tac", tac, &b_TAC);
   //fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   //fChain->SetBranchAddress("elum", elum, &b_ELUM);
   //fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   //fChain->SetBranchAddress("ezero", ezero, &b_EZERO);
   //fChain->SetBranchAddress("ezero_t", ezero_t, &b_EZEROTimestamp);
}

Bool_t MonGamma::Notify()
{

   return kTRUE;
}

#endif // #ifdef MonGamma_cxx
