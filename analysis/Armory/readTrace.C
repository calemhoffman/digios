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

void readTrace(TString fileName){
   
/**///==============================================================   

   TFile * f1 = new TFile (fileName, "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 0, 800, 300);
   cRead->Divide(1,1);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
/**///==============================================================   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);
   
   int eventID;
   float e[24], xf[24], xn[24];
   float te[24];
   float rdt[8];
   
   tree->SetBranchAddress("eventID",   &eventID);
   tree->SetBranchAddress("e",         e);
   tree->SetBranchAddress("xf",        xf);
   tree->SetBranchAddress("xn",        xn);
   tree->SetBranchAddress("te",        te);
   tree->SetBranchAddress("rdt",      rdt);

	char s [1] ;
	char b [1] ;
	b[0] = '1';

   TLine timeVLine;

   bool breakFlag = false;
   for( int ev = 0; ev < totnumEntry; ev++){
      arr->Clear();
      tree->GetEntry(ev);
      
      bool nextFlag = true;
      
      for( int i = 0; i < 24; i++){
         if( TMath::IsNaN(e[i]) ) continue;    
         if( e[i] > 0 ){
            nextFlag = false;
         }
         printf("========= ev: %d, #trace: %d | %d, (e, xf, xn , te[i]) = (%7.2f, %7.2f, %7.2f, %7.2f)\n", 
                    eventID, arr->GetEntriesFast(), i, e[i], xf[i], xn[i], te[i]);
        
      }
      for( int i = 0; i < 8; i++){
         if( TMath::IsNaN(rdt[i]) ) continue;    
         if( rdt[i] > 0 ){
            nextFlag = false;
         }
         printf("      rdt: %d , %7.2f\n", i, rdt[i]);
        
      }
      
      if( nextFlag ) continue;
      
      for( int j = 0; j < arr->GetEntriesFast() ; j++){

         TGraph * g  = (TGraph*) arr->At(j);
         
         TF1 * gFit = (TF1 *) g->FindObject("gFit");
         if( gFit != NULL ){ 
            double base, time = 0, riseTime, energy, chiSq;
            energy   = gFit->GetParameter(0);
            time     = gFit->GetParameter(1);
            riseTime = gFit->GetParameter(2);
            base     = gFit->GetParameter(3);
            chiSq     = gFit->GetChisquare()/gFit->GetNDF();
            int kind = gFit->GetLineColor();
            int det  = gFit->GetLineStyle();
            
            //if( det < 100 ) continue;
            if( det != 18 && det != 19 && det !=12 ) continue;
            
            TString gTitle;
            gTitle.Form("(%d,%d), base: %5.1f, rise: %5.3f, time: %5.2f, energy: %6.1f | chi2: %6.2f, %6.2f",
                     det, kind, base, riseTime, time, energy, chiSq, TMath::Sqrt(chiSq)/energy);
            
            printf("%s", gTitle.Data());
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
   
   gROOT->ProcessLine(".q");

/*
   TH2F * hEE = new TH2F("hEE", "jj; e(trace); e(digit)", 500, 0, 8000, 500, 0, 8000);
   for( int ev = 0; ev < totnumEntry; ev++){
      tree->GetEntry(ev);
      for( int i = 0; i < 24; i++){
         if( TMath::IsNaN(te[i]) || TMath::IsNaN(e[i]) ) continue;  
         hEE->Fill(te[i] , e[i]);
      }
   }
   hEE->Draw();
   
*/   
}
