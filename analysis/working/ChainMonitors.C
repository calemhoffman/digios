#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    //chain->Add("../root_data/gen_run01[2-4].root"); // alpha

    //chain->Add("../root_data/gen_run02[2-9].root"); // 15C(d,d')
    //chain->Add("../root_data/gen_run03[0-5].root"); // 15C(d,d')
    
    
    //chain->Add("../root_data/gen_run036.root"); // 14C(d,d')
    
    //chain->Add("../root_data/gen_run03[7-8].root"); // 15C(p,p'), CH2 180ug/cm^2
    //chain->Add("../root_data/gen_run039.root"); // 15C(p,p'), trace delay 3us
    
    //chain->Add("../root_data/gen_run04[0-9].root"); // 15C(p,p'), AutoRun
    //chain->Add("../root_data/gen_run05[0-8].root"); // 15C(p,p'), AutoRun

    
    //chain->Add("../root_data/trace_run04[0-9].root"); // 15C(p,p'), AutoRun
    //chain->Add("../root_data/trace_run05[0-8].root"); // 15C(p,p'), AutoRun

    
    //chain->Add("../root_data/gen_run059.root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run06[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run07[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run080.root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run08[2-9].root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run09[0-9].root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run101.root"); // alpha 3-MeV
    
    
    //chain->Add("../root_data/trace_run059.root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run06[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run07[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run080.root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/trace_run08[2-9].root"); //15C(p,p') Target move +8 cm after retune
    chain->Add("../root_data/trace_run09[0-9].root"); //15C(p,p') Target move +8 cm after retune
    
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
