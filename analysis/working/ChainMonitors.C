#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    //chain->Add("../root_data/gen_run00[6-8].root"); //(d,p)
    
    //---------- (12C,a)
    //chain->Add("../root_data/gen_run009.root");
    //chain->Add("../root_data/gen_run01[0-6].root");
    //chain->Add("../root_data/gen_run018.root");
    //chain->Add("../root_data/gen_run02[0-9].root");
    //chain->Add("../root_data/gen_run03[0-9].root");
    
    //no recoils
    chain->Add("../root_data/gen_run04[0-9].root");
    chain->Add("../root_data/gen_run05[0-9].root");
    chain->Add("../root_data/gen_run06[0-9].root");
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
