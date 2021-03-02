#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    // chain->Add("../root_data/gen_run01[1-5].root");
    // chain->Add("../root_data/gen_run01[6-9].root");
    //chain->Add("../root_data/gen_run02[4-9].root");
    //chain->Add("../root_data/gen_run03[0-9].root");
    //chain->Add("../root_data/gen_run04[0-9].root");
  
    //chain->Add("../root_data/gen_run05[5-9].root");
    //chain->Add("../root_data/gen_run06[0-9].root");
  
  
    //chain->Add("../root_data/gen_run06[6-9].root"); // alpha test
    //chain->Add("../root_data/gen_run066.root"); // alpha test
    
    chain->Add("../root_data/gen_run07[1-4].root"); //UdpF
    
    
    chain->Add("../root_data/gen_run07[5-9].root"); //UdpF, DAQ reboot, timeStamp reset
    chain->Add("../root_data/gen_run08[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run09[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run10[0-8].root"); //UdpF
    
    chain->Add("../root_data/gen_run109.root"); //UdpF, new target
    chain->Add("../root_data/gen_run11[0-5].root"); //UdpF
    chain->Add("../root_data/gen_run11[6-9].root"); //UdpF, new target
    chain->Add("../root_data/gen_run12[0-3].root"); //UdpF
    
    chain->Add("../root_data/gen_run12[4-9].root"); //UdpF, reboot DAQ, new target
     
    chain->Add("../root_data/gen_run13[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run14[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run15[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run16[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run17[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run18[0-9].root"); //UdpF
    chain->Add("../root_data/gen_run19[0-1].root"); //UdpF
    
  
    //chain->Add("../root_data/gen_run19[2-9].root"); //carbon
    //chain->Add("../root_data/gen_run20[0-9].root"); //carbon
    //chain->Add("../root_data/gen_run211.root"); //carbon
    
    //chain->Add("../root_data/gen_run21[2-6].root"); //alpha

    ///********** end Marker for AutoCalibration.
    
    
  }else{
    
    TString fileName;
    int endRUNNUM = RUNNUM2;
    if( RUNNUM2 == -1) endRUNNUM = RUNNUM;
    
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
  if( saveCanvas ) selector->printControl(0); //quit after terminated
  chain->Process(selector, "");

}
