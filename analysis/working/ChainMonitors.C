#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

     ///chain->Add("../root_data/gen_run999.root"); // alpha calibration from 10B run
     ///chain->Add("../root_data/gen_run06[1-3].root"); // long over night run, slits open, x100 att. so around 1e7 pps, 175 ug/cm2
     ///chain->Add("../root_data/gen_run061.root"); // long over night run, slits open, x100 att. so around 1e7 pps, 175 ug/cm2
     ///chain->Add("../root_data/gen_run062.root"); // short run, slits open, x100 att. so around 1e7 pps, 70 ug/cm2
     chain->Add("../root_data/gen_run063.root"); // long over night run, slits open, x300 att. so around 3e6 pps, 120 ug/cm2
     ///chain->Add("../root_data/trace_run135.root");
  
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
