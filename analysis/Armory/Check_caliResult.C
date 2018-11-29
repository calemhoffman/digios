#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph2D.h>
#include <TGraph.h>
#include <TStyle.h>

void Check_caliResult(){

   TFile * caliResult = new TFile("caliResult.root", "read");
   
   gStyle->SetPalette(1,0);

   Int_t Div[2] = {6,4};
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cCheck1 = new TCanvas("cCheck1", "cCheck1", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cCheck1->GetShowEditor() )cCheck1->ToggleEditor();
   if(cCheck1->GetShowToolBar() )cCheck1->ToggleToolBar();
   cCheck1->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck1->cd(i)->SetGrid();
   }

   TGraph2D * gDist = NULL;
   for(int i = 0; i < 24 ; i ++){
     cCheck1->cd(i+1);
     
     TString name;
     name.Form("gDist%d", i);
     gDist = (TGraph2D*) caliResult->FindObjectAny(name);
     
     double zMax = gDist->GetZmax();
     double zMin = gDist->GetZmin();
      
     gDist->SetMaximum(zMax);
     gDist->SetMinimum(zMin);
   
     gDist->Draw("tri2");

     cCheck1->cd(i+1)->SetTheta(90);
     cCheck1->cd(i+1)->SetPhi(0);
     cCheck1->Update();
   }
}
