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
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 1000, 800, 600);
   cRead->Divide(1,2);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
   gStyle->SetOptFit(0);
   
/**///==============================================================   
   UShort_t   id[200];
   UInt_t      numHit;
   UShort_t trace[200][1024];
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
   
   for( int ev = 0; ev < totnumEntry; ev++){
      
      if( lastEvFlag ) {         
         ev = oldEv[evPointer];
         lastEvFlag = false;
      }
      tree->GetEntry(ev);
      
      int countJ = 0;
      
      for( int j = 0; j <  numHit ; j++){
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

         
         // cRead->Clear();
         cRead->cd(1);
         g->Draw("Al");
         //g->GetXaxis()->SetRangeUser(0, g->GetN());
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         cRead->cd(2);
         //jaja->Draw();
         specS->Draw("");
      
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
