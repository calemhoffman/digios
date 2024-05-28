#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TProof.h>

void process_run(Int_t RUNNUM=5, int isTrace=0){
  
    TString name;
    name.Form("../root_data/run%03d.root", RUNNUM);
    TFile f(name);

    if( !f.IsOpen() ) return;

    TTree *t1 = (TTree*)f.Get("tree");

    if( t1 == nullptr){
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!! cannot find tree in ../root_data/run%03d.root\n", RUNNUM);
        return;
    }

    isTrace = abs(isTrace);

    TString processCmd;
    if( isTrace == 1 ) {
        processCmd.Form("../Armory/GeneralSort.C++");
        if( t1 != nullptr) t1->Process(processCmd);
        f.Close();
        //}else if(isTrace == 1){
        //processCmd.Form("../Armory/GeneralSortTrace.C+");
        //t1->Process(processCmd);
        //f.Close();
    }else if(isTrace > 1){
        
        TChain * chain = new TChain("tree");
        chain->Add(name); 

        TProof * p = TProof::Open("", Form("workers=%d", isTrace));
        p->ShowCache(); // this should be ~/proof/....
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


}
