#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    //chain->Add("../root_data/gen_run00[8-9].root"); // alpha runs 238Th
    //chain->Add("../root_data/gen_run01[2].root"); // alpha runs  Am+Cs
    
    //chain->Add("../root_data/gen_run013.root"); // alpha runs  dE
  
  
    //chain->Add("../root_data/trace_run009.root"); // alpha runs  238Th
    
    
    //chain->Add("../root_data/gen_run01[5-9].root"); // 137 ug CD2. 20Ne(d,p)
    //chain->Add("../root_data/gen_run020.root"); // 137 ug CD2. 20Ne(d,p)
    
    
    //chain->Add("../root_data/gen_run02[2-9].root"); // 15N(d,p)
    //chain->Add("../root_data/gen_run03[0-1].root"); // 15N(d,p)    
  
    //chain->Add("../root_data/gen_run03[7-9].root"); // 10Be(d,p)
    //chain->Add("../root_data/gen_run04[0-2].root"); // 10Be(d,p)
    
    
   chain->Add("../root_data/gen_run04[3-9].root"); // 10Be(t,p)
   chain->Add("../root_data/gen_run05[0-9].root"); // 10Be(t,p)
   chain->Add("../root_data/gen_run06[0-9].root"); // 10Be(t,p)
   chain->Add("../root_data/gen_run07[0-2].root"); // 10Be(t,p)
   chain->Add("../root_data/gen_run07[3-9].root"); // 10Be(t,p) good beam
   chain->Add("../root_data/gen_run08[0-9].root"); // 10Be(t,p) good beam
   chain->Add("../root_data/gen_run09[0-9].root"); // 10Be(t,p) good beam
   chain->Add("../root_data/gen_run10[0-9].root"); // 10Be(t,p) good beam
   chain->Add("../root_data/gen_run11[0-9].root"); // 10Be(t,p) good beam  
   chain->Add("../root_data/gen_run12[0-9].root"); // 10Be(t,p) good beam  
   chain->Add("../root_data/gen_run13[0-9].root"); // 10Be(t,p) good beam   
   chain->Add("../root_data/gen_run14[0-3].root"); // 10Be(t,p) good beam   
   chain->Add("../root_data/gen_run14[4-9].root"); // 10Be(t,p) good beam    
   chain->Add("../root_data/gen_run15[0-9].root"); // 10Be(t,p) good beam    
   chain->Add("../root_data/gen_run16[0-9].root"); // 10Be(t,p) good beam    
   chain->Add("../root_data/gen_run17[0-9].root"); // 10Be(t,p) good beam    
   chain->Add("../root_data/gen_run18[0-9].root"); // 10Be(t,p) good beam super high purity
   chain->Add("../root_data/gen_run19[0-9].root"); // 10Be(t,p) good beam super high purity
   chain->Add("../root_data/gen_run20[0-9].root"); // 10Be(t,p) good beam super high purity
   chain->Add("../root_data/gen_run210.root"); // 10Be(t,p) good beam super high purity
   chain->Add("../root_data/gen_run21[1-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run22[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run23[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run24[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run25[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run26[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run27[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run28[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run29[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run30[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run31[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run32[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run33[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run34[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run35[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run36[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run37[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run38[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run39[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run40[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run41[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run42[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run43[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run44[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run45[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run46[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run47[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run48[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run49[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position 
   chain->Add("../root_data/gen_run50[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run51[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   chain->Add("../root_data/gen_run52[0-9].root"); // 10Be(t,p) good beam super high purity, adjusted target position
   
    
    //chain->Add("../root_data/trace_run04[3-9].root"); 
    //chain->Add("../root_data/trace_run05[0-9].root"); 
    //chain->Add("../root_data/trace_run06[0-9].root"); 
    //chain->Add("../root_data/trace_run07[0-9].root"); 
    //chain->Add("../root_data/trace_run08[0-9].root"); 
    //chain->Add("../root_data/trace_run09[0-9].root"); 
    //chain->Add("../root_data/trace_run100.root"); 
    
  
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
