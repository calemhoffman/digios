#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    // Medium target 114
    chain->Add("../root_data/gen_run009.root");
    chain->Add("../root_data/gen_run01[0-3].root");

    // Thin target 88
    chain->Add("../root_data/gen_run01[4-7].root");
    chain->Add("../root_data/gen_run02[0-3].root");

    // "Thick" target 119
    chain->Add("../root_data/gen_run02[4-5].root");

    // change VME02:DIG2 to VME01:DIG3
      // chain->Add("../root_data/gen_run018.root");

    // switch board so the mapping does not need to change
      // chain->Add("../root_data/gen_run019.root"); // trigger issue, junk

      // chain->Add("../root_data/gen_run02[0-9].root");
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
