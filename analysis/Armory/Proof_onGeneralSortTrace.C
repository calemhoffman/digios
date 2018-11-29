#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TProof.h>

void Proof_onGeneralSortTrace(){
   
   //+++++++++++++++++++++++++++++
   //    for some unknown reason, have to copy it and run in local folder. 
   //    cannot use symbolic link
   //++++++++++++++++++++++++++++++
   
   //=============== process after GEBSortMerge
   TChain * chain = new TChain("tree");
   //filename should be in the format "*runXXXX.root", otherwise, the runID will be incorrect
   //chain->Add("/lcrc/project/HELIOS/h057_he6/root_data/run200.root"); 
   chain->Add("data/trace_run70.root"); 
   
   TProof * p = TProof::Open("", "workers=3");
   
   chain->SetProof();
   chain->Process("GeneralSortTraceProof.C+");
   
   delete chain;
   delete p;
   
   
   /**///=============== process for calibaration and coinTime
   /* something wrong.
   TChain * chain2 = new TChain("tree");
   chain2->Add("sortedTrace107.root");
   
   TProof * p2 = TProof::Open("", "workers=1");
   
   chain2->SetProof();
   chain2->Process("../AutoCali/Cali_e_trace_Proof.C+");
   
   delete chain2;
   delete p2;
   /**/
}
