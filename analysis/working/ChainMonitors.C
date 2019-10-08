#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    //chain->Add("../root_data/gen_run02[8,9].root"); //14N, 440ug/cm2, 700mm
    //chain->Add("../root_data/gen_run038.root");     //14N, 440ug/cm2, 700mm, good run
    
    //chain->Add("../root_data/gen_run03[1-2].root"); //14N, 140ug/cm2, 700mm
    //chain->Add("../root_data/gen_run033.root");     //14N, 140ug/cm2, 700mm, good run
    //chain->Add("../root_data/gen_run03[4-7].root"); //14N, 140ug/cm2, 700mm
    //chain->Add("../root_data/gen_run04[3,6].root"); //14N, 140ug/cm2, 700mm
    
    //chain->Add("../root_data/gen_run047.root"); //14N, gold foil, slit normal
    //chain->Add("../root_data/gen_run048.root"); //14N, gold foil, slit out
    
    chain->Add("../root_data/gen_run049.root");     //14N, 140ug/cm2, 600mm
    chain->Add("../root_data/gen_run05[0-9].root"); //14N, 140ug/cm2, 600mm
    chain->Add("../root_data/gen_run06[0-9].root"); //14N, 140ug/cm2, 600mm
    chain->Add("../root_data/gen_run070.root");     //14N, 140ug/cm2, 600mm
    chain->Add("../root_data/gen_run07[7-9].root"); //14N, 140ug/cm2, 600mm
    chain->Add("../root_data/gen_run08[0-2].root"); //14N, 140ug/cm2, 600mm
    
    //chain->Add("../root_data/gen_run07[1-6].root"); //14N, 400ug/cm2, 600mm
    
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
