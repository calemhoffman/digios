#include "TROOT.h"
#include "TFile.h"
#include "TMacro.h"
#include "TChain.h"
#include "TSelector.h"
#include "TProof.h"

void ProofWSSearch(){
	
	//WSMakeTree

	//TChain for Proof
	TChain * chain = new TChain("tree");
	chain->Add("template.root");
	
	//PRoof-lite section
	TProof * p = TProof::Open("", "workers=2");
	chain->SetProof();
  
   //Read searchRange
   TFile * infile = new TFile("template.root");
   TMacro * tMacro = (TMacro*) infile->FindObjectAny("searchRange");
   tMacro->Print();
   infile->Close();
   
   //TSelector
	TString setting = "energy209Pb.dat"; //TODO, can be a txt file
   chain->Process("WSProof.C", setting);
   
   //add back the searchRange into the root
   TFile * file = new TFile("haha.root", "UPDATE");
   tMacro->Write("searchRange");
   file->Close();

}
