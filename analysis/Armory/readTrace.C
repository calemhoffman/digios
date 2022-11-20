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

void readTrace(TString fileName, int minDetID = 0, int maxDetID = 1000, bool isGoodOnly = false){

/**///==============================================================   

   TFile * f1 = new TFile (fileName, "read");
   TTree * tree = (TTree *) f1->Get("gen_tree");
   
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
   TClonesArray * arr = new TClonesArray("TGraph");
   //tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   //tree->GetBranch("trace")->SetAutoDelete(kTRUE);
   tree->SetBranchAddress("trace", &arr);
   
   //TODO UP-Down arrow for pervious-next control
   TLine timeVLine;
   
   TLatex text ;
   text.SetNDC();
   text.SetTextFont(62);
   text.SetTextSize(0.06);
   text.SetTextColor(2);
   
   bool breakFlag = false;  
   bool lastEvFlag = false; 
   vector<int> oldEv;
   int evPointer = 0;
   
   for( int ev = 0; ev < totnumEntry; ev++){
      
      if( lastEvFlag ) {         
         ev = oldEv[evPointer];
         lastEvFlag = false;
      }
      tree->GetEntry(ev);
      
      int nTrace = arr->GetEntriesFast();

      int countJ = 0;
      
      for( int j = 0; j <  nTrace ; j++){

         TGraph * g  = (TGraph*) arr->At(j);
         
         TString gTitle;
         gTitle = g->GetTitle();
         ///printf("TGraph Title : %s\n", gTitle.Data());
         
         int detID = 0;
         int pos = gTitle.Index("id=");
         gTitle.Remove(0, pos+3);
         gTitle.Remove(3);
         detID = gTitle.Atoi();
         
         if( !(minDetID <=  detID &&  detID <= maxDetID ) ) continue;

         if( countJ == 0 )  printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);

         
         TF1 * gFit = (TF1 *) g->FindObject("gFit");
         
         TString kTitle; 
         
         if( gFit != NULL ){ 
            double base, time = 0, riseTime, energy, chiSq, decay, pre_rise_slope;
            energy   = gFit->GetParameter(0);
            time     = gFit->GetParameter(1);
            riseTime = gFit->GetParameter(2);
            base     = gFit->GetParameter(3);
            decay    = gFit->GetParameter(4);
            pre_rise_slope = gFit->GetParameter(5);
            chiSq    = gFit->GetChisquare()/gFit->GetNDF();
            int kind = gFit->GetLineColor();
            int det  = gFit->GetLineStyle();
            
            ///if( !(minDetID <= det && det <= maxDetID ) ) continue;
            
            if( isGoodOnly){
              if( abs(energy) < 1.5* g->GetRMS(2) ) continue;
              if( time > gFit->GetXmax() || time < gFit->GetXmin()) continue;
              if( time > 200 || time < 20) continue;
              if( riseTime > gFit->GetXmax()/7. || riseTime < 0 ) continue;
            }
            
            //if( energy < 400 ) continue;
                        
            kTitle = Form("(%3d,%d), base: %8.1f, rise: %6.2f, time: %6.1f, energy: %8.1f | chi2: %8.1f, RMS: %6.1f\n    decay: %6.2f, pre_rise_slope: %6.2f",
                     det, kind, base, riseTime, time, energy, chiSq, g->GetRMS(2), decay, pre_rise_slope);
            
            printf("%s |(q = break, w = last one)", kTitle.Data());

           
            timeVLine.SetX1(time);
            timeVLine.SetX2(time);
            timeVLine.SetY1(-1000);
            timeVLine.SetY2(20000);
            timeVLine.SetLineColor(4);
         }
         
         cRead->cd(1);
         //cRead->Clear();
         g->Draw("AC");
         //g->GetXaxis()->SetRangeUser(0, g->GetN());
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         timeVLine.Draw("same");
         if( gFit != NULL ) text.DrawText(0.11, 0.85, kTitle.Data());
         
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
