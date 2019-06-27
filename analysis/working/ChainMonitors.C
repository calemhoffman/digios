#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    //chain->Add("../root_data/gen_run054.root"); //31Si?
    //chain->Add("../root_data/gen_run055.root"); //31Si?
    //chain->Add("../root_data/gen_run05[7-9].root");
    //chain->Add("../root_data/gen_run060.root");  
    
    
    //chain->Add("../root_data/gen_run062.root"); // beam tuning run from 20 enA to 250 enA
    //chain->Add("../root_data/gen_run06[3-9].root"); // after beam tuning on the June-22 night. 
    //chain->Add("../root_data/gen_run070.root");
   // chain->Add("../root_data/gen_run07[8-9].root");
    //chain->Add("../root_data/gen_run08[0-9].root");
    //chain->Add("../root_data/gen_run09[0-1].root"); 
    
    //227ug target, singles
    //chain->Add("../root_data/gen_run12[7,9].root");
    //chain->Add("../root_data/gen_run13[1,3,5,7,9].root");
    //chain->Add("../root_data/gen_run14[1,3,5].root");
    //chain->Add("../root_data/gen_run15[1,3,5,7-9].root");
    //chain->Add("../root_data/gen_run16[1,3,5,7,9].root");
    //chain->Add("../root_data/gen_run17[1,3,5,7,9].root");
    //chain->Add("../root_data/gen_run18[1,3,5].root");
    
    //227ug target, array triggering
     //chain->Add("../root_data/gen_run128.root");
     //chain->Add("../root_data/gen_run13[0,2,4,6,8].root");
     //chain->Add("../root_data/gen_run14[0,2,4,6,7,9].root");
     //chain->Add("../root_data/gen_run15[2,4,6].root");
     //chain->Add("../root_data/gen_run16[0,2,4,6,8].root");
     //chain->Add("../root_data/gen_run17[0,2,4,6,8].root");
     //chain->Add("../root_data/gen_run18[0,2,4,6].root");
     
    chain->Add("../root_data/gen_run039.root");
    chain->Add("../root_data/gen_run04[0-9].root");
    chain->Add("../root_data/gen_run05[0-9].root");
    chain->Add("../root_data/gen_run06[0-9].root"); 
    chain->Add("../root_data/gen_run07[0-9].root"); 
    chain->Add("../root_data/gen_run08[0-9].root");  
    chain->Add("../root_data/gen_run09[0-9].root");
    chain->Add("../root_data/gen_run10[0-9].root");
    chain->Add("../root_data/gen_run11[0-9].root");

    //227ug, all
    chain->Add("../root_data/gen_run12[7-9].root");
    chain->Add("../root_data/gen_run13[0-9].root");
    chain->Add("../root_data/gen_run14[0-7,9].root");
    chain->Add("../root_data/gen_run15[1-9].root");
    chain->Add("../root_data/gen_run16[0-9].root");
    chain->Add("../root_data/gen_run17[0-9].root");
    chain->Add("../root_data/gen_run18[0-9].root");
    chain->Add("../root_data/gen_run19[0-5].root");
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
