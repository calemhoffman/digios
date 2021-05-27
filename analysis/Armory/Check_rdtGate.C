#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <fstream>
#include <TCutG.h>

void Check_rdtGate(TString dataList, TString rdtCut, TString treeName = "gen_tree", TString gate ="", double eRange = 6000, double deRange = 4000){  
   
   //============================= RDT cut
   TFile * fileCut = new TFile(rdtCut);
   
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
      
   if( fileCut->IsOpen() ){
      
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      printf(" ======== found %d cuts in %s \n", numCut, fileCut->GetName());
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         cut[i]->SetLineWidth(2);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }   
   }
   
   ///TString ezCutFile = "EZCut.root";
   ///TFile * fileEZCut = new TFile(ezCutFile);
   ///TCutG * ezCut = (TCutG *) fileEZCut->FindObjectAny("cutez");

   printf("gate : %s \n", gate.Data());
   
/**///======================================================== read tree         
   TChain * chain = new TChain(treeName);
   chain->Add(dataList);
   chain->GetListOfFiles()->Print();

   printf("===================> Total #Entry: %10lld \n",  chain->GetEntries());
   
   gStyle->SetOptStat(10001);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);

   TCanvas * cRDTCut = new TCanvas("cRDTCut", "RDT Cut Checker | " + dataList + " | " + rdtCut, 100, 100, 800, 800);
   if( cRDTCut->GetShowToolBar() ) cRDTCut->ToggleToolBar();
   cRDTCut->Divide(2,2);

/**///======================================================== Analysis

   TH2F * h[4];
   TString varX, varY;
   TString expression[4];

   for( int i = 0 ; i < 4; i++){
     
     varX.Form("trdt[%d]",2*i); 
     varY.Form("trdt[%d]",2*i+1);

     h[i] = new TH2F(Form("h%d", i), Form("%s - %s", varY.Data(), varX.Data()), 500, 0, eRange, 500, 0, deRange);

     expression[i].Form("%s:%s>>h%d", 
                        varY.Data(),
                        varX.Data(),
                        i);

     cRDTCut->cd(i+1);
     chain->Draw(expression[i],gate, "colz");
     cut[i]->Draw("same");
     

   }


}
