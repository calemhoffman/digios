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

   int maxEvents = 1e6;
   int detID = 0;
   TH1F * hrdta[8];
   TH1F * hrdtb[8];
   TH1F * hbase[8];
   TH1F * hmax[8];
   TH1F * hmaxID[8];
   TH1F * hslope[8];
   TH1F * hslopeID[8];
   TH1F * hrise[8];

   TH2F * hdeVe[4];
   TH2F * hslopeVe[4];
   TH2F * hriseVe[4];
   TH2F * heVslope[8];
   TH2F * heVslopeID[8];
   TH2F * heVmaxID[8];
   double rdtCorr[8] = {1.0,1.4,1.0,1.22,1.0,1.2,1.0,1.2};//match DE to E

   for (int i=0;i<8;i++ ) {
      hrdta[i] = new TH1F(Form("hrdt%da",i),Form("hrdt%da ; Eres [arb]",i),300,1000,4000);
      hrdtb[i] = new TH1F(Form("hrdt%db",i),Form("hrdt%db ; Eres [arb]",i),300,1000,4000);
      hmax[i] = new TH1F(Form("hmax%d",i),Form("hmax%d ; hmax [arb]",i),400,8000,16000);
      hmaxID[i] = new TH1F(Form("hmaxID%d",i),Form("hmaxID%d ; hmaxID [arb]",i),400,0,400);
      hbase[i] = new TH1F(Form("hbase%d",i),Form("hbase%d ; Base [arb]",i),400,7000,9000);
      hslope[i] = new TH1F(Form("hslope%d",i),Form("hslope%d ; Eres [arb]",i),1000,0,200);
      hrise[i] = new TH1F(Form("hrise%d",i),Form("hrise%d ; rise [arb]",i),500,0,250);
      hslopeID[i] = new TH1F(Form("hslopeID%d",i),Form("hslopeID%d ; hslopeID [arb]",i),400,0,400);
      heVslope[i] = new TH2F(Form("heVslope%d",i),Form("heVslope%d ; Eres [arb]; Slope [arb]",i),1000,0,4500,200,0,200);
      hriseVe[i] = new TH2F(Form("hriseVe%d",i),Form("hriseVe%d ; Eres [arb]; rise [arb]",i),2000,100,6500,500,0,250);
      heVslopeID[i] = new TH2F(Form("heVslopeID%d",i),Form("heVslopeID%d ; Eres [arb]; SlopeID [arb]",i),1000,0,4500,200,0,200);
      if (i<4) {
         hdeVe[i] = new TH2F(Form("hdeVe%d",2*i),Form("hdeVe%d",2*i),500,2000,6000,500,0,2000);
         hslopeVe[i] = new TH2F(Form("hslopeVe%d",2*i),Form("hslopeVe%d",2*i),500,2000,6000,200,0,200);
      }
   }   

   for( int ev = 0; ev < maxEvents; /*totnumEntry;*/ ev++){
      tree->GetEntry(ev);
      int nTrace = arr->GetEntriesFast();
      int countJ = 0;
      Double_t data[8][20];// id, [0 - 10 energies, 11 - baseVal, 12-baseID, 13 - maxVal, 14 - maxID, 15 - slope, 
      // 16 - slopeID, 17 - rise] for each reaciton
      for (int ii=0;ii<8;ii++) {
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
         int pos = gTitle.Index("id=");
         gTitle.Remove(0, pos+3);
         gTitle.Remove(3);
         detID = gTitle.Atoi();
         int id = detID-100;
         if( !(minDetID <=  detID &&  detID <= maxDetID ) ) continue;
        //manipulate the trace
        //get baseline, max ? 10 - 90 for slope / rise time / 
         Double_t metha[8][2];
         Double_t methb[8][2];
         Double_t maxValue=0; Int_t maxID=0;
         Int_t baseID=100; // % use LED value for now
         Double_t slopeMax=0; Int_t slopeID=0;
         for (int j=0;j<300;j++) { //full loop so add ifs 
            if (j<65) {
               methb[id][0]+=tempY[j];metha[id][0]+=tempY[j];metha[id][1]+=tempY[j+205];
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
         //slopeMax = (tempY[maxID] - tempY[100])/((float)maxID-100.);
         data[id][15] = 220. - slopeMax; data[id][16] = slopeID;
         //printf("metha1, metha2: %f %f\n",metha[id][1],metha[id][0]);

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

        // fill the histograms
        if (detID >= 100 && detID <=107) {
         hrdta[id]->Fill(metha[id][1] - metha[id][0]);hrdtb[id]->Fill(methb[id][1] - methb[id][0]);
         hbase[id]->Fill(data[id][11]); hmax[id]->Fill(data[id][13]);hmaxID[id]->Fill(data[id][14]);
         hslope[id]->Fill(data[id][15]); hslopeID[id]->Fill(data[id][16]);
         heVslope[id]->Fill(metha[id][1]-metha[id][0],data[id][15]);
         heVslopeID[id]->Fill(metha[id][1]-metha[id][0],data[id][16]);
         hrise[id]->Fill(data[id][17]);
        }
         countJ ++;   
      }
      for (int mm = 0 ; mm < 4; mm++) {
         if (data[2*mm][0] > 100 /*&& data[2*mm+1][0]> 100*/){
            hdeVe[mm]->Fill(data[2*mm][0]+1.25*data[2*mm+1][0],data[2*mm+1][0]);
            hslopeVe[mm]->Fill(data[2*mm][0]+1.25*data[2*mm+1][0],(data[2*mm][15]));
            hriseVe[mm]->Fill(data[2*mm][0],data[2*mm][17]); 
         }
      }
      if (ev%10000 == 0) {
          if (nTrace == 1) {printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d | detID : %d\n", ev, evPointer, nTrace, detID);}
       else {
       printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);
          }
      }
   }
   //gROOT->ProcessLine(".q");
     
   
    TCanvas * cRead = new TCanvas("cRead", "Read Trace", 1000, 2500, 1200, 1000);
   cRead->Clear(); cRead->Divide(2,2);
   for (int i=0;i<4;i++) { 
      cRead->cd(i+1);
      // hrdta[i*2]->Draw();
      // hrdtb[i*2]->SetLineColor(kBlue); hrdtb[2*i]->Draw("");
      // hslope[i*2]->Draw();
      hdeVe[i]->Draw("colz");
      // hrise[i*2]->Draw();
   }
    TCanvas * cRead2 = new TCanvas("cRead2", "Read Trace2", 1000, 2500, 1200, 1000);
   cRead2->Clear(); cRead2->Divide(2,2);
   for (int i=0;i<4;i++) { 
      cRead2->cd(i+1);
      // hrdta[i*2]->Draw();
      // hrdtb[i*2]->SetLineColor(kBlue); hrdtb[2*i]->Draw("");
      // hslope[i*2]->Draw();
      // heVslopeID[i*2]->Draw("colz");
      // hslopeVe[i]->Draw("colz");
      hriseVe[i]->Draw("colz");
   }
}
