#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TString.h>
#include <TObjArray.h>

void CutCreator(){
	
	printf("================ Graphic Cut Creator ============== \n");
	/*
	printf("1 = Exit dE - E cut \n");
	printf("2 = Exit dE - T cut \n");
	printf("3 = Exit  E - T cut \n");
	printf("4 = AtScat dE - E cut \n");
	printf("5 = AtScat dE - T cut \n");
	printf("6 = AtScat  E - T cut \n");
	printf("7 = AtScat totE - T cut \n");
	printf("============== Choice? : ");
	*/
	int option = 1; // assume exit dE - E
	//scanf("%d", &option);
	
	TString varX, varY, tag;
	switch(option){
		case 1 : varX = "TMath::Abs(ezero[1])"; varY = "TMath::Abs(ezero[0])"; tag = "1"; break;
		case 2 : varX = "TMath::Abs(tac[0])";   varY = "TMath::Abs(ezero[0])"; tag = "2"; break;
		case 3 : varX = "TMath::Abs(tac[0])";   varY = "TMath::Abs(ezero[1])"; tag = "3"; break;
		case 4 : varX = "TMath::Abs(ezero[3])"; varY = "TMath::Abs(ezero[2])"; tag = "4"; break;
		case 5 : varX = "TMath::Abs(tac[0])";   varY = "TMath::Abs(ezero[2])"; tag = "5"; break;
		case 6 : varX = "TMath::Abs(tac[0])";   varY = "TMath::Abs(ezero[3])"; tag = "6"; break;
		case 7 : varX = "TMath::Abs(tac[0])";   varY = "TMath::Abs(ezero[4])"; tag = "7"; break;
	}
	
	Float_t ezero_cal[5][2]={{0.0367,4.6086},{0.0367,4.6086},{0.01,0},{0.01,0},{0.01,0}};
	//Float_t ezero_cal[5][2]={{1.0,0.0},{1.0,0.0},{0.01,0},{0.01,0},{0.01,0}};
	
	gStyle->SetOptStat(00000);
	
	TCanvas * cCutCreator = new TCanvas("cCutCreator", "Cut Creator", 100, 100, 800, 800);
	if( !cCutCreator->GetShowToolBar() ) cCutCreator->ToggleToolBar();
	
	TString expression; //CRH
	expression.Form("%s*%f + %f:%s * %f + %f>>h(2000, 0, 200, 2000, 0, 80)", 
					varY.Data(), ezero_cal[0][0], ezero_cal[0][1],
					varX.Data(), ezero_cal[1][0], ezero_cal[1][1]);
					
	//expression.Form("%s:%s>>h(500, 0, 5000, 500, 0, 1000)", 
	//				varY.Data(),
	//				varX.Data());
	
	
	gen_tree->Draw(expression, "", "colz", 100000);

	
	TFile * cutFile = new TFile("cutsFile.root", "recreate");
	cCutCreator->Update();
	
	TCutG * cut = NULL;
	TObjArray * cutList = new TObjArray();
	
	int count = 1;
	do{
		printf("======== make a graphic cut on the plot (double click on plot to stop) : " );
	
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
