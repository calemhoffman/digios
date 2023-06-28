#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>
#include <TLine.h>
#include <iostream>

void manipTrace(TString fileName, int minDetID = 0, int maxDetID = 1000, bool isGoodOnly = false){

/**///==============================================================   

   TFile * f1 = new TFile (fileName, "read");
   TTree * tree = (TTree *) f1->Get("gen_tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
//    for( int i = 1; i <= 2 ; i++){
//       cRead->cd(i)->SetGrid();
//    }
//    cRead->SetGrid();
   
   gStyle->SetOptFit(0);
   
/**///==============================================================   
   TClonesArray * arr = new TClonesArray("TGraph");
   //tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   //tree->GetBranch("trace")->SetAutoDelete(kTRUE);
   tree->SetBranchAddress("trace", &arr);
      
   TLatex text ;
   text.SetNDC();
   text.SetTextFont(62);
   text.SetTextSize(0.06);
   text.SetTextColor(2);
   
   bool breakFlag = false;  
   bool lastEvFlag = false; 
   vector<int> oldEv;
   int evPointer = 0;

   int maxEvents = 1e5;
   int detID = 0;
   TH1F * htime[50];

   TH2F * heVtime[50];
   TH2F * heVx[50]; //array
   TH2F * hxfxn[50];

   double rdtCorr[8] = {1.0,1.4,1.0,1.22,1.0,1.2,1.0,1.2};//match DE to E

   for (int i=0;i<50;i++ ) {
      htime[i] = new TH1F(Form("htime%d",i),Form("htime%d ; time [arb]",i),450,190,205);
      hxfxn[i] = new TH2F(Form("hxfxn%d",i),Form("hxfxn%d",i),1000,0,8000,1000,0,8000);
   }   

   for( int ev = 0; ev < maxEvents; /*totnumEntry;*/ ev++){
      tree->GetEntry(ev);
      int nTrace = arr->GetEntriesFast();
      int countJ = 0;
      Double_t data[50][20];// id, e, e, [e3,xf0,xn1,r4], slope, rise, ... id=0-3, det 21, id=4 - 7 det 22
      for (int ii=0;ii<50;ii++) {
         for (int jj=0;jj<20;jj++) {
            data[ii][jj]=TMath::QuietNaN();
         }
      }
      for( int j = 0; j <  nTrace ; j++){
         TGraph * g  = (TGraph*) arr->At(j);
         //pulling XY data //CRH
         Double_t * tempX;
         Double_t * tempY;
         tempX = g->GetX(); tempY = g->GetY();
         // for (int ii=0;ii<300;ii++){
         //    printf("X:Y %f:%f\n",tempX[ii],tempY[ii]);
         // }     
         TString gTitle;
         gTitle = g->GetTitle();
         detID = 0;
         int id=-10;
         int pos = gTitle.Index("id=");
         gTitle.Remove(0, pos+3);
         gTitle.Remove(3);
         detID = gTitle.Atoi();
         if (detID>=100 && detID<=107) {
            id = detID-90; //recoils from 10 - 17
         }
         if (detID>=21 && detID<=22) {
            id = (detID-21)*4;
         }
         TF1 * gFit = (TF1 *) g->FindObject("gFit");
         TString kTitle; 
         int kind=0;
         if( gFit != NULL ){ 
            double base, time = 0, riseTime, energy, chiSq;
            energy   = gFit->GetParameter(0);
            time     = gFit->GetParameter(1);
            riseTime = gFit->GetParameter(2);
            base     = gFit->GetParameter(3);
            chiSq    = gFit->GetChisquare()/gFit->GetNDF();
            kind = gFit->GetLineColor();
            int det  = gFit->GetLineStyle();
         }
         id = id + kind - 1;
         if( id < 0 ) continue; //only det 21, 22, recoils
        //manipulate the trace
        //get baseline, max ? 10 - 90 for slope / rise time / 
         Double_t metha[8][2];
         Double_t methb[8][2];
         Double_t maxValue=0; Int_t maxID=0;
         Int_t baseID=100; // % use LED value for now
         Double_t slopeMax=0; Int_t slopeID=0;
         Double_t tmpA=0.;
         Double_t tmpB=0.;
         Double_t postSlope=0.;
         for (int j=0;j<300;j++) { //full loop so add ifs 
            if (j<5) {
               tmpA += tempY[220+j]/5.;
               tmpB += tempY[290+j]/5.;
            }
            if (j<65) {
               methb[id][0]+=tempY[j];metha[id][0]+=tempY[j];metha[id][1]+=tempY[j+225];
               }
            if (j<95) {methb[id][1]+=tempY[j+155]; }
            if (j<200) {
               if (tempY[j+65] > maxValue) {maxValue = tempY[j+65]; maxID = j+65;}
               Double_t slope = tempY[j+66] - tempY[j+65];
               if (slope > slopeMax) {slopeMax = slope; slopeID = j+66;}
            }
         } // end of full loop
         metha[id][0] = metha[id][0]/65.;metha[id][1] = metha[id][1]/65.;methb[id][0] = methb[id][0]/65.;
         methb[id][1] = methb[id][1]/95.;
         data[id][0] = methb[id][1] - methb[id][0];
         data[id][1] = metha[id][1] - metha[id][0];
         data[id][2] = maxValue - metha[id][0];
         data[id][11] = metha[id][0]; data[id][12] = baseID;
         data[id][13] = maxValue;data[id][14] = maxID;
         data[id][15] = 220. - slopeMax; data[id][16] = slopeID;
         postSlope = (tmpB - tmpA)/70.;
         data[id][18] = postSlope;

         Double_t range=(maxValue - data[id][11]); //could also just use the energy! range=data[id][0];
         Double_t tenPercent=0.0;
         Double_t nintyPercent=0.0;
         //second loop for risetime / slop from % values
         //could also norm all the traces to view them
         for (int j=0;j<300;j++) { //second full loop so add ifs 
            if ((tempY[j] > 0.1*range+data[id][11]) && tenPercent==0.0) {
               Double_t tempSlope = (tempY[j] - tempY[j+1]);
               tenPercent = (Double_t)j - ( tempY[j]- (range*0.1+data[id][11])) / tempSlope;
            }
            if ((tempY[j] > 0.9*range+data[id][11]) && nintyPercent==0.0) {
               Double_t tempSlope = (tempY[j] - tempY[j-1]); //pos or negative ?????
               nintyPercent = (Double_t)j - 1. - (tempY[j-1]- (range*0.9+data[id][11])) / tempSlope;
            }
         }//
         data[id][17] = 200.0 - (nintyPercent - tenPercent);
         htime[id]->Fill(data[id][17]);
         //fill histos inside the trace loop
         if (data[0][0]>100 && data[1][0]>100) {
            hxfxn[0]->Fill(data[0][0],data[1][0]);
         }
         if (data[4][0]>100 && data[5][0]>100) {
            hxfxn[1]->Fill(data[4][0],data[5][0]);
         }
         countJ ++;   
      }
      
      if (ev%100000 == 0) {
          if (nTrace == 1) {printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d | detID : %d\n", ev, evPointer, nTrace, detID);}
       else {
       printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);
          }
      }
   }//event loop
   //gROOT->ProcessLine(".q");
     
   
    TCanvas * cRead = new TCanvas("cRead", "Read Trace", 1000, 2500, 1200, 1000);
   cRead->Clear(); cRead->Divide(2,2);
   for (int i=0;i<4;i++) { 
      cRead->cd(i+1);
      // hxfxn[i]->Draw("colz");
      htime[i*4+3]->Draw();
   }
   //  TCanvas * cRead2 = new TCanvas("cRead2", "Read Trace2", 1000, 2500, 1200, 1000);
   // cRead2->Clear(); cRead2->Divide(2,2);
   // for (int i=0;i<4;i++) { 
   //    cRead2->cd(i+1);

   // }
}
