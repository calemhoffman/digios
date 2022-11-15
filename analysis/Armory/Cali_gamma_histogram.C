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

void Cali_gamma_histogram(TH1F * hist, float threshold = 0.1){
  /**///========================================================  load tree

  printf("============================================================= \n");
  printf("====================== Cali_gamma.C ========================= \n");
  printf("============================================================= \n");

  /**///======================================================== Browser or Canvas

  Int_t Div[2] = {1,3};  //x,y
  Int_t size[2] = {600,300}; //x,y
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


  cAlpha->cd(1);
  hist->Draw();
  cAlpha->Update();
  gSystem->ProcessEvents();

  int dummy = 0;
  int temp = 0;

  int nPeaks = 10;
  vector<double> energy; 
  vector<double> refEnergy;
  vector<double> fitEnergy;


  printf("---- finding peak using TSepctrum Class...\n");

  TSpectrum * spec = new TSpectrum(20);
  nPeaks = spec->Search(hist, 1, "", threshold);
  printf("---- found %2d peaks | ",  nPeaks);

  double * xpos = spec->GetPositionX();
  double * ypos = spec->GetPositionY();
  
  cAlpha->Update();
  gSystem->ProcessEvents();

  vector<double> height;

  int * inX = new int[nPeaks];
  TMath::Sort(nPeaks, xpos, inX, 0 );
  for( int j = 0; j < nPeaks; j++){
    energy.push_back(xpos[inX[j]]);
    height.push_back(ypos[inX[j]]);
  }

  for( int j = 0; j < nPeaks; j++) printf("%7.2f, ", energy[j]);
  printf("\n");


  //------------ 3, correction
  int option = 0;
  printf("========== which detector to be the reference?\n");
  printf("-1 =  manual reference\n");
  printf("-2 =  use 228Th, first 5 strongest peaks \n");
  printf("-3 =  use 207Bi, 3 peaks \n");
  printf("-4 =  use 152Eu, 11 peaks \n");
  printf("-9 =  stop \n");
  printf("your choice = ");
  temp = scanf("%d", &option);

  if( option == -9 ) {
    printf("------ stopped by user.\n");
    return;
  }

  

  //======== fill reference energy
  if(option == -1){
    for( int i = 0; i < nPeaks; i++){
      float eng;
      printf("%2d-th peak energy %f ch (-1 to skip, -2 to skip all):", i, energy[i]);
      temp = scanf("%f", &eng);
      if( eng >= 0 ) {
        refEnergy.push_back(eng);
        fitEnergy.push_back(energy[i]);
        printf("             input: %f \n", eng);
      }else if ( eng == -1 ){
        printf("             input: skipped \n");
      }else if ( eng == -2 ){
        break;
      }
    };
  }

  if( option == -2 ){
    fitEnergy = energy;
    
    refEnergy.clear();
    refEnergy.push_back(5.423);
    refEnergy.push_back(5.685);
    refEnergy.push_back(6.288);
    refEnergy.push_back(6.778);
    refEnergy.push_back(8.785);
  }
  
  if( option == -3 ){
    
    fitEnergy = energy;
    
    refEnergy.clear();
    refEnergy.push_back(0.569702);
    refEnergy.push_back(1.063662);
    refEnergy.push_back(1.770237);
  }

  if( option == -4 ){
    refEnergy.clear();
    refEnergy.push_back( 121.783);
    refEnergy.push_back( 244.699);
    refEnergy.push_back( 344.281);
    refEnergy.push_back( 411.115);
    refEnergy.push_back( 443.965);
    refEnergy.push_back( 778.903);
    refEnergy.push_back( 867.390);
    refEnergy.push_back( 964.055);
    refEnergy.push_back(1085.842);
    refEnergy.push_back(1112.087);
    refEnergy.push_back(1408.022);
  }
  
  //==================== adjusting energy
  int n = refEnergy.size();
  for( int k = 0; k < n; k++) printf("%2d-th peak : %f \n", k,  refEnergy[k]);


  nPeaks = energy.size();
   
  vector<double> k1 = fitEnergy;
  vector<double> k2 = refEnergy;  
  //===== when nPeaks != refEnergy.size(), need to matching the two vector size by checking the r-squared.
  if( option != -1 ){
    vector<vector<double>> haha = FindMatchingPair(fitEnergy, refEnergy);
    k1 = haha[0];   
    k2 = haha[1];   
  }
  
  TGraph * graph = new TGraph(min(n, nPeaks), &k1[0], &k2[0] );
  cAlpha->cd(2);
  graph->Draw("A*");

  TF1 * fit = new TF1("fit", "pol2" );
  graph->Fit("fit", "q");

  double a0 = fit->GetParameter(0);
  double a1 = fit->GetParameter(1);
  double a2 = fit->GetParameter(2);

  //printf("----- a0: %6.3f, a1: %6.3f (%14.8f) \n", a0, a1, 1./a1);

  //====== Plot adjusted spectrum
  
  cAlpha->cd(3);
  
  int bin = hist->GetNbinsX();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  
  double xMinC, xMaxC;
  if( a1 > 0 ) {
    xMinC = xMin*xMin*a2 + xMin*a1 + a0;
    xMaxC = xMax*xMax*a2 + xMax*a1 + a0;
  }else{
    xMaxC = xMin*xMin*a2 + xMin*a1 + a0;
    xMinC = xMax*xMax*a2 + xMax*a1 + a0;
  }
  
  TH1F * calH = new TH1F("calH", "calibrated energy", bin, xMinC, xMaxC);
  
  FILE * paraOut;
  TString filename;
  filename.Form("hist%s.dat", hist->GetName());
  paraOut = fopen (filename.Data(), "w+");
  
  for( int i = 1; i <= bin; i ++){
    int y = hist->GetBinContent(i);
    int x = hist->GetBinCenter(i);
    
    calH->Fill(x*x*a2 + x*a1+a0, y);
    
    fprintf(paraOut, "%9.6f, %9d\n", x*x*a2 + x*a1+a0, y);
    
  }
  
  fflush(paraOut);
  fclose(paraOut);
  
  calH->Draw();
    
  return;
   
}
