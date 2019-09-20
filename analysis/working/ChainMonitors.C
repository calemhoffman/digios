#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    chain->Add("../root_data/gen_run016.root"); //136Xe
    chain->Add("../root_data/gen_run02[0-7].root"); //136Xe
    
  }else{
    
    TString fileName;
  
    if( RUNNUM2 == -1 ) RUNNUM2 = RUNNUM;
    
    for( int i = RUNNUM ; i <= RUNNUM2 ; i++){
      fileName.Form("../root_data/gen_run%03d.root", i);
      chain->Add(fileName);
    }
  }
  
  
  printf("\033[0;31m========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  printf("========================================== Number of Files : %2d\033[0m\n",chain->GetListOfFiles()->GetEntries());
  
  //Simple call TSelector
  //chain->Process("Monitors.C+");
  
  
  //Some input to TSelector
  Monitors * selector = new Monitors();
  selector->testingInput(1231441241);
  chain->Process(selector, "");

}
