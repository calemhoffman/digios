#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TString.h"

TChain * tree;
TH1F * hElum[200];   
TH1F * hBIC[200];   
TH1F * hTD[200];   
TH1F * hTDAll;

int tRange[2] = {5000, 8000};

void script_targetDegrade(int fileID = 0){

   tree = new TChain("tree");
   if( fileID == 0 ) tree->Add("h073_82Kr_gen_run019-080.root");
   if( fileID == 1 ) tree->Add("h073_82Kr_gen_run081-125.root");
   if( fileID == 2 ) tree->Add("h073_82Kr_gen_run141-158.root ");
   if( fileID == 3 ) tree->Add("h073_82Kr_gen_run164-220.root");
   if( fileID == 4 ) tree->Add("h073_TD_run019-125_141-158_164-220.root");
   
   gStyle->SetOptStat("");
   
   TCanvas * canvas = new TCanvas("can", "c", 1200, 1200);
   canvas->Divide(1,3);

   ///========================= event by event

   int run;
   float elum;
   ULong64_t elum_t;
   ULong64_t tac_t[2];

   tree->SetBranchAddress("run",    &run);
   tree->SetBranchAddress("elum",    &elum);
   tree->SetBranchAddress("elum_t", &elum_t);
   tree->SetBranchAddress("tac_t",   tac_t);

   int totEntry = tree->GetEntries();

   hElum[0] = new TH1F("hElum", "Elum", 14000, 100, 14100);
   hBIC[0]  = new TH1F("hBIC",  "BIC" , 14000, 100, 14100);

   for( int ev = 0; ev < totEntry; ev++){

    tree->GetEntry(ev,0);

    if( run <= 80 ){
      if( 960 < elum && elum < 1100 ) {
        hElum[0]->Fill(elum_t/1e8/60.);
      }
    }else{
      if( 860 < elum && elum < 960 ) {
        hElum[0]->Fill(elum_t/1e8/60.);
      }
    }

    hBIC[0]->Fill(tac_t[1]/1e8/60.);

   }

  hTD[0] = new TH1F("hTD", "TD", 14000, 100, 14100);
  hTD[0]->Divide(hElum[0], hBIC[0]);

  canvas->cd(1); hElum[0]->Draw();
  canvas->cd(2); hBIC[0]->Draw();
  canvas->cd(3); hTD[0]->Draw();

   
   /*
   TString exp, gate;
   
   double yMaxElum = 0;
   double yMaxBIC = 0;
   double yMaxTD = 0;
   
   int start = 81; 
   int end = 119;
   
   if(fileID == 0){ tRange[0] =   100; tRange[1] =  4200; start =  19; end =  80;}
   if(fileID == 1){ tRange[0] =  4800; tRange[1] =  8000; start =  81; end = 119;}
   if(fileID == 2){ tRange[0] =  8600; tRange[1] = 10000; start = 141; end = 158;}
   if(fileID == 3){ tRange[0] = 10000; tRange[1] = 14000; start = 164; end = 220;}
   if(fileID == 4){ tRange[0] =   100; tRange[1] = 14000; start =  19; end = 220;}
   
   for( int runID = start; runID <= end; runID ++){
            
      int id = runID-start;
    
      //=========================== ELUM
      
      hElum[id] = new TH1F(Form("h%2d", runID), Form("ELUM%2d", runID), tRange[1] - tRange[0], tRange[0], tRange[1]);
      hElum[id]->SetLineColor(runID%4 + 1);
      
      exp.Form("elum_t[0]/1e8/60.>>h%2d", runID);
      if( runID <= 80 ) gate.Form("960 < elum[0] && elum[0] < 1100 && run== %d", runID); // 82Kr
      if( runID > 80  ) gate.Form("860 < elum[0] && elum[0] < 960 && run== %d", runID); // 82Se
      

      canvas->cd(1);
      tree->Draw(exp, gate, "colz");
      
      double y = hElum[id]->GetMaximum();
      if( y > yMaxElum ) yMaxElum = y;
      
      canvas->Update();
      gSystem->ProcessEvents();
      
      //=========================== BIC
      
      hBIC[id] =  new TH1F(Form("b%2d", runID), Form("BIC%2d", runID), tRange[1] - tRange[0], tRange[0], tRange[1]);
      hBIC[id]->SetLineColor(runID%4 + 1);
      
      exp.Form("tac_t[1]/1e8/60.>>b%2d", runID);
      gate.Form(" run== %d", runID);
      
      canvas->cd(2);
      tree->Draw(exp, gate, "colz");
      
      y = hBIC[id]->GetMaximum();
      if( y > yMaxBIC ) yMaxBIC = y;
      
      canvas->Update();
      gSystem->ProcessEvents();
      
      
      //========================== Elum/BIC
      
      hTD[id] = new TH1F(Form("t%2d", runID), Form("TD%2d", runID), tRange[1] - tRange[0], tRange[0], tRange[1]);
      hTD[id]->Divide(hElum[id], hBIC[id]);
      hTD[id]->SetLineColor(runID%4 + 1);
      
      canvas->cd(3);
      hTD[id]->Draw();
      
      y = hTD[id]->GetMaximum();
      if( y > yMaxTD ) yMaxTD = y;
      
      canvas->Update();
      gSystem->ProcessEvents();
      
   }
   
   hTDAll = new TH1F("tdALL", "TD; time [min]; count/min", tRange[1] - tRange[0], tRange[0], tRange[1]);
   
   for( int runID = start; runID <= end; runID ++){
      
      int id = runID -start;
      
      //=================== 
      canvas->cd(1);
      hElum[id]->GetYaxis()->SetRangeUser(0, yMaxElum*1.2);
      if( id == 0 ) hElum[id]->Draw();
      hElum[id]->Draw("same");
      
      //=================== 
      canvas->cd(2);
      hBIC[id]->GetYaxis()->SetRangeUser(0, yMaxBIC*1.2);
      if( id == 0 ) hBIC[id]->Draw();
      hBIC[id]->Draw("same");
      
      //=================== 
      canvas->cd(3);
      hTD[id]->GetYaxis()->SetRangeUser(0, yMaxTD*1.2);
      if( id == 0 ) hTD[id]->Draw();
      hTD[id]->Draw("same");
      
      hTDAll->Add(hTD[id]);
      
   }
   
   if( fileID == 0 ) hTDAll->SaveAs("h073_TD_019-080.root");
   if( fileID == 1 ) hTDAll->SaveAs("h073_TD_081-119.root");
   if( fileID == 2 ) hTDAll->SaveAs("h073_TD_141-158.root");
   if( fileID == 3 ) hTDAll->SaveAs("h073_TD_164-220.root");
      
   */
}
