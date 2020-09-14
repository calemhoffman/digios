#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1 || RUNNUM == -10){
    
    //alphs
    //chain->Add("../root_data/gen_run00[1-2].root"); 
    
    //CD2 227 ug
    //chain->Add("../root_data/gen_run00[4-7].root"); 
    
    //CD2 147 ug
    //chain->Add("../root_data/gen_run010.root"); 
    
    //CH2 360
    //chain->Add("../root_data/gen_run011.root"); 
    
    //tritium-2 att = 1e3
    //chain->Add("../root_data/gen_run012.root"); 
    //chain->Add("../root_data/gen_run01[5-8].root"); 

    
    //tritium-2 att = 1e4
    //chain->Add("../root_data/gen_run019.root"); 
    //chain->Add("../root_data/gen_run02[0-6].root"); 
    
    //tritium-2 
    //chain->Add("../root_data/gen_run02[7-9].root"); 
    
    //CD2 227
    //chain->Add("../root_data/gen_run033.root"); 
    
    
    //tritium-2
    chain->Add("../root_data/gen_run03[4-9].root"); 
    chain->Add("../root_data/gen_run04[0-9].root"); 
    
    
    
  }else{
    
    TString fileName;
    int endRUNNUM = RUNNUM2;
    if( RUNNUM2 == -1 || RUNNUM2 == -10 ) endRUNNUM = RUNNUM;
    
    for( int i = RUNNUM ; i <= endRUNNUM ; i++){
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
  if( RUNNUM == -10 || RUNNUM2 == -10 ) selector->printControl(0); //quit after terminated
  chain->Process(selector, "");

}
