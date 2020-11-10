#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TString.h"

TChain * tree;
TH2F * hRun[200];
TH1F * hElum[200];   
TH1F * hBG[200];   
TH1F * hBIC[200];   
TH1F * hTD[200];   
TH1F * hTDAll;

int tRange[2] = {5000, 8000};

void script_targetDegrade(){

   tree = new TChain("tree");
   tree->Add("h075_TD.root");
   
   gStyle->SetOptStat("");
   
   TCanvas * canvas = new TCanvas("can", "c", 1800, 800);
   canvas->Divide(1,3);

   ///========================= event by event

   int run;
   float elum;
   int elumID;
   ULong64_t elum_t;
   //ULong64_t tac_t[2];

   tree->SetBranchAddress("runID",     &run);
   tree->SetBranchAddress("elum",    &elum);
   tree->SetBranchAddress("elumID",  &elumID);
   tree->SetBranchAddress("elum_t",  &elum_t);
   //tree->SetBranchAddress("tac_t",   tac_t);

   int totEntry = tree->GetEntries();

   int timeRange[2] = {0, 6000};

   hRun[0]  = new TH2F("hRun",  "Run",  timeRange[1]-timeRange[0], timeRange[0], timeRange[1], 120, 0, 120);
   hElum[0] = new TH1F("hElum", "Elum", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);
   hBG[0]   = new TH1F("hElum", "BG",   timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);
   hBIC[0]  = new TH1F("hBIC",  "BIC" , timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);

   for( int ev = 0; ev < totEntry; ev++){

    tree->GetEntry(ev,0);

    if( elumID != 3 ) continue;

    if ( 734 < elum && elum < 967 ) hElum[0]->Fill(elum_t/1e8/60.);

    if ( 1000 < elum && elum < 1340 ) hBG[0]->Fill(elum_t/1e8/60.);

    /*
    if( run <= 80 ){
      if( 980 < elum && elum < 1080 ) {
        hElum[0]->Fill(elum_t/1e8/60.);
      }
      if( ( 900 < elum && elum < 950 ) || ( 1100 < elum && elum < 1150) ) {
         hBG[0]->Fill(elum_t/1e8/60.);
      }
    }else{
      if( 860 < elum && elum < 970 ) {
        hElum[0]->Fill(elum_t/1e8/60.);
      }
      if( 1000 < elum && elum < 1136.9 ){
         hBG[0]->Fill(elum_t/1e8/60.);
      }
    }
    **/

    hRun[0]->Fill(elum_t/1e8/60., run);
    //hBIC[0]->Fill(tac_t[1]/1e8/60.);

   }

  hTD[0] = new TH1F("hTD", "TD", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);
  hTD[0]->Divide(hElum[0], hBIC[0]);

   TProfile * hRun_pfx = new TProfile("hRun_pfx", "RUNID; time [min]; FCUP mean", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);
   hRun[0]->ProfileX("hRun_pfx");

  canvas->cd(1); hRun[0]->Draw("colz");
  canvas->cd(2); hElum[0]->Draw();
  canvas->cd(3); hBG[0]->Draw();
//  canvas->cd(4); hBIC[0]->Draw();
  //canvas->cd(3); hTD[0]->Draw();

   ///=================== Save to txt file
   int nBin = hElum[0]->GetNbinsX();
   
   FILE * paraOut;
   TString fileName = "TD_20Ne.txt";
   paraOut = fopen (fileName.Data(), "w+");
   printf("=========== save histogram to %s \n", fileName.Data());
   
   for( int i = 1; i <= nBin ; i++){
      
      double x     = hElum[0]->GetBinCenter(i);
      double y1    = hElum[0]->GetBinContent(i);
      double yRun  = hRun_pfx->GetBinContent(i);
      double yBG   = hBG[0]->GetBinContent(i);

      fprintf(paraOut, "%14.8f\t%9.6f\t%3.0f\t%9.6f\n", x, y1, yRun, yBG);

//      double yFCup = hBIC[0]->GetBinContent(i);
//      fprintf(paraOut, "%14.8f\t%9.6f\t%3.0f\t%9.6f\t%9.6f\n", x, y1, yRun, yBG, yFCup);
   }

   fflush(paraOut);
   fclose(paraOut);

   
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
