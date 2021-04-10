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

#include "../working/GeneralSortMapping.h"

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
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 1500, 800, 300);
   cRead->Divide(1,1);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
   gStyle->SetOptFit(0);
   
/**///==============================================================   
   Short_t   id[200];
   Int_t      numHit;
   Short_t trace[200][1024];
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
   vector<int> oldEv;
   int evPointer = 0;

   TGraph * g = new TGraph();
   
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
            //printf("%4d, %5d |", k, trace[j][k]);
            //if( k % 10 ==0 ) printf("\n");
         }         
         
         g->SetTitle(Form("ev: %d, nHit : %d, id : %d, idKind : %d, trace Length : %u\n", ev, j, idDet, idKind, traceLength[j]));
         
         cRead->cd(1);
         cRead->Clear();
         g->Draw("Al");
         //g->GetXaxis()->SetRangeUser(0, g->GetN());
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         
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
