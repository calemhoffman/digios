//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Mar 23 13:43:23 2019 by ROOT version 6.10/02
// from TTree tree/tree
// found on file: template.root
//////////////////////////////////////////////////////////

#ifndef WSProof_h
#define WSProof_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TProofOutputFile.h>
#include <TMacro.h>
#include <vector>
#include <fstream>

#include "/home/goluckyryan/Desktop/digios/analysis/Woods-Saxon/RK4.h"
#include "/home/goluckyryan/Desktop/digios/analysis/Woods-Saxon/WS.h"

// Header file for the classes stored in the TTree if any.

class WSProof : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Double_t        V0;
   Double_t        R0;
   Double_t        r0;
   Double_t        a0;
   Double_t        VSO;
   Double_t        RSO;
   Double_t        rSO;
   Double_t        aSO;

   // List of branches
   TBranch        *b_V0;   //!
   TBranch        *b_R0;   //!
   TBranch        *b_r0;   //!
   TBranch        *b_a0;   //!
   TBranch        *b_VSO;   //!
   TBranch        *b_RSO;   //!
   TBranch        *b_rSO;   //!
   TBranch        *b_aSO;   //!
   
   TString energyFile; 
   TMacro expEnergy;
   vector<string> NLJ;
	vector<double> BE;  //binding enegry of orbital
   void ReadEnergyFile(TString expFile);
   vector<string> SplitStr(string tempLine, string splitter, int shift=0);
   
   TString saveFileName;
   TFile * saveFile; //!
   TProofOutputFile * proofFile ; //!
   TTree * newTree; //!
   
   double rms, lesq; // rms, least-sq
   WSPara wsp;
   
   WSProof(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~WSProof() { }
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

   ClassDef(WSProof,0);
};

#endif

#ifdef WSProof_cxx
void WSProof::Init(TTree *tree)
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

   fChain->SetBranchAddress("V0", &V0, &b_V0);
   fChain->SetBranchAddress("R0", &R0, &b_R0);
   fChain->SetBranchAddress("r0", &r0, &b_r0);
   fChain->SetBranchAddress("a0", &a0, &b_a0);
   fChain->SetBranchAddress("VSO", &VSO, &b_VSO);
   fChain->SetBranchAddress("RSO", &RSO, &b_RSO);
   fChain->SetBranchAddress("rSO", &rSO, &b_rSO);
   fChain->SetBranchAddress("aSO", &aSO, &b_aSO);
   
   
}

Bool_t WSProof::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef WSProof_cxx
