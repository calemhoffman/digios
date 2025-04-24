#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    //chain->Add("../root_data/gen_run010.root");
     //chain->Add("../root_data/gen_run010.root");
     //chain->Add("../root_data/gen_run011.root");
     //chain->Add("../root_data/gen_run014.root");
     //chain->Add("../root_data/gen_run015.root");
     //chain->Add("../root_data/gen_run016.root");
     
     //chain->Add("../root_data/gen_run017.root");
     //chain->Add("../root_data/gen_run018.root");
     
     //chain->Add("../root_data/trace_run010.root");
     //chain->Add("../root_data/trace_run011.root");
     //chain->Add("../root_data/trace_run014.root");
     //chain->Add("../root_data/trace_run015.root");
     //chain->Add("../root_data/trace_run015.root");
     chain->Add("../root_data/trace_run016.root");
     chain->Add("../root_data/trace_run017.root");
     chain->Add("../root_data/trace_run018.root");
     chain->Add("../root_data/trace_run019.root");
     chain->Add("../root_data/trace_run020.root");
     chain->Add("../root_data/trace_run021.root");
     chain->Add("../root_data/trace_run022.root");
     chain->Add("../root_data/trace_run023.root");
     chain->Add("../root_data/trace_run024.root");
     chain->Add("../root_data/trace_run025.root");
     chain->Add("../root_data/trace_run02[6-9].root");
     chain->Add("../root_data/trace_run03[0-9].root");
    //chain->Add("../root_data/trace_run018.root");

     //chain->Add("../root_data/gen_run011.root"); //alpha on (d,t) array 228Th
     //chain->Add("../root_data/gen_run013.root"); //alpha on (d,p) array Gd-Cm

     //chain->Add("../root_data/gen_run02[0-9].root"); //first good (d,t) (d,p) run with 223 ug/cm2 target
     //chain->Add("../root_data/gen_run03[0-9].root"); //good (d,t) (d,p) run with 223 ug/cm2 target

     //chain->Add("../root_data/trace_run02[0-9].root"); //first good (d,t) (d,p) run with 223 ug/cm2 target
     //chain->Add("../root_data/trace_run03[0-9].root"); //good (d,t) (d,p) run with 223 ug/cm2 target
     //chain->Add("../root_data/trace_run04[0-9].root");
     //chain->Add("../root_data/trace_run05[0-9].root");
     //chain->Add("../root_data/trace_run06[0-9].root");
     //chain->Add("../root_data/trace_run07[0-9].root");
     //chain->Add("../root_data/trace_run08[0-9].root");
     //chain->Add("../root_data/trace_run090.root")
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
