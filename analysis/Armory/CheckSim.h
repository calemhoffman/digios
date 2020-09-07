//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Sep  6 16:33:27 2020 by ROOT version 6.18/04
// from TTree tree/tree
// found on file: transfer.root
//////////////////////////////////////////////////////////

#ifndef CheckSim_h
#define CheckSim_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TMacro.h>
#include <TObjArray.h>
#include <fstream>
#include <TCutG.h>
#include "AnalysisLibrary.h"


// Header file for the classes stored in the TTree if any.

class CheckSim : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           hit;
   Double_t        thetab;
   Double_t        phib;
   Double_t        Tb;
   Double_t        thetaB;
   Double_t        phiB;
   Double_t        TB;
   Double_t        thetaCM;
   Double_t        e;
   Double_t        x;
   Double_t        z;
   Double_t        z0;
   Double_t        t;
   Double_t        tB;
   Double_t        recoilT;
   Int_t           detID;
   Int_t           detRowID;
   Int_t           loop;
   Double_t        dphi;
   Double_t        rho;
   Int_t           ExAID;
   Double_t        ExA;
   Int_t           ExID;
   Double_t        Ex;
   Double_t        ExCal;
   Double_t        thetaCMCal;
   Double_t        beamTheta;
   Double_t        beamPhi;
   Double_t        beamKEA;
   Double_t        xArray;
   Double_t        yArray;
   Double_t        rhoArray;
   Double_t        xRecoil;
   Double_t        yRecoil;
   Double_t        rhoRecoil;
   Double_t        xElum1;
   Double_t        yElum1;
   Double_t        rhoElum1;
   Double_t        xRecoil1;
   Double_t        yRecoil1;
   Double_t        rhoRecoil1;
   Double_t        xRecoil2;
   Double_t        yRecoil2;
   Double_t        rhoRecoil2;

   // List of branches
   TBranch        *b_hit;   //!
   TBranch        *b_thetab;   //!
   TBranch        *b_phib;   //!
   TBranch        *b_Tb;   //!
   TBranch        *b_thetaB;   //!
   TBranch        *b_phiB;   //!
   TBranch        *b_TB;   //!
   TBranch        *b_thetaCM;   //!
   TBranch        *b_e;   //!
   TBranch        *b_x;   //!
   TBranch        *b_z;   //!
   TBranch        *b_z0;   //!
   TBranch        *b_t;   //!
   TBranch        *b_tB;   //!
   TBranch        *b_recoilT;   //!
   TBranch        *b_detID;   //!
   TBranch        *b_detRowID;   //!
   TBranch        *b_loop;   //!
   TBranch        *b_dphi;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_ExAID;   //!
   TBranch        *b_ExA;   //!
   TBranch        *b_ExID;   //!
   TBranch        *b_Ex;   //!
   TBranch        *b_ExCal;   //!
   TBranch        *b_thetaCMCal;   //!
   TBranch        *b_beamTheta;   //!
   TBranch        *b_beamPhi;   //!
   TBranch        *b_beamKEA;   //!
   TBranch        *b_xArray;   //!
   TBranch        *b_yArray;   //!
   TBranch        *b_rhoArray;   //!
   TBranch        *b_xRecoil;   //!
   TBranch        *b_yRecoil;   //!
   TBranch        *b_rhoRecoil;   //!
   TBranch        *b_xElum1;   //!
   TBranch        *b_yElum1;   //!
   TBranch        *b_rhoElum1;   //!
   TBranch        *b_xRecoil1;   //!
   TBranch        *b_yRecoil1;   //!
   TBranch        *b_rhoRecoil1;   //!
   TBranch        *b_xRecoil2;   //!
   TBranch        *b_yRecoil2;   //!
   TBranch        *b_rhoRecoil2;   //!

   CheckSim(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~CheckSim() { }
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

   ClassDef(CheckSim,0);
};

#endif

#ifdef CheckSim_cxx
void CheckSim::Init(TTree *tree)
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

   fChain->SetBranchAddress("hit", &hit, &b_hit);
   fChain->SetBranchAddress("thetab", &thetab, &b_thetab);
   fChain->SetBranchAddress("phib", &phib, &b_phib);
   fChain->SetBranchAddress("Tb", &Tb, &b_Tb);
   fChain->SetBranchAddress("thetaB", &thetaB, &b_thetaB);
   fChain->SetBranchAddress("phiB", &phiB, &b_phiB);
   fChain->SetBranchAddress("TB", &TB, &b_TB);
   fChain->SetBranchAddress("thetaCM", &thetaCM, &b_thetaCM);
   fChain->SetBranchAddress("e", &e, &b_e);
   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("z", &z, &b_z);
   fChain->SetBranchAddress("z0", &z0, &b_z0);
   fChain->SetBranchAddress("t", &t, &b_t);
   fChain->SetBranchAddress("tB", &tB, &b_tB);
   fChain->SetBranchAddress("recoilT", &recoilT, &b_recoilT);
   fChain->SetBranchAddress("detID", &detID, &b_detID);
   fChain->SetBranchAddress("detRowID", &detRowID, &b_detRowID);
   fChain->SetBranchAddress("loop", &loop, &b_loop);
   fChain->SetBranchAddress("dphi", &dphi, &b_dphi);
   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("ExAID", &ExAID, &b_ExAID);
   fChain->SetBranchAddress("ExA", &ExA, &b_ExA);
   fChain->SetBranchAddress("ExID", &ExID, &b_ExID);
   fChain->SetBranchAddress("Ex", &Ex, &b_Ex);
   fChain->SetBranchAddress("ExCal", &ExCal, &b_ExCal);
   fChain->SetBranchAddress("thetaCMCal", &thetaCMCal, &b_thetaCMCal);
   fChain->SetBranchAddress("beamTheta", &beamTheta, &b_beamTheta);
   fChain->SetBranchAddress("beamPhi", &beamPhi, &b_beamPhi);
   fChain->SetBranchAddress("beamKEA", &beamKEA, &b_beamKEA);
   fChain->SetBranchAddress("xArray", &xArray, &b_xArray);
   fChain->SetBranchAddress("yArray", &yArray, &b_yArray);
   fChain->SetBranchAddress("rhoArray", &rhoArray, &b_rhoArray);
   fChain->SetBranchAddress("xRecoil", &xRecoil, &b_xRecoil);
   fChain->SetBranchAddress("yRecoil", &yRecoil, &b_yRecoil);
   fChain->SetBranchAddress("rhoRecoil", &rhoRecoil, &b_rhoRecoil);
   fChain->SetBranchAddress("xElum1", &xElum1, &b_xElum1);
   fChain->SetBranchAddress("yElum1", &yElum1, &b_yElum1);
   fChain->SetBranchAddress("rhoElum1", &rhoElum1, &b_rhoElum1);
   fChain->SetBranchAddress("xRecoil1", &xRecoil1, &b_xRecoil1);
   fChain->SetBranchAddress("yRecoil1", &yRecoil1, &b_yRecoil1);
   fChain->SetBranchAddress("rhoRecoil1", &rhoRecoil1, &b_rhoRecoil1);
   fChain->SetBranchAddress("xRecoil2", &xRecoil2, &b_xRecoil2);
   fChain->SetBranchAddress("yRecoil2", &yRecoil2, &b_yRecoil2);
   fChain->SetBranchAddress("rhoRecoil2", &rhoRecoil2, &b_rhoRecoil2);
}

Bool_t CheckSim::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef CheckSim_cxx
