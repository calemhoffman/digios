#include "Monitors.C+" // the plus sign mean compliation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    //chain->Add("root_data/gen_run01[2-3].root"); // 300 ug 130Xe
    
    //chain->Add("root_data/gen_run01[4-9].root"); // 165 ug 130Xe
    //chain->Add("root_data/gen_run02[0-1].root"); // 165 ug 130Xe
    
    //chain->Add("root_data/gen_run02[2-4].root"); // 300 ug
    //chain->Add("root_data/gen_run02[5-6].root"); // 165 ug only
    //chain->Add("root_data/gen_run031.root"); //background
    
    chain->Add("../root_data/gen_run004.root");
    
    //chain->Add("../root_data/gen_run02[5-9].root");
    //chain->Add("../root_data/gen_run030.root");
    //chain->Add("../root_data/gen_run03[2-9].root");
    //chain->Add("../root_data/gen_run041.root");
    //chain->Add("../root_data/gen_run04[3-9].root");
    //chain->Add("../root_data/gen_run05[0-9].root");
    //chain->Add("../root_data/gen_run06[0-4].root");
    
    //chain->Add("root_data/gen_run065.root"); // alpha source
  
  }else{
    
    TString fileName;
  
    if( RUNNUM2 == -1 ) RUNNUM2 = RUNNUM;
    
    for( int i = RUNNUM ; i <= RUNNUM2 ; i++){
      fileName.Form("../root_data/gen_run%03d.root", i);
      chain->Add(fileName);
    }
  }
  
  printf("========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  printf("========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  
  //Simple call TSelector
  //chain->Process("Monitors.C+");
  
  
  //Some input to TSelector
  Monitors * selector = new Monitors();
  selector->testingInput(1231441241);
  chain->Process(selector, "");

}
