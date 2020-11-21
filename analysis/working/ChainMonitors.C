#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1 || RUNNUM == -10){
    ///********** start Marker for AutoCalibration.


    //chain->Add("../root_data/gen_run00[4-6].root"); //alpha
    
    //chain->Add("../root_data/gen_run008.root"); //30Si(d,p)
    
    chain->Add("../root_data/gen_run010.root"); //29Al(d,p), overnight tune
    
    chain->Add("../root_data/gen_run01[1-8].root"); //29Al(d,p)
    chain->Add("../root_data/gen_run019.root"); //29Al(d,p), new tune, recoil file no open
    chain->Add("../root_data/gen_run02[0-9].root"); //29Al(d,p), new tune

//    chain->Add("../root_data/gen_run03[0-9].root"); //29Al(d,p), new tune


    ///********** end Marker for AutoCalibration.
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
