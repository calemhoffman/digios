#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    //chain->Add("../root_data/gen_run018.root"); //30Si(d,p)
    
    ///29Al(d,p) 250ug Saturated recoil
    //chain->Add("../root_data/gen_run020.root"); 
    
    ///29Al(d,p) 350ug Saturated recoil
    //chain->Add("../root_data/gen_run02[1,3,5,7,9].root"); 
    //chain->Add("../root_data/gen_run042.root"); // from run41, RDT-0, HV = 50V
    
    ///29Al(d,p) 250ug, new recoil preamp - recoil trigger
    //chain->Add("../root_data/gen_run03[1,4].root");
    
    ///29Al(d,p) 250ug, new recoil preamp
    chain->Add("../root_data/gen_run03[2,3,5,7].root"); 
    chain->Add("../root_data/gen_run04[1,3].root"); // from run41, RDT-0, HV = 50V
    
    ///from run 49, RDT-4, HV = 50 V, RDT-5 HV = 30V
    chain->Add("../root_data/gen_run049.root"); 
    chain->Add("../root_data/gen_run05[1,4,5,6,7].root"); 
    
    ///from run 59, TAC signal is almost properly done.
    chain->Add("../root_data/gen_run059.root"); 
    chain->Add("../root_data/gen_run063.root"); 
    
     ///from run 67, TAC signal is properly done.
    chain->Add("../root_data/gen_run06[7,9].root"); 
    chain->Add("../root_data/gen_run07[0,1,4,8,9].root"); 
    chain->Add("../root_data/gen_run08[1,5,7,8,9].root");
    chain->Add("../root_data/gen_run09[0,1,2,3,4,6,7,8,9].root"); //runs 99 onwards have higher thresholds because of noise in the array
    //chain->Add("../root_data/gen_run099.root"); 
    chain->Add("../root_data/gen_run10[0-3,5-9].root"); 
    chain->Add("../root_data/gen_run11[0-9].root"); 
    chain->Add("../root_data/gen_run12[0-9].root"); 
    chain->Add("../root_data/gen_run13[0-9].root"); 

    
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
