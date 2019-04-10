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
   string sym = "Pb";
   TString energyFile = "energy209Pb.dat";
   TString searchRange = "wsSearch_Range.txt";
   TString templateRoot = "template.root";
   TString outRootFile = Form("ws_%d%s_all.root", A, sym.c_str());

   int numWorker = 36;
   int maxEvent = 0; // if zero = All event
   
   int nStep = 200;
   double dr = 0.1; // fm

   bool isProgressGUI = true;

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

   int numEntries = chain->GetEntries();
   float workerPower = 2.5; // event / sec / worker
   double estTime = numEntries / workerPower / numWorker; // in sec
   printf(" Estimated Time (%4.1f events/ sec / worker): %6.0f sec = %5.0f min = %4.1f hour\n",
          workerPower, estTime, estTime/60., estTime/60./60.);
   printf(" Results save to : %s \n", outRootFile.Data());

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
   TString option = Form("%d,%s,%s,%d,%f",A, energyFile.Data(),outRootFile.Data(), nStep, dr);
   if( maxEvent > 0 ) {
     chain->Process("WSProof.C+", option, maxEvent);
   }else{
     chain->Process("WSProof.C+", option);
   }
   //add back the searchRange into the root
   TFile * file = new TFile(outRootFile, "UPDATE");
   tMacro->Write("searchRange");
   file->Close();

}
