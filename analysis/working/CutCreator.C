#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TString.h>
#include <TObjArray.h>

void CutCreator(){
	
	printf("================ Graphic Cut Creator for ISS ============== \n");
	/*
	printf("1 = Recoil cuts \n");
	printf("2 = E vs. Z cut \n");
	printf("============== Choice? : ");
	*/
	int option = 1; // assume exit dE - E
	//scanf("%d", &option);
	
	TString varX, varY, tag;
	switch(option){
	case 1 : varX = "TMath::Abs(rdt[4])"; varY = "TMath::Abs(rdt[0])"; tag = "1"; break;
	}
		
	gStyle->SetOptStat(00000);
	
	TCanvas * cCutCreator = new TCanvas("cCutCreator", "Cut Creator", 100, 100, 800, 800);
	if( !cCutCreator->GetShowToolBar() ) cCutCreator->ToggleToolBar();
	
	TString expression[10];

	for (Int_t i=0;i<4;i++) {
	  varX.Form("TMath::Abs(rdt[%d])",i+4); varY.Form("TMath::Abs(rdt[%d])",i);
	  
	  expression[i].Form("%s:%s>>h(4192, 0, 8192, 4192, 0, 8192)", 
			     varY.Data(),
			     varX.Data());
	}


	TFile * cutFile = new TFile("cutsFile.root", "recreate");
	cCutCreator->Update();
	
	TCutG * cut = NULL;
	TObjArray * cutList = new TObjArray();
	
	int count = 1;
	do{
	  printf("======== make a graphic cut on the plot (double click on plot to stop) : " );

	  gen_tree->Draw(expression[count-1], "", "colz", 100000);
	  cCutCreator->Modified(); cCutCreator->Update();
	  
	  gPad->WaitPrimitive();
	  
	  cut = (TCutG*) gROOT->FindObject("CUTG");
	  
	  if( cut == NULL) {
	    printf(" break \n");
	    count --;
	    break;
	  }
	  
	  TString name; name.Form("cut%d", count);
	  cut->SetName(name);
	  cut->SetVarX(varX.Data());
	  cut->SetVarY(varY.Data());
	  cut->SetTitle(tag);
	  cut->SetLineColor(count);
	  cutList->Add(cut);
	  //cut->Write();
	  
	  printf(" cut-%d \n", count);			
	  count++;
	  
	}while( cut != NULL );
	
	cutList->Write("cutList", TObject::kSingleKey);
	
	printf("====> saved %d cuts into cutsFile.root\n", count);
	
}
