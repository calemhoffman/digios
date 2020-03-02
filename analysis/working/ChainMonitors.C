#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    
    //chain->Add("../root_data/gen_run006.root"); //alpha
    
    ///adjusting things
    //chain->Add("../root_data/gen_run009.root"); //82Kr
    //chain->Add("../root_data/gen_run01[0-8].root"); //82Kr
    
    ///start a stable run
    chain->Add("../root_data/gen_run019.root"); //82Kr
    chain->Add("../root_data/gen_run02[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run03[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run04[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run05[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run06[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run07[0-9].root"); //82Kr
    //chain->Add("../root_data/gen_run080.root"); //82Kr
    
    
    //chain->Add("../root_data/gen_run08[1-9].root"); //82Se
    //chain->Add("../root_data/gen_run09[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run10[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run11[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run12[0-5].root"); //82Se
    //chain->Add("../root_data/gen_run14[1-9].root"); //82Se
    //chain->Add("../root_data/gen_run15[0-8].root"); //82Se 
    
    //chain->Add("../root_data/gen_run16[4-9].root"); //82Se (after modified tune and ELUM)
    //chain->Add("../root_data/gen_run17[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run18[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run19[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run20[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run21[0-9].root"); //82Se
    //chain->Add("../root_data/gen_run220.root"); //82Se
    
    
    //chain->Add("../root_data/gen_run12[6-9].root"); //carbon
    //chain->Add("../root_data/gen_run13[0-4].root"); //carbon
    //chain->Add("../root_data/gen_run15[9].root"); //carbon
    //chain->Add("../root_data/gen_run22[1-8].root"); // carbon
     
  }else{
    
    TString fileName;
  
    if( RUNNUM2 == -1 ) RUNNUM2 = RUNNUM;
    
    for( int i = RUNNUM ; i <= RUNNUM2 ; i++){
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
  selector->testingInput(1231441241);
  chain->Process(selector, "");

}
