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
   
//    TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 1500, 800, 600);
//    cRead->Divide(1,2);
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

   int maxEvents = 1e4;
   int detID = 0;
   TH1F * hrdt[8];
   hrdt[0] = new TH1F("hrdt0","hrdt0; E",1000,0,6000);
   
   for( int ev = 0; ev < maxEvents; /*totnumEntry;*/ ev++){
      tree->GetEntry(ev);
      int nTrace = arr->GetEntriesFast();
      int countJ = 0;
      
      for( int j = 0; j <  nTrace ; j++){
         TGraph * g  = (TGraph*) arr->At(j);

         //pulling XY data //CRH
         Double_t * tempX;
         Double_t * tempY;
         tempX = g->GetX(); tempY = g->GetY();
        //  for (int ii=0;ii<300;ii++){
        //     printf("X:Y %f:%f\n",tempX[ii],tempY[ii]);
        //  }     
         TString gTitle;
         gTitle = g->GetTitle();
         detID = 0;
         int pos = gTitle.Index("id=");
         gTitle.Remove(0, pos+3);
         gTitle.Remove(3);
         detID = gTitle.Atoi();
         if( !(minDetID <=  detID &&  detID <= maxDetID ) ) continue;
        //  if( countJ == 0 )  printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);

        //manipulate the trace

        // fill the histograms
        if (detID == 100) hrdt[0]->Fill(tempY[200]-tempY[25]);
       
         countJ ++;   
      }
      if (ev%10000 == 0) {
          if (nTrace == 1) {printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d | detID : %d\n", ev, evPointer, nTrace, detID);}
       else {
       printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);
          }
      }
   }
   //gROOT->ProcessLine(".q");
   hrdt[0]->Draw();
}
