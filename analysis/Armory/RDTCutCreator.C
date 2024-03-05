#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TString.h>
#include <TObjArray.h>

Float_t rdtCorr2[8][2];

void LoadRDTCorr2(){
   //========================================= e correction
   printf(" loading rdt correction.");
   ifstream file;
   file.open("correction_rdt.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= 8) break;
         rdtCorr2[i][0] = a;  ///  a1
         rdtCorr2[i][1] = b;  ///  a0 , e' = e * a1 + a0
         i = i + 1;
      }
      printf("............ done.\n");
      
   }else{
      printf("............ fail.\n");
      for( int i = 0; i < 8 ; i++){
         rdtCorr2[i][0] = 1.;
         rdtCorr2[i][1] = 0.;
      }
   }
   file.close();
   
   for( int i = 0; i < 8; i++){
      printf("%d | %f, %f \n", i, rdtCorr2[i][0], rdtCorr2[i][1]);
   }
   printf("=================================\n");
   
}

void RDTCutCreator(TString dataList, 
                   TString saveFileName = "rdtCuts.root", 
                   int eRange=5000, 
                   int deRange=3000, 
                   bool isLogz = false,
                   TString gate = "Ex>0 && Ex<7", 
                   TString treeName = "tree", 
                   bool useTrace = false
                   ){
   
   printf("================ Graphic Cut Creator for RDT ============== \n");
   
   LoadRDTCorr2();
   
   TChain * chain = new TChain(treeName);
   chain->Add(dataList);
   
   chain->GetListOfFiles()->Print();
   
   TString varX, varY, tag;

   gStyle->SetOptStat("neiou");

   TCanvas * cCutCreator = new TCanvas("cCutCreator", "RDT Cut Creator", 100, 100, 800, 800);
   if( !cCutCreator->GetShowToolBar() ) cCutCreator->ToggleToolBar();

   cCutCreator->Update();
   if( isLogz ) cCutCreator->cd()->SetLogz();

   TCutG * cut = NULL;
   TObjArray * cutList = new TObjArray();

   TString expression[10];

   TH2F * h[4];

   int count = 0;
   double rdtCorr[8] = {1.0,1.4,1.0,1.22,1.0,1.2,1.0,1.2};//match DE to E
   double rdtOff[8] = {0.0,-115.,0.0,104.0,0.0,-36.,0.0,2.0};//offset to match
   for (Int_t i = 0; i < 4; i++) {

      printf("======== make a graphic cut on the plot (double click to stop), %d-th cut: ", i );

      if( useTrace ){   
         // varX.Form("trdte[%d]",i); varY.Form("trdt[%d]",2*i+1);
         varX.Form("trdt[%d]",2*i); varY.Form("trdt[%d]",2*i+1);
      }else{
         ///varX.Form("rdt[%d]",i+4); varY.Form("rdt[%d]",i); // dE grouped
         // varX.Form("rdte[%d]",i); varY.Form("rdt[%d]",2*i+1);
         varX.Form("rdt[%d]",2*i); varY.Form("rdt[%d]",2*i+1);
      }

      h[i] = new TH2F(Form("h%d", i), Form("%s - %s", varY.Data(), varX.Data()), 1000, 500, eRange, 1000, 100, deRange);

      expression[i].Form("%s:%s>>h%d", 
                         varY.Data(),varX.Data(),i);
                         
      ///printf("%s \n", expression[i].Data());

      chain->Draw(expression[i], gate, "");
      
      if( h[i]->Integral() < 1000 ) {
         h[i]->SetMarkerStyle(20);
         h[i]->SetMarkerSize(0.4);
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
              
      count ++;
   }

   TFile * cutFile = new TFile(saveFileName, "recreate");
   cutList->Write("cutList", TObject::kSingleKey);
   
   printf("====> saved %d cuts into %s\n", count, saveFileName.Data());
   gROOT->ProcessLine(".q");

}
