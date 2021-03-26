///#include "Monitors.C+" /// the plus sign mean compilation
#include "MonGamma.C+" /// the plus sign mean compilation
///#include "../Armory/Cali_gamma.C+"

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1) {
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){

    ///chain->Add("../root_data/gen_run00[8-9].root"); ///15N degraded, 500ug/cm2
    ///chain->Add("../root_data/gen_run01[0-3].root"); ///15N degraded, 500ug/cm2
    
    ///chain->Add("../root_data/gen_run02[3-9].root"); ///16N, 500ug/cm2
    ///chain->Add("../root_data/gen_run03[0-4].root"); ///16N, 500ug/cm2
    
    ///chain->Add("../root_data/gen_run03[5-6].root"); ///16N, 500ug/cm2, 2nd tune
    
    ///chain->Add("../root_data/gen_run04[0-9].root"); ///16N, 500ug/cm2, 2nd tune, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run05[0-4].root"); ///16N, 500ug/cm2, 2nd tune, recoil trace length = 800 ch
    
    
    ///chain->Add("../root_data/gen_run05[6-9].root"); ///16N, 226ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run06[0-5].root"); ///16N, 226ug/cm2, sweeper, recoil trace length = 800 ch
    
    ///chain->Add("../root_data/gen_run10[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run11[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run12[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run13[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch    
    ///chain->Add("../root_data/gen_run14[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run15[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run16[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run17[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run18[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    ///chain->Add("../root_data/gen_run19[1-9].root"); ///16N, 366ug/cm2, sweeper, recoil trace length = 800 ch
    
    
    //######################################## Sept-2020
    
    ///chain->Add("../root_data/gen_run23[1-9].root"); ///Calibrated source
    ///chain->Add("../root_data/gen_run24[0-6].root"); ///Calibrated source
    ///chain->Add("../root_data/gen_run247.root"); ///BG
    ///chain->Add("../root_data/gen_run248.root"); ///152Eu
    ///chain->Add("../root_data/gen_run249.root"); ///152Eu, outside HPGe-3
    ///chain->Add("../root_data/gen_run25[0-9].root"); ///Cali-source, outside HPGe-3
    ///chain->Add("../root_data/gen_run26[0-3].root"); ///Cali-source, outside HPGe-3
    ///chain->Add("../root_data/gen_run264.root"); ///137Cs, at beam stopper, 661.7 keV
    ///chain->Add("../root_data/gen_run265.root"); ///137Cs, at beam stopper, 661.7 keV, 10 min
    ///chain->Add("../root_data/gen_run266.root"); ///133Ba, at beam stopper, 302.9, 356.0 keV, 10 min
    ///chain->Add("../root_data/gen_run267.root"); ///22Na, at beam stopper
    ///chain->Add("../root_data/gen_run268.root"); ///Junk, receiver problem
    ///chain->Add("../root_data/gen_run269.root"); ///22Na, at beam stopper, 10 min    
    ///chain->Add("../root_data/gen_run270.root"); ///152Eu, at beam stopper, 10 min    
    ///chain->Add("../root_data/gen_run271.root"); ///152Eu, farside, outside, 10 min    
    ///chain->Add("../root_data/gen_run272.root"); ///152Eu, farside, inside, 10 min    
    ///chain->Add("../root_data/gen_run273.root"); ///152Eu, farside, inside FC out, 20 min    
    ///chain->Add("../root_data/gen_run274.root"); ///152Eu, farside, outside FC out, 20 min
    ///chain->Add("../root_data/gen_run275.root"); ///152Eu, Al stopper, 10 min    
    ///chain->Add("../root_data/gen_run27[6-9].root"); ///mixed source, farside, inside  
    ///chain->Add("../root_data/gen_run28[0-5].root"); ///mixed source, farside, inside  
    ///chain->Add("../root_data/gen_run28[6-9].root"); ///BG
    ///chain->Add("../root_data/gen_run29[0-5].root"); ///BG
    ///chain->Add("../root_data/gen_run30[8-9].root"); /// 20 pnA, Al stopper
    ///chain->Add("../root_data/gen_run31[0-9].root"); 
    ///chain->Add("../root_data/gen_run32[0-9].root"); 
    ///chain->Add("../root_data/gen_run33[0-3].root");
    ///chain->Add("../root_data/gen_run334.root"); ///BG
    
    ///==========Calibrateion runs
    ///chain->Add("../root_data/gen_run335.root"); ///14pnA, Si stopper, 100x att, ryan_isomer_6
    ///chain->Add("../root_data/gen_run336.root"); ///14pnA, Al stopper, 100x att
    ///chain->Add("../root_data/gen_run337.root"); ///40pnA, Al stopper, 100x att, ryan_isomer_7 before, 
    ///chain->Add("../root_data/gen_run338.root"); ///100pnA, Al stopper, no att 
    
    ///==========lower beam energy run
    ///chain->Add("../root_data/gen_run339.root"); ///
    ///chain->Add("../root_data/gen_run340.root"); ///
    ///chain->Add("../root_data/gen_run341.root"); ///
    ///chain->Add("../root_data/gen_run342.root"); ///dE-E
    ///chain->Add("../root_data/gen_run343.root"); ///Ta, 16enA
    ///chain->Add("../root_data/gen_run344.root"); ///Al
    ///chain->Add("../root_data/gen_run345.root"); ///Al
    ///chain->Add("../root_data/gen_run34[6-9].root"); ///Al, 50 enA
    ///chain->Add("../root_data/gen_run35[0-4].root"); ///Al, 50 enA
    
    ///=============== useful data
    ///========= soucre at Al stopper
    //chain->Add("../root_data/gen_run355.root"); ///137Cs, upstream
    ///chain->Add("../root_data/gen_run356.root"); ///137Cs, downstream
    
    //chain->Add("../root_data/gen_run357.root"); ///133Ba, upstream
    ///chain->Add("../root_data/gen_run358.root"); ///133Ba, downstream
    
    ///chain->Add("../root_data/gen_run359.root"); ///152Eu, upstream
    //chain->Add("../root_data/gen_run360.root"); ///152Eu, upstream
    ///chain->Add("../root_data/gen_run361.root"); ///152Eu, downstream
    
    ///chain->Add("../root_data/gen_run362.root"); ///226Ra, upstream
    //chain->Add("../root_data/gen_run363.root"); ///226Ra, upstream
    ///chain->Add("../root_data/gen_run364.root"); ///226Ra, downstream
    
    ///chain->Add("../root_data/gen_run365.root"); ///207Bi, upstream
    //chain->Add("../root_data/gen_run366.root"); ///207Bi, upstream
    ///chain->Add("../root_data/gen_run367.root"); ///207Bi, downstream

    
    //chain->Add("../root_data/gen_run368.root"); ///BG
    //chain->Add("../root_data/gen_run369.root"); ///BG
    
    //====== high energy   
    //chain->Add("../root_data/gen_run306.root"); ///16N, 140enA, Ta stopper
    //chain->Add("../root_data/gen_run308.root"); ///16N, 140enA, Al stopper

    ///chain->Add("../root_data/gen_run30[2-3].root"); ///16N, 27.5enA
    chain->Add("../root_data/gen_run30[7-9].root"); ///16N, 140enA
    chain->Add("../root_data/gen_run31[0-9].root"); ///16N, 140enA
    chain->Add("../root_data/gen_run32[0-9].root"); ///16N, 140enA
    chain->Add("../root_data/gen_run33[0-7].root"); ///16N, 140enA
    ///chain->Add("../root_data/gen_run338.root"); ///16N, 700enA
    
    //====== low energy
    ///chain->Add("../root_data/gen_run339.root");     ///16N, 1.6enA
    //chain->Add("../root_data/gen_run34[0-1].root"); ///16N, 1.6enA
    //chain->Add("../root_data/gen_run34[4-5].root"); ///16N, 16enA
    //chain->Add("../root_data/gen_run34[6-9].root"); ///16N, 50enA
    //chain->Add("../root_data/gen_run35[0-4].root"); ///16N, 50enA
    
    
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
  
  ///Simple call TSelector
  ///chain->Process("Monitors.C+");
  
  
  ///Some input to TSelector
  
  ///Cali_gamma(chain, RUNNUM);
  
  ///Monitors * selector = new Monitors();
  ///if( RUNNUM == -10 || RUNNUM2 == -10 ) selector->printControl(0); ///quit after terminated
  ///chain->Process(selector, "");

  MonGamma * selector = new MonGamma();
  chain->Process(selector, "");

}

