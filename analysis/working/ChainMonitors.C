#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

     //chain->Add("../root_data/gen_run003.root"); /// alpha run
     //chain->Add("../root_data/gen_run005.root"); /// alpha run
     
     //chain->Add("../root_data/gen_run01[0-2].root"); /// (d,p) mainly alpha run
     
     //chain->Add("../root_data/gen_run023.root");
     //chain->Add("../root_data/gen_run024.root"); // slit out
     
     
     //chain->Add("../root_data/trace_run024.root"); // slit out
     
    //  chain->Add("../root_data/trace_run026.root"); // good run ********** 180 torr
    //  chain->Add("../root_data/trace_run02[8-9].root"); // good run
    //  chain->Add("../root_data/trace_run03[0-3].root"); // good run     
    //  chain->Add("../root_data/trace_run033.root"); // good run
     
     //chain->Add("../root_data/trace_run034.root"); // good run **** 225 torr
     
     //chain->Add("../root_data/trace_run03[5-9].root"); // good run **** 204 torr
     //chain->Add("../root_data/trace_run04[0-1].root"); // good run **** 204 torr
     //run 42 is 11 GB of garbage
     //chain->Add("../root_data/trace_run04[3-5].root"); // good run **** 204 torr 
     //chain->Add("../root_data/trace_run04[8-9].root"); // good run **** 204 torr 100ug /cm2
     //chain->Add("../root_data/trace_run05[0-9].root"); // good run **** 204 torr 100ug /cm2
    //  chain->Add("../root_data/gen_run053.root"); // alpha run with source at 30 cm 

    chain->Add("../root_data/trace_run033.root"); 
    // chain->Add("../root_data/trace_run050.root"); // alpha run with source at 30 cm 
  
    ///********** end Marker for AutoCalibration.
    
    
  }else{
    
    TString fileName;
    int endRUNNUM = RUNNUM2;
    if( RUNNUM2 == -1) endRUNNUM = RUNNUM;
    
    for( int i = RUNNUM ; i <= endRUNNUM ; i++){
      
      if( isTraceON == false )fileName.Form("../root_data/gen_run%03d.root", i);
      if( isTraceON == true ) fileName.Form("../root_data/trace_run%03d.root", i);
      chain->Add(fileName);
    }
  }
  
  
  printf("\033[0;31m========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  printf("========================================== Number of Files : %2d\033[0m\n",chain->GetListOfFiles()->GetEntries());
  printf(" number of entries : %llu \n", chain->GetEntries());
  
  //Simple call TSelector
  //chain->Process("Monitors.C+");
  
  //Some input to TSelector
  Monitors * selector = new Monitors();
  if( saveCanvas ) selector->printControl(0); //quit after terminated
  chain->Process(selector, "");

}
