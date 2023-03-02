//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Feb 18 00:06:49 2023 by ROOT version 6.26/04
// from TTree tree/tree
// found on file: h083_85Kr_trace_run084_087_089-098.root
//////////////////////////////////////////////////////////

#ifndef NewMacroTraces_h
#define NewMacroTraces_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class NewMacroTraces {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eventID;
   Int_t           run;
   Float_t         e[24];
   Float_t         xf[24];
   Float_t         xn[24];
   Float_t         ring[24];
   Float_t         x[24];
   Float_t         z[24];
   Int_t           detID;
   Int_t           hitID[24];
   Int_t           multiHit;
   Float_t         Ex;
   Float_t         thetaCM;
   Float_t         thetaLab;
   ULong64_t       e_t[24];
   Float_t         rdt[20];
   ULong64_t       rdt_t[20];
   Int_t           rdtID;
   Int_t           rdtdEMultiHit;
   Float_t         elum[32];
   ULong64_t       elum_t[32];
   Float_t         ezero[10];
   ULong64_t       ezero_t[10];
   Int_t           coin_t;
   Float_t         tcoin_t;
   Float_t         coinTimeUC;
   Float_t         coinTime;
   Float_t         te[24];
   Float_t         te_r[24];
   Float_t         te_t[24];
   Float_t         trdt[20];
   Float_t         trdt_t[20];
   Float_t         trdt_r[20];

   // List of branches
   TBranch        *b_eventID;   //!
   TBranch        *b_run;   //!
   TBranch        *b_e;   //!
   TBranch        *b_xf;   //!
   TBranch        *b_xn;   //!
   //TBranch        *b_xn;   //!
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

   NewMacroTraces(TTree *tree=0);
   virtual ~NewMacroTraces();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef NewMacroTraces_cxx
NewMacroTraces::NewMacroTraces(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("h083_85Kr_trace_run084_087_089-098.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("h083_85Kr_trace_run084_087_089-098.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

NewMacroTraces::~NewMacroTraces()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NewMacroTraces::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NewMacroTraces::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void NewMacroTraces::Init(TTree *tree)
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
   fCurrent = -1;
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
   fChain->SetBranchAddress("rdtID", &rdtID, &b_rdtID);
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
   Notify();
}

Bool_t NewMacroTraces::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NewMacroTraces::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NewMacroTraces::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NewMacroTraces_cxx
