#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraph.h>
#include <TLine.h>
#include <TSpectrum.h>

#include "../Armory/AnalysisLibrary.h"
#include "../Armory/AutoFit.C"

void Cali_xf_xn(TTree * tree){
/**///======================================================== initial input
   
   const int rowDet = 4;
   const int colDet = 6;
   
   int energyRange[3] = {400, 1000, 2600}; // bin, min, max
   double threshold = 0.2;
   
/**///========================================================  load tree

   printf("============================================================= \n");
   printf("====================== Cali_xf_xn.C ========================= \n");
   printf("============================================================= \n");
   printf("==   calibration for PSD detectors using alpha souce. \n");
   printf("------------------------------------------------------------- \n");
   printf("1, calibration energy using charateristic energy of alpha souce. \n");
   printf("2, calibration xf-xn with energy-gate. \n");
   printf("------------------------------------------------------------- \n");
   printf("=========== Total #Entry: %10lld \n", tree->GetEntries());
   
   int temp = 0;
   printf(" Min Raw Energy [ch] (default = %d ch) : ", energyRange[1]);
   scanf("%d", &temp);
   energyRange[1] = temp;

   printf(" Max Raw Energy [ch] (default = %d ch) : ", energyRange[2]);
   scanf("%d", &temp);   
   energyRange[2] = temp;
   
   printf("     Raw Energy is now %d ch to %d ch\n", energyRange[1], energyRange[2]);
/**///======================================================== Canvas

   const int numDet = rowDet * colDet;

   Int_t Div[2] = {colDet,rowDet};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cAlpha = new TCanvas("cAlpha", "cAlpha", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAlpha->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAlpha->cd(i)->SetGrid();
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
   TH1F ** q = new TH1F*[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("q%d", i);
      q[i] = new TH1F(name, name, energyRange[0], energyRange[1], energyRange[2]);
      q[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d] >> q%d" ,i, i);
      //gate[i].Form("ring[%d]==0 && !TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i, i);
      //gate[i].Form("!TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i);
      //gate[i].Form("e[%d] > 0", i);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "");
      cAlpha->Update();
      gSystem->ProcessEvents();
   }
   
   //----------- 1, pause for save Canvas
   int dummy = 0;
   temp = 0;
   cAlpha->Update();
   gSystem->ProcessEvents();

   //----------- 2, find the edge of the energy    
   printf("============== method to find edge:\n");
//   printf("1) maximum peak edge.\n");
   printf("2) TSpectrum search\n");
   printf("3) TSpectrum fit.\n");
   printf("9) Exit.\n");
   printf("====== choice : ");
   int method = 0;
   temp = scanf("%d", &method);
   
   if( method == 9 ) {
      gROOT->ProcessLine(".q");
      return;
   }
  
   vector<double> * energy = new vector<double> [numDet]; 
   double a0[numDet];
   double a1[numDet];
   vector<double> refEnergy;
      
   if ( method == 2 ){
      printf("---- finding peak using TSpectrum Class...\n");   
      
      printf(" peak threshold (default = %.3f) : ", threshold);
      scanf("%lf", &threshold);
      printf("     threshold is now %.3f\n", threshold);
         
      for( int i = 0; i < numDet; i++){
         
         TSpectrum * spec = new TSpectrum();
         int nPeaks = spec->Search(q[i], 3, "", threshold);
         printf("%2d | found %d peaks | ", i,  nPeaks);

         double * xpos = spec->GetPositionX();
         double * ypos = spec->GetPositionY();
         
         vector<double> height;
         
         int * inX = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, inX, 0 );
         for( int j = 0; j < nPeaks; j++){
            energy[i].push_back(xpos[inX[j]]);
            height.push_back(ypos[inX[j]]);
         }
         
         for( int j = 0; j < nPeaks; j++){
            printf("%7.2f, ", energy[i][j]);
         }
         printf("\n");
      }
   
   }
   
   if ( method == 3){
      printf("---- find and fit peak with Gaussian using AutoFit.C \n");
      for( int i = 0; i < numDet; i++){
         cAlpha->cd(i+1);
         energy[i] = fitAuto(q[i], -1, 0.3, 20, 4, "");
         int nPeaks = energy[i].size(); 
         printf("%2d | found %d peaks | ", i,  nPeaks);
         for( int j = 0; j < nPeaks; j++){
            printf("%7.2f, ", energy[i][j]);
         }
         printf("\n");
      }
   }
   
   if( method == 2 ) {
      for( int i = 0; i < numDet; i++){
         cAlpha->cd(i+1);
         q[i]->Draw();
         cAlpha->Update();
         gSystem->ProcessEvents();
      }
   }
   //------------ 3, correction
   int refID = 0;
   printf("========== which detector to be the reference?\n");
   printf(" X =  det-X reference\n");
   printf("-1 =  manual reference\n");
   printf("-2 =  use 228Th, first 7 strongest peaks \n");
   printf("-3 =  use 148Gd + 244 Cm, (3.1828, 5.8048 MeV)\n");
   //printf("-3 =  use 241Am, 5.481 MeV \n");
   printf("-9 =  stop \n");
   printf("your choice = ");
   temp = scanf("%d", &refID);
   
   if( refID == -9 ) {
      printf("------ stopped by user.\n");
      return;
   }
   
   //======== fill reference energy
   if( refID >= 0 ){
      int n = energy[refID].size();
      for( int k = 0; k < n; k++) refEnergy.push_back(energy[refID][k]);
   }
   
   if(refID == -1){
      int n = 0;
      float eng = -1;
      do{
         printf("%2d-th peak energy (< 0 to stop):", n);
         temp = scanf("%f", &eng);
         printf("             input: %f \n", eng);
         if( eng >= 0 ) refEnergy.push_back(eng);
         n ++ ;
      }while(eng >= 0);
   }
   
   if( refID == -2 ){
      refEnergy.clear();
      refEnergy.push_back(5.34);
      refEnergy.push_back(5.423);
      refEnergy.push_back(5.685);
      refEnergy.push_back(6.050);
      refEnergy.push_back(6.288);
      refEnergy.push_back(6.778);
      refEnergy.push_back(8.785);
   }
   
   if( refID == -3 ){
      refEnergy.clear();
      refEnergy.push_back(3.1828);
      refEnergy.push_back(5.8048);
   }
   
   printf("----- adjusting the energy to det-%d......\n", refID);
   for( int k = 0; k < refEnergy.size(); k++) printf("%2d-th peak : %f \n", k,  refEnergy[k]);
   
   const vector<double> refEnergy0 = refEnergy; 
   
   for( int i = 0; i < numDet; i ++){
     
     printf("------- refID - %d, nPeaks: %lu \n", i, energy[i].size());
     
     refEnergy = refEnergy0;
     
     if( refID >= 0 && refID == i ){
       a0[i] = 0;
       a1[i] = 1;
       printf("skipped - itself\n");
       continue;
     }
     
     if( energy[i].size() == 0) {
       a0[i] = 0;
       a1[i] = 1;
       printf("skipped\n");
       continue;
     }
     
     printf("   Energy : ");
     for( int k = 0; k < energy[i].size(); k++){ printf("%.1f, ", energy[i][k]);};printf("\n");
     vector<vector<double>> output =  FindMatchingPair(energy[i], refEnergy);
 
     vector<double> haha1 = output[0];
     vector<double> haha2 = output[1];
     
     TGraph * graph = new TGraph(haha1.size(), &haha1[0], &haha2[0] );
     cAlpha->cd(i+1);
     graph->Draw("A*");

     TF1 * fit = new TF1("fit", "pol1" );
     graph->Fit("fit", "q");

     a0[i] = fit->GetParameter(0);
     a1[i] = fit->GetParameter(1);

     printf("%2d | a0: %6.3f, a1: %6.3f (%14.8f) \n", i, a0[i], a1[i], 1./a1[i]);
     
   }
      
   
  //====== Plot adjusted spectrum
  TCanvas * cAux = new TCanvas ("cAux", "cAux", 600, 400);
  TH2F ** p = new TH2F*[numDet];
  double yMax = 0;

  for ( int i = 0; i < numDet; i ++){
    TString name;
    name.Form("p%d", i);
    p[i] = new TH2F(name, name, 24, 0, 24, energyRange[0], refEnergy[0] * 0.9, refEnergy.back() * 1.1);
    
    TString expression;
    expression.Form("e * %.8f + %.8f : Iteration$ >> p%d", a1[i], a0[i], i);
    gate[i].Form("Iteration$ == %d", i);
    tree->Draw(expression, gate[i] , "colz");
    
    cAux->Update();
    gSystem->ProcessEvents();
  }   
  
  cAux->cd(1);
  TString hhhName;
  if( method == 2 ) hhhName="Calibrated energy using TSpectrum peak";
  if( method == 3 ) hhhName="Calibrated energy using Gaussian fitting";
  TH2F * hhh = new TH2F("hhh", hhhName, 24, 0, 24, energyRange[0], refEnergy[0] * 0.9, refEnergy.back() * 1.1);
  for( int  i = 0; i < numDet; i++){
    hhh->Add(p[i]);
  }
  hhh->Draw("colz");
  cAux->Update();
  gSystem->ProcessEvents();
   
   //----------- 4, pause for saving correction parameters
   cAlpha->Update();
   gSystem->ProcessEvents();
   //printf("0 for stop, 1 for save e-correction & Canvas, 2 for xf - xn correction: ");
   //temp = scanf("%d", &dummy);
   dummy = 1;
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_e_alpha.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save e-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%14.8f\t%9.6f\n", 1./a1[i], a0[i]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      //cAlpha->SaveAs("alpha_e_corrected.pdf");
   }  
   
   gSystem->ProcessEvents();
   
   //############################################################  for xf-xn correction
   printf("############## xf - xn correction \n");
   TLine line(0,0,0,0);
   line.SetLineColor(4);
   
   double eGate = 0;
   gSystem->ProcessEvents();
   int peakID = 0;
   printf("------ pick the i-th peak (0, 1, ... , %d, -1 to stop): ", (int) refEnergy.size() - 1);
   temp = scanf("%d", &peakID);
   if( peakID < 0 ) {
      return;
   }
   eGate = refEnergy[peakID];
   printf("------ using the peak at : %f \n", eGate);

   
   printf("----- plotting xf vs xn with energy gate near the peak...\n");
   TH2F ** h = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH2F(name, name,  energyRange[0], 0, energyRange[2], energyRange[0], 0, energyRange[2]);
      name.Form("xf[%d]", i); h[i]->SetYTitle(name);
      name.Form("xn[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]>> h%d" , i, i, i);
      gate[i].Form("xf[%d]>0 && xn[%d]>0 && TMath::Abs(e[%d] * %f + %f - %f)< %f", i, i, i, a1[i], a0[i], eGate, eGate * 0.05);
      
      cAlpha->cd(i+1);
      tree->Draw(expression, gate[i] , "colz");
      
      cAlpha->Update();
      gSystem->ProcessEvents();
      
   }
   
   //-------- 1, pause for saving Canvas
   cAlpha->Update();
   gSystem->ProcessEvents();
   //printf("0 for stop, 1 for save Canvas, 2 for continuous : ");
   //temp = scanf("%d", &dummy);
   //if( dummy == 0 ) {
   //   return;
   //}else if(dummy == 1){
   //   cAlpha->SaveAs("alpha_xf_xn.pdf");
   //}
   
   //-------- 2, profileX and get the fit function
   printf("----- profile and obtain the fit function...\n");
   double para[numDet][2];
   double xnScale[numDet];
   TF1 * fit = new TF1("fit", "pol1");
   for( int i = 0; i < numDet; i++){
      cAlpha->cd(i+1);
      
      if( energy[i].size() == 0) {
        xnScale[i] = 0.000;
        printf("skipped - detID-%d\n", i);
        continue;
      }
      
      h[i]->ProfileX()->Fit("fit", "Q");
      fit->GetParameters(para[i]);
      xnScale[i] = -para[i][1]; 
   }

   //--------- 3, correction
   printf("----- correcting...\n");
   TH2F ** k = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name,  energyRange[0], 0, energyRange[2],  energyRange[0], 0, energyRange[2]);
      name.Form("xf[%d]", i); k[i]->SetYTitle(name);
      name.Form("xn[%d]", i); k[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("xf[%d]:xn[%d]*%f>> k%d" ,i ,i, xnScale[i], i);
      //gate[i].Form("xf[%d]!=0 && xn[%d]!=0", i, i);
      
      cAlpha->cd(i+1);
      
      tree->Draw(expression, gate[i] , "colz");
      line.SetX2(para[i][0]);
      line.SetY1(para[i][0]);
      line.Draw("same");
      
      cAlpha->Update();
      gSystem->ProcessEvents();
   }
   
   //--------- 4, pause for saving correction parameter
   cAlpha->Update();
   gSystem->ProcessEvents();
   printf("0 for stop, 1 for save xf-xn-correction : ");
   temp = scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){   
      FILE * paraOut;
      TString filename;
      filename.Form("correction_xf_xn.dat");
      paraOut = fopen (filename.Data(), "w+");
      printf("=========== save xf_xn-correction parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f\n", -para[i][1]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      //cAlpha->SaveAs("alpha_xf_xn_corrected.pdf");
   }
   
   gROOT->ProcessLine(".q");
   return;
   
}

