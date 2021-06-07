#include "Monitors.C+" // the plus sign mean compilation

void ChainMonitors(int RUNNUM = -1, int RUNNUM2 = -1, bool saveCanvas = false, bool isTraceON = false) {
  
  ///default saveCanvas = false, no save Cavas
  ///                   = true, save Canvas
  
  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///********** start Marker for AutoCalibration.

    //chain->Add("../root_data/gen_run01[2-4].root"); // alpha

    //chain->Add("../root_data/gen_run02[2-9].root"); // 15C(d,d')
    //chain->Add("../root_data/gen_run03[0-5].root"); // 15C(d,d')
    
    
    //chain->Add("../root_data/gen_run036.root"); // 14C(d,d')
    
    //chain->Add("../root_data/gen_run03[7-8].root"); // 15C(p,p'), CH2 180ug/cm^2
    //chain->Add("../root_data/gen_run039.root"); // 15C(p,p'), trace delay 3us
    
    //chain->Add("../root_data/gen_run04[0-9].root"); // 15C(p,p'), AutoRun
    //chain->Add("../root_data/gen_run05[0-8].root"); // 15C(p,p'), AutoRun

    
    //chain->Add("../root_data/trace_run04[0-9].root"); // 15C(p,p'), AutoRun
    //chain->Add("../root_data/trace_run05[0-8].root"); // 15C(p,p'), AutoRun

    
    //chain->Add("../root_data/gen_run059.root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run06[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run07[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/gen_run080.root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run08[2-9].root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run09[0-9].root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/gen_run101.root"); // alpha 3-MeV
    
    
    //chain->Add("../root_data/trace_run059.root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run06[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run07[0-9].root"); //15C(p,p') Target move +8 cm
    //chain->Add("../root_data/trace_run080.root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/trace_run08[2-9].root"); //15C(p,p') Target move +8 cm after retune
    //chain->Add("../root_data/trace_run09[0-9].root"); //15C(p,p') Target move +8 cm after retune
    
    
    ///============= alpha source
    //chain->Add("../root_data/gen_run102.root"); // mixed 
    //chain->Add("../root_data/gen_run003.root"); // 228Th
    //chain->Add("../root_data/gen_run082.root"); // 228Th
    //chain->Add("../root_data/gen_run055.root"); // 228Th
    
    //chain->Add("../root_data/trace_run104.root");// alpha for recoils
    
    //chain->Add("../root_data/gen_run108.root");// 15C on 363 CD2    
    //chain->Add("../root_data/gen_run109.root");// 15C on 363 CD2
    //chain->Add("../root_data/gen_run11[0-9].root");// 15C on 363 CD2
    //chain->Add("../root_data/gen_run12[0-6].root");// 15C on 363 CD2     
    //chain->Add("../root_data/gen_run12[7-9].root");// 15C on 363 CD2 increase HV on recoils
    //chain->Add("../root_data/gen_run13[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/gen_run14[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/gen_run15[0-9].root");// 15C on 363 CD2 332 mm from target to array
    //chain->Add("../root_data/gen_run16[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/gen_run17[0-7].root");// 15C on 363 CD2 

    //chain->Add("../root_data/trace_run108.root");// 15C on 363 CD2    
    //chain->Add("../root_data/trace_run109.root");// 15C on 363 CD2
    //chain->Add("../root_data/trace_run11[0-9].root");// 15C on 363 CD2
    //chain->Add("../root_data/trace_run12[0-6].root");// 15C on 363 CD2     
    //chain->Add("../root_data/trace_run12[7-9].root");// 15C on 363 CD2 increase HV on recoils
    //chain->Add("../root_data/trace_run13[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run14[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run15[0-9].root");// 15C on 363 CD2 332 mm from target to array
    //chain->Add("../root_data/trace_run16[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run17[0-7].root");// 15C on 363 CD2 
 
        
    //chain->Add("../root_data/gen_run17[8-9].root");// 15C on 363 CD2 -6 cm
    //chain->Add("../root_data/gen_run18[0-9].root");// 15C on 363 CD2  -6 cm
    //chain->Add("../root_data/gen_run19[0-9].root");// 15C on 363 CD2  -6 cm 272 mm from target to array
    //chain->Add("../root_data/trace_run20[0-9].root");// 15C on 363 CD2  -6 cm
          
    //chain->Add("../root_data/gen_run21[8-9].root");// 15C on 381 CH2  +8 cm
    //chain->Add("../root_data/gen_run22[0-9].root");// 15C on 381 CH2  +8 cm
    //chain->Add("../root_data/gen_run23[0-9].root");// 15C on 381 CH2  +8 cm  352 mm from target to array
    //chain->Add("../root_data/gen_run24[0-2].root");// 15C on 381 CH2  +8 cm
    
    chain->Add("../root_data/trace_run21[8-9].root");// 15C on 381 CH2  +8 cm
    chain->Add("../root_data/trace_run22[0-9].root");// 15C on 381 CH2  +8 cm
    chain->Add("../root_data/trace_run23[0-9].root");// 15C on 381 CH2  +8 cm  352 mm from target to array
    chain->Add("../root_data/trace_run24[0-2].root");// 15C on 381 CH2  +8 cm

    
    //chain->Add("../root_data/gen_run24[3-9].root");// 15C on 381CH2  +8 cm 8 hours later 
    //chain->Add("../root_data/gen_run25[0-7].root");// 15C on 381CH2  +8 cm 8 hours later
    
    
    //chain->Add("../root_data/trace_run13[0-9].root");// 15C on 363 CD2     
    //chain->Add("../root_data/trace_run14[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run15[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run16[0-9].root");// 15C on 363 CD2 
    //chain->Add("../root_data/trace_run17[0-7].root");// 15C on 363 CD2 
    
   // chain->Add("../root_data/trace_run21[8-9].root");// 15C on 381 CH2  +8 cm
    //chain->Add("../root_data/trace_run22[0-9].root");// 15C on 381 CH2  +8 cm
    //chain->Add("../root_data/trace_run23[0-9].root");// 15C on 381 CH2  +8 cm
    //chain->Add("../root_data/trace_run24[0-2].root");// 15C on 381 CH2  +8 cm


    //chain->Add("../root_data/gen_run26[0-9].root");// 14C on TiD
    //chain->Add("../root_data/gen_run27[0-2].root");// 14C on TiD


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
