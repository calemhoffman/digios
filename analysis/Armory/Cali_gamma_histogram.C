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

//===============
//const int numDet = 3;
//
//int detID[numDet] = {2,3,5}; 
//
//float energyRange[3] = {1800, 200, 2000}; // bin, min, max

//================ prototype
int nPeaksFpeaks = 16;
Double_t fpeaks(Double_t *x, Double_t *par);
vector<vector<double>> combination(vector<double> arr, int r);
double* sumMeanVar(vector<double> data);
double*  fitSlopeIntercept(vector<double> dataX, vector<double> dataY);
vector<vector<double>> FindMatchingPair(vector<double> en1, vector<double> en2);


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

/*#####################################################################*/
/*#####################################################################*/
/*#####################################################################*/

Double_t fpeaks(Double_t *x, Double_t *par) {
  Double_t result = 0;
  for (Int_t p=0;p<nPeaksFpeaks;p++) {
    Double_t norm  = par[3*p+0];
    Double_t mean  = par[3*p+1];
    Double_t sigma = par[3*p+2];
    result += norm * TMath::Gaus(x[0],mean,sigma, 1);
  }
  return result;
}

vector<vector<double>> combination(vector<double> arr, int r){
  
  vector<vector<double>> output;
  
  int n = arr.size();
  std::vector<int> v(n);
  std::fill(v.begin(), v.begin()+r, 1);
  do {
    //for( int i = 0; i < n; i++) { printf("%d ", v[i]); }; printf("\n");
    
    vector<double> temp;
    for (int i = 0; i < n; ++i) { 
      if (v[i]) {
        //printf("%.1f, ", arr[i]); 
        temp.push_back(arr[i]);
      }
    }
    //printf("\n");
    
    output.push_back(temp);
    
  } while (std::prev_permutation(v.begin(), v.end()));
  
  return output;
}

double* sumMeanVar(vector<double> data){
  
  int n = data.size();
  double sum = 0;
  for( int k = 0; k < n; k++) sum += data[k];
  double mean = sum/n;
  double var = 0;
  for( int k = 0; k < n; k++) var += pow(data[k] - mean,2);
  
  static double output[3];
  output[0] = sum;
  output[1] = mean;
  output[2] = var;
  
  return output;
}


double*  fitSlopeIntercept(vector<double> dataX, vector<double> dataY){
  
  double * smvY = sumMeanVar(dataY);
  double sumY = smvY[0];
  double meanY = smvY[1];

  double * smvX = sumMeanVar(dataX);
  double sumX = smvX[0];
  double meanX = smvX[1];
  double varX = smvX[2];
  
  int n = dataX.size();
  double sumXY = 0;
  for( int j = 0; j < n; j++) sumXY += dataX[j] * dataY[j];

  double slope = ( sumXY - sumX * sumY/n ) / varX;
  double intercept = meanY - slope * meanX;
  
  static double output[2];
  output[0] = slope;
  output[1] = intercept;
  
  return output;
  
}


vector<vector<double>> FindMatchingPair(vector<double> enX, vector<double> enY){
   
   int nX = enX.size();
   int nY = enY.size();
   
   vector<double> fitEnergy;
   vector<double> refEnergy;
   
   if( nX > nY ){
      
      vector<vector<double>> output = combination(enX, nY);
      
      double * smvY = sumMeanVar(enY);
      double sumY = smvY[0];
      double meanY = smvY[1];
      double varY = smvY[2];
      
      double optRSquared = 0;
      double absRSqMinusOne = 1;
      int maxID = 0;
      
      for( int k = 0; k < output.size(); k++){
        
        double * smvX = sumMeanVar(output[k]);
        double sumX = smvX[0];
        double meanX = smvX[1];
        double varX = smvX[2];
        
        double sumXY = 0;
        for( int j = 0; j < nY; j++) sumXY += output[k][j] * enY[j];
        
        double rSq = abs(sumXY - sumX*sumY/nY)/sqrt(varX*varY);
        
        //for( int j = 0; j < nY ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = output[maxID];
      refEnergy = enY;
      
      printf(" R^2 : %.20f\n", optRSquared);      
      
      //calculation fitting coefficient
      //double * si = fitSlopeIntercept(fitEnergy, refEnergy);
      //printf( " y = %.4f x + %.4f\n", si[0], si[1]);
      
    }else if( nX < nY ){
    
      vector<vector<double>> output = combination(enY, nX);
      
      
      double * smvX = sumMeanVar(enX);
      double sumX = smvX[0];
      double meanX = smvX[1];
      double varX = smvX[2];
      
      double optRSquared = 0;
      double absRSqMinusOne = 1;
      int maxID = 0;
      
      for( int k = 0; k < output.size(); k++){
        
        double * smvY = sumMeanVar(output[k]);
        double sumY = smvY[0];
        double meanY = smvY[1];
        double varY = smvY[2];
        
        double sumXY = 0;
        for( int j = 0; j < nX; j++) sumXY += output[k][j] * enX[j];
        
        double rSq = abs(sumXY - sumX*sumY/nX)/sqrt(varX*varY);
        
        //for( int j = 0; j < nX ; j++){ printf("%.1f, ", output[k][j]); }; printf("| %.10f\n", rSq);
        
        if( abs(rSq-1) < absRSqMinusOne ) {
          absRSqMinusOne = abs(rSq-1);
          optRSquared = rSq;
          maxID = k;
        }
      }
      
      fitEnergy = enX;
      refEnergy = output[maxID];
      printf(" R^2 : %.20f\n", optRSquared);   
    
    }else{
      fitEnergy = enX;
    }
    
    
    printf("fitEnergy = ");for( int k = 0; k < fitEnergy.size() ; k++){ printf("%7.2f, ", fitEnergy[k]); }; printf("\n");
    printf("refEnergy = ");for( int k = 0; k < refEnergy.size() ; k++){ printf("%7.2f, ", refEnergy[k]); }; printf("\n");
    
    vector<vector<double>> haha;
    haha.push_back(fitEnergy);
    haha.push_back(refEnergy);
    
    return haha;
   
}

