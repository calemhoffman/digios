#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    // chain->Add("../root_data/gen_run010.root"); alpha cal array 

    // chain->Add("../root_data/gen_run01[8-9].root"); // 29Si(d,p), 218 ug/cm2
    // chain->Add("../root_data/gen_run02[0-9].root"); // 29Si(d,p), 218 ug/cm2
    // chain->Add("../root_data/gen_run03[3-7].root"); // 29Si(d,p), 2018 ug/cm2 (missing runs of 30, 31, 32 are recoil rate checks with Y map)

    // chain->Add("../root_data/trace_run01[8-9].root"); // 29Si(d,p), 218 ug/cm2
    // chain->Add("../root_data/trace_run02[0-9].root"); // 29Si(d,p), 218 ug/cm2
    // chain->Add("../root_data/trace_run03[3-7].root"); // 29Si(d,p), 218 ug/cm2 (missing runs of 30, 31, 32 are recoil rate checks with Y map)

    // chain->Add("../root_data/gen_run03[8-9].root"); // 29Mg(d,p) 381 mkg/cm^2
    // chain->Add("../root_data/gen_run04[0-9].root"); // 29Mg(d,p) 381 mkg/cm^2, run 46 is large
    // chain->Add("../root_data/gen_run05[0-5].root"); // 29Mg(d,p) 381 mkg/cm^2

    chain->Add("../root_data/trace_run03[8-9].root"); // 29Mg(d,p) 381 mkg/cm^2
    chain->Add("../root_data/trace_run04[0-9].root"); // 29Mg(d,p) 381 mkg/cm^2, run 46 is large
    chain->Add("../root_data/trace_run05[0-5].root"); // 29Mg(d,p) 381 mkg/cm^2

    // chain->Add("../root_data/gen_run03[0-2].root"); //recoil check (for rate calculations ... three min runs)

     
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
