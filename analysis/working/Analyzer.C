#define Analyzer_cxx

#include "Analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TBenchmark.h>
#include <TMath.h>
//#include "../AutoCali/AutoFit.C"

TH1F * hRun; // runID
TH1F * hMultiHit ; 

TH1F * hEBISi[24]; // EBIS for each detector

TH1F * hei[24]; // e
TH2F * hexi[24]; // e vs x for each detector

TH1F * hxi[24]; // x for each detector

TH2F * hez;  // e vs z
TH2F * heza;  // e vs z, |x| < 0.9
TH2F * hezr [4]; // e vs z for each row

TH2F * hxExi[24]; // x vs Ex for each detector

TH1F * hEx; // Ex
TH1F * hExi [24]; // Ex for each detector

TH1F * hExc[6]; // Ex for each column
TH1F * hExcA[6]; // Ex for each column more detectors summed
TH1F * hExca[6]; // Ex gated x<0
TH1F * hExcb[6]; // Ex gated x>0

TH2F * hExzr[4]; // Ex vs z

TH2F * hExT; // Ex vs thetaCM
TH2F * hExTa; // Ex vs thetaCM, |x| < 0.9

TH1F * hExr[4]; // Ex for each row
TH2F * hExTr[4]; // Ex vs thetaCM for each row 

TH1F * hT; // thetaCM
TH1F * hTa; // thetaCM, |x| < 0.9

TCanvas * cAna;
Int_t totnumEntry;
TBenchmark gClock;
Bool_t shown;

