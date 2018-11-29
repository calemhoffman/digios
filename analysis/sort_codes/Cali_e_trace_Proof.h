//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 10 21:31:47 2018 by ROOT version 6.10/02
// from TTree gen_tree/PSD Tree
// found on file: gen_run11.root
//////////////////////////////////////////////////////////

#ifndef Cali_e_trace_Proof_h
#define Cali_e_trace_Proof_h

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
#include <TProofOutputFile.h>

// Headers needed by this particular selector


class Cali_e_trace_Proof : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Float_t         e[100];
   ULong64_t       e_t[100];
   Float_t         xf[100];
   ULong64_t       xf_t[100];
   Float_t         xn[100];
   ULong64_t       xn_t[100];
   Float_t         rdt[100];
   ULong64_t       rdt_t[100];
   Float_t         tac[100];
   ULong64_t       tac_t[100];
   
   //Trace
   Float_t         te[24];
   Float_t         te_r[24];
   Float_t         te_t[24];
   Float_t         trdt[8];
   Float_t         trdt_t[8];
   Float_t         trdt_r[8];

   // List of branches
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_XNTimestamp;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   TBranch        *b_TAC;   //!
   TBranch        *b_TACTimestamp;   //!
   
   TBranch        *b_Trace_Energy;  //!
   TBranch        *b_Trace_Energy_Time;  //!
   TBranch        *b_Trace_Energy_RiseTime;  //!
   
   TBranch        *b_Trace_RDT;  //!
   TBranch        *b_Trace_RDT_Time;  //!
   TBranch        *b_Trace_RDT_RiseTime;  //!
   
   bool isTraceDataExist; // if b_Trace_** exist
   
   //=============================== 
   TFile * saveFile; //!
   TProofOutputFile * proofFile; //!
   TTree * newTree; //!
   
   //tree  
   Int_t eventID;
   Int_t run;
   Float_t eC[24];
   ULong64_t eC_t[24];
   Float_t x[24]; // unadjusted position, range (-1,1)
   Float_t z[24]; 
   int det;
   int hitID; // is e, xf, xn are all fired.
   int multiHit; // multipicity of hit
   
   Float_t thetaCM;
   Float_t Ex;   
   Float_t thetaLab;
   
   Float_t rdtC[8];
   ULong64_t rdtC_t[8];
   
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
   
   double cTCorr[24][9]; // coinTime correction
   TF1 ** f7 ; //!
   
   bool isReaction;
   double G, Z, H; // for excitation calcualtion
   double mass, q;
   double beta, gamma;
   double alpha ;
   double Et, massB;
   
   
   Cali_e_trace_Proof(TTree * /*tree*/ =0) : fChain(0), saveFile(0), proofFile(0), newTree(0), f7(0), fList(0) { }
   virtual ~Cali_e_trace_Proof() { }
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

   ClassDef(Cali_e_trace_Proof,0);


};

#endif

#ifdef Cali_e_trace_Proof_cxx
void Cali_e_trace_Proof::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).
   
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   //printf("========== number of tree loaded : %d \n", fChain->GetNTree());
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);   
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);

   fChain->SetBranchAddress("tac", tac, &b_TAC);
   fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);

   isTraceDataExist = false;
   
   TBranch * br = (TBranch *) fChain->GetListOfBranches()->FindObject("te_t");
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
   
}

Bool_t Cali_e_trace_Proof::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Cali_e_trace_Proof_cxx
