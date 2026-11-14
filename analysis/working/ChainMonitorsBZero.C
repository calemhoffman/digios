#include "MonitorsBZero.C+" // the plus sign mean compilation

/// B = 0 T monitor (h098 20Ne(a,p)23Na). Called by Process_RUN with -BZ
void ChainMonitorsBZero(int RUNNUM = -1, int RUNNUM2 = -1, bool isTraceON = false) {

  TChain * chain = new TChain("gen_tree");

  if( RUNNUM == -1){
    /// this list only for manual Chain sort
    ///chain->Add("../root_data/gen_run002.root");

  }else{
    TString fileName;
    int endRUNNUM = RUNNUM2;
    if( RUNNUM2 == -1) endRUNNUM = RUNNUM;

    for( int i = RUNNUM ; i <= endRUNNUM ; i++){
      if( isTraceON == false ) fileName.Form("../root_data/gen_run%03d.root", i);
      if( isTraceON == true  ) fileName.Form("../root_data/trace_run%03d.root", i);
      chain->Add(fileName);
    }
  }

  printf("\033[0;31m========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  printf("========================================== Number of Files : %2d\033[0m\n",chain->GetListOfFiles()->GetEntries());
  printf(" number of entries : %llu \n", chain->GetEntries());

  MonitorsBZero * selector = new MonitorsBZero();
  chain->Process(selector, "");

}
