//#include "Monitors.C+" // the plus sign mean compilation
#include "../Armory/Cali_gamma.C+"

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    //chain->Add("../root_data/gen_run00[8-9].root"); //15N degraded, 500ug/cm2
    //chain->Add("../root_data/gen_run01[0-3].root"); //15N degraded, 500ug/cm2
    
    //chain->Add("../root_data/gen_run02[3-9].root"); //16N, 500ug/cm2
    //chain->Add("../root_data/gen_run03[0-4].root"); //16N, 500ug/cm2
    
    //chain->Add("../root_data/gen_run03[5-6].root"); //16N, 500ug/cm2, 2nd tune
    
    //chain->Add("../root_data/gen_run04[0-9].root"); //16N, 500ug/cm2, 2nd tune, recoil trace length = 800 ch
    //chain->Add("../root_data/gen_run05[0-4].root"); //16N, 500ug/cm2, 2nd tune, recoil trace length = 800 ch
    
    
    //chain->Add("../root_data/gen_run05[6-9].root"); //16N, 226ug/cm2, sweeper, recoil trace length = 800 ch
    //chain->Add("../root_data/gen_run06[0-5].root"); //16N, 226ug/cm2, sweeper, recoil trace length = 800 ch
    
    
    
    chain->Add("../root_data/gen_run10[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run11[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run12[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run13[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch    
    chain->Add("../root_data/gen_run14[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run15[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run16[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run17[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run18[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    chain->Add("../root_data/gen_run19[1-9].root"); //16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    
    

    
    
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
  
  Cali_gamma(chain, RUNNUM);
  
  //Monitors * selector = new Monitors();
  //if( RUNNUM == -10 || RUNNUM2 == -10 ) selector->printControl(0); //quit after terminated
  //chain->Process(selector, "");

}