void Analyzer::Begin(TTree *tree)
{
   TString option = GetOption();

   totnumEntry = tree->GetEntries();
   hRun = new TH1F("hRun", "runID with gate; runID; count / run", 50, 20, 70);
   hMultiHit = new TH1F("hMultiHit", "MultiHit; hit; count / hit ", 10, 0, 10);
   
   //ex spectrum
   double range[3] = { 30, -0.5, 3.0}; // resol. [keV], low Ex, high Ex
   hEx   = new TH1F("hEx"  ,  Form("Ex; Ex[MeV]; count / %2.0f keV", range[0])          , (range[2]-range[1])/range[0]*1000., range[1], range[2]);

   // thetaCM spectrum
   double rangeCM[3] = {1, 0, 50}; // resol. [deg], low deg, high deg
   hT  = new TH1F("hT",  Form("thetaCM ; thetaCM [deg]; count / %2.0f deg", rangeCM[0])     , (rangeCM[2]-rangeCM[1])/rangeCM[0], rangeCM[1], rangeCM[2]);
   hTa = new TH1F("hTa", Form("thetaCM (|x|<0.9); thetaCM [deg]; count / %2.0f deg", rangeCM[0]), (rangeCM[2]-rangeCM[1])/rangeCM[0], rangeCM[1], rangeCM[2]);
   
   //E vs Z
   hez  = new TH2F("hez" , "e - z; z [mm]; e [MeV]", 450, -450, 0, 400, 0, 10);
   heza = new TH2F("heza", "e - z (|x|<0.9); z [mm]; e [MeV]", 450, -450, 0, 400, 0, 10);
   
   //Ex vs thetaCM
   hExT  = new TH2F("hExT" , "Ex - thetaCM; thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);
   hExTa = new TH2F("hExTa", "Ex - thetaCM (|x|<0.9); thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);

   for( int i = 0; i < 24; i ++){
      hei[i]     = new TH1F(Form("hei%d", i) , Form("e (det-%d); e [MeV]; count/ 10 keV", i), 1000, 0, 10);
      hxi[i]     = new TH1F(Form("hxi%d", i) , Form("x (det-%d); x [a.u.]; count ", i), 400, -1.2, 1.2);
      hEBISi[i]  = new TH1F(Form("hEBISi%d", i) , Form("e_t - EBIS_t (det-%d); time [us]; count/ 1 us", i), 1500, 0, 1500);
      hExi[i]    = new TH1F(Form("hExi%d", i) , Form("Ex (det-%d); Ex[MeV]; count/ %2.0f keV", i,  range[0]), (range[2]-range[1])/range[0]*1000., range[1], range[2]);
      hexi[i]    = new TH2F(Form("hexi%d", i) , Form("e - x (det-%d); x [a.u]; e [MeV]", i), 450, -1.2, 1.2, 400, 0, 10);
      hxExi[i]   = new TH2F(Form("hxExi%d", i), Form("x - Ex (det-%d); Ex [MeV); x [a.u.]", i), 400, -1, 5, 400, -1.2, 1.2);         
   }
   
   for( int i = 0; i < 4; i++){
      hExTr[i] = new TH2F(Form("hExTr%d", i), Form("Ex - thetaCM (row-%d); thetaCM [deg]; Ex [MeV]", i) , 400, 0, 50, 400, -1, 5);
      hExzr[i] = new TH2F(Form("hExzr%d", i), Form("Ex - z (row-%d); z [mm]; Ex [MeV]", i) , 400, -450, 0, 400, -1, 5);
      hezr[i]  = new TH2F(Form("hezr%d", i) , Form("e - z (row-%d); z [mm]; e [MeV]", i) , 400, -450, 0, 400, 0, 13);
      hExr[i]  = new TH1F(Form("hExr%d", i) , Form("Ex (row-%d); e [MeV] ; count / %2.0f keV", i, range[0]) , (range[2]-range[1])/range[0]*1000., range[1], range[2]);
   }
   
   for( int i = 0; i < 6; i++){
      hExc[i] = new TH1F(Form("hExc%d", i), Form("Ex (col-%d); Ex [MeV]; count / %2.0f keV", i, range[0]) , (range[2]-range[1])/range[0]*1000., range[1], range[2]);
      hExcA[i] = new TH1F(Form("hExcA%d", i), Form("Ex (col-%d) All; Ex [MeV]; count / %2.0f keV", i, range[0]) , (range[2]-range[1])/range[0]*1000., range[1], range[2]);
      
      hExca[i]  = new TH1F(Form("hExca%d",i) ,  Form("Ex (x<0, col-%d); Ex[MeV]; count / %2.0f keV",i, range[0]), (range[2]-range[1])/range[0]*1000., range[1], range[2]);
      hExcb[i]  = new TH1F(Form("hExcb%d",i) ,  Form("Ex (x>0, col-%d); Ex[MeV]; count / %2.0f keV",i, range[0]), (range[2]-range[1])/range[0]*1000., range[1], range[2]);
   
   }
   
   
   gClock.Reset();
   gClock.Start("timer");
   shown = 0;
}

void Analyzer::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

// for 0.0 MeV state
double ExOffset[24] = {
     0.072,   //  0
     0.062,   //  1
     0.042,   //  2
     0.030,   //  3
     0.028,   //  4
    -0.024,   //  5
     0.049,   //  6
     0.020,   //  7
    -0.017,   //  8
     0.007,   //  9
    -0.002,   // 10
    -0.000,   // 11
    -0.095,   // 12
     0.001,   // 13
    -0.013,   // 14
    -0.040,   // 15
    -0.072,   // 16
    -0.120,   // 17 // use 1st excited state
    -0.015,   // 18
     0.082,   // 19
    -0.003,   // 20
    -0.005,   // 21
    -0.036,   // 22
     0.055};  // 23

// for 1.2 MeV state
double ExOffset2[24] = { 
     0.072,   //  0 //use ground state
     0.094,   //  1
     0.042,   //  2
     0.030,   //  3
     0.035,   //  4
    -0.035,   //  5
     0.000,   //  6
     0.018,   //  7
     0.021,   //  8
    -0.005,   //  9
    -0.036,   // 10
    -0.000,   // 11
    -0.075,   // 12 // use ground state
    -0.005,   // 13
    -0.014,   // 14
    -0.029,   // 15
    -0.066,   // 16
    -0.120,   // 17
    -0.063,   // 18 
     0.066,   // 19
     0.025,   // 20
    -0.013,   // 21
    -0.006,   // 22
     0.050};  // 23
   
double ExOffset130Xe[24] = {
    0.022,   //  0
    0.030,   //  1
    0.003,   //  2
   -0.042,   //  3
    0.000,   //  4
    0.000,   //  5
   -0.001,   //  6
    0.021,   //  7
   -0.034,   //  8
   -0.101,   //  9
   -0.134,   // 10
    0.000,   // 12
   -0.246,   // 13
   -0.037,   // 13
   -0.058,   // 14
   -0.113,   // 15
    0.000,   // 16
    0.000,   // 17
   -0.052,   // 18
    0.000,   // 19
   -0.095,   // 20
   -0.069,   // 21
   -0.060,   // 22
    0.087};  // 23
   

Bool_t Analyzer::Process(Long64_t entry)
{
   b_run->GetEntry(entry, 0);
   b_e->GetEntry(entry,0);
   //b_xf->GetEntry(entry,0);
   //b_xn->GetEntry(entry,0);
   b_x->GetEntry(entry,0);
   b_z->GetEntry(entry,0);
   b_Ex->GetEntry(entry, 0);
   b_thetaCM->GetEntry(entry, 0);
   b_e_t->GetEntry(entry, 0);
   b_EBIS_t->GetEntry(entry, 0);
   //b_hitID->GetEntry(entry, 0);
   
   hRun->Fill(run);
   
   int multiHit = 0;

   
   for( int i = 0 ; i < 24 ; i++){
      
      if( e_t[i] == 0 || TMath::IsNaN(e[i])) continue;
      if( e[i] < 0.5 ) continue; 
            
      hEBISi[i]->Fill( (e_t[i]-ebis_t)/1e2 );
      
      //======== cut-EBIS
      if( !(100 < (e_t[i]-ebis_t)/1e2 && (e_t[i]-ebis_t)/1e2 < 900 )) continue;
      //if( 11000 < (e_t-EBIS_t)/1e2 && (e_t-EBIS_t)/1e2 < 19000 ) 
      
      //======== cut-e
      if( !(e[i] > 1.2) ) continue;
      
      //======== cut-x
      if( !(TMath::Abs(x[i]) < 0.95) ) continue;
      
      //======== cut-hitID
      //if( hitID[i] != 0 ) continue;
      
      //======== cut-thetaCM
      //if( thetaCM > 10 ) continue;
      
      //======== cut-get
      //if( i == 1 || i == 5 || i == 11 || i == 17) continue;  // for ground state
      
      //======== special gate;
      //if( i == 16 && e[i] < 6 ) continue;
      
      Ex = Ex + ExOffset2[i];
      
      //Ex = Ex + ExOffset130Xe[i];
      
      multiHit ++;
      
      hei[i]->Fill( e[i] );
      
      if( e[i] < 20 ) hxi[i]->Fill( x[i] );
      
      hexi[i]->Fill(x[i], e[i]);
      
      hExi[i]->Fill(Ex);
      hxExi[i]->Fill(Ex, x[i]);
      
      hez->Fill(z[i],e[i]);
      hExT->Fill(thetaCM,Ex);
      hT->Fill(thetaCM);      
      
      heza->Fill(z[i],e[i]);
      hExTa->Fill(thetaCM,Ex);
      hTa->Fill(thetaCM);
      
      int row = -1;
      if ( i < 6 ){
         row = 0;
      }else if ( 6 <= i && i < 12 ) {
         row = 1;
      }else if ( 12 <= i && i < 18 ) {
         row = 2;
      }else if ( 18 <= i && i < 24 ) {
         row = 3;
      }
      hezr[row]->Fill(z[i], e[i]);
      hExTr[row]->Fill(thetaCM, Ex);
      hExzr[row]->Fill(z[i], Ex);
      
      
      if( i%6 == 0 ) hExcA[0]->Fill(Ex);
      if( i%6 == 1 ) hExcA[1]->Fill(Ex);
      if( i%6 == 2 ) hExcA[2]->Fill(Ex);
      if( i%6 == 3 ) hExcA[3]->Fill(Ex);
      if( i%6 == 4 ) hExcA[4]->Fill(Ex);
      if( i%6 == 5 ) hExcA[5]->Fill(Ex);
      
      hExr[row]->Fill(Ex);
      
      //======== special gate;
      if( i == 1 || i == 5 || i == 17 || i == 6 || i == 7 || i == 8 || i == 12 || i == 18 || i == 19) continue;  // for beyound ground state
      
      if( i%6 == 0 ) hExc[0]->Fill(Ex);
      if( i%6 == 1 ) hExc[1]->Fill(Ex);
      if( i%6 == 2 ) hExc[2]->Fill(Ex);
      if( i%6 == 3 ) hExc[3]->Fill(Ex);
      if( i%6 == 4 ) hExc[4]->Fill(Ex);
      if( i%6 == 5 ) hExc[5]->Fill(Ex);
      
      hEx->Fill(Ex);
      
      if( x[i] < 0) {
         if( i%6 == 0 ) hExca[0]->Fill(Ex);
         if( i%6 == 1 ) hExca[1]->Fill(Ex);
         if( i%6 == 2 ) hExca[2]->Fill(Ex);
         if( i%6 == 3 ) hExca[3]->Fill(Ex);
         if( i%6 == 4 ) hExca[4]->Fill(Ex);
         if( i%6 == 5 ) hExca[5]->Fill(Ex);
      }
      
      if( x[i] > 0) {
         if( i%6 == 0 ) hExcb[0]->Fill(Ex);
         if( i%6 == 1 ) hExcb[1]->Fill(Ex);
         if( i%6 == 2 ) hExcb[2]->Fill(Ex);
         if( i%6 == 3 ) hExcb[3]->Fill(Ex);
         if( i%6 == 4 ) hExcb[4]->Fill(Ex);
         if( i%6 == 5 ) hExcb[5]->Fill(Ex);
      }
      
   }
   
   hMultiHit->Fill(multiHit);
   
   //#################################################################### Timer  
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   gClock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%3d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(entry+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Analyzer::SlaveTerminate()
{
}

double findXMax(TH1F ** hist){
  
  double max = 0;
  int maxDetID = -1;
  for( int i = 0; i < 24; i++){
    if( i == 11 ) continue;
    double temp = hist[i]->GetMaximum();
    if( temp > max) {
      max = temp;
      maxDetID = i;
    }
  }
  printf("====== detID %d is max : %f \n", maxDetID, max);
  
  return max;
}

void Analyzer::Terminate()
{
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   printf("%10d[%3d%%]|%3.0f min %5.2f sec | finished.\n", 
               totnumEntry, 
               100,
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.);
  
   gStyle->SetOptStat("neiou");
   
   //=========== load transfer.root
   TFile * ft = new TFile("transfer.root");
   TObjArray * fxList = (TObjArray *) ft->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray *) ft->FindObjectAny("gList");
   
   //=========== load AutoFit.C
   gROOT->ProcessLine(".L ../Armory/AutoFit.C");
  
   //=========== Plot
   cAna = new TCanvas("cAna", "Analyzer", 1200, 800);
   cAna->Divide(6,4);
   if( cAna->GetShowEditor() ) cAna->ToggleEditor(); 
   
   //double max = findXMax(hei);
   double max = findXMax(hExi);
   
   //hEx->Draw();
   
   for( int i = 0; i < 24; i++){
     cAna->cd(i+1);
     cAna->cd(i+1)->SetGrid();
     //cAna->cd(i+1)->SetLogy();
     //hei[i]->GetYaxis()->SetRangeUser(0.5, max * 1.1);
     //hei[i]->Draw();
     
     //hxi[i]->Draw();
     
     //hEBISi[i]->Draw();
     //hExi[i]->GetYaxis()->SetRangeUser(0, max*1.2);
     //hExi[i]->Draw();
   
     //fit2GaussP1(hExi[i], 0.0, 0.05, 1.2, 0.05, -1, 1.5, 0);
   
     //hExPi[i]->SetLineColor(2);
     //hExPi[i]->Draw("same");
   
     //2-D 
     //hexi[i]->Draw(); 
     hxExi[i]->SetMarkerStyle(7);
     hxExi[i]->SetMarkerColor(4);
     hxExi[i]->Draw("scat");
   } 
   
   //for( int i = 0; i < 24; i++){
   //   //hei[i]->GetYaxis()->SetRangeUser(0, max);
   //   hExi[i]->GetYaxis()->SetRangeUser(0, max);
   //   if( i < 6 ) {
   //      cAna->cd(i+1);
   //      hExi[i]->Draw();
   //   }
   //   if( 6 <= i && i < 12) {
   //      cAna->cd(i-6+1);
   //      hExi[i]->SetLineColor(2); 
   //      hExi[i]->Draw("same");
   //   }
   //   if(12 <= i && i < 18) {
   //      cAna->cd(i-12+1);
   //      hExi[i]->SetLineColor(kGreen+3); 
   //      hExi[i]->Draw("same");
   //   }
   //   if(18 <= i && i < 24) {
   //      cAna->cd(i-18+1);
   //      hExi[i]->SetLineColor(6); 
   //      hExi[i]->Draw("same");
   //   }
   //} 
   
   //for( int i = 0; i < 4; i++){
   //   cAna->cd(i+1);
   //   //cAna->cd(i+1)->SetGrid();
   //   //hezr[i]->Draw("colz");
   //   //fxList->At(0)->Draw("same");
   //   //fxList->At(2)->Draw("same");
   //   //fxList->At(5)->Draw("same");
   //   //fxList->At(7)->Draw("same");
   //   //gList->At(0)->Draw("same");
   //   //gList->At(10)->Draw("same");
   //   //hezr[i]->Draw("colz same");
   //   
   //   hExTr[i]->Draw("colz");
   //   //hExzr[i]->Draw("colz");
   //   //hExPzr[i]->Draw("colz");
   //}
   
   //for( int i = 0; i < 6; i++){
   //   cAna->cd(i+1);
   //   cAna->cd(i+1)->SetGrid();
   //   hExc[i]->Draw();
   //}
   
   
}
