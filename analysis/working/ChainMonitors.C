#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.


    chain->Add("../root_data/trace_run006.root"); // good alpha
    //chain->Add("../root_data/trace_run02[1-9].root"); // good (d,p) data
    //chain->Add("../root_data/trace_run03[0-9].root"); // good data
    //chain->Add("../root_data/trace_run04[0-9].root"); // good data
    //chain->Add("../root_data/trace_run05[0-9].root"); // good data
    //chain->Add("../root_data/trace_run06[0-9].root"); // good data
    //chain->Add("../root_data/trace_run07[0-9].root"); // good data
    //chain->Add("../root_data/trace_run08[0-5].root"); // good data
    //chain->Add("../root_data/trace_run135.root"); // good data

    /// chain->Add("../root_data/trace_run09[3-9].root"); // good data
    /// chain->Add("../root_data/trace_run10[0-9].root"); // good data
    /// chain->Add("../root_data/trace_run11[0-9].root"); // good data
    /// chain->Add("../root_data/trace_run12[0-9].root"); // good data
    /// chain->Add("../root_data/trace_run13[0-9].root"); // good data
    /// chain->Add("../root_data/trace_run14[0-9].root"); // good data
    /// chain->Add("../root_data/trace_run15[0-7].root"); // good data
   
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
  if( saveCanvas ) {
    selector->printControl(0); //quit after terminated
    //if (RUNNUM2 == -1 ) selector->printControl(1); //s0003, push isotope data to DataBase
  
  }
  //if (RUNNUM != -1 ) selector->printControl(-1); //s0003, push isotope data to DataBase
  chain->Process(selector, "");

}
