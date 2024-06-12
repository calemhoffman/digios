#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

     ///chain->Add("../root_data/gen_run00[4-9].root"); //alpha runs
     //chain->Add("../root_data/gen_run010.root"); //alpha
     //chain->Add("../root_data/gen_run01[1-6].root"); //early beam runs (nonsense)
     //chain->Add("../root_data/gen_run019.root"); //new setup //run 17 noisy junk, run 18 - 1hour on a cup
     //chain->Add("../root_data/gen_run02[0-9].root"); // run 19 and 20 is the 319 ug/cm2 target, run 21 is the 127 ug/cm2 tgt with approx x15 more beam


//Trace sorting

     //chain->Add("../root_data/trace_run019.root"); //new setup //run 17 noisy junk, run 18 - 1hour on a cup
     chain->Add("../root_data/trace_run019.root"); //thick tgt
     chain->Add("../root_data/trace_run020.root"); //thick tgt
     //chain->Add("../root_data/trace_run02[0-9].root"); // run 19 and 20 is the 319 ug/cm2 target, runs 21,22 is the 127 ug/cm2 tgt with approx x15 more beam, run 23 with 120 ug/cm2
  
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
