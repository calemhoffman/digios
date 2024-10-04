#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    // chain->Add("../root_data/gen_run00[4,5].root"); // alpha
        
    chain->Add("../root_data/trace_run01[2-8].root"); // > 1.5e5 pps
    chain->Add("../root_data/trace_run019.root"); // approx 2e5 pps
    chain->Add("../root_data/trace_run02[0-9].root"); // approx 2e5 pps
    chain->Add("../root_data/trace_run030.root"); // approx 2e5 pps
    chain->Add("../root_data/trace_run03[2-6].root"); // approx 2e5 pps
    chain->Add("../root_data/trace_run03[8-9].root"); // approx 2e5 pps
    chain->Add("../root_data/trace_run040.root"); // approx 2e5 pps


    // chain->Add("../root_data/gen_run02[0-9].root"); //  ~ 200 x 1000 pps, remove dE attenuation 
    // chain->Add("../root_data/gen_run030.root"); //  ~ 200 x 1000 pps, remove dE attenuation 
    // // chain->Add("../root_data/gen_run031.root"); // beam changed?
    // chain->Add("../root_data/gen_run03[2-3].root"); //  ~ 200 x 1000 pps, remove dE attenuation 
    // chain->Add("../root_data/gen_run03[4-6].root"); // 476 ug target
    // // chain->Add("../root_data/gen_run037.root"); // beam changed?
    // chain->Add("../root_data/gen_run03[8-9].root"); // 476 ug target
    // chain->Add("../root_data/gen_run040.root"); // 476 ug target, last 11C run
  


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
