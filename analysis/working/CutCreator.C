#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TString.h>
#include <TObjArray.h>

void CutCreator(){
	
	printf("================ Graphic Cut Creator for RDT ============== \n");
   
   TChain * chain = new TChain("gen_tree");
   chain->Add("../../root_data/gen_run60.root");
   //chain->Add("data/gen_run49.root");
   //chain->Add("data/gen_run50.root");
   
   chain->GetListOfFiles()->Print();
   
	TString varX, varY, tag;
		
	gStyle->SetOptStat(00000);
	
	TCanvas * cCutCreator = new TCanvas("cCutCreator", "RDT Cut Creator", 100, 100, 800, 800);
	if( !cCutCreator->GetShowToolBar() ) cCutCreator->ToggleToolBar();
	
   
   TFile * cutFile = new TFile("rdtCuts.root", "recreate");
	cCutCreator->Update();
	
	TCutG * cut = NULL;
	TObjArray * cutList = new TObjArray();
	
   
	TString expression[10];

	for (Int_t i = 0; i < 4; i++) {

      printf("======== make a graphic cut on the plot, %d-th cut: ", i );

      varX.Form("rdt[%d]",i+4); varY.Form("rdt[%d]",i);

      expression[i].Form("%s:%s>>h(2096, 0, 8192, 2096, 0, 8192)", 
            varY.Data(),
            varX.Data());


      chain->Draw(expression[i], "", "col");
      cCutCreator->Modified(); cCutCreator->Update();

      gPad->WaitPrimitive();

      cut = (TCutG*) gROOT->FindObject("CUTG");

      TString name; name.Form("cut%d", i);
      cut->SetName(name);
      cut->SetVarX(varX.Data());
      cut->SetVarY(varY.Data());
      cut->SetTitle(tag);
      cut->SetLineColor(i);
      cutList->Add(cut);

      printf(" cut-%d \n", i);
              
     
	}
	
	cutList->Write("cutList", TObject::kSingleKey);
	
	printf("====> saved %d cuts into rdtCuts.root\n", 4);
	
}
