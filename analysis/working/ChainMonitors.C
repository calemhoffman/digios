#include "Monitors.C" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.
    chain->Add("../root_data/trace_run133.root");
    // chain->Add("../root_data/trace_run12[3-9].root");
    // chain->Add("../root_data/trace_run13[0-9].root");
    // chain->Add("../root_data/trace_run14[0-9].root");
    // chain->Add("../root_data/trace_run15[0-9].root");
    // chain->Add("../root_data/trace_run16[0-9].root");
    // chain->Add("../root_data/trace_run17[0-9].root");
    // chain->Add("../root_data/trace_run18[0-9].root");
    // chain->Add("../root_data/trace_run19[0-9].root");
    // chain->Add("../root_data/trace_run20[0-5].root");
///     chain->Add("../root_data/gen_run052.root"); //PDB 10.0?? 
///     chain->Add("../root_data/gen_run025.root"); //PDB 10.6?
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
