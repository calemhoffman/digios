#include <TROOT.h>
#include <TChain.h>
#include <TSystem.h>
#include <TFile.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TSelector.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1.h>
#include <TLine.h>
#include <TSpectrum.h>
#include <iostream>

#include "../working/GeneralSortMapping.h"

#include "AutoFit.C"

double WSFunc(double *x , double * par ){
   // par[0] = A
   // par[1] = R
   // par[2] = a
   // par[3] = B

   return par[0]* TMath::Exp( ( x[0] - par[1])/par[2] ) + par[3];
}

void readRawTrace(TString fileName, int minDetID = 0, int maxDetID = 1000){

/**///==============================================================   

   TFile * f1 = new TFile (fileName, "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
   if( tree == NULL ) {
      printf("===== Are you using gen_runXXX.root ? please use runXXX.root\n");
      return;
   }
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 1000, 1000, 800, 300);
   // cRead->Divide(1,2);
   // for( int i = 1; i <= 2 ; i++){
   //    cRead->cd(i)->SetGrid();
   // }
   cRead->SetGrid();
   
   gStyle->SetOptFit(0);
   
/**///==============================================================   
   UShort_t   id[200];
   UInt_t      numHit;
   UShort_t  trace[200][1250]; // need to match the tree
   UShort_t  traceLength[200];
   tree->SetBranchAddress("id", id);
   tree->SetBranchAddress("NumHits", &numHit);
   tree->SetBranchAddress("trace", trace);
   tree->SetBranchAddress("trace_length", traceLength);
   
   TLatex text ;
   text.SetNDC();
   text.SetTextFont(62);
   text.SetTextSize(0.06);
   text.SetTextColor(2);
   
   bool breakFlag = false;  
   bool lastEvFlag = false; 
   std::vector<int> oldEv;
   int evPointer = 0;

   TGraph * g = new TGraph();

   TH1F * jaja = new TH1F();
   jaja->SetName("jaja");
   jaja->SetTitle("jaja");

   //==== fitting
   TF1 * fit = new TF1("fit", "[0]/(1+TMath::Exp((x-[1])/[2])) + [3]", 0, 300);
   fit->SetLineWidth(2);
   fit->SetLineColor(2);
   fit->SetNpx(1000);

   double * para_default = new double[4];
   para_default[0] = -1000;
   para_default[1] = 100;
   para_default[2] = 2;
   para_default[3] = 8500;

   fit->SetParameters(para_default);
   
   for( int ev = 0; ev < totnumEntry; ev++){
      
      if( lastEvFlag ) {         
         ev = oldEv[evPointer];
         lastEvFlag = false;
      }
      tree->GetEntry(ev);
      
      int countJ = 0;
            
      for( int j = 0; j <  numHit ; j++){
         // for( int k = 0; k < traceLength[j] ; k++){
         //    printf("%4d, %5d \n", k, trace[j][k]);
         // }  
      
         Int_t idTemp   = id[j] - idConst;
         int idDet  = idDetMap[idTemp];
         int idKind = idKindMap[idTemp];
         
         if( !(minDetID <=  idDet &&  idDet <= maxDetID ) ) continue;

         if( countJ == 0 )  printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, numHit);

         printf("nHit: %d, id : %d, idKind : %d, trace Length : %u ( enter = next , q = stop, w = last)\n", j, idDet, idKind, traceLength[j]);
         
         g->Clear();
         g->Set(traceLength[j]);  
         double base = 0;
         for( int k = 0; k < traceLength[j] ; k++){
            if( trace[j][k] < 16000){
               base = trace[j][k];
               g->SetPoint(k, k, trace[j][k]);
            }else{
               g->SetPoint(k, k, base);
            }
            //printf("%4d, %5d \n", k, base);
            //if( k % 10 ==0 ) printf("\n");

         }
         
         g->SetTitle(Form("ev: %d, nHit : %d, id : %d, idKind : %d, trace Length : %u\n", ev, j, idDet, idKind, traceLength[j]));
         g->GetXaxis()->SetTitle("tick = 10 ns");

         fit->SetRange(0, traceLength[j]);
         fit->SetParameters(para_default);
         g->Fit(fit, "Rq");

         double * para = fit->GetParameters();

         TString fitResultStr = Form("Amp: %.2f, Time: %.2f tick, Rise time(10%%-90%%): %.3f ticks, baseline : %.2f", 
                           -para[0], para[1], para[2]*4.29, para[3]);

         // printf("%s\n", fitResultStr.Data());

         
         // cRead->Clear();
         //g->GetXaxis()->SetRangeUser(0, g->GetN());
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         
         cRead->cd(1);
         g->Draw("Al");
         text.DrawText(0.11, 0.85, fitResultStr.Data());

         /*
         ///===== using offset-different method 
         int offset = 30;

         jaja->Clear();
         jaja->Reset();
         jaja->SetBins(traceLength[j]-offset, 0, traceLength[j] - offset);

         for( int k = 0; k < g->GetN() - offset; k++){
            double a = g->GetPointY(k+offset);
            double b = g->GetPointY(k);
            jaja->Fill(k, (a -b) + offset);
            jaja->SetBinError(k, 0);
         }

         // //========= moving average
         TH1F * specS = (TH1F*) jaja->Clone();
         specS->Rebin(4);

         {
            TSpectrum * peak = new TSpectrum(10);

            printf("============= estimating background...\n");
            TH1 * h1 = peak->Background(specS, 30);
            //h1->Draw("same");
            printf("============= substracting the linear background...\n");
            //specS->Sumw2();
            specS->Add(h1, -1.);
            
            delete peak;
            peak = new TSpectrum(10);
            int nPeaks = peak->Search(specS, 5, "", 0.2);
            double * xpos = peak->GetPositionX();
            double * ypos = peak->GetPositionY();

            int * inX = new int[nPeaks];
            TMath::Sort(nPeaks, xpos, inX, 0 );
            std::vector<double> energy, height;
            for( int j = 0; j < nPeaks; j++){
               energy.push_back(xpos[inX[j]]);
               height.push_back(ypos[inX[j]]);
            }
            for( int j = 0; j < nPeaks; j++){
               printf(" position : %f , %f \n", energy[j], height[j]);
            }

            delete peak;
         }
         cRead->cd(2);
         specS->Draw("");
         */
         
      
         cRead->Update();              
         gSystem->ProcessEvents();

         char s[80];
         fgets(s, sizeof s, stdin); 

         if( s[0] == 113 ) { // 'q' = 113
            breakFlag = true;
            break;
         }else if( s[0] == 119 ) { // 'w' = 119
            
            if( j > 0 || countJ > 0 ) {
                j = j - 2;
            }
            
            if( (j == 0 && (int)oldEv.size() >= 0 && evPointer > 0 ) || countJ == 0 ) {
               if( evPointer > 0 ) evPointer --;
               if( evPointer == 0 ) printf(" the first event!!! \n");
               lastEvFlag = true;
               //printf(" ev : %d, evPt : %d \n", oldEv[evPointer], evPointer);
               break;
            }
         }
         
         countJ ++;
         
      }
      if( breakFlag ) break;
      
      if( lastEvFlag == false && countJ > 0 ){
         if( oldEv.size() == evPointer ) oldEv.push_back(ev);
         evPointer ++;
      }
      
   }
   
   //gROOT->ProcessLine(".q");

}
