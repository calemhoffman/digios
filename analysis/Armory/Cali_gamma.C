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
#include <TLatex.h>
#include <TLegend.h>
#include <TSpectrum.h>

#include "../Armory/AnalysisLibrary.h"

//=============== User input
const int numDet = 3;
int detID[numDet] = {0,1,2}; 
//TString detName[numDet] = {"HPGe-1","HPGe-2","HPGe-3", "NaI"} ;
TString detName[numDet] = {"HPGe-1","HPGe-2","HPGe-3"} ;

float energyRange[3] = {1000, 10000, 14000}; // bin, min, max

//================ end of user input

void Cali_gamma(TTree * tree, int runID = -1 , float threshold = 0.05){
  /**///========================================================  load tree

  printf("============================================================= \n");
  printf("====================== Cali_gamma.C ========================= \n");
  printf("============================================================= \n");
  printf("calibration gamma energy between different detectors. \n");
  printf("------------------------------------------------------------- \n");
  printf("=========== Total #Entry: %10lld \n", tree->GetEntries());

  /**///======================================================== Browser or Canvas


  int rowDet = (int) floor(sqrt(numDet));
  int colDet = TMath::Ceil(numDet*1.0/rowDet);

  Int_t Div[2] = {colDet,rowDet};  //x,y
  Int_t size[2] = {600,400}; //x,y
  TCanvas * cAlpha = new TCanvas("cAlpha", Form("RUN:%03d", runID), 0, 0, size[0]*Div[0], size[1]*Div[1]);
  cAlpha->Divide(Div[0],Div[1]);

  for( int i = 1; i <= Div[0]*Div[1] ; i++){
    cAlpha->cd(i)->SetGrid();
    //cAlpha->cd(i)->SetLogy();
  }

  gStyle->SetOptStat("neiou");
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.1);

  if(cAlpha->GetShowEditor()  )cAlpha->ToggleEditor();
  if(cAlpha->GetShowToolBar() )cAlpha->ToggleToolBar();

  double a0[numDet];
  double a1[numDet];


  /**///========================================================= Analysis

  //############################################################ energy correction
  printf("############## e correction \n");
  TH1F ** q = new TH1F*[numDet];
  TString * gate = new TString[numDet];
  for( int i = 0; i < numDet; i ++){
    int id = detID[i];

    TString name;

    name.Form("q%d", id);
    q[i] = new TH1F(name, detName[i], energyRange[0], energyRange[1], energyRange[2]);
    q[i]->SetXTitle("energy [ch]");

    TString expression;
    //expression.Form("e[%d] >> q%d" ,id, id);
    expression.Form("rdt[%d] >> q%d" ,id, id);
    //expression.Form("ring[%d] >> q%d" ,id, id);
      
    //gate[i].Form("ring[%d]==0 && !TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i, i);
    //gate[i].Form("!TMath::IsNaN(xf[%d]) && !TMath::IsNaN(xn[%d])", i, i);
    //gate[i].Form("e[%d] > 0", id);
    //gate[i].Form("ch == %d", id);
    gate[i].Form("!TMath::IsNaN(rdt[%d])", id);

    cAlpha->cd(i+1);
    tree->Draw(expression, gate[i] , "");
    cAlpha->Update();
    gSystem->ProcessEvents();
  }

  int dummy = 0;
  int temp = 0;
  cAlpha->Update();
  gSystem->ProcessEvents();
  int option = 0;

  int nPeaks = 10;
  vector<double> * energy = new vector<double> [numDet]; 
  vector<double> refEnergy;


  printf("---- finding edge using TSepctrum Class...\n");
  for( int i = 0; i < numDet; i++){

    TSpectrum * spec = new TSpectrum(20);
    nPeaks = spec->Search(q[i], 3, "", threshold);
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
  printf("========== which detector to be the reference?\n");
  printf(" X =  det-X reference\n");
  printf("-1 =  manual reference\n");
  printf("-3 =  use 207Bi, the  3 strongest peaks \n");
  printf("-4 =  use 152Eu, the 11 strongest peaks + 511 keV\n");
  printf("-5 =  use 133Ba, the  5 strongest peaks \n");
  printf("-6 =  use 226Ra, the  5 strongest peaks \n");  
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

  if( option == -2 ){ ///==== 238Th, alpha
    refEnergy.clear();
    refEnergy.push_back(5.423);
    refEnergy.push_back(5.685);
    refEnergy.push_back(6.288);
    refEnergy.push_back(6.778);
    refEnergy.push_back(8.785);
  }
  
  if( option == -3 ){ ///==== 207Bi, gamma
    refEnergy.clear();
    refEnergy.push_back( 569.702);
    refEnergy.push_back(1063.662);
    refEnergy.push_back(1770.237);
  }

  if( option == -4 ){ ///==== 152Eu, gamma
    refEnergy.clear();
    refEnergy.push_back( 121.783);
    refEnergy.push_back( 244.699);
    refEnergy.push_back( 344.281);
    refEnergy.push_back( 411.115);
    refEnergy.push_back( 443.965);
    refEnergy.push_back( 510.99895000);
    refEnergy.push_back( 778.903);
    refEnergy.push_back( 867.390);
    refEnergy.push_back( 964.055);
    refEnergy.push_back(1085.842);
    refEnergy.push_back(1112.087);
    refEnergy.push_back(1408.022);
  }
  
  if( option == -5 ){ ///==== 133Ba, gamma
    refEnergy.clear();
    refEnergy.push_back(  80.998);
    refEnergy.push_back( 276.339);
    refEnergy.push_back( 302.851);
    refEnergy.push_back( 356.013);
    refEnergy.push_back( 383.849);
  }
  
  if( option == -6 ){ ///==== 226Ra, gamma
    refEnergy.clear();
    refEnergy.push_back(  186.221);
    refEnergy.push_back(  262.27);
    refEnergy.push_back(  414.60);
    refEnergy.push_back(  449.37);
    refEnergy.push_back(  600.66);
  }
  
  if( option == -7 ){ ///==== 16N decay gamme
    refEnergy.clear();
    refEnergy.push_back(  120.42);
    //refEnergy.push_back( 2741.50);
    refEnergy.push_back( 510.99895000);
    refEnergy.push_back( 5106.62);
    refEnergy.push_back( 5617.62);
    refEnergy.push_back( 6128.63);
    //refEnergy.push_back( 7115.15);
  }
  
  //==================== adjusting energy
  int n = refEnergy.size();
  for( int k = 0; k < n; k++) printf("%2d-th peak : %f \n", k,  refEnergy[k]);

  TCanvas * cFit = new TCanvas ("cFit", Form("RUN:%03d", runID), 100, 100, size[0]*Div[0], size[1]*Div[1]);
  cFit->Divide(Div[0],Div[1]);
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
  for( int i = 0; i < numDet; i ++){
    printf("------- refID - %d \n", i);
    
    if( energy[i].size() == 0 || detID[i] == option) {
      a0[i] = 0;
      a1[i] = 1;
      printf("skipped\n");
      continue;
    }

    nPeaks = energy[i].size();
    
    for( int k = 0; k < (int) energy[i].size(); k++){ printf("%.1f, ", energy[i][k]);};printf("\n");
    vector<vector<double>> output =  FindMatchingPair(energy[i], refEnergy);
 
    vector<double> haha1 = output[0];
    vector<double> haha2 = output[1];
    
    TGraph * graph = new TGraph(haha1.size(), &haha1[0], &haha2[0] );
    cFit->cd(i+1);
    graph->Draw("A*");

    TF1 * fit = new TF1("fit", "pol1" );
    graph->Fit("fit", "q");

    a0[i] = fit->GetParameter(0);
    a1[i] = fit->GetParameter(1);

    printf("%2d | a0: %16.8f, a1: %16.8f (%16.8f) \n", i, a0[i], a1[i], 1./a1[i]);


    text.DrawLatex(0.15, 0.8, Form("a0: %16.8f\n", a0[i]));
    text.DrawLatex(0.15, 0.7, Form("a1: %16.8f\n", a1[i]));
  
    cFit->Update();
    gSystem->ProcessEvents();

  }
    
  
  //====== Plot adjusted spectrum
  TCanvas * cAux = new TCanvas ("cAux", Form("Aux RUN:%03d", runID), 200, 200, 1200, 600);
  cAux->cd(1)->SetLogy();

  
  if( -9 < option && option <= -2 ){
    energyRange[1] = refEnergy.front()*.5;
    energyRange[2] = refEnergy.back()*1.1;
    energyRange[0] = energyRange[2]-energyRange[1];
  }
  
  
  TLegend * legend = new TLegend(0.7,0.6,0.89,0.8);
  double yMax = 0;
  TH1F ** p = new TH1F*[numDet];
  
  for ( int i = 0; i < numDet; i ++){

    p[i] = new TH1F(Form("p%d", detID[i]), detName[i],  energyRange[0], energyRange[1], energyRange[2]);
    p[i]->SetXTitle("keV");
    p[i]->SetYTitle("count / keV");
    p[i]->SetLineColor(i+1);

    TString expression;
    //expression.Form("e[%d] * %.8f + %.8f >> p%d", detID[i], a1[i], a0[i], detID[i]);
    expression.Form("rdt[%d] * %.8f + %.8f >> p%d", detID[i], a1[i], a0[i], detID[i]);
    //expression.Form("ring[%d] * %.8f + %.8f >> p%d", detID[i], a1[i], a0[i], detID[i]);
    //gate[i].Form("ch == %d", detID[i]);
    
    tree->Draw(expression, gate[i] , "");

    legend->AddEntry(p[i], detName[i]);
    
    double yM = p[i]->GetMaximum();
    if( yM > yMax ) yMax = yM;
    
    cAux->Update();
    gSystem->ProcessEvents();

  }
  
  cAux->cd(1);
  for( int  i = 0; i < numDet; i++){
    p[i]->SetMaximum( yMax * 1.1);
    p[i]->Draw("same");
    gSystem->ProcessEvents();
  }
  
  legend->Draw();

  //gROOT->ProcessLine(".q");

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

/*#####################################################################*/
/*#####################################################################*/
/*#####################################################################*/
