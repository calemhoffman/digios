#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1 || RUNNUM == -10){
    ///********** start Marker for AutoCalibration.

    //chain->Add("../root_data/gen_run001.root"); //alpha
    //chain->Add("../root_data/gen_run002.root"); //alpha
    //chain->Add("../root_data/gen_run003.root"); //alpha, all detector

    //chain->Add("../root_data/gen_run004.root"); //14N, 317 ug
    
    //chain->Add("../root_data/gen_run005.root"); //14N, 168 ug, can only sort 1st 24 min...
    //chain->Add("../root_data/gen_run007.root"); //14N, 168 ug
    
    
    //chain->Add("../root_data/gen_run010.root"); //14N 14C, 168 ug & 317 ug
    
    //chain->Add("../root_data/gen_run01[1-6].root"); //14N 14C, 317 ug
    //chain->Add("../root_data/gen_run01[7-8].root"); //14N 14C, 317 ug, refreshed IC gas
    
    //chain->Add("../root_data/gen_run019.root"); //14N 14C, 317 ug, IC trigger, no trace, threshold (15,15)
    //chain->Add("../root_data/gen_run020.root"); //14N 14C, 317 ug, IC trigger, no trace, threshold (9,19)
    
    //chain->Add("../root_data/gen_run021.root"); //junk
    
    //chain->Add("../root_data/gen_run02[2-6].root"); //14N 14C, 317 ug, refreshed IC gas
    
    ///============= resume
    //chain->Add("../root_data/gen_run027.root"); //14N, 317 ug
    //
    //chain->Add("../root_data/gen_run028.root"); //14N + 14C, 317 ug
    //chain->Add("../root_data/gen_run029.root"); //14N + 14C, 317 ug, reduce attenuation

    chain->Add("../root_data/gen_run03[0-9].root"); //14N + 14C, 127 ug
    chain->Add("../root_data/gen_run04[0-6].root"); //14N + 14C, 127 ug
    
    
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
  
  printf(" number of entries : %llu \n", chain->GetEntries());
  
  //Simple call TSelector
  //chain->Process("Monitors.C+");
  
  
  //Some input to TSelector
  Monitors * selector = new Monitors();
  if( RUNNUM == -10 || RUNNUM2 == -10 ) selector->printControl(0); //quit after terminated
  chain->Process(selector, "");

}
