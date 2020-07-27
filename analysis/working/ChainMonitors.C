#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1 || RUNNUM == -10){

    //chain->Add("../root_data/gen_run00[6-8].root"); //alpha
    
    //140 ug
    //chain->Add("../root_data/gen_run01[3-7].root");
    
    //56 ug
    chain->Add("../root_data/gen_run01[8-9].root");
    chain->Add("../root_data/gen_run02[0-9].root");
    chain->Add("../root_data/gen_run03[0-5].root");
    
    //54 ug
    chain->Add("../root_data/gen_run03[6-9].root");
    chain->Add("../root_data/gen_run04[0-9].root");
    chain->Add("../root_data/gen_run05[0-9].root");
    chain->Add("../root_data/gen_run06[0-9].root");
    chain->Add("../root_data/gen_run07[0-9].root");
    
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
  if( RUNNUM == -10 ) selector->printControl(0); //quit after terminated
  chain->Process(selector, "");

}
