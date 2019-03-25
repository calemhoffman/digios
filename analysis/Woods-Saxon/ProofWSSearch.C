#include "TROOT.h"
#include "TFile.h"
#include "TMacro.h"
#include "TChain.h"
#include "TSelector.h"
#include "TProof.h"
#include "TList.h"
//#include "WSProof.h"
//#include "WSProof.C"

void ProofWSSearch(){
   //=============== User input
   int A = 209;
   TString energyFile = "energy209Pb.dat";
   TString searchRange = "wsSearch_Range.txt";
   TString templateRoot = "template.root";
   TString outRootFile = "haha.root";

   int numWorker = 36;

   bool isProgressGUI = false;

   //==================== End of User input

	//WSMakeTree
   TString expression = Form("./WSMakeTree %d %s %s", A, searchRange.Data(), templateRoot.Data());
   system(expression.Data());

   
	//TChain for Proof
	TChain * chain = new TChain("tree");
	chain->Add(templateRoot);
	
	//PRoof-lite section
	TProof * p = TProof::Open("", Form("workers=%d", numWorker));
   p->SetProgressDialog(isProgressGUI);
	chain->SetProof();
  
   //Read searchRange
   TFile * infile = new TFile(templateRoot);
   TMacro * tMacro = (TMacro*) infile->FindObjectAny("searchRange");
   tMacro->Print();
   infile->Close();

   //TSelector, not working
   //p->Load("WSProof.C+");
   //WSProof * selector = new WSProof();
   //selector->ReadEnergyFile(energyFile);
   //p->Process(selector, 2000);

   //TSelector, try again, not working
   //WSProof * selector = (WSProof *)TSelector::GetSelector("WSProof.C");
   //selector->ReadEnergyFile(energyFile);
   //chain->Process(selector, "", 1000);

   //Simplist way to do
   TString option = Form("%d,%s,%s",A, energyFile.Data(),outRootFile.Data());
   chain->Process("WSProof.C+", option);

   //add back the searchRange into the root
   TFile * file = new TFile("haha.root", "UPDATE");
   tMacro->Write("searchRange");
   file->Close();

}
