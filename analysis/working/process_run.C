#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TProof.h>

void process_run(Int_t RUNNUM=5, int isTrace=0, Int_t SORTNUM=0){
  
  if (SORTNUM==0) {
    TString name;
    name.Form("../root_data/run%03d.root", RUNNUM);
    TFile f(name);

    if( !f.IsOpen() ) return;
    
    TTree *t1 = (TTree*)f.Get("tree");

    isTrace = abs(isTrace);
    
    TString processCmd;
    if( isTrace == 0 ) {
        processCmd.Form("../Armory/GeneralSort.C+");
        t1->Process(processCmd);
        f.Close();
    }else if(isTrace == 1){
        processCmd.Form("../Armory/GeneralSortTrace.C+");
        t1->Process(processCmd);
        f.Close();
    }else if(isTrace > 1){
        TChain * chain = new TChain("tree");
        chain->Add(name); 
   
        TProof * p = TProof::Open("", Form("workers=%d", isTrace));
   
        chain->SetProof();
        chain->Process("../Armory/GeneralSortTraceProof.C++", name);
                
        delete chain;
        delete p;
        
    }else{
        return;
    }
    
    printf("======================================done RUN-%03d \n", RUNNUM);

    if( isTrace < 2 ){
        name.Form("../root_data/gen_run%03d.root", RUNNUM);
    }else{
        name.Form("../root_data/trace_run%03d.root", RUNNUM);
    }
    printf("%s \n", name.Data());
    printf("============================================= \n");
    //gROOT->ProcessLine(".q");
    
  }else if (SORTNUM==1) {
    TString name("gen.root");
    TFile ff(name);
    TTree *t2 = (TTree*)ff.Get("gen_tree");
    t2->Process("../codes/Monitors.C++");
    ff.Close();
  }
}
