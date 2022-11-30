#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <TF1.h>
#include <TLine.h>
#include <iostream>
​
​
TGraph * TrapezoidFilter(TGraph * trace){
   ///Trapezoid filter https://doi.org/10.1016/0168-9002(94)91652-7
​
   int baseLineEnd = 80;
   
   int riseTime = 10; //ch
   int flatTop = 4;
   float decayTime = 2000;
   
   TGraph *  trapezoid = new TGraph();
   trapezoid->Clear();
   
   ///find baseline;
   double baseline = 0;
   for( int i = 0; i < baseLineEnd; i++){
      baseline += trace->Eval(i);
   }
   baseline = baseline*1./baseLineEnd;
   
   int length = trace->GetN();
   
   double pn = 0.;
   double sn = 0.;
   for( int i = 0; i < length ; i++){
   
      double dlk = trace->Eval(i) - baseline;
      if( i - riseTime >= 0            ) dlk -= trace->Eval(i - riseTime)             - baseline;
      if( i - flatTop - riseTime >= 0  ) dlk -= trace->Eval(i - flatTop - riseTime)   - baseline;
      if( i - flatTop - 2*riseTime >= 0) dlk += trace->Eval(i - flatTop - 2*riseTime) - baseline;
      
      if( i == 0 ){
         pn = dlk;
         sn = pn + dlk*decayTime;
      }else{
         pn = pn + dlk;
         sn = sn + pn + dlk*decayTime;
      }    
      
      trapezoid->SetPoint(i, i, sn / decayTime / riseTime);
    
   }
   
   return trapezoid;
   
}
​
​
TGraph * gTrap = NULL;
​
TH1F * gHist = NULL;
​
TSpectrum * peak = NULL;
​
TF1 * fit = NULL;
​
​
int nPeaks = 1;
​
int numPara = nPeaks *2 + 4;
​
​
double haha(double x, double A, double t, double r, double d){
   return A * (1 - TMath::Exp(- (x - t) / r) ) * TMath::Exp(- (x - t) / d);
}
​
double fitFunc(double * x, double * par){
​
   /// par[0] = baseline
   /// par[1] = pre-rise slope
​
   /// par[2] = rise time
   /// par[3] = decay
​
   /// par[4 + 2*i] = A_i
   /// par[4 + 2*i+1] = t_i
​
   double result = 0;
   
   if( x[0] < par[5] ) return par[0] + par[1] * (x[0] - par[5]);
​
   result = par[0];
​
   for( int i = 0; i < nPeaks -1 ; i++){      
      result += haha( x[0], par[4+2*i], par[4+2*i+1], par[2], par[3]);
   }
​
   
   if (  par[4 + 2*(nPeaks-1)+1] <= x[0] ) {
      result += haha( x[0], par[4+2*(nPeaks-1)], par[4+2*(nPeaks-1)+1], par[2], par[3]);
   }
   
   
   return result;
}
​
​
​
void testTraceFit(TString fileName, int minDetID = 0, int maxDetID = 1000, bool isGoodOnly = false){
​
/**///==============================================================   
​
   TFile * f1 = new TFile (fileName, "read");
   TTree * tree = (TTree *) f1->Get("gen_tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 1500, 800, 900);
   cRead->Divide(1,2);
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
​
      int countJ = 0;
      
      for( int j = 0; j <  nTrace ; j++){
​
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
​
         if( countJ == 0 )  printf("-------------------------------- ev : %d, evPointer : %d| num Trace : %d\n", ev, evPointer, nTrace);
​
         
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
​
            
            printf("%s |(q = break, w = last one)", kTitle.Data());
​
           
            timeVLine.SetX1(time);
            timeVLine.SetX2(time);
            timeVLine.SetY1(-1000);
            timeVLine.SetY2(20000);
            timeVLine.SetLineColor(4);
         }
         
         cRead->cd(1);
         //cRead->Clear();
         g->Draw("AC");
​
​
         //g->GetXaxis()->SetRangeUser(0, g->GetN());
         //g->GetYaxis()->SetRangeUser(7500, 35000);
         timeVLine.Draw("same");
         if( gFit != NULL ) text.DrawText(0.11, 0.85, kTitle.Data());
​
​
         cRead->cd(2);
         if( gTrap != NULL ) delete gTrap;
         gTrap = TrapezoidFilter(g);
         gTrap->Draw("AC");
​
         if( gHist != NULL ) delete gHist;
         gHist = new TH1F("gHist", "gHist", gTrap->GetN(), 0, gTrap->GetN()); 
​
         for( int k = 0; k < gTrap->GetN(); k++){
            double x, y;
            gTrap->GetPoint(k, x, y);
            gHist->Fill(x-0.5, y);
         }
         
​
         //cRead->cd(2);
         gHist->Draw("hist");
​
​
         //if( peak != NULL ) delete peak;
         //peak = new TSpectrum(10);
         //int nPeaksRaw = peak->Search(gHist, 4, "", 0.4);
​
         //peak->Draw("hist");
​
         //gHist->Draw("hist");
​
         //double * xpos = peak->GetPositionX();
         //double * ypos = peak->GetPositionY();
         //
         //int inX[nPeaksRaw];
         //TMath::Sort(nPeaksRaw, xpos, &inX[0], 0);
         //
         //double fitRange = 450;
         //
         //printf("\n");
         //vector<double> xfit;
         //vector<double> yfit;
         //for( int k = 0; k < nPeaksRaw; k++){
         //   printf("%d | %f %f\n", k, xpos[inX[k]], ypos[inX[k]]);
         //   if( 80 < xpos[inX[k]] && xpos[inX[k]] < fitRange  ) {
         //      xfit.push_back(xpos[inX[k]]);
         //      yfit.push_back(ypos[inX[k]]);
         //   }
         //}
         //
         //nPeaks = xfit.size();
         //
         //numPara = nPeaks *2 + 4;
         //
         //if( fit != NULL ) delete fit;
         //fit = new TF1("fit", fitFunc, 0, fitRange, numPara);
         //
         //fit->SetParameter(0, 8000);
         //fit->SetParameter(1, -0.1);
         //fit->SetParameter(2, 15);
         //fit->SetParameter(3, 400);
         //
         //fit->SetParLimits(0, 7000,  9000);
         //fit->SetParLimits(1, -0.5, -0.01);
         //fit->SetParLimits(2, 10, 20);
         //fit->SetParLimits(3, 200, 1500);
         //
         //
         //for( int k = 0; k < nPeaks; k++){
         //   fit->SetParameter(4 + 2*k, yfit[k]);
         //   fit->SetParameter(4 + 2*k+1, xfit[k]-20);
         //
         //   fit->SetParLimits(4 + 2*k,   yfit[k]*0.2, yfit[k] * 1.5);
         //   fit->SetParLimits(4 + 2*k+1, xfit[k]-40, xfit[k]-5);
         //
         //}
         //
         //cRead->cd(3);         
         //g->Draw("AC");
         //g->Fit("fit", "R", "", 0, fitRange);
         //
         //fit->Draw("same");
​
         
         cRead->Update();         
         gSystem->ProcessEvents();
         
         
         char s[80];
         fgets(s, sizeof s, stdin); 
​
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
​
}
