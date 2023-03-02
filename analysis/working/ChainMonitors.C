#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

     ///chain->Add("../root_data/gen_run999.root"); // alpha calibration from 10B run
     ///chain->Add("../root_data/gen_run06[1-3].root"); // long over night run, slits open, x100 att. so around 1e7 pps, 175 ug/cm2
     ///chain->Add("../root_data/gen_run061.root"); // long over night run, slits open, x100 att. so around 1e7 pps, 175 ug/cm2
     ///chain->Add("../root_data/gen_run062.root"); // short run, slits open, x100 att. so around 1e7 pps, 70 ug/cm2
     ///chain->Add("../root_data/gen_run063.root"); // long over night run, slits open, x300 att. so around 3e6 pps, 120 ug/cm2
     ///chain->Add("../root_data/gen_run076.root"); // alpha run
     
     ///chain->Add("../root_data/gen_run085.root"); //short run no LaBar 150 ug target x10 att. (6e7 pps)
     ///chain->Add("../root_data/gen_run086.root"); //short run no CsI 150 ug target x10 att. (6e7 pps)
    
     ///chain->Add("../root_data/gen_run084.root"); //no FC and no slits 150 ug CD2 target x10 att. (6e7 pps)
     ///chain->Add("../root_data/gen_run08[7-9].root"); //no FC and no slits 150 ug CD2 target x10 att. (6e7 pps) run 88 starts the 100 ug target)
     ///chain->Add("../root_data/gen_run09[0-8].root"); //no FC and no slits 150 ug CD2  target x10 att. (6e7 pps) run 88 starts the 100 ug target) upto run 98     
     
     //CARBON TARGET -- GEN RUN
     ///chain->Add("../root_data/gen_run101.root"); // carbon
     ///chain->Add("../root_data/gen_run106.root"); // carbon (CLOCK reset at 105)
     ///chain->Add("../root_data/gen_run15[7-9].root"); // carbon since run 157 (100 ug/cm2)
     ///chain->Add("../root_data/gen_run16[0-9].root"); // carbon since run 157
     ///chain->Add("../root_data/gen_run17[0-9].root"); // carbon since run 157
     ///chain->Add("../root_data/gen_run18[0-1].root"); // carbon since run 157 
     ///chain->Add("../root_data/gen_run20[5-9].root"); // Carbon 100 ug/cm2
     ///chain->Add("../root_data/gen_run21[0-9].root"); // Carbon 100 ug/cm2
     ///chain->Add("../root_data/gen_run22[0-7].root"); // Carbon 100 ug/cm2      
     
     //CD2 TARGET -- GEN RUN
     ///chain->Add("../root_data/gen_run084.root"); // CD2 100 ug/cm2
     ///chain->Add("../root_data/gen_run08[7-9].root"); // CD2 100 ug/cm2 
     ///chain->Add("../root_data/gen_run09[0-8].root"); // CD2 100 ug/cm2 (CLOCK reset at 105)
     ///
     ///chain->Add("../root_data/gen_run10[7-9].root"); // CD2 90 ug/cm2 
     ///chain->Add("../root_data/gen_run11[0-9].root"); // CD2 90 ug/cm2 
     ///chain->Add("../root_data/gen_run12[0-9].root"); // CD2 90 ug/cm2 (run 127 empty)
     ///chain->Add("../root_data/gen_run13[0-5].root"); // CD2 90 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run13[6-9].root"); // CD2 94 ug/cm2 
     ///chain->Add("../root_data/gen_run14[0-8].root"); // CD2 94 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run149.root"); // CD2 137 ug/cm2 
     ///chain->Add("../root_data/gen_run15[0-7].root"); // CD2 137 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run18[2-9].root"); // CD2 125 ug/cm2 
     ///chain->Add("../root_data/gen_run19[0-3].root"); // CD2 125 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run19[4-9].root"); // CD2 115 ug/cm2 
     ///chain->Add("../root_data/gen_run20[0-4].root"); // CD2 115 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run229.root"); // CD2 98(a) ug/cm2 
     ///chain->Add("../root_data/gen_run23[0-6].root"); // CD2 98(a) ug/cm2 untile run236, then 92 ug/cm2 
     ///
     ///chain->Add("../root_data/gen_run23[7-9].root"); // CD2 92 ug/cm2 
     ///chain->Add("../root_data/gen_run24[0-9].root"); // CD2 92 ug/cm2 
     ///chain->Add("../root_data/gen_run25[0-7].root"); // CD2 92 ug/cm2
     ///
     ///chain->Add("../root_data/gen_run25[8-9].root"); // CD2 98(b) ug/cm2 (current increased during this period by factor of ~2)
     ///chain->Add("../root_data/gen_run26[0-5].root"); // CD2 98(b) ug/cm2 
     
     ///chain->Add("../root_data/gen_run26[6-9].root"); // CD2 91 ug/cm2 
     ///chain->Add("../root_data/gen_run27[0-1].root"); // CD2 91 ug/cm2
     
     ///chain->Add("../root_data/gen_run27[2-6].root"); // CD2 120 ug/cm2 
     
     ///chain->Add("../root_data/gen_run27[7-9].root"); // CD2 121 ug/cm2
     ///chain->Add("../root_data/gen_run28[0-2].root"); // CD2 121 ug/cm2
     
     ///chain->Add("../root_data/gen_run28[3-9].root"); // carbon 102 ug/cm2
     ///chain->Add("../root_data/gen_run29[0-9].root"); // carbon 102 ug/cm2
     ///chain->Add("../root_data/gen_run30[0-6].root"); // carbon 102 ug/cm2

     ///chain->Add("../root_data/gen_run30[7-9].root"); // CD2 86 ug/cm2 at larger tgt-array sep, around ~450 mm
     
     ///chain->Add("../root_data/gen_run31[0-9].root"); // CD2 112 ug/cm2 at larger tgt-array sep, around ~450 mm

     ///chain->Add("../root_data/gen_run32[0-9].root"); // CD2 112 ug/cm2 at larger tgt-array sep, around ~450 mm
     
     ///chain->Add("../root_data/gen_run32[0-9].root"); // Carbon since run 322
     ///chain->Add("../root_data/gen_run33[0-5].root"); // Carbon since run 322
     
    ///chain->Add("../root_data/gen_run33[0-5].root"); // alpha source at 445 mm + Apollo gammas
    
      //CD2 TARGET -- TRACE RUN
     ///chain->Add("../root_data/trace_run084.root"); // CD2 100 ug/cm2
     ///chain->Add("../root_data/trace_run08[7-9].root"); // CD2 100 ug/cm2 
     ///chain->Add("../root_data/trace_run09[0-8].root"); // CD2 100 ug/cm2 (CLOCK reset at 105)
     ///chain->Add("../root_data/trace_run307.root"); // CD2 86 ug/cm2 at larger tgt-array sep, around ~500 mm
     ///
     ///chain->Add("../root_data/trace_run084.root"); // CD2 100 ug/cm2
     ///chain->Add("../root_data/trace_run08[7-9].root"); // CD2 100 ug/cm2 
     ///chain->Add("../root_data/trace_run09[0-8].root"); // CD2 100 ug/cm2 (CLOCK reset at 105)
     ///
     ///chain->Add("../root_data/trace_run10[7-9].root"); // CD2 90 ug/cm2 
     ///chain->Add("../root_data/trace_run11[0-9].root"); // CD2 90 ug/cm2 
     ///chain->Add("../root_data/trace_run12[0-9].root"); // CD2 90 ug/cm2 (run 127 empty)
     ///chain->Add("../root_data/trace_run13[0-5].root"); // CD2 90 ug/cm2 
     ///
     ///chain->Add("../root_data/trace_run13[6-9].root"); // CD2 94 ug/cm2 
     ///chain->Add("../root_data/trace_run14[0-8].root"); // CD2 94 ug/cm2 
     ///
     ///chain->Add("../root_data/trace_run149.root"); // CD2 137 ug/cm2 
     ///chain->Add("../root_data/trace_run15[0-7].root"); // CD2 137 ug/cm2 
     ///
     ///chain->Add("../root_data/trace_run18[2-9].root"); // CD2 125 ug/cm2 
     ///chain->Add("../root_data/trace_run19[0-3].root"); // CD2 125 ug/cm2 
     
     
     chain->Add("../root_data/trace_run101.root"); // carbon
     chain->Add("../root_data/trace_run106.root"); // carbon (CLOCK reset at 105)
     chain->Add("../root_data/trace_run15[7-9].root"); // carbon since run 157 (100 ug/cm2)
     chain->Add("../root_data/trace_run16[0-9].root"); // carbon since run 157
     chain->Add("../root_data/trace_run17[0-9].root"); // carbon since run 157
     chain->Add("../root_data/trace_run18[0-1].root"); // carbon since run 157 
     chain->Add("../root_data/trace_run20[5-9].root"); // Carbon 100 ug/cm2
     chain->Add("../root_data/trace_run21[0-9].root"); // Carbon 100 ug/cm2
     chain->Add("../root_data/trace_run22[0-7].root"); // Carbon 100 ug/cm2  
     
     
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
