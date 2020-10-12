#define MonGamma_cxx
// The class definition in MonGamma.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

#include "MonGamma.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <TH1.h>

//============= User setting
const int numDet = 4;
int detID[numDet] = {0,1,2,6}; 
TString detName[numDet] = {"HPGe-1","HPGe-2","HPGe-3", "NaI"} ;

double resol = 1; // keV
int eRange1[2] = { 40, 2000 }; // keV
int eRange2[2] = { 1300, 2000 }; // keV
int eRange3[2] = { 20, 200 }; // keV

int tRange[2] = {-1, 600}; //min, relative to start time


double cali[numDet][2]= { {  -0.70173408, 0.62460567},
                          {  -3.26102072, 0.64988671},
                          {  -1.86186218, 0.67175540},
                          { -15.31935350, 0.69013302}};
                          
//===========================

TH1F ** hgamma1 = new TH1F* [numDet];
TH1F ** hgamma2 = new TH1F* [numDet];
TH1F ** hgamma3 = new TH1F* [numDet];

TH2F * hTime;



ULong64_t ProcessedEntries = 0;
ULong64_t NumEntries = 0;
TStopwatch StpWatch;
Float_t Frac = 0.1; //Progress bar
TString canvasTitle ="";
int lastRunID = -1;
bool contFlag = false;

ULong64_t initTime = 0;


double FindMaxY(TH1F ** histList, int n){
   
   double yMax = 0;
   for( int i = 0 ; i < n ; i++){
      double y = histList[i]->GetMaximum();
      
      if( y > yMax ) yMax = y;
      
   }
   
   return yMax * 1.1;
}



void MonGamma::Begin(TTree * tree)
{
   TString option = GetOption();

   NumEntries = tree->GetEntries();   

   for( int i = 0; i < numDet; i ++){
      hgamma1[i] = new TH1F(Form("h1g%d", i), Form("%s; energy [keV]; count/ %.2f keV",detName[i].Data(), resol), (eRange1[1]-eRange1[0])/resol, eRange1[0], eRange1[1]);
      hgamma2[i] = new TH1F(Form("h2g%d", i), Form("%s; energy [keV]; count/ %.2f keV",detName[i].Data(), resol), (eRange2[1]-eRange2[0])/resol, eRange2[0], eRange2[1]);
      hgamma3[i] = new TH1F(Form("h3g%d", i), Form("%s; energy [keV]; count/ %.2f keV",detName[i].Data(), resol), (eRange3[1]-eRange3[0])/resol, eRange3[0], eRange3[1]);
   }
   
   hTime = new TH2F("hTime", "hTime", (tRange[1] - tRange[0]), tRange[0], tRange[1], eRange2[1]-eRange2[0], eRange2[0], eRange2[1]);

   printf("================================ start\n");
}

void MonGamma::SlaveBegin(TTree * /*tree*/)
{
   
   TString option = GetOption();

}

Bool_t MonGamma::Process(Long64_t entry)
{
   ProcessedEntries ++;
   
   ///=========== Progress bar
   if (ProcessedEntries>NumEntries*Frac-1) {
      TString msg; msg.Form("%llu", NumEntries/1000);
      int len = msg.Sizeof();
      printf(" %3.0f%% (%*llu/%llu k) processed in %6.1f sec | expect %6.1f sec\n",
               Frac*100, len, ProcessedEntries/1000,NumEntries/1000,StpWatch.RealTime(), StpWatch.RealTime()/Frac);
      StpWatch.Start(kFALSE);
      Frac+=0.1;
   }
   
   b_runID->GetEntry(entry);
   b_RDT->GetEntry(entry);
   b_RDTTimestamp->GetEntry(entry);
   
   ///========== canvas title
   if( entry == 0 ) {
       if( runID == lastRunID + 1 ) {
          int len = canvasTitle.Sizeof();
          if( contFlag == false) {
             canvasTitle.Remove(len-3);
             canvasTitle += " - ";
          }
          if( contFlag == true){
             canvasTitle.Remove(len-6);
          }
          contFlag = true;
       }
       if( runID > lastRunID + 1 ) contFlag = false;
       canvasTitle += Form("%03d, ", runID );
       lastRunID = runID;
    }
   
   if( initTime == 0 ){
      if( rdt[detID[0]] > 0 ) initTime = rdt_t[detID[0]];
   }
   
   
   ///============== RDT
   double ex[numDet]; 
   for( int i = 0 ; i < numDet; i++){
      int id = detID[i];
      
      ex[i] = rdt[id]*cali[i][1] + cali[i][0];
      
      //if( runID <= 357 ) hgamma1[i]->Fill(ex[i]);
      //if( runID >= 358 ) hgamma2[i]->Fill(ex[i]);
      hgamma1[i]->Fill(ex[i]);
      hgamma2[i]->Fill(ex[i]);
      hgamma3[i]->Fill(ex[i]);
      
      
      //if( id == 0 ) hTime->Fill((rdt_t[0]-initTime)/1e8/60., ex[i]);
   }

   
   return kTRUE;
}

void MonGamma::SlaveTerminate()
{

}

void MonGamma::Terminate()
{
   TCanvas * cCanvas  = new TCanvas("cCanvas","RUN : " + canvasTitle , 1600 , 1200 );
   cCanvas->Modified(); cCanvas->Update();
   
   cCanvas->Divide(4,3);
   
   double yMax = FindMaxY(hgamma1, numDet);
   for( int i = 0 ; i < numDet; i++){
      cCanvas->cd(i+1);
      hgamma1[i]->GetYaxis()->SetRangeUser(0, yMax);
      hgamma1[i]->Draw();
   }
   
   yMax = FindMaxY(hgamma2, numDet);
   for( int i = 0 ; i < numDet; i++){
      cCanvas->cd(i+5);
      //cCanvas->cd(i+5)->SetLogy();
      hgamma2[i]->GetYaxis()->SetRangeUser(0, yMax);
      hgamma2[i]->Draw("hist");
   }
   
   
   //============ Normalized 
   
   double mean = 356;
   double sigma = 3;
   
   //for( int i = 0; i < numDet; i++){
   //   int yMin1 = hgamma1[i]->GetBinCenter(mean - 3*sigma);
   //   int yMax1 = hgamma1[i]->GetBinCenter(mean + 3*sigma);
   //   double count1 = hgamma1[i]->Integral(yMin1, yMax1);
   //   
   //   int yMin2 = hgamma2[i]->GetBinCenter(mean - 3*sigma);
   //   int yMax2 = hgamma2[i]->GetBinCenter(mean + 3*sigma);
   //   double count2 = hgamma2[i]->Integral(yMin2, yMax2);
   //   
   //   printf(" count: %f, %f \n", count1, count2);
   //   
   //   //scale hgamma2
   //   hgamma2[i]->Scale( count1/count2); 
   //   hgamma2[i]->SetLineColor(2);
   //}
   
   
   for( int i = 0 ; i < numDet; i++){
      cCanvas->cd(i+9);
      
      hgamma3[i]->Draw();
      //hgamma1[i]->Draw("same");
      //hgamma2[i]->Draw("same hist");
   }
   
   //TCanvas * cAux  = new TCanvas("cAux", "RUN : " + canvasTitle , 400 , 400 );
   //
   //hTime->Draw("colz");
   

}
