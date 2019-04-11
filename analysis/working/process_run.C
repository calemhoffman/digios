void process_run(Int_t RUNNUM=5, bool isTrace=false, Int_t SORTNUM=0)
{
  
  if (SORTNUM==0) {
    TString name;
    name.Form("../root_data/run%03d.root", RUNNUM);
    TFile f(name);

    if( !f.IsOpen() ) return;
    
    TTree *t1 = (TTree*)f.Get("tree");

    
    TString processCmd;
    if( isTrace ) {
        processCmd.Form("../Armory/GeneralSortTrace.C+");
    }else{
        processCmd.Form("../Armory/GeneralSort.C+");
    }
    t1->Process(processCmd);
    f.Close();
    
    //use cp then rm, because the ownership in ../root_data may be not the same
    //processCmd.Form(".!cp -v gen_run%03d.root ../root_data/gen_run%03d.root", RUNNUM, RUNNUM);
    //gROOT->ProcessLine(processCmd);
    //processCmd.Form(".!rm -fv gen_run%03d.root", RUNNUM);
    //gROOT->ProcessLine(processCmd);

    printf("======================================done RUN-%03d \n", RUNNUM);
    name.Form("../root_data/gen_run%03d.root", RUNNUM);
    printf("%s \n", name.Data());
    printf("============================================= \n");
    gROOT->ProcessLine(".q");
    
  }else if (SORTNUM==1) {
    TString name("gen.root");
    TFile ff(name);
    TTree *t2 = (TTree*)ff.Get("gen_tree");
    t2->Process("../codes/Monitors.C++");
    ff.Close();
  }
}
