#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TString.h>
#include <TObjArray.h>

void RDTCutCreator(TString dataList, TString saveFileName = "rdtCuts.root", int eRange=5000, int deRange=3000, bool isLogz = false){
	
	printf("================ Graphic Cut Creator for RDT ============== \n");
   
   //TChain * chain = new TChain("gen_tree");
   TChain * chain = new TChain("tree");
   chain->Add(dataList);
   //chain->Add("../root_data/gen_run03[2,3,5,7].root"); 
   //chain->Add("../root_data/gen_run04[1,3].root");
   //chain->Add("../root_data/gen_run018.root");
   
   chain->GetListOfFiles()->Print();
   
	TString varX, varY, tag;
   
	gStyle->SetOptStat(00000);
	
	TCanvas * cCutCreator = new TCanvas("cCutCreator", "RDT Cut Creator", 100, 100, 800, 800);
	if( !cCutCreator->GetShowToolBar() ) cCutCreator->ToggleToolBar();
	
   cCutCreator->Update();
   if( isLogz ) cCutCreator->cd()->SetLogz();
	
	TCutG * cut = NULL;
	TObjArray * cutList = new TObjArray();
	
   
	TString expression[10];

   TH2F * h[4];

	for (Int_t i = 0; i < 4; i++) {

      printf("======== make a graphic cut on the plot (double click to stop), %d-th cut: ", i );

      //varX.Form("rdt[%d]",i+4); varY.Form("rdt[%d]",i); // dE grouped
      
      varX.Form("trdt[%d]",2*i); 
      varY.Form("trdt[%d]",2*i+1);

      h[i] = new TH2F(Form("h%d", i), Form("%s - %s", varY.Data(), varX.Data()), 500, 0, eRange, 500, 0, deRange);

      expression[i].Form("%s:%s>>h%d", 
                         varY.Data(),
                         varX.Data(),
                         i);

      chain->Draw(expression[i], "", "col");
      
      if( h[i]->Integral() < 1000 ) {
         h[i]->SetMarkerStyle(20);
         h[i]->SetMarkerSize(0.1);
         h[i]->Draw("");
      }
      
      cCutCreator->Modified(); cCutCreator->Update();

      gPad->WaitPrimitive();

      cut = (TCutG*) gROOT->FindObject("CUTG");
      
      if( cut == NULL ){
         printf(" stopped by user. no file saved or changed. \n");
         break;
      }

      TString name; name.Form("cut%d", i);
      cut->SetName(name);
      cut->SetVarX(varX.Data());
      cut->SetVarY(varY.Data());
      cut->SetTitle(tag);
      cut->SetLineColor(i+1);
      cutList->Add(cut);

      printf(" cut-%d \n", i);
              
     
	}
	
   TFile * cutFile = new TFile(saveFileName, "recreate");
	cutList->Write("cutList", TObject::kSingleKey);
	
	printf("====> saved %d cuts into %s\n", 4, saveFileName.Data());
   gROOT->ProcessLine(".q");
	
}
