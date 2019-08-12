#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLatex.h>
#include <TSpectrum.h>


void Cali_e_single(TTree * tree, int detID, int minEnergyRange = 300, int maxEnergyRange = 7000){
  
  if( detID < 0 ){
    return;
  }
  
/**///======================================================== initial input

   int nBin = 200;

  //TString gate; gate.Form("!TMath::IsNaN(e[%d])", detID);
   TString gate; gate.Form("!TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", detID, detID);

   
/**///========================================================  load tree

   printf("============================================================= \n");
   printf("====================== Cali_e_single.C ====================== \n");
   printf("============ calibration energy for single detector ========= \n");
   printf("============================================================= \n");
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {3,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   TCanvas * cAlpha = (TCanvas *) gROOT->FindObjectAny("cAlpha");
   if( cAlpha == NULL ) {
     cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
     cAlpha->Divide(Div[0],Div[1]);
   
     for( int i = 1; i <= Div[0]*Div[1] ; i++){
       cAlpha->cd(i)->SetGrid();
     }
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   if(cAlpha->GetShowEditor()  )cAlpha->ToggleEditor();
   if(cAlpha->GetShowToolBar() )cAlpha->ToggleToolBar();
   
/**///========================================================= Analysis

   //############################################################ energy correction
   printf("############## e correction \n");
   TString name;
   name.Form("q%d", detID);
   TH1F * q = new TH1F(name, name, nBin, minEnergyRange, maxEnergyRange);
   q->SetXTitle(name);
   
   TString expression;
   expression.Form("e[%d] >> q%d" ,detID, detID);
   cAlpha->cd(1);
   tree->Draw(expression, gate , "");
   cAlpha->Update();
   gSystem->ProcessEvents();
   
   //----------- 1, pause for save Canvas
   int dummy = 0;
   int temp = 0;

   //----------- 2, find the edge of the energy    
   printf("============== method to find edge:\n");
   printf("2) TSpectrum fit.\n");
   printf("9) Exit.\n");
   printf("====== choice : ");
   int method = 0;
   temp = scanf("%d", &method);
   
   if( method == 9 ) return;
   
   double xHalf;
   
   int nPeaks = 10;
   vector<double> energy; 
   double a0;
   double a1;
   vector<double> refEnergy;
   if( method == 2 ){
      printf("---- finding edge using TSepctrum Class...\n");
         
      TSpectrum * spec = new TSpectrum(10);
      nPeaks = spec->Search(q, 1, "", 0.2);
      printf("found %d peaks | ", nPeaks);

      double * xpos = spec->GetPositionX();
      
      int * inX = new int[nPeaks];
      TMath::Sort(nPeaks, xpos, inX, 0 );
      for( int j = 0; j < nPeaks; j++){
         energy.push_back(xpos[inX[j]]);
      }
      
      for( int j = 0; j < nPeaks; j++){
         printf("%7.2f, ", energy[j]);
      }
      printf("\n");
      
      q->Draw();
      cAlpha->Update();
      gSystem->ProcessEvents();
   
      
      //------------ 3, correction      
      //======== fill reference energy
      vector<double> tempEnergy;
      int n = 0;
      float eng = -1;
      printf("=================== input reference energy\n");
      do{
         printf("What should be the %2d-th peak energy [MeV] ? (< 0 to skip):", n  );
         temp = scanf("%f", &eng);
         if( eng >= 0 ) {
            printf("       %f ch --> %f MeV\n", energy[n],  eng);
            refEnergy.push_back(eng);
            tempEnergy.push_back(energy[n]);
         }else{
            printf("       %f ch --> skipped \n", energy[n]);
         }
         n++;
      }while( n < nPeaks);
      
      if( refEnergy.size() == 0 ) return;
      
      energy.clear();
      
      printf("----- adjusting the energy .....\n");
      n = refEnergy.size();
      for( int k = 0; k < n; k++){
         energy.push_back(tempEnergy[k]);
         printf("%2d-th peak : %f MeV | %f ch \n", k,  refEnergy[k], energy[k]);
      }
      printf("----------------------------------\n");
      
      cAlpha->cd(2);
      TGraph * graph = new TGraph(n, &energy[0], &refEnergy[0] );
      graph->Draw("A*");
      gSystem->ProcessEvents();
   
      TF1 * fit = new TF1("fit", "pol1" );
      graph->Fit("fit", "q");
      gSystem->ProcessEvents();
      
      a0 = fit->GetParameter(0);
      a1 = fit->GetParameter(1);

      TLatex text;
      text.SetNDC();
      text.SetTextFont(82);
      text.SetTextSize(0.04);
      text.SetTextColor(2);

      text.DrawLatex(0.15, 0.8, Form("1/a1 = %12.7f", 1./a1));
      text.DrawLatex(0.15, 0.7, Form(" a0  = %12.7f", a0));

      //printf("a0: %9.6f, a1: %9.6f \n", a0, a1);
      
      printf("Please manually save to file as : %12.7f\t%12.7f\n", 1./a1, a0);
      
      cAlpha->cd(3);
      TString name;
      name.Form("p%d", detID);
      TH1F * p = new TH1F(name, name, nBin, 0., refEnergy.back() * 1.3);
      p->SetXTitle(name);
         
      TString expression;
      expression.Form("e[%d] * %f + %f >> p%d", detID, a1, a0, detID);
      gate.Form("e[%d] > 0", detID);
      tree->Draw(expression, gate , "");
      cAlpha->Update();
      gSystem->ProcessEvents();
      
   }
   
   return;
   
}

