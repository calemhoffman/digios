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

void readTrace(TString fileName,bool isGoodOnly = false, int minDetID = 0, int maxDetID = 1000){
   
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
   
/**///==============================================================   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);
   
	char s [1] ;
	char b [1] ;
	b[0] = 'q';

   TLine timeVLine;

   bool breakFlag = false;
   for( int ev = 0; ev < totnumEntry; ev++){
      tree->GetEntry(ev);
      
      for( int j = 0; j < arr->GetEntriesFast() ; j++){

         TGraph * g  = (TGraph*) arr->At(j);
         
         TString gTitle;
         gTitle = g->GetName();
         g->SetTitle(gTitle);
         
         TF1 * gFit = (TF1 *) g->FindObject("gFit");
         if( gFit != NULL ){ 
            double base, time = 0, riseTime, energy, chiSq;
            energy   = gFit->GetParameter(0);
            time     = gFit->GetParameter(1);
            riseTime = gFit->GetParameter(2);
            base     = gFit->GetParameter(3);
            chiSq    = gFit->GetChisquare()/gFit->GetNDF();
            int kind = gFit->GetLineColor();
            int det  = gFit->GetLineStyle();
            
            if( !(minDetID <= det && det <= maxDetID ) ) continue;
            
            if( isGoodOnly){
              if( abs(energy) < 1.5* g->GetRMS(2) ) continue;
              if( time > gFit->GetXmax() || time < gFit->GetXmin()) continue;
              if( time > 200 || time < 20) continue;
              if( riseTime > gFit->GetXmax()/7. || riseTime < 0 ) continue;
            }
            
            if( energy < 400 ) continue;
                        
            gTitle.Form("(%3d,%d), base: %8.1f, rise: %6.2f, time: %6.1f, energy: %8.1f | chi2: %8.1f, RMS: %6.1f",
                     det, kind, base, riseTime, time, energy, chiSq, g->GetRMS(2));
            
            printf("%s |(%s for break)", gTitle.Data(), b);
            g->SetTitle(gTitle);
           
           
            timeVLine.SetX1(time);
            timeVLine.SetX2(time);
            timeVLine.SetY1(-1000);
            timeVLine.SetY2(20000);
            timeVLine.SetLineColor(4);
         }
         
         cRead->cd(1);
         cRead->Clear();
         g->Draw("");
         //g->GetXaxis()->SetRangeUser(0, 200);
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         timeVLine.Draw("same");
         cRead->Update();
         
         gSystem->ProcessEvents();
         
         gets(s);    
         if( s[0] == b[0] ) {
            breakFlag = true;
            break;
         }
      }
      if( breakFlag ) break;
      
   }
   
   //gROOT->ProcessLine(".q");

}
