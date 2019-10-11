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

const int nDet = 8 ;

void readTraceRDT(TString fileName){
   
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
   
   int eventID;
   float rdt[30];
   ULong64_t rdt_t[30];
   float trdt[8];
   float trdt_t[8];
   float trdt_r[8];
   
   tree->SetBranchAddress("eventID",   &eventID);
   tree->SetBranchAddress("rdt",       rdt);
   tree->SetBranchAddress("rdt_t",     rdt_t);
   tree->SetBranchAddress("trdt",      trdt);
   tree->SetBranchAddress("trdt_t",    trdt_t);
   tree->SetBranchAddress("trdt_r",    trdt_r);
	char s [1] ;
	char b [1] ;
	b[0] = '1';

   TLine timeVLine;

   bool breakFlag = false;
   for( int ev = 0; ev < totnumEntry; ev++){
     //arr->Clear();
      tree->GetEntry(ev);
      
      bool nextFlag = true;
      
      for( int i = 0; i < nDet; i++){ 
        if( TMath::IsNaN(rdt[i]) ) continue;    
        printf("==== ev: %d, #trace: %d | %d, (rdt, trdt) = (%7.2f, %7.2f)\n",  ev, arr->GetEntriesFast(), i, rdt[i], trdt[i]);
        //if( rdt[i] > 2000 ) nextFlag = false;
        nextFlag = false;
      }
      
      if( nextFlag ) continue;
      
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
            chiSq     = gFit->GetChisquare()/gFit->GetNDF();
            int kind = gFit->GetLineColor();
            int det  = gFit->GetLineStyle();
                        
            gTitle.Form("(%d,%d), base: %5.1f, rise: %5.3f, time: %5.2f, energy: %6.1f | chi2: %6.2f, %6.2f |(1 for break)",
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
   
   //gROOT->ProcessLine(".q");

}
