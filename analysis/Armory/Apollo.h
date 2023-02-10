//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb  9 21:57:47 2023 by ROOT version 6.26/06
// from TTree tree/tree
// found on file: transfer.root
//////////////////////////////////////////////////////////

#ifndef Apollo_h
#define Apollo_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

class Apollo : public TSelector {
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
   Int_t           detID;
   Int_t           detRowID;
   Int_t           loop;
   Double_t        rho;
   Double_t        rhoB;
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

   // List of branches
   TBranch        *b_hit;   //!
   TBranch        *b_thetab;   //!
   TBranch        *b_phib;   //!
   TBranch        *b_Tb;   //!
   TBranch        *b_thetaB;   //!
   TBranch        *b_phiB;   //!
   TBranch        *b_TB;   //!
   TBranch        *b_thetaCM;   //!
   TBranch        *b_energy_light;   //!
   TBranch        *b_detector_x;   //!
   TBranch        *b_array_hit_z;   //!
   TBranch        *b_z_cycle;   //!
   TBranch        *b_cycle_time_light;   //!
   TBranch        *b_recoil_hit_time;   //!
   TBranch        *b_detID;   //!
   TBranch        *b_detRowID;   //!
   TBranch        *b_loop;   //!
   TBranch        *b_orbit_radius_light;   //!
   TBranch        *b_orbit_radius_heavy;   //!
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

   Apollo(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Apollo() { }
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

   ClassDef(Apollo,0);

   //============
   TFile * saveFile;
   TTree * newTree;

   Float_t xH, yH, zH;
   Int_t  iH, jH;
   Double_t pTheta, pPhi, pE;
   Double_t pThetaCM;

};

#endif

#ifdef Apollo_cxx
void Apollo::Init(TTree *tree)
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
   fChain->SetBranchAddress("e", &e, &b_energy_light);
   fChain->SetBranchAddress("x", &x, &b_detector_x);
   fChain->SetBranchAddress("z", &z, &b_array_hit_z);
   fChain->SetBranchAddress("z0", &z0, &b_z_cycle);
   fChain->SetBranchAddress("t", &t, &b_cycle_time_light);
   fChain->SetBranchAddress("tB", &tB, &b_recoil_hit_time);
   fChain->SetBranchAddress("detID", &detID, &b_detID);
   fChain->SetBranchAddress("detRowID", &detRowID, &b_detRowID);
   fChain->SetBranchAddress("loop", &loop, &b_loop);
   fChain->SetBranchAddress("rho", &rho, &b_orbit_radius_light);
   fChain->SetBranchAddress("rhoB", &rhoB, &b_orbit_radius_heavy);
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

   saveFile = new TFile("hahaApollo.root", "recreate");
   newTree = new TTree("tree", "tree");
   newTree->Branch("x", &xH, "x/F");
   newTree->Branch("y", &yH, "y/F");
   newTree->Branch("z", &zH, "z/F");
   newTree->Branch("i", &iH, "i/I");
   newTree->Branch("j", &jH, "j/I");
   newTree->Branch("theta", &pTheta, "theta/D");
   newTree->Branch("phi", &pPhi, "phi/D");
   newTree->Branch("e", &pE, "e/D");
   newTree->Branch("thetaCM", &pThetaCM, "thetaCM/D");
}

Bool_t Apollo::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Apollo_cxx
