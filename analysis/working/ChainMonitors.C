#include "Monitors.C+" // the plus sign mean compliation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    
      chain->Add("../root_data/gen_run01[5-9].root"); // 6Li
      chain->Add("../root_data/gen_run02[0,1,4-9].root"); // 6Li
      chain->Add("../root_data/gen_run03[0-9].root"); // 6Li
      chain->Add("../root_data/gen_run04[0-9].root"); // 6Li
      chain->Add("../root_data/gen_run05[0-2].root"); // 6Li
      
      
      /*chain->Add("../root_data/gen_run03[3-8].root"); // 6Li
      chain->Add("../root_data/gen_run040.root"); // 6Li
      */
      
     // chain->Add("../root_data/gen_run04[2-9].root"); // 6Li
     // chain->Add("../root_data/gen_run05[0-2].root"); // 6Li

      
     /* 
      chain->Add("../root_data/gen_run05[3-9].root"); // 7Li
      chain->Add("../root_data/gen_run06[0-5,7-9].root"); // 7Li
      chain->Add("../root_data/gen_run07[1-6,9].root"); // 7Li
      chain->Add("../root_data/gen_run08[0,1].root"); // 7Li

*/

    //chain->Add("../root_data/gen_run10[2-3].root"); // B11+CD2 120ug
    // chain->Add("../root_data/gen_run10[4-9].root"); // B11+Ti+3H 450/30ug
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
