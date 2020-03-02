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

//===============
const int numDet = 3;

int detID[numDet] = {2,3,5}; 

float energyRange[3] = {1800, 200, 2000}; // bin, min, max

void Cali_gamma(TTree * tree, float threshold = 0.1){
  /**///========================================================  load tree

  printf("============================================================= \n");
  printf("====================== Cali_gamma.C ========================= \n");
  printf("============================================================= \n");
  printf("calibration gamma energy between different detectors. \n");
  printf("------------------------------------------------------------- \n");
  printf("=========== Total #Entry: %10lld \n", tree->GetEntries());

  /**///======================================================== Browser or Canvas


  int rowDet = (int) floor(sqrt(numDet));
  int colDet = numDet/rowDet;

  Int_t Div[2] = {colDet,rowDet};  //x,y
  Int_t size[2] = {600,400}; //x,y
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
    int id = detID[i];

    TString name;

    name.Form("q%d", id);
    q[i] = new TH1F(name, name, energyRange[0], energyRange[1], energyRange[2]);
    q[i]->SetXTitle(name);

    TString expression;
    expression.Form("e[%d] >> q%d" ,id, id);
    //expression.Form("ring[%d] >> q%d" ,id, id);
    //gate[i].Form("ring[%d]==0 && !TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i, i);
    //gate[i].Form("!TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i);
    //gate[i].Form("e[%d] > 0", id);
    //gate[i].Form("ch == %d", id);

    cAlpha->cd(i+1);
    tree->Draw(expression, gate[i] , "");
    cAlpha->Update();
    gSystem->ProcessEvents();
  }

  int dummy = 0;
  int temp = 0;
  cAlpha->Update();
  gSystem->ProcessEvents();

  int nPeaks = 10;
  vector<double> * energy = new vector<double> [numDet]; 
  double a0[numDet];
  double a1[numDet];
  vector<double> refEnergy;


  printf("---- finding edge using TSepctrum Class...\n");
  for( int i = 0; i < numDet; i++){

    TSpectrum * spec = new TSpectrum(20);
    nPeaks = spec->Search(q[i], 1, "", threshold);
    printf("%2d | found %2d peaks | ", i,  nPeaks);

    double * xpos = spec->GetPositionX();
    double * ypos = spec->GetPositionY();

    vector<double> height;

    int * inX = new int[nPeaks];
    TMath::Sort(nPeaks, xpos, inX, 0 );
    for( int j = 0; j < nPeaks; j++){
      energy[i].push_back(xpos[inX[j]]);
      height.push_back(ypos[inX[j]]);
    }

    for( int j = 0; j < nPeaks; j++) printf("%7.2f, ", energy[i][j]);
    printf("\n");

  }

  for( int i = 0; i < numDet; i++){
    cAlpha->cd(i+1);
    q[i]->Draw();
    cAlpha->Update();
    gSystem->ProcessEvents();
  }


  //------------ 3, correction
  int option = 0;
  printf("========== which detector to be the reference?\n");
  printf(" X =  det-X reference\n");
  printf("-1 =  manual reference\n");
  printf("-2 =  use 228Th, first 5 strongest peaks \n");
  printf("-3 =  use 207Bi, 3 peaks \n");
  //printf("-4 =  use 152Eu, the 7 strongest peaks \n");
  printf("-9 =  stop \n");
  printf("your choice = ");
  temp = scanf("%d", &option);

  if( option == -9 ) {
    printf("------ stopped by user.\n");
    return;
  }

  //======== fill reference energy
  if( option >= 0 ){
    
    //find refID by option
    int refID = 0;
    for( int i = 0; i < numDet; i++){
      if( detID[i] == option ) refID = i;
    } 
    
    int n = energy[refID].size();
    for( int k = 0; k < n; k++) refEnergy.push_back(energy[refID][k]);
    
    printf("----- adjusting the energy to det-%d......\n", option);
  }

  if(option == -1){
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

  if( option == -2 ){
    refEnergy.clear();
    refEnergy.push_back(5.423);
    refEnergy.push_back(5.685);
    refEnergy.push_back(6.288);
    refEnergy.push_back(6.778);
    refEnergy.push_back(8.785);
  }
  
  if( option == -3 ){
    refEnergy.clear();
    refEnergy.push_back(0.569702);
    refEnergy.push_back(1.063662);
    refEnergy.push_back(1.770237);
  }
  
  //==================== adjusting energy
  for( int k = 0; k < refEnergy.size(); k++) printf("%2d-th peak : %f \n", k,  refEnergy[k]);

  for( int i = 0; i < numDet; i ++){
    printf("------- refID - %d \n", i);
    
    if( energy[i].size() == 0 || detID[i] == option) {
      a0[i] = 0;
      a1[i] = 1;
      printf("skipped\n");
      continue;
    }

    //===== when nPeaks != refEnergy.size(), need to matching the two vector size by checking the r-squared.
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
  TH1F ** p = new TH1F*[numDet];
  
  if( option == -2 ){
    energyRange[0] = 500;
    energyRange[1] = 4;
    energyRange[2] = 10;
  }
  
  if( option == -3 ){
    energyRange[0] = 1950;
    energyRange[1] = 0.05;
    energyRange[2] = 2;
  }
  
  
  TLegend * legend = new TLegend(0.7,0.2,0.99,0.6);
  double yMax = 0;
  for ( int i = 0; i < numDet; i ++){
    TString name;
    name.Form("p%d", detID[i]);

    p[i] = new TH1F(name, name,  energyRange[0]/2, energyRange[1], energyRange[2]);
    p[i]->SetXTitle(name);
    p[i]->SetLineColor(i+1);

    TString expression;
    expression.Form("e[%d] * %.8f + %.8f >> p%d", detID[i], a1[i], a0[i], detID[i]);
    //expression.Form("ring[%d] * %.8f + %.8f >> p%d", detID[i], a1[i], a0[i], detID[i]);
    //gate[i].Form("ch == %d", detID[i]);
    
    tree->Draw(expression, gate[i] , "");
    legend->AddEntry(p[i], Form("p%d", i));
    
    double yM = p[i]->GetMaximum();
    if( yM > yMax ) yMax = yM;
    
    cAux->Update();
    gSystem->ProcessEvents();
       
  }   
  
  cAux->cd(1);
  p[0]->SetMaximum( yMax * 1.1);
  p[0]->Draw();
  gSystem->ProcessEvents();
  for( int  i = 0; i < numDet; i++){
    p[i]->SetMaximum( yMax * 1.1);
    p[i]->Draw("same");
    gSystem->ProcessEvents();
  }
  
  legend->Draw();

  /*
  //----------- 4, pause for saving correction parameters
  cAlpha->Update();
  gSystem->ProcessEvents();
  printf("0 for stop, 1 for save e-correction. ");
  temp = scanf("%d", &dummy);
  //dummy = 1;
  if( dummy == 0 ) return;
  if( dummy == 1 ){
    FILE * paraOut;
    TString filename;
    filename.Form("correction_gamma.dat");
    paraOut = fopen (filename.Data(), "w+");
    printf("=========== save e-correction parameters to %s \n", filename.Data());
    for( int i = 0; i < numDet; i++) fprintf(paraOut, "%14.8f\t%9.6f\n", 1./a1[i], a0[i]);
    fflush(paraOut);
    fclose(paraOut);
  } 

  //gROOT->ProcessLine(".q");
  * 
  */ 
  return;
   
}

