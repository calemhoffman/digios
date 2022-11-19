/***************************************************
 * This is a root macro for Auto fitting
 *
 * Created by Tsz Leung (Ryan) TANG, around 2019.
 *                            updated 01-04-2022
 *
 * contact goluckyryan@gmail.com
 ***************************************************/

#ifndef AutoFit_C
#define AutoFit_C

#include <TF1.h>
#include <TGraph.h>
#include <TColor.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <TRandom.h>
#include <TMarker.h>
#include <vector>

//Global fit paramaters

vector<double> BestFitMean;
vector<double> BestFitCount;
vector<double> BestFitSigma;

TString recentFitMethod;

void ShowFitMethod(){
  printf("----------------------- Method of Fitting ---------------\n");
  printf("---------------------------------------------------------\n");
  printf("         fitAuto() - estimate BG, find peak, and fit n-Gauss \n");
  printf("     fitGaussPol() - fit 1 Gauss + pol-n BG\n");
  printf("     fit2GaussP1() - fit 2 Gauss + pol-1 BG \n");
  printf("       fitGF3Pol() - fit GF3 + pol-n BG \n");
  //printf("         fitNGF3() - fit n-GF3, estimated BG \n");
  printf("       fitNGauss() - fit n-Gauss, estimated BG, need input\n");
  printf("    fitNGaussSub() - fit estimated BG with Pol, subtract, fit n-Guass\n");
  printf("    fitNGaussPol() - fit n-Gauss + pol-n BG \n");
  printf(" fitNGaussPolSub() - subtract Pol-n BG, fit n-Gauss \n");
  printf("\n");
  printf("------- Mouse click Fit : \n");
  printf("    clickFitNGaussPol() - fit n-Gauss + pol-n BG \n");
  printf(" clickFitNGaussPolSub() - Fit Pol-n BG, subtract, fit n-Gauss\n");
  printf("\n");
  printf("------- Utility : \n");
  printf("          SaveFitPara() - Save the inital/Best fit parameters.\n");
  printf("        ShowFitMerhod() - Show this menual.\n");
  printf("---------------------------------------------------------\n");
}

void AutoFit(){
  ShowFitMethod();
}

std::vector<std::string> SplitStrAF(std::string tempLine, std::string splitter, int shift = 0){

  std::vector<std::string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    std::string secStr;
    if( pos == std::string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    if( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());
    
  }while(pos != std::string::npos );

  return output;
}

TColor RGBWheel(double ang){

  ang = ang * TMath::DegToRad();
  
  double r = max(0., (1+2*cos(ang))/3.);
  double g = max(0., (1 - cos(ang) + sqrt(3)* sin(ang))/3.);
  double b = max(0., (1 - cos(ang) - sqrt(3)* sin(ang))/3.);

  TColor col(r,g,b);

  return col;

}

int nPeaks = 16;
Double_t nGauss(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1); // normalized Gaussian
   }
   return result;
}

int nPols = 1;

Double_t nPolFunc(Double_t *x, Double_t *par) {
  Double_t result = 0;
  
  for( int p = 0; p < nPols+1; p++){
    result += par[p]*TMath::Power(x[0], p);
  }
  return result;
}

Double_t nGaussPol(Double_t *x, Double_t *par) {

  Double_t result = 0;
  for (Int_t p=0;p<nPeaks;p++) {
    Double_t norm  = par[3*p+0];
    Double_t mean  = par[3*p+1];
    Double_t sigma = par[3*p+2];
    result += norm * TMath::Gaus(x[0],mean,sigma, 1); // normalized Gaussian
  }

  for( int p = 0; p < nPols+1; p++){
    result += par[3*nPeaks + p]*TMath::Power(x[0], p);
  }

  return result;
}

Double_t nGF3(Double_t *x, Double_t *par){

  /** this is the fitting function for gamma peak from gf3, RadWare **/
  Double_t result = 0;
  for( Int_t p = 0; p < nPeaks; p++){
    
    Double_t norm  = par[6*p+0];
    Double_t mean  = par[6*p+1];
    Double_t sigma = par[6*p+2];
    Double_t ratio = par[6*p+3];
    Double_t beta  = par[6*p+4]; // skewness
    Double_t step  = par[6*p+5];
    
    result += norm * (1-ratio)* TMath::Gaus(x[0], mean, sigma, 1) ;
    result += norm * ratio * exp( sigma * sigma/2/beta/beta) / (2* beta )* exp((x[0]-mean)/beta) * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) + sigma/beta/sqrt(2)) ;
    result += norm * step * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) );
  }  
  return result;
}


Double_t nGF3Pol(Double_t *x, Double_t *par){

  /** this is the fitting function for gamma peak from gf3, RadWare **/
  Double_t result = 0;
  for( Int_t p = 0; p < nPeaks; p++){
    Double_t norm  = par[6*p+0];
    Double_t mean  = par[6*p+1];
    Double_t sigma = par[6*p+2];
    Double_t ratio = par[6*p+3];
    Double_t beta  = par[6*p+4]; // skewness
    Double_t step  = par[6*p+5];

    result += norm * (1.0-ratio)* TMath::Gaus(x[0], mean, sigma, 1) ;
    result += norm * ratio * exp( sigma * sigma/2/beta/beta) / (2* beta )* exp((x[0]-mean)/beta) * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) + sigma/beta/sqrt(2)) ;
    result += norm * step * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) );
  }

  for( int p = 0; p < nPols + 1; p++){
    result += par[6*nPeaks + p]*TMath::Power(x[0], p);
  }
  
  return result;
}

void PrintPar(TF1 * fit, int numParPerPeak){
   int totPar = fit->GetNpar();
   int count = totPar/numParPerPeak;
   printf("ID  ");
   for( int i = 0; i < numParPerPeak; i++){
     printf("par%d       ", i);
   }
   printf("\n");
   
   for( int i = 0; i < count ; i++){
     printf("%3d ", i);
     for( int j = 0; j < numParPerPeak; j++){
       int parID = numParPerPeak * i + j;
       printf("%.3f(%.3f) ", fit->GetParameter(parID), fit->GetParError(parID));
     }
     printf("\n");
   }
}

void GoodnessofFit(TH1F * hist, TF1 * fit){
  
   int nBin = hist->GetNbinsX();
   int effBin = 0;   
   double mean = 0;
   double ysq = 0;
   double SSR = 0;
   double chisq = 0; //with estimated error be sqrt(y)
   double Xsq = 0; // for Pearson's chi-sq test
   for( int i = 1; i <= nBin; i++){

      double e = hist->GetBinError(i);
      if( e > 0  ) {
         effBin ++;
         double y = hist->GetBinContent(i);
         double x = hist->GetBinCenter(i);
         double ybar = fit->Eval(x);
         ysq += y*y;
         mean += y;
         SSR += (y - ybar)*(y-ybar);
         chisq += (y - ybar)*(y-ybar)/e/e;
         
         
         if( ybar > e ) {
           Xsq += (y - ybar)*(y-ybar)/ybar;
         }else{
           Xsq += (y - ybar)*(y-ybar)/e;
         }
         //printf(" %d | x : %f, y : %f, ybar : %f , X-sq : %f\n", i, x, y, ybar, Xsq);
      }
   }
   mean = mean / nBin;
   double SSTotal = ysq + mean*mean;
   
   int npar = fit->GetNpar();
   int ndf = effBin - npar;
   printf("#################################################\n");
   printf("##               Goodness of Fit.              ##\n");
   printf("#################################################\n");
   printf(" eff. Bin(%d) - numPar(%d) = ndf = %d \n", effBin, npar, ndf);

   printf("============== Regression analysis\n");
   printf("----------------- R-sq \n");
   printf("    SSTotal = %f \n", SSTotal);
   printf("        SSR = %f \n", SSR);
   printf("        MSR = %f  <-- is it similar to sample variance?\n", SSR/ndf);
   double Rsq = 1 - SSR/SSTotal;
   printf("       R-sq = %f \n", Rsq );
   
   printf("----------------- Chi-sq \n");
   printf("     Chi-sq = %f \n", chisq);
   printf(" rd. Chi-sq = %f \n", chisq/ndf);
   printf("ROOT Chi-Sq = %f , NDF = %d \n", fit->GetChisquare(), fit->GetNDF());
   //================ chi-sq test
   printf("============== Hypothesis testing\n");
   printf(" Null Hypothesis : the fitting model is truth. \n");
   printf(" * p-value = prob. that Null Hypo. is truth. \n");
   printf(" * the Pearson's test in here only for background free data \n");
   printf("     Pearson's X-sq = %.2f \n", Xsq);
   double p = 1 - TMath::Prob(Xsq, ndf);
   printf("  Pearson's p-value = %.2f %s 0.05 | %s\n", p, p < 0.05 ? "<": ">", p < 0.05 ? "reject" : "cannot reject");
   double pchi = 1 - TMath::Prob(chisq, ndf);
   printf("     Chi-sq p-value = %.2f %s 0.05 | %s\n", pchi, pchi < 0.05 ? "<": ">", pchi < 0.05 ? "reject" : "cannot reject");
   double pRoot = 1- fit->GetProb();
   printf("ROOT Chi-sq p-value = %.2f %s 0.05 | %s\n", pRoot, pRoot < 0.05 ? "<": ">", pRoot < 0.05 ? "reject" : "cannot reject");
   printf("################################################\n");  
}

vector<double> energy, height, sigma, lowE, highE ;
vector<int> energyFlag, sigmaFlag;

bool loadFitParameters(TString fitParaFile){
  
  energy.clear(); energyFlag.clear();
  sigma.clear();  sigmaFlag.clear();
  
  lowE.clear(); highE.clear();
  
  height.clear();
  
  bool paraRead = false;

  printf("====================================================================== \n");
  printf("----- loading fit parameters from : %s", fitParaFile.Data());
  ifstream file;
  file.open(fitParaFile.Data());

  if( !file){
    printf("\ncannot read file : %s \n", fitParaFile.Data());
    return 0;
  }

  while( file.good()) {
    string tempLine;
    getline(file, tempLine);

    if( tempLine.substr(0, 1) == "#" ) continue;
    if( tempLine.substr(0, 2) == "//" ) continue;
    if( tempLine.size() < 5 ) continue;
    
    ///printf("%s\n", tempLine.c_str());

    vector<string> temp = SplitStrAF(tempLine, " ");

    if( temp.size() < 7 ) continue;

    energy.push_back(    atof(temp[0].c_str()));
    lowE.push_back(      atof(temp[1].c_str()));
    highE.push_back(     atof(temp[2].c_str()));
    energyFlag.push_back(atoi(temp[3].c_str()));         
    sigma.push_back(     atof(temp[4].c_str()));
    sigmaFlag.push_back( atoi(temp[5].c_str()));
    height.push_back(    atof(temp[6].c_str()));

  }

  printf("... done.\n");
    
  int n = energy.size();
  TString limStr = "(fixed)";
  printf("%2s| %34s | %10s \n", "ID", "Peak [MeV]", "Sigma [MeV]");
  for( int j = 0; j < n; j ++){    
    if( energyFlag[j] == 0 ) limStr.Form("(limited, %6.3f - %6.3f)", lowE[j], highE[j]);
    printf("%2d| %7.4f %-26s | %7.4f (%5s) \n", j, energy[j], limStr.Data(), sigma[j], sigmaFlag[j] == 1 ? "fixed" : "free"); 
  }
    
  paraRead = true;

  printf("====================================================================== \n");

  return  paraRead;
  
}

TCanvas * NewCanvas(TString name, TString title, int divX, int divY, int padSizeX, int padSizeY){
  TCanvas * output = NULL;
  if( gROOT->FindObjectAny(name) == NULL ){
    output = new TCanvas(name, title, divX * padSizeX, divY * padSizeY);
  }else{
    output = (TCanvas *) gROOT->FindObjectAny(name) ;
    output->Clear();
  }
  output->Divide(divX, divY);
  return output;
}

void ScaleAndDrawHist(TH1F * hist, double xMin, double xMax){

  if ( xMin != xMax ) hist->GetXaxis()->SetRangeUser(xMin, xMax);
  int maxBin = hist->GetMaximumBin();
  double ymax = hist->GetBinContent(maxBin);
  hist->GetYaxis()->SetRangeUser(0, 1.1 * ymax);
  hist->Draw();
  
}

void PlotResidual(TH1F * hist, TF1 * fit){

  TH1F * hRes = (TH1F*) hist->Clone();
  hRes->GetListOfFunctions()->Clear();
  hRes->SetTitle("Residual");
  hRes->SetName("hRes");
  hRes->SetYTitle("Hist - fit");
  hRes->Sumw2(0);
  hRes->Sumw2(1);
  hRes->Add(fit, -1);
  hRes->Draw();
  
}


//########################################
//### Fit a Gauss + Pol-n
//########################################
void fitGaussPol(TH1F * hist, double mean, double sigmaMax, int degPol, double xMin, double xMax, TString optStat = ""){

  printf("=========================================================\n");
  printf("================ fit 1-Gauss + Pol-%d BG ================\n", degPol);
  printf(" * mean Range  +- 5 sigmaMax \n");
  printf(" * inital parameters of the polynomial is random/pow(10, i) \n"); 
  printf("==========================================================\n");

  recentFitMethod = "fitGaussPol";
  
  gStyle->SetOptStat(optStat);
  TCanvas * cFitGaussPol = NewCanvas("cFitGaussPol", Form("fit Gauss & Pol-%d | fitGaussPol", degPol), 1, 2, 800, 350);  
  cFitGaussPol->cd(1);
  
  ScaleAndDrawHist(hist, xMin, xMax);

  const int nPar = 3 + degPol + 1;

  TString funcExp = "[0] * TMath::Gaus(x, [1], [2], 1)";
  for( int i = 0; i < degPol+1 ; i++){
    funcExp += Form(" + [%d]*TMath::Power(x,%d)", i+3 , i);
  }  
  TF1 * fit = new TF1("fit", funcExp.Data(), xMin, xMax);

  double * para = new double[nPar]; 
  para[0] = 100 * 0.05 * TMath::Sqrt(TMath::TwoPi());
  para[1] = mean;
  para[2] = sigmaMax/2.;
  for( int i = 0 ; i < degPol+1; i++){
    para[3 + i ] = gRandom->Rndm()/TMath::Power(10, i);
  }

  fit->SetLineWidth(2);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  fit->SetParLimits(0, 0, 1e9);
  fit->SetParLimits(1, mean - 5*sigmaMax, mean + 5 * sigmaMax);
  fit->SetParLimits(2, 0, sigmaMax);

  hist->Fit("fit", "Rq");

  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  double bw = hist->GetBinWidth(1);

  printf("histogram name : %s \n====== Gaussian:\ncount: %8.0f(%3.0f)\nmean : %8.4f(%8.4f)\nsigma: %8.4f(%8.4f) \n", 
            hist->GetName(),
            paraA[0] / bw,   paraE[0] /bw, 
            paraA[1], paraE[1],
            paraA[2], paraE[2]);

  printf("------- the polnomail BG:\n");
  for(int i = 0 ; i < degPol+1; i++){
    printf("%2d | %8.4f(%8.4f) \n", i, paraA[3+i], paraE[3+i]);
  }
                        
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  
  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.DrawLatex(0.12, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));
  text.DrawLatex(0.12, 0.75,Form("count: %4.0f(%3.0f)",  paraA[0] / bw,   paraE[0] /bw));
  text.DrawLatex(0.12, 0.70,Form("E_{x}: %6.3f(%5.3f) MeV",  paraA[1], paraE[1]));
  text.DrawLatex(0.12, 0.65,Form("#sigma: %3.0f(%3.0f) keV",  paraA[2] * 1000., paraE[2] * 1000.));

  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.60, 0.85 - 0.05*i ,Form("%3s: %8.3f(%8.3f)", Form("p%d", i),  paraA[3+i], paraE[3+i]));
  }

  TString expression = "[0] ";
  for( int j = 1; j < degPol + 1; j++){
    expression += Form(" + [%d]*TMath::Power(x, %d)", j, j);
  }
  TF1 * g0 = new TF1("g0", expression.Data(), xMin, xMax);
  for( int j = 0; j < degPol + 1 ; j++){
    g0->SetParameter(j, paraA[3+j]);
  }
  g0->SetLineColor(4);
  g0->Draw("same");

  TF1* f0 = new TF1("f0", "[0] * TMath::Gaus(x, [1], [2], 1)", xMin, xMax);
  f0->SetParameter(0, paraA[0]);
  f0->SetParameter(1, paraA[1]);
  f0->SetParameter(2, paraA[2]);
  f0->SetLineColor(2);
  f0->SetNpx(1000);
  f0->Draw("same");

//  GoodnessofFit(hist, fit);

  cFitGaussPol->cd(2);
  PlotResidual(hist, fit);

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();

  BestFitCount.push_back(paraA[0]);
  BestFitMean.push_back(paraA[1]);
  BestFitSigma.push_back(paraA[2]);

}

//########################################
//#### fit 2 gauss + pol-1  // not updated
//########################################
vector<double> fit2GaussP1(TH1F * hist, double mean1, double sigma1, 
                                       double mean2, double sigma2, 
                           double xMin, double xMax, TString optStat = "", bool newCanvas = false){


  printf("=========================================================\n");
  printf("================ fit 2-Gauss + Pol-1 BG ================\n" );
  printf(" NOT updated. It works, but the code is old \n"); 
  printf("==========================================================\n");

  recentFitMethod = "fit2GaussP1";
  
  vector<double> output;
  output.clear();

  gStyle->SetOptStat(optStat);  
  TCanvas * cFit2GaussP1 = NewCanvas("cFit2GaussP1", "fit Gauss & P1  | fit2GaussP1", 1, 1, 800, 350);
  cFit2GaussP1->cd(1);

  ScaleAndDrawHist(hist, xMin, xMax);
  
  TF1 * fit = new TF1("fit", "[0] * TMath::Gaus(x, [1], [2], 1) + [3] * TMath::Gaus(x, [4], [5], 1) + [6] + [7]*x", xMin, xMax);
  
  double * para = new double[8]; 
  para[0] = 20 * 0.05 * TMath::Sqrt(TMath::TwoPi());
  para[1] = mean1;
  para[2] = sigma1;
  para[3] = 100 * 0.05 * TMath::Sqrt(TMath::TwoPi());
  para[4] = mean2;
  para[5] = sigma2;
  para[6] = 1;
  para[7] = 0;

  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  hist->Fit("fit", "Rq");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  double bw = hist->GetBinWidth(1);

  printf("%7s ====== count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            hist->GetName(),
            paraA[0] / bw,   paraE[0] /bw, 
            paraA[1], paraE[1],
            paraA[2], paraE[2]);
  printf("%7s ====== count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            "",
            paraA[3] / bw,   paraE[3] /bw, 
            paraA[4], paraE[4],
            paraA[5], paraE[5]);
            
  output.push_back( paraA[0]/bw);
  output.push_back( paraE[0]/bw);
  output.push_back( paraA[1]);
  output.push_back( paraE[1]);
  output.push_back( paraA[2]);
  output.push_back( paraE[2]);

  output.push_back( paraA[3]/bw);
  output.push_back( paraE[3]/bw);
  output.push_back( paraA[4]);
  output.push_back( paraE[4]);
  output.push_back( paraA[5]);
  output.push_back( paraE[5]);
            
            
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  
  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  text.DrawLatex(0.15, 0.75,Form("count: %4.0f(%3.0f), E_{x}: %6.3f(%5.3f) MeV, #sigma: %3.0f(%3.0f) keV ", 
                                    paraA[0] / bw,   paraE[0] /bw,
                                    paraA[1], paraE[1],
                                    paraA[2] * 1000., paraE[2] * 1000.));
  text.DrawLatex(0.15, 0.7, Form("count: %4.0f(%3.0f), E_{x}: %6.3f(%5.3f) MeV, #sigma: %3.0f(%3.0f) keV  ", 
                                    paraA[3] / bw,   paraE[3] /bw,
                                    paraA[4], paraE[4],
                                    paraA[5] * 1000., paraE[5] * 1000.));
                                    
  text.DrawLatex(0.15, 0.6, Form("Line : %6.3f(%5.3f) + %6.3f(%5.3f)x ",
                                    paraA[6], paraE[6],
                                    paraA[7], paraE[7]));
                                    
  GoodnessofFit(hist, fit);


  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < 2; i++){
    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
  
  return output;
}


//########################################
//#### fit for gamma + pol-n BG
//########################################
void fitGF3Pol(TH1F * hist, double mean, double sigmaMax, double ratio, double beta, double step, int degPol, double xMin, double xMax, TString optStat = ""){

  printf("=========================================================\n");
  printf("================ fit GF1 + Pol-%d BG ================\n", degPol);
  printf(" * mean Range  = xMin, xMax \n");
  printf(" * inital parameters of the polynomial is random/pow(10, i) \n"); 
  printf("==========================================================\n");

  recentFitMethod = "fitGF3Pol";

  gStyle->SetOptStat(optStat);

  gStyle->SetOptStat(optStat);  
  TCanvas * cFitGF3Pol = NewCanvas("cFitGF3Pol", Form("fit GF3 + pol-%d | fitGF3Pol", degPol), 1, 2, 800, 350);
  cFitGF3Pol->cd(1);

  ScaleAndDrawHist(hist, xMin, xMax);
  
  nPeaks = 1;
  nPols = degPol;
  int nPar = 6*nPeaks + degPol + 1;

  TF1 * fit = new TF1("fit", nGF3Pol, xMin, xMax, nPar);

  fit->Print();

  double * para = new double[nPar]; 
  para[0] = hist->GetMaximum() *4;
  para[1] = mean;
  para[2] = sigmaMax/2.;
  para[3] = ratio ;
  para[4] = beta ;
  para[5] = step ;
  for( int i = 0 ; i < degPol + 1; i++){
    para[6+i] = gRandom->Rndm()/TMath::Power(10, i);
  }
  
  fit->SetLineWidth(2);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  fit->SetParLimits(0, 0, 1e9);
  fit->SetParLimits(1, xMin, xMax);
  fit->SetParLimits(2, 0.00000001, sigmaMax);
  fit->SetParLimits(3, 0, 0.5);
  fit->SetParLimits(4, 1, 400);
  fit->SetParLimits(5, 0, 0.5);
  
  hist->Fit("fit", "Rq");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  double chisquare = fit->GetChisquare();
  int ndf = fit->GetNDF();
  double bw = hist->GetBinWidth(1);

  printf("histogram : %s \n", hist->GetName());
  printf("========= The Gaussian \n");
  printf("count: %8.0f(%3.0f)\n",  paraA[0] / bw,   paraE[0] /bw);
  printf("mean : %8.4f(%8.4f)\n",  paraA[1], paraE[1]);
  printf("sigma: %8.4f(%8.4f)\n",  paraA[2], paraE[2]);
            
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(1);
  
  text.DrawLatex(0.12, 0.65, Form("count : %5.0f(%5.0f)", paraA[0]/bw, paraE[0]/bw));
  text.DrawLatex(0.12, 0.60, Form(" mean : %5.3f(%5.3f) keV", paraA[1], paraE[1]));
  text.DrawLatex(0.12, 0.55, Form("sigma : %5.3f(%5.3f) keV", paraA[2], paraE[2]));
  text.DrawLatex(0.12, 0.50, Form(" FWHM : %5.3f(%5.3f) keV", paraA[2] *2.355, paraE[2]*2.355));
   
  text.DrawLatex(0.12, 0.40, Form("#chi^2/ndf : %5.3f", chisquare/ndf));
   
   //GoodnessofFit(hist, fit);

  ///                  0       1        2        3       4       5   
  string label[8] = {"Area", "mean", "sigma", "ratio", "beta", "step"};
  printf("---------- The detail\n");
  for(int i = 0 ; i < 6 ; i++){
    printf("%d | %8s | %f (%f) \n", i, label[i].c_str(), paraA[i], paraE[i]);
    text.DrawLatex(0.65, 0.85-0.05*i, Form("%6s: %5.3f(%5.3f)", label[i].c_str(), paraA[i], paraE[i]));
  }
  for(int i = 6 ; i < nPar; i++){
    printf("%d | %8s | %f (%f) \n", i, Form("p%d", i-6), paraA[i], paraE[i]);
    text.DrawLatex(0.65, 0.85-0.05*i, Form("%6s: %5.3f (%5.3f) \n", Form("p%d", i-6), paraA[i], paraE[i]));
  }

  /// norm * (1-ratio)* TMath::Gaus(x[0], mean, sigma, 1)
  TF1 * g0 = new TF1("g0", "[0] * (1.0-[3]) * TMath::Gaus(x, [1], [2], 1)", xMin, xMax);
  g0->SetParameter(0, paraA[0]);
  g0->SetParameter(1, paraA[1]);
  g0->SetParameter(2, paraA[2]);
  g0->SetParameter(3, paraA[3]);
  g0->SetNpx(1000);
  g0->SetLineColor(kRed);

  ///                     norm * ratio * exp( sigma * sigma/2/beta/beta)* exp((x[0]-mean)/beta) * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) + sigma/beta/sqrt(2)) ;
  TF1 * g1 = new TF1("g1", "[0] * [3] * exp( [2] * [2]/2/[4]/[4]) / (2* [4])* exp((x-[1])/[4]) * TMath::Erfc( (x-[1])/([2] * sqrt(2)) + [2]/[4]/sqrt(2)) ", xMin, xMax);
  g1->SetParameter(0, paraA[0]);
  g1->SetParameter(1, paraA[1]);
  g1->SetParameter(2, paraA[2]);
  g1->SetParameter(3, paraA[3]);
  g1->SetParameter(4, paraA[4]);
  g1->SetNpx(1000);
  g1->SetLineColor(kGreen +3);

  /// norm * step * TMath::Erfc( (x[0]-mean)/(sigma * sqrt(2)) );
  TF1 * g2 = new TF1("g2", "[0] * [3] * TMath::Erfc( (x-[1])/([2] * sqrt(2)) );", xMin, xMax);
  g2->SetParameter(0, paraA[0]);
  g2->SetParameter(1, paraA[1]);
  g2->SetParameter(2, paraA[2]);
  g2->SetParameter(3, paraA[5]);
  g2->SetNpx(1000);
  g2->SetLineColor(kViolet);


  TString expression = "[0] ";
  for( int j = 1; j < degPol + 1; j++){
    expression += Form(" + [%d]*TMath::Power(x, %d)", j, j);
  }
  TF1 * g3 = new TF1("g3", expression.Data(), xMin, xMax);
  for( int j = 0; j < degPol + 1 ; j++){
    g3->SetParameter(j, paraA[6+j]);
  }
  g3->SetLineColor(kBlue);
  g3->Draw("same");


  g0->Draw("same");
  g1->Draw("same");
  g2->Draw("same");
  g3->Draw("same");

  cFitGF3Pol->cd(2);
  PlotResidual(hist, fit);
   
}

//##############################################
//##### Auto Fit n-Gauss with estimated BG 
//##############################################
vector<double> fitAuto(TH1F * hist, int bgEst = 10, 
                       double peakThreshold = 0.05, 
                       double sigmaMax = 0, 
                       int peakDensity = 4, 
                       TString optStat = ""){

  printf("================================================================\n");
  printf("========== Auto Fit n-Gauss with estimated BG ==================\n");
  printf(" * bgEst = parameter of BG estimation, larger BG, more linear \n");
  printf(" * peakThreshold = precentage of the highest peak that will count \n");
  printf(" * sigmaMax  =  maximum sigma, if -1, program try to find the sigma \n");
  printf(" * peakDensity = peak will closer when the number is larger ");
  printf(" \n");
  printf(" after peaks found, the i-th peaks will be limited by the mid-point\n");
  printf("   by the (i-1)-th peak and the i-th peak, and the mid-point of the\n");
  printf("    i-th peak and (i+1)-th peak \n");
  printf("   i.e. [peak(i-1)+peak(i)]/2  < limit of peak(i) < [peak(i)+peak(i+1)]/2 \n");
  printf("================================================================\n");

  recentFitMethod = "fitAuto";

  gStyle->SetOptStat(optStat);
  TCanvas *cFitAuto = NewCanvas("cFitAuto","Auto Fitting | fitAuto", 1, 4, 800, 300);
  cFitAuto->cd(1);

  ScaleAndDrawHist(hist, 0, 0);
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitted spectrum (BG=%d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  ///specS->GetXaxis()->SetTitleSize(0.06);
  ///specS->GetYaxis()->SetTitleSize(0.06);
  ///specS->GetXaxis()->SetTitleOffset(0.7);
  ///specS->GetYaxis()->SetTitleOffset(0.6);
  
  //=================== find peak and fit
  gStyle->SetOptFit(0);
  TSpectrum * peak = new TSpectrum(50);
  nPeaks = peak->Search(hist, peakDensity, "", peakThreshold); 
  
  if( bgEst > 0 ) {
    printf("============= estimating background...\n");
    TH1 * h1 = peak->Background(hist, bgEst);
    h1->Draw("same");
    printf("============= substracting the linear background...\n");
    specS->Sumw2();
    specS->Add(h1, -1.);
  }
  
  cFitAuto->cd(2)->SetGrid();
  cFitAuto->cd(2);
  specS->Draw("hist");

  //========== Fitting 
  printf("============= Fitting.....");
  printf(" found %d peaks \n", nPeaks);
  
  double * xpos = peak->GetPositionX();
  double * ypos = peak->GetPositionY();

  int * inX = new int[nPeaks];
  TMath::Sort(nPeaks, xpos, inX, 0 );
  vector<double> energy, height;
  for( int j = 0; j < nPeaks; j++){
    energy.push_back(xpos[inX[j]]);
    height.push_back(ypos[inX[j]]);
  }
  for( int j = 0; j < nPeaks; j++){
    printf(" energy : %f , %f \n", energy[j], height[j]);
  }
  
  
  if( sigmaMax == 0 ){
    printf("------------- Estimate sigma for each peak \n");
    sigma.clear();
    int binMin = hist->FindBin(xMin);
    int binMax = hist->FindBin(xMax);
    for( int i = 0; i < nPeaks ; i++){
      int b0 = hist->FindBin(energy[i]);
      double sMin = (xMax-xMin)/5., sMax = (xMax-xMin)/5.;
      //---- backward search, stop when 
      for( int b = b0-1 ; b > binMin ; b-- ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y  < (height[i])/2. ) {
          sMin = energy[i] - hist->GetBinCenter(b);
          break;
        }
      }
      //---- forward search, stop when 
      for( int b = b0+1 ; b < binMax  ; b++ ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y < (height[i])/2. ) {
          sMax = hist->GetBinCenter(b) - energy[i];
          break;
        }
      }

      double temp = TMath::Min(sMin, sMax);
      /// When there are multiple peaks closely packed : 
      if( i > 0 && temp > 2.5 * sigma.back() ) temp = sigma.back();
      sigma.push_back(temp);
      
      printf("%2d | x : %8.2f | sigma(est) %f \n", i, energy[i], sigma[i]);
    }
  }else if( sigmaMax < 0 ){
    printf("========== use user input sigma : %f (fixed)\n", abs(sigmaMax));
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(abs(sigmaMax));
  }else if( sigmaMax > 0 ){
    printf("========== use user input sigma : %f/2. \n", sigmaMax/2.);
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(sigmaMax/2.);
  }
    
  
  
  int numParPerPeak = 3;  
  const int  n = numParPerPeak * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[numParPerPeak*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[numParPerPeak*i+1] = energy[i];
    if( sigmaMax == 0 ){
      para[numParPerPeak*i+2] = sigma[i];
    }else if(sigmaMax < 0 ){
      para[numParPerPeak*i+2] = abs(sigmaMax);
    }else if(sigmaMax > 0 ){
      para[numParPerPeak*i+2] = sigmaMax/2.;
    }
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3 * nPeaks );
  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  if( nPeaks > 1 ){
    for( int i = 0; i < nPeaks; i++){
      fit->SetParLimits(numParPerPeak*i+0, 0, 1e+9);
      double de1 = 1, de2 = 1;
      if( i == 0 ){ 
        de2 = (energy[i+1] - energy[i])/2.;
        de1 = de2;
      }else if( i < nPeaks -1 ){
        de1 = (energy[i] - energy[i-1])/2.;
        de2 = (energy[i+1] - energy[i])/2.;
      }else{
        de1 = (energy[i] - energy[i-1])/2.;
        de2 = de1;
      }
      
      fit->SetParLimits(numParPerPeak*i+1, energy[i] - de1 , energy[i] + de2);
      if( sigmaMax== 0 ) fit->SetParLimits(numParPerPeak*i+2, 0, 1.5*sigma[i]); // add 50% margin of sigma
      if( sigmaMax < 0 ) fit->FixParameter(numParPerPeak*i+2, abs(sigmaMax));
      if( sigmaMax > 0 ) fit->SetParLimits(numParPerPeak*i+2, 0, sigmaMax);
    }
  }else{
    fit->SetParLimits(0, 0, 1e+9);
    fit->SetParLimits(2, 0, sigmaMax); 
  } 
  
  specS->Fit("fit", "q");
  
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  //======== calculate reduce chi-squared
  //GoodnessofFit(specS, fit);
  
  double bw = specS->GetBinWidth(1);

  vector<double> exPos;
  for(int i = 0; i < nPeaks ; i++){
    exPos.push_back(paraA[numParPerPeak*i+1]);
    printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[numParPerPeak*i] / bw,   paraE[numParPerPeak*i] /bw, 
            paraA[numParPerPeak*i+1], paraE[numParPerPeak*i+1],
            paraA[numParPerPeak*i+2], paraE[numParPerPeak*i+2]);
    
  }
  cFitAuto->Update();

  //draw the indivual fit
  fit->Draw("same");
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[numParPerPeak*i]);
      gFit[i]->SetParameter(1, paraA[numParPerPeak*i+1]);
      gFit[i]->SetParameter(2, paraA[numParPerPeak*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  specS->Draw("hist same");

  //======== print text on plot
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  cFitAuto->cd(3);
  PlotResidual(specS, fit);

  cFitAuto->cd(4);
  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
  
  return exPos;
  
}


//########################################
//###### NOT tested
//########################################
vector<double> fitNGF3(TH1 * hist, int bgEst = 10, 
                       double peakThreshold = 0.1, 
                       double sigmaMax = 20, 
                       int peakDensity = 4, 
                       TString optStat = "", bool newPlot = true){

  TCanvas *cFitAuto = NULL;
  if( newPlot ){
    cFitAuto = new TCanvas("cFitAuto","Auto Fitting", 100, 100, 800,800);
    cFitAuto->Divide(1,2);
    
    gStyle->SetOptStat(optStat);
    cFitAuto->cd(1);
    hist->Draw();
  }

  recentFitMethod = "fitNGF3";
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitted spectrum (BG=%d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  ///specS->GetXaxis()->SetTitleSize(0.06);
  ///specS->GetYaxis()->SetTitleSize(0.06);
  ///specS->GetXaxis()->SetTitleOffset(0.7);
  ///specS->GetYaxis()->SetTitleOffset(0.6);
  
  //=================== find peak and fit
  gStyle->SetOptFit(0);
  TSpectrum * peak = new TSpectrum(50);
  nPeaks = peak->Search(hist, peakDensity, "", peakThreshold); 
  
  if( bgEst > 0 ) {
    printf("============= estimating background...\n");
    TH1 * h1 = peak->Background(hist, bgEst);
    h1->Draw("same");
    printf("============= substracting the linear background...\n");
    specS->Sumw2();
    specS->Add(h1, -1.);
  }
  
  if( newPlot ){
    cFitAuto->cd(2)->SetGrid();
    cFitAuto->cd(2);
  }
  specS->Draw("hist");
  

  //========== Fitting 
  if( newPlot ){
    printf("============= Fitting.....");
    printf(" found %d peaks \n", nPeaks);
  }
  double * xpos = peak->GetPositionX();
  double * ypos = peak->GetPositionY();

  int * inX = new int[nPeaks];
  TMath::Sort(nPeaks, xpos, inX, 0 );
  vector<double> energy, height;
  for( int j = 0; j < nPeaks; j++){
    energy.push_back(xpos[inX[j]]);
    height.push_back(ypos[inX[j]]);
  }
  if( newPlot ){
    for( int j = 0; j < nPeaks; j++){
      printf(" energy : %f , %f \n", energy[j], height[j]);
    }
  }
  
  int numParPerPeak = 6;  
  const int  n = numParPerPeak * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[numParPerPeak*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[numParPerPeak*i+1] = energy[i];
    para[numParPerPeak*i+2] = sigmaMax;
    para[numParPerPeak*i+3] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi()) * 0.1;
    para[numParPerPeak*i+4] = sigmaMax;
    para[numParPerPeak*i+5] = -4;
  }

  TF1 * fit = new TF1("fit", nGF3, xMin, xMax, numParPerPeak * nPeaks );
  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  if( nPeaks > 1 ){
    for( int i = 0; i < nPeaks; i++){
      fit->SetParLimits(numParPerPeak*i+0, 0, 1e+9);
      double de1 = 1, de2 = 1;
      if( i == 0 ){ 
        de2 = (energy[i+1] - energy[i])/2.;
        de1 = de2;
      }else if( i < nPeaks -1 ){
        de1 = (energy[i] - energy[i-1])/2.;
        de2 = (energy[i+1] - energy[i])/2.;
      }else{
        de1 = (energy[i] - energy[i-1])/2.;
        de2 = de1;
      }
      
      fit->SetParLimits(numParPerPeak*i+1, energy[i] - de1 , energy[i] + de2); 
      fit->SetParLimits(numParPerPeak*i+2, 0, sigmaMax * 5); 
      
      fit->SetParLimits(numParPerPeak*i+3, 0, 1e+9);
      fit->SetParLimits(numParPerPeak*i+4, 0, sigmaMax); 
      fit->SetParLimits(numParPerPeak*i+5, -10, -2); 
          
    }
  }else{
    fit->SetParLimits(0, 0, 1e+9);
    fit->SetParLimits(2, 0, sigmaMax); 
    fit->SetParLimits(3, 0, 1e+9);
    fit->SetParLimits(4, 0, sigmaMax); 
    fit->SetParLimits(5, -10, -2);
  }
  specS->Fit("fit", "q");
  
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  //======== calculate reduce chi-squared
  if( newPlot ) GoodnessofFit(specS, fit);
  
  double bw = specS->GetBinWidth(1);

  vector<double> exPos;
  for(int i = 0; i < nPeaks ; i++){
    exPos.push_back(paraA[numParPerPeak*i+1]);
    double totCount = paraA[numParPerPeak*i] + paraA[numParPerPeak*i+3];
    double totCountErr = sqrt(paraE[numParPerPeak*i]*paraE[numParPerPeak*i] + paraE[numParPerPeak*i+3]*paraE[numParPerPeak*i+3]);
    printf("%2d , count: %8.0f(%3.0f)+%8.0f(%3.0f)=%8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f), skewneww: %4.1f(%4.1f) \n", 
            i,
            paraA[numParPerPeak*i] / bw,   paraE[numParPerPeak*i] /bw, 
            paraA[numParPerPeak*i+3] / bw,   paraE[numParPerPeak*i+3] /bw, 
            totCount / bw,   totCountErr /bw, 
            paraA[numParPerPeak*i+1], paraE[numParPerPeak*i+1],
            paraA[numParPerPeak*i+2], paraE[numParPerPeak*i+2],
            paraA[numParPerPeak*i+5], paraE[numParPerPeak*i+5]);
    
    //PrintPar(fit, numParPerPeak);
  }
  if( newPlot ) cFitAuto->Update();

  //draw the indivual fit
  fit->Draw("same");
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  TF1 ** kFit = new TF1 *[nn];
  TF1 ** zFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1) + [3] * TMath::Gaus(x,[1],[4], 1) * ( 1 + TMath::Erf( [5]*(x-[1])/sqrt(2)/[4] ))", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[numParPerPeak*i]);
      gFit[i]->SetParameter(1, paraA[numParPerPeak*i+1]);
      gFit[i]->SetParameter(2, paraA[numParPerPeak*i+2]);
      gFit[i]->SetParameter(3, paraA[numParPerPeak*i+3]);
      gFit[i]->SetParameter(4, paraA[numParPerPeak*i+4]);
      gFit[i]->SetParameter(5, paraA[numParPerPeak*i+5]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
      
      kFit[i] = new TF1(Form("kFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1) * ( 1 + TMath::Erf( [3]*(x-[1])/sqrt(2)/[2] ))", xMin, xMax);
      kFit[i]->SetParameter(0, paraA[numParPerPeak*i+3]);
      kFit[i]->SetParameter(1, paraA[numParPerPeak*i+1]);
      kFit[i]->SetParameter(2, paraA[numParPerPeak*i+4]);
      kFit[i]->SetParameter(3, paraA[numParPerPeak*i+5]);
      kFit[i]->SetLineColor(i+1);
      kFit[i]->SetNpx(1000);
      kFit[i]->SetLineWidth(1);
      kFit[i]->Draw("same");
      
      zFit[i] = new TF1(Form("zFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      zFit[i]->SetParameter(0, paraA[numParPerPeak*i]);
      zFit[i]->SetParameter(1, paraA[numParPerPeak*i+1]);
      zFit[i]->SetParameter(2, paraA[numParPerPeak*i+2]);
      zFit[i]->SetLineColor(i+1);
      zFit[i]->SetNpx(1000);
      zFit[i]->SetLineWidth(1);
      zFit[i]->Draw("same");
      
  }
  
  specS->Draw("hist same");
  
  return exPos;
  
}

//########################################
//###### fir N Gauss with estimated BG
//########################################
void fitNGauss(TH1F * hist, int bgEst = 10, TString fitFile = "AutoFit_para.txt", TString optStat = ""){

  printf("================================================================\n");
  printf("================ fit N-Gauss with estimated  BG ================\n");
  printf(" * bgEst = larger of bgEst, more linear the estimated BG \n");
  printf(" * need the file input \n");
  printf(" \n");
  printf(" 1) The histogram will be subtracted by the estimated BG. \n");
  printf(" 2) n-Gauss will then be fitted the BG subtracted histogram \n");
  printf("================================================================\n");

  recentFitMethod = "fitNGauss";

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }
  
  nPeaks = energy.size();
  
  gStyle->SetOptStat(optStat);
  TCanvas *cFitNGauss = NewCanvas("cFitNGauss","Fit n-Gauss | fitNGauss", 1,4, 800, 300);;
  cFitNGauss->cd(1);

  ScaleAndDrawHist(hist, 0, 0);
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitNGauss (BG = %2d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  //=================== find peak and fi
  
  gStyle->SetOptFit(0);
  //cFitNGauss->cd(2)->SetGrid();
  cFitNGauss->cd(2);
  
  if( bgEst > 0 ) {
    printf("============= estimating background...\n");
    TSpectrum * peak = new TSpectrum(50);
    TH1 * h1 = peak->Background(hist, bgEst);
    cFitNGauss->cd(1);
    h1->Draw("same");
    cFitNGauss->cd(2);
    printf("============= substracting the estimated background...\n");
    specS->Sumw2();
    specS->Add(h1, -1.);
  }
  
  specS->Draw("hist");

  //========== Fitting 
  printf("============= Fitting %d-Gauss..... \n", nPeaks);
  
  const int  n = 3 * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i]/2.;
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  , 0, 1e9); 
    
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fit->FixParameter(3*i+2, sigma[i]);
    }else{
      fit->SetParLimits(3*i+2, 0, sigma[i]);
    }
  }
  
  specS->Fit("fit", "q");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  //======== calculate reduce chi-squared
  //GoodnessofFit(specS, fit);
  
  double bw = specS->GetBinWidth(1);

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");

  //draw the indivual fit
  specS->Draw("hist");
  fit->Draw("same");
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  specS->Draw("hist same");
  //specS->Draw("E same");

  
  //======== print text on plot
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.06);

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  cFitNGauss->cd(3);
  PlotResidual(specS, fit);
  
  cFitNGauss->cd(4);
  
  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
  
  cFitNGauss->Update();
}


//########################################
//#### not updated
//########################################
void fitNGaussSub(TH1F * hist, int bgEst = 10, int degPol = 1, TString fitFile = "AutoFit_para.txt", TString optStat = ""){


  printf("==================================================================\n");
  printf("======== fit N-Gauss with estimated  BG (method-2) ================\n");
  printf(" * bgEst = larger of bgEst, more linear the estimated BG \n");
  printf(" * degPol = degree of polynomial \n");
  printf(" * need the file input \n");
  printf(" \n");
  printf(" 1) A BG is estimated, and then the BG is fitted by a polynomial. \n");
  printf(" 2) The histogram will be subtracted by the polynomial. \n"); 
  printf(" 3) n-Gauss will then be fitted the subtracted histogram \n");
  printf("================================================================\n");

  recentFitMethod = "fitNGaussSub";

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  nPeaks = energy.size();
  nPols = degPol;

  gStyle->SetOptStat(optStat);
  TCanvas *cFitNGaussSub = NewCanvas("cFitNGaussSub","Fit n-Gauss, replace estimated BG with Pol-n  | fitNGauss2", 1, 4, 800, 300 );  
  //if(! cFitNGaussSub->GetShowEventStatus() ) cFitNGaussSub->ToggleEventStatus();
  
  cFitNGaussSub->cd(1);
  ScaleAndDrawHist(hist, 0, 0);
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitNGauss2 (replace Est. BG with Pol-%d) (BG = %2d); Ex [MeV]; Count / %4.0f keV", degPol, bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  printf("============= estimating background...\n");
  TSpectrum * peak = new TSpectrum(50);
  TH1 * h1 = peak->Background(hist, bgEst);
  
  printf("============= fit the est-background with a polynomial function...\n");

  TString polExp = "[0]";
  for( int i = 1; i < degPol + 1; i++){
    polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  }
  TF1 * bg = new TF1("bg", polExp.Data(), xMin, xMax);
  
  bg->SetParameter(0, 50);
  bg->SetParameter(0, 0);
  bg->SetLineColor(2);
  bg->SetNpx(1000);
  h1->Fit("bg", "R", "");
  
  hist->Draw();
  bg->Draw("same");


  //======== print text on plot
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);

  const Double_t * paraAt = bg->GetParameters();
  const Double_t * paraEt = bg->GetParErrors();

  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraAt[i], paraEt[i]));
  }
  
  gStyle->SetOptFit(0);
//  cFitNGaussSub->cd(2)->SetGrid();
  cFitNGaussSub->cd(2);

  printf("============= substracting the polynomial background...\n");
  specS->Sumw2();
  specS->Add(bg, -1.);
  specS->Draw("hist");

  //========== Fitting 
  printf("============= Fitting..... \n");
  
  const int  n = 3 * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i]/2.;
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9); 
    
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fit->FixParameter(3*i+2, sigma[i]);
    }else{
      fit->SetParLimits(3*i+2, 0, sigma[i]); 
    }
  }
  
  specS->Fit("fit", "q");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  GoodnessofFit(specS, fit);  
  
  double bw = specS->GetBinWidth(1);
   
  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");
  

  //draw the indivual fit
  specS->Draw("hist");
  fit->Draw("same");
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  specS->Draw("hist same");
  //specS->Draw("E same");

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));


  cFitNGaussSub->cd(3);
  PlotResidual(specS, fit);

  cFitNGaussSub->cd(4);

  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));
  
  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
  
}


//########################################
//#### fit N-Gauss with pol-n BG
//########################################
void fitNGaussPol(TH1F * hist, int degPol,  TString fitFile = "AutoFit_para.txt",double xMin = 0, double xMax = 0, TString optStat = ""){

  printf("================================================================\n");
  printf("================ fit N-Gauss with Pol-%1d BG ==================\n", degPol);
  printf(" * degPol = degree of polynomial \n");
  printf(" * need the file input \n");
  printf(" * xMin, xMax = if left empty, full range will be used\n");
  printf(" \n");
  printf(" 1) The histogram will be fitted by n-Gauss + Pol \n");
  printf("================================================================\n");

  recentFitMethod  = "fitNGaussPol";

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  gStyle->SetOptStat(optStat);
  nPeaks = energy.size();
  nPols = degPol;

  TCanvas * cFitNGaussPol = NewCanvas("cFitNGaussPol", Form("Fitting with n-Gauss + pol-%d  | fitNGaussPol", degPol), 1, 3, 800, 300);
  //if(! cFitNGaussPol->GetShowEventStatus() ) cFitNGaussPol->ToggleEventStatus();
  cFitNGaussPol->cd(1);

  ScaleAndDrawHist(hist, xMin, xMax);
  
  if( xMin == xMax){ 
    xMin = hist->GetXaxis()->GetXmin();
    xMax = hist->GetXaxis()->GetXmax();
  }
  int xBin = hist->GetXaxis()->GetNbins();

  
  printf("============= find the polynomial background ..... \n");
  
  int  nPar = 3 * nPeaks + nPols + 1;
  double * para = new double[nPar]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i]/2.;
  }

  for(int i = 0; i < nPols + 1; i++){
    //if( ggg == NULL ){
      para[3*nPeaks+i] = gRandom->Rndm()/TMath::Power(10, 3*i);
    //}else{
    //  para[3*nPeaks+i] = gPara[i];
    //}
  }
  
  TF1 * fit = new TF1("fit", nGaussPol, xMin, xMax, nPar );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9); 
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fit->FixParameter(3*i+2, sigma[i]);
    }else{
      fit->SetParLimits(3*i+2, 0, sigma[i]);
    }
  }
  
  hist->Fit("fit", "Rq");
  
  //=========== get the polynomial part 
  const Double_t* paraA = fit->GetParameters();
  const Double_t* paraE = fit->GetParErrors();

  TString polExp = "[0]";
  for( int i = 1; i < degPol + 1; i++){
    polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  }

  TF1 * bg = new TF1("bg", polExp.Data(), xMin, xMax);
  for( int i = 0; i < degPol + 1; i++){
    bg->SetParameter(i, paraA[3*nPeaks+i]);
  }
  bg->SetNpx(1000);

  for( int i = 0; i < degPol + 1; i++){
    printf("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]);
  }
  printf("====================================\n");
  
  cFitNGaussPol->cd(1);
  bg->Draw("same");
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(1);

  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]));
  }

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  
  //GoodnessofFit(specS, fit); 
  
  double bw = hist->GetBinWidth(1);

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");
  
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  cFitNGaussPol->Update();

  cFitNGaussPol->cd(2);
  PlotResidual(hist, fit);

  cFitNGaussPol->cd(3);
  
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));
  
  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
}




//########################################
//#### fit N-Gauss with pol-n BG
//########################################
void fitNGaussPolSub(TH1F * hist, int degPol,  TString fitFile = "AutoFit_para.txt",double xMin = 0, double xMax = 0, TString optStat = ""){

  printf("================================================================\n");
  printf("========= fit N-Gauss with Pol-%1d BG / 2nd method ============\n", degPol);
  printf(" * degPol = degree of polynomial \n");
  printf(" * need the file input \n");
  printf(" * xMin, xMax = if left empty, full range will be used\n");
  printf(" \n");
  printf(" 1) The histogram will be fitted by n-Gauss + Pol -> to get the estimated BG \n");
  printf(" 2) The histogram will be subtracted by the Pol BG. \n");
  printf(" 3) n-Gauss will then be fitted the BG subtracted histogram \n");
  printf("================================================================\n");

  recentFitMethod = "fitNGaussPolSub";

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  gStyle->SetOptStat(optStat);
  nPeaks = energy.size();
  nPols = degPol;

  TCanvas * cFitNGaussPolSub = NewCanvas("cFitNGaussPolSub", Form("Fitting with n-Gauss + pol-%d (method-2)  | fitGaussPol2", degPol), 1, 4, 800, 300);
  //if(! cFitNGaussPol->GetShowEventStatus() ) cFitNGaussPol->ToggleEventStatus();
  cFitNGaussPolSub->cd(1);

  ScaleAndDrawHist(hist, xMin, xMax);
  
  if( xMin == xMax){ 
    xMin = hist->GetXaxis()->GetXmin();
    xMax = hist->GetXaxis()->GetXmax();
  }
  int xBin = hist->GetXaxis()->GetNbins();
  
  printf("============= find the polynomial background ..... \n");
  
  int  nPar = 3 * nPeaks + nPols + 1;
  double * para = new double[nPar]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i]/2.;
  }

  for(int i = 0; i < nPols + 1; i++){
    para[3*nPeaks+i] = gRandom->Rndm()/TMath::Power(10, 3*i);
  }
  
  TF1 * fit = new TF1("fit", nGaussPol, xMin, xMax, nPar );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9); 
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fit->FixParameter(3*i+2, sigma[i]);
    }else{
      fit->SetParLimits(3*i+2, 0, sigma[i]);
    }
  }
  
  hist->Fit("fit", "nq");
  
  //=========== get the polynomial part and substract
  const Double_t* paraAt = fit->GetParameters();
  const Double_t* paraEt = fit->GetParErrors();

  TString polExp = "[0]";
  for( int i = 1; i < degPol + 1; i++){
    polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  }

  TF1 * bg = new TF1("bg", polExp.Data(), xMin, xMax);
  for( int i = 0; i < degPol + 1; i++){
    bg->SetParameter(i, paraAt[3*nPeaks+i]);
  }
  bg->SetNpx(1000);

  for( int i = 0; i < degPol + 1; i++){
    printf("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraAt[3*nPeaks+i], paraEt[3*nPeaks+i]);
  }
  printf("====================================\n");
  
  cFitNGaussPolSub->cd(1);
  bg->Draw("same");
  
  TH1F * specS = (TH1F*) hist->Clone();
  TString titleH;
  titleH.Form("pol-%d BG Subtracted spectrum (fitNGaussPol-2); Ex [MeV]; Count / %4.0f keV", degPol, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  //=================== find peak and fit
  
  gStyle->SetOptFit(0);
  ///cFitNGaussPol->cd(2)->SetGrid();
  cFitNGaussPolSub->cd(2);
  
  printf("============= substracting the polynomial background...\n");
  specS->Sumw2();
  specS->Add(bg, -1.);
  specS->Draw("hist");

  //======= fit again
  printf("============= fitting the subtracted spectrum.... \n");
  
  nPar = 3* nPeaks;
  TF1 * fita = new TF1("fita", nGauss, xMin, xMax, nPar );
  fita->SetLineWidth(3);
  fita->SetLineColor(1);
  fita->SetNpx(1000);
  fita->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fita->SetParLimits(3*i  ,       0, 1e9); 
    
    if( energyFlag[i] == 1 ) {
      fita->FixParameter(3*i+1, energy[i]);
    }else{
      fita->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fita->FixParameter(3*i+2, sigma[i]);
    }else{
      fita->SetParLimits(3*i+2, 0, sigma[i]);
    }
  }
  
  specS->Fit("fita", "q");
  
  const Double_t* paraE = fita->GetParErrors();
  const Double_t* paraA = fita->GetParameters();
  
  //GoodnessofFit(specS, fit); 
  
  double bw = specS->GetBinWidth(1);

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");
  

  //draw the indivual fit
  specS->Draw("hist");
  fita->Draw("same");
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  specS->Draw("hist same");
  //specS->Draw("E same");


  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(1);

  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]));
  }

  double chi2 = fita->GetChisquare();
  int ndf = fita->GetNDF();
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));


  cFitNGaussPolSub->cd(3);
  PlotResidual(specS, fita);

  cFitNGaussPolSub->cd(4);
  
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }

  
}

//#################################################
//#### fit N-Gauss with pol-n BG using mouse click
//#################################################

int nClick = 0;
bool peakFlag = 1;
vector<double> xPeakList;
vector<double> yPeakList;
vector<double> xBGList;
vector<double> yBGList;

TH1F * tempHist;
int markerStyle = 23;
int markerColor = 2;
int markerBGColor = 4;

void Clicked() {
  int event = gPad->GetEvent();
  if (event != 11) return;
  TObject *select = gPad->GetSelected();
  if (!select) return;
  
  TH1F *h = (TH1F*)select;
  int px = gPad->GetEventX();
  double xd = gPad->AbsPixeltoX(px);
  float x = gPad->PadtoX(xd);

  if( peakFlag ) {
    xPeakList.push_back(x);
  }else{
    xBGList.push_back(x);
  }
  int b = tempHist->FindBin(x);
  double y = tempHist->GetBinContent(b);
  if( peakFlag ){
    yPeakList.push_back(y);
  }else{
    yBGList.push_back(y);
  }
  // add marker in the histogram
  TMarker * mark = new TMarker(x,y, markerStyle);
  mark->SetMarkerColor(markerColor);
  tempHist->GetListOfFunctions()->Add(mark);

  printf("%2d | x : %8.2f , y : %.0f \n", nClick, x, y);

  nClick ++;

}


void SaveFitPara(bool isBestFit = true, TString fileName = "AutoFit_para.txt"){

  if( xPeakList.size() == 0 && BestFitMean.size() == 0 ){
    printf(" no fit paramaters availible. \n");
    return;
  }

  if( recentFitMethod == "fitGF3Pol" || recentFitMethod == "fitGF3" ){
    printf(" Not support for GF3 fitting. \n");
    return;
  } 

  printf("Save to : %s \n", fileName.Data()); 
  FILE * file_out;
  file_out = fopen (fileName.Data(), "w+");

  fprintf(file_out, "# for n-Gauss fit, can use \"#\", or \"//\" to comment out whole line\n");
  fprintf(file_out, "# peak    low    high   fixed?  sigma_Max    fixed?  hight\n");


  if ( xPeakList.size() == 0 || isBestFit  ){
    for( int i = 0 ; i < BestFitMean.size() ; i++){
      fprintf(file_out, "%.3f    %.3f    %.3f    0    %.3f     0    %.0f\n",
                        BestFitMean[i],
                        BestFitMean[i] - 5*BestFitSigma[i],
                        BestFitMean[i] + 5*BestFitSigma[i],
                        BestFitSigma[i],
                        BestFitCount[i]);
    }
  }else{
    for( int i = 0 ; i < xPeakList.size() ; i++){
      fprintf(file_out, "%.3f    %.3f    %.3f    0    %.3f     0    %.0f\n",
                        xPeakList[i],
                        xPeakList[i] - 5*sigma[i],
                        xPeakList[i] + 5*sigma[i],
                        sigma[i],
                        yPeakList[i]);
    }
  }
  fclose(file_out);
}


void clickFitNGaussPol(TH1F * hist, int degPol, double sigmaMax = 0, double meanRange = 0){

  printf("=========================================================\n");
  printf("======== fit n-Gauss + Pol-%d BG using mouse click =====\n", degPol );
  printf("==========================================================\n");

  recentFitMethod = "clickFitNGaussPol";

  gStyle->SetOptStat("");
  gStyle->SetOptFit(0);
  
  TCanvas * cClickFitNGaussPol = NULL;
  if( gROOT->FindObjectAny("cClickFitGaussPol") == NULL ){
    cClickFitNGaussPol = new TCanvas("cClickFitNGaussPol", Form("fit Gauss & Pol-%d by mouse click | clickFitGaussPol", degPol), 1400, 1200);
  }else{
    delete gROOT->FindObjectAny("cClickFitNGaussPol") ;
    cClickFitNGaussPol = new TCanvas("cClickFitNGaussPol", Form("fit Gauss & Pol-%d by mouse click | clickFitGaussPol", degPol), 1400, 1200);
  }
  cClickFitNGaussPol->Divide(1, 4);
  for(int i = 1; i <= 4 ; i++){
    cClickFitNGaussPol->cd(i)->SetGrid(0,0);
  }
  
  if(! cClickFitNGaussPol->GetShowEventStatus() ) cClickFitNGaussPol->ToggleEventStatus();
  cClickFitNGaussPol->cd(1);


  hist->GetListOfFunctions()->Clear();
  ScaleAndDrawHist(hist, 0, 0);
  TH1F* hspec = (TH1F*) hist->Clone();
  hspec->Sumw2();
  cClickFitNGaussPol->Update();
  cClickFitNGaussPol->Draw();

  TLatex helpMsg;
  helpMsg.SetNDC();
  helpMsg.SetTextFont(82);
  helpMsg.SetTextSize(0.06);
  helpMsg.SetTextColor(kRed);
  helpMsg.DrawLatex(0.15, 0.8, "Click for peaks: (Double-click / x / Ctrl to end) ");

  printf("--------- Click the histogram for peaks: (Double-click / x / Ctrl to end) \n");
  nClick = 0;
  xPeakList.clear();
  yPeakList.clear();
  markerColor = 2;
  markerStyle = 23;
  peakFlag = 1;
  cClickFitNGaussPol->cd(1)->SetCrosshair(1);
  cClickFitNGaussPol->cd(1)->AddExec("ex", "Clicked()");
  tempHist = hist;
  TObject * obj ;
  do{
    obj = gPad->WaitPrimitive();
    if( obj == NULL  ) break;
  }while( obj != NULL);

  if( degPol >= 0 ){
    printf("--------- Click the histogram for Background: (Double-click / x / Ctrl to end) \n");
    printf("                         * when no input, program will estimate  \n");

    cClickFitNGaussPol->cd(1)->Clear();
    hist->Draw();
    helpMsg.SetTextColor(markerBGColor);
    helpMsg.DrawLatex(0.15, 0.8, "Click for BG: (Double-click / x / Ctrl to end) ");
    helpMsg.DrawLatex(0.15, 0.75, "* when no input, program will estimate");
    cClickFitNGaussPol->Update();
    cClickFitNGaussPol->Draw();

    nClick = 0;
    xBGList.clear();
    yBGList.clear();
    markerColor = markerBGColor;
    markerStyle = 33;
    peakFlag = 0;
    cClickFitNGaussPol->cd(1)->AddExec("ex", "Clicked()");
    do{
      obj = gPad->WaitPrimitive();
      if( obj == NULL  ) break;
    }while( obj != NULL);

  }
  
  cClickFitNGaussPol->cd(1)->DeleteExec("ex");
  cClickFitNGaussPol->cd(1)->SetCrosshair(0);
  cClickFitNGaussPol->cd(1)->Clear();
  hist->Draw();

  tempHist = NULL;

  cClickFitNGaussPol->Update();
  cClickFitNGaussPol->Draw();

  nPols = degPol;
  double xMin = hspec->GetXaxis()->GetXmin();
  double xMax = hspec->GetXaxis()->GetXmax();

  TString polExp = "[0]";
  for( int i = 1; i < degPol + 1; i++) polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  TF1 *bg = new TF1("bg", polExp.Data(), xMin, xMax);
  bg->SetNpx(1000);
  bg->SetLineColor(markerBGColor);
  bg->SetLineWidth(1);
  if( xBGList.size() > 0 ) {
    printf("---------------- fit the BG with Pol-%d \n", nPols);
    TGraph * gBG = new TGraph((int) xBGList.size(), &xBGList[0], &yBGList[0]);
    for( int i = 0; i < degPol + 1; i++) bg->SetParameter(i, gRandom->Rndm()/TMath::Power(10, 3*i));
    gBG->Fit("bg", "Rq");
    bg->Draw("same");
    //printf("--------------- Subtracting the BG \n");
    //hspec->Add(bg, -1);
  }else{
    for( int i = 0; i < nPols+1; i++) bg->SetParameter(i, 0.);
  }

  nPeaks = (int) xPeakList.size();
  if( sigmaMax == 0 ){
    printf("------------- Estimate sigma for each peak \n");
    sigma.clear();
    int binMin = hist->FindBin(xMin);
    int binMax = hist->FindBin(xMax);
    for( int i = 0; i < nPeaks ; i++){
      int b0 = hist->FindBin(xPeakList[i]);
      double estBG = bg->Eval(xPeakList[i]);    
      double sMin = (xMax-xMin)/5., sMax = (xMax-xMin)/5.;
      //---- backward search, stop when 
      for( int b = b0-1 ; b > binMin ; b-- ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y - (bg->Eval(x)) < (yPeakList[i]-estBG)/2. ) {
          sMin = xPeakList[i] - hist->GetBinCenter(b);
          break;
        }
      }
      //---- forward search, stop when 
      for( int b = b0+1 ; b < binMax  ; b++ ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y - (bg->Eval(x)) < (yPeakList[i]-estBG)/2. ) {
          sMax = hist->GetBinCenter(b) - xPeakList[i];
          break;
        }
      }

      double temp = TMath::Min(sMin, sMax);
      /// When there are multiple peaks closely packed : 
      if( i > 0 && temp > 2.5 * sigma.back() ) temp = sigma.back();
      sigma.push_back(temp);
      
      printf("%2d | x : %8.2f | sigma(est) %f \n", i, xPeakList[i], sigma[i]);

    }

    //---- use the mean of the sigma
    double sigma0 = 0;
    for( int i = 0; i < nPeaks ; i++) sigma0 += sigma[i];
    sigma0 = sigma0/(nPeaks+1);
    for( int i = 0; i < nPeaks ; i++) sigma[i] = sigma0;
    printf("========== use the mean sigma : %f \n", sigma0);
  }else if( sigmaMax < 0 ){
    printf("========== use user input sigma : %f (fixed)\n", abs(sigmaMax));
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(abs(sigmaMax));
  }else if( sigmaMax > 0 ){
    printf("========== use user input sigma : %f/2. \n", sigmaMax/2.);
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(sigmaMax/2.);
  }

  printf("-------------- Fit the spectrum with %d-Gauss + Pol-%d\n", nPeaks, nPols );
  cClickFitNGaussPol->cd(2);
  hspec->Draw("hist");

  int  nPar = 3 * nPeaks + nPols + 1;
  double * para = new double[nPar]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = yPeakList[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = xPeakList[i];
    if( sigmaMax == 0){
      para[3*i+2] = sigma[i];  
    }else if(sigmaMax < 0 ){
      para[3*i+2] = abs(sigmaMax);
    }else if(sigmaMax > 0 ){
      para[3*i+2] = sigmaMax/2.;
    }
  }
  for(int i = 0; i < nPols+1 ; i++){
    para[3*nPeaks+i] = bg->GetParameter(i);
  }

  TF1 * fit = new TF1("fit", nGaussPol, xMin, xMax, nPar );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  //limit parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9);
    
    if( meanRange <= 0 ) {
      double dE1, dE2;
      if( i == 0 ){
        dE2 = xPeakList[i+1] - xPeakList[i];
        dE1 = dE2;
      }else if ( i == nPeaks-1 ){
        dE1 = xPeakList[i] - xPeakList[i-1];
        dE2 = dE1;
      }else{
        dE1 = xPeakList[i] - xPeakList[i-1];
        dE2 = xPeakList[i+1] - xPeakList[i];
      }      
      fit->SetParLimits(3*i+1, xPeakList[i] - dE1 , xPeakList[i] + dE2 );
    }else{
      fit->SetParLimits(3*i+1, xPeakList[i] - meanRange/2. , xPeakList[i] + meanRange/2. );
    }
    if( sigmaMax== 0 ) fit->SetParLimits(3*i+2, 0, 1.5*sigma[i]); // add 50% margin of sigma
    if( sigmaMax < 0 ) fit->FixParameter(3*i+2, abs(sigmaMax));
    if( sigmaMax > 0 ) fit->SetParLimits(3*i+2, 0, sigmaMax);
  }
  
  hspec->Fit("fit", "Rq");
  fit->Draw("same");

  //=========== get the polynomial part 
  const Double_t* paraA = fit->GetParameters();
  const Double_t* paraE = fit->GetParErrors();

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  double bw = hspec->GetBinWidth(1);

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");

  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]));
  }

  TF1 ** gFit = new TF1 *[nPeaks];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }

  if( degPol >= 0 ){
    TF1 *bg2 = new TF1("bg", polExp.Data(), xMin, xMax);
    bg2->SetNpx(1000);
    bg2->SetLineColor(markerBGColor);
    bg2->SetLineWidth(1);
    for( int i = 0; i < nPols + 1; i++){
      bg2->SetParameter(i, paraA[3*nPeaks+i]);
    }
    bg2->Draw("same");
  }

  cClickFitNGaussPol->cd(3);
  PlotResidual(hspec, fit);
  
  cClickFitNGaussPol->cd(4);

  text.SetTextSize(0.05);
  text.SetTextColor(2);
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));

    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }

}



void clickFitNGaussPolSub(TH1F * hist, int degPol, double sigmaMax = 0, double meanRange = 0){

  printf("=========================================================\n");
  printf("= fit n-Gauss + Pol-%d BG using mouse click (method-2) =\n", degPol );
  printf("==========================================================\n");

  recentFitMethod = "clickFitNGaussPolSub";
  
  gStyle->SetOptStat("");
  gStyle->SetOptFit(0);
  
  TCanvas * cClickFitNGaussPolsub = NULL;
  if( gROOT->FindObjectAny("cClickFitGaussPolsub") == NULL ){
    cClickFitNGaussPolsub = new TCanvas("cClickFitNGaussPol", Form("fit Gauss & Pol-%d by mouse click (sub) | clickFitGaussPolsub", degPol), 1400, 1200);
  }else{
    delete gROOT->FindObjectAny("cClickFitNGaussPolsub") ;
    cClickFitNGaussPolsub = new TCanvas("cClickFitNGaussPolsub", Form("fit Gauss & Pol-%d by mouse click (sub) | clickFitGaussPolsub", degPol), 1400, 1200);
  }
  cClickFitNGaussPolsub->Divide(1, 4);
  for(int i = 1; i <= 4 ; i++){
    cClickFitNGaussPolsub->cd(i)->SetGrid(0,0);
  }
  
  if(! cClickFitNGaussPolsub->GetShowEventStatus() ) cClickFitNGaussPolsub->ToggleEventStatus();
  cClickFitNGaussPolsub->cd(1);


  hist->GetListOfFunctions()->Clear();
  ScaleAndDrawHist(hist, 0, 0);
  TH1F* hspec = (TH1F*) hist->Clone();
  hspec->Sumw2();
  cClickFitNGaussPolsub->Update();
  cClickFitNGaussPolsub->Draw();

  TLatex helpMsg;
  helpMsg.SetNDC();
  helpMsg.SetTextFont(82);
  helpMsg.SetTextSize(0.06);
  helpMsg.SetTextColor(kRed);
  helpMsg.DrawLatex(0.15, 0.8, "Click for peaks: (Double-click / x / Ctrl to end) ");


  printf("--------- Click the histogram for peaks: (Double-click / x / Ctrl to end) \n");
  nClick = 0;
  xPeakList.clear();
  yPeakList.clear();
  markerColor = 2;
  markerStyle = 23;
  peakFlag = 1;
  cClickFitNGaussPolsub->cd(1)->SetCrosshair(1);
  cClickFitNGaussPolsub->cd(1)->AddExec("ex", "Clicked()");
  tempHist = hist;
  TObject * obj ;
  do{
    obj = gPad->WaitPrimitive();
    if( obj == NULL  ) break;
  }while( obj != NULL);

  if( degPol >= 0 ){
    printf("--------- Click the histogram for Background: (Double-click / x / Ctrl to end) \n");

    cClickFitNGaussPolsub->cd(1)->Clear();
    hist->Draw();
    helpMsg.SetTextColor(markerBGColor);
    helpMsg.DrawLatex(0.15, 0.8, "Click for BG: (Double-click / x / Ctrl to end) ");
    cClickFitNGaussPolsub->Update();
    cClickFitNGaussPolsub->Draw();

    nClick = 0;
    xBGList.clear();
    yBGList.clear();
    markerColor = markerBGColor;
    markerStyle = 33;
    peakFlag = 0;  
    do{
      obj = gPad->WaitPrimitive();
      if( obj == NULL  ) break;
    }while( obj != NULL);
  }
  cClickFitNGaussPolsub->cd(1)->DeleteExec("ex");
  cClickFitNGaussPolsub->cd(1)->SetCrosshair(0);
  cClickFitNGaussPolsub->cd(1)->Clear();
  hist->Draw();

  tempHist = NULL;

  if( xBGList.size() == 0 ) helpMsg.DrawLatex(0.15, 0.75, " No BG defined ! fitting could be problematics. ");

  cClickFitNGaussPolsub->Update();
  cClickFitNGaussPolsub->Draw();

  nPols = degPol;
  double xMin = hspec->GetXaxis()->GetXmin();
  double xMax = hspec->GetXaxis()->GetXmax();

  TString polExp = "[0]";
  for( int i = 1; i < degPol + 1; i++) polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  TF1 *bg = new TF1("bg", polExp.Data(), xMin, xMax);
  bg->SetNpx(1000);
  bg->SetLineColor(markerBGColor);
  bg->SetLineWidth(1);
  if( xBGList.size() > 0 ) {
    printf("---------------- fit the BG with Pol-%d \n", nPols);
    
    TGraph * gBG = new TGraph((int) xBGList.size(), &xBGList[0], &yBGList[0]);

    for( int i = 0; i < degPol + 1; i++) bg->SetParameter(i, gRandom->Rndm()/TMath::Power(10, 3*i));

    gBG->Fit("bg", "Rq");
    bg->Draw("same");

    printf("--------------- Subtracting the BG \n");
    hspec->Add(bg, -1);
  }else{
    for( int i = 0; i < nPols+1; i++) bg->SetParameter(i, 0.);
  }
  
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.06);
  
  double chi2BG = bg->GetChisquare();
  int ndfBG = bg->GetNDF();
  
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2BG/ndfBG));

  //=========== get the polynomial BG
  const Double_t* paraAt = bg->GetParameters();
  const Double_t* paraEt = bg->GetParErrors();
  
  for( int i = 0; i < degPol + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.2f(%8.2f)\n", Form("p%d", i), paraAt[i], paraEt[i]));
  }

  nPeaks = (int) xPeakList.size();
  if( sigmaMax == 0 ){
    printf("------------- Estimate sigma for each peak \n");
    sigma.clear();
    int binMin = hist->FindBin(xMin);
    int binMax = hist->FindBin(xMax);
    
    for( int i = 0; i < nPeaks ; i++){
      int b0 = hist->FindBin(xPeakList[i]);
      double estBG = bg->Eval(xPeakList[i]);    
      double sMin = (xMax-xMin)/5., sMax = (xMax-xMin)/5.;
      //---- backward search, stop when 
      for( int b = b0-1 ; b > binMin ; b-- ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y - (bg->Eval(x)) < (yPeakList[i]-estBG)/2. ) {
          sMin = xPeakList[i] - hist->GetBinCenter(b);
          break;
        }
      }
      //---- forward search, stop when 
      for( int b = b0+1 ; b < binMax  ; b++ ){
        double y = hist->GetBinContent(b);
        double x = hist->GetBinCenter(b);
        if( y - (bg->Eval(x)) < (yPeakList[i]-estBG)/2. ) {
          sMax = hist->GetBinCenter(b) - xPeakList[i];
          break;
        }
      }

      double temp = TMath::Min(sMin, sMax);
      /// When there are multiple peaks closely packed : 
      if( i > 0 && temp > 2.5 * sigma.back() ) temp = sigma.back();
      sigma.push_back(temp);

      printf("%2d | x : %8.2f | sigma(est) %f \n", i, xPeakList[i], sigma[i]);

    }  
  }else if( sigmaMax < 0 ){
    printf("========== use user input sigma : %f (fixed)\n", abs(sigmaMax));
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(abs(sigmaMax));
  }else if( sigmaMax > 0 ){
    printf("========== use user input sigma : %f/2. \n", sigmaMax/2.);
    sigma.clear();
    for( int i = 0; i < nPeaks ; i++) sigma.push_back(sigmaMax/2.);
  }
    
  printf("-------------- Fit the subtracted spectrum with %d-Gauss\n", nPeaks );
  cClickFitNGaussPolsub->cd(2);
  hspec->Draw("hist");

  int  nPar = 3 * nPeaks;
  double * para = new double[nPar]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = yPeakList[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = xPeakList[i];
    if( sigmaMax == 0){
      para[3*i+2] = sigma[i];  
    }else if(sigmaMax < 0 ){
      para[3*i+2] = abs(sigmaMax);
    }else if(sigmaMax > 0 ){
      para[3*i+2] = sigmaMax/2.;
    }
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, nPar );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  //limit parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9);
    
    if( meanRange <= 0 ) {
      double dE1, dE2;
      if( i == 0 ){
        dE2 = xPeakList[i+1] - xPeakList[i];
        dE1 = dE2;
      }else if ( i == nPeaks-1 ){
        dE1 = xPeakList[i] - xPeakList[i-1];
        dE2 = dE1;
      }else{
        dE1 = xPeakList[i] - xPeakList[i-1];
        dE2 = xPeakList[i+1] - xPeakList[i];
      }
      fit->SetParLimits(3*i+1, xPeakList[i] - dE1 , xPeakList[i] + dE2 );
    }else{
      fit->SetParLimits(3*i+1, xPeakList[i] - meanRange/2. , xPeakList[i] + meanRange/2. );
    }
    if( sigmaMax== 0 ) fit->SetParLimits(3*i+2, 0, 1.5*sigma[i]); // add 50% margin of sigma
    if( sigmaMax < 0 ) fit->FixParameter(3*i+2, abs(sigmaMax));
    if( sigmaMax > 0 ) fit->SetParLimits(3*i+2, 0, sigmaMax);
  }
  
  hspec->Fit("fit", "Rq");
  fit->Draw("same");

  //=========== get the fit parameters 
  const Double_t* paraA = fit->GetParameters();
  const Double_t* paraE = fit->GetParErrors();

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  double bw = hspec->GetBinWidth(1);
  
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");

  TF1 ** gFit = new TF1 *[nPeaks];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }

  cClickFitNGaussPolsub->cd(3);
  PlotResidual(hspec, fit);
  
  cClickFitNGaussPolsub->cd(4);

  text.SetTextSize(0.05);
  text.SetTextColor(2);
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));

  BestFitCount.clear();
  BestFitMean.clear();
  BestFitSigma.clear();
  
  for( int i = 0; i < nPeaks; i++){
    text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));
            
    BestFitCount.push_back(paraA[3*i]/ bw);
    BestFitMean.push_back(paraA[3*i+1]);
    BestFitSigma.push_back(paraA[3*i+2]);
  }
}


//########################################
//#### fit N-Gauss with pol-1 BG at fixed for < 0
//########################################
void fitSpecial(TH1F * hist, TString fitFile = "AutoFit_para.txt"){

  printf("================================================================\n");
  printf("================ Special fit for h074_82Kr =====================\n");
  printf(" * need the file input \n");
  printf("================================================================\n");
  

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  gStyle->SetOptStat("");
  gStyle->SetOptFit(0);
  nPeaks = energy.size();
  nPols = 1;

  TCanvas * cFitSpecial = NewCanvas("cFitSpecial", "Fitting for h074_82Kr", 1, 3, 800, 300);
  //if(! cFitSpecial->GetShowEventStatus() ) cFitSpecial->ToggleEventStatus();
  cFitSpecial->cd(1);

  ScaleAndDrawHist(hist, 0, 0);
  
  double  xMin = hist->GetXaxis()->GetXmin();
  double  xMax = hist->GetXaxis()->GetXmax();
  int     xBin = hist->GetXaxis()->GetNbins();

  printf("============= find the pol-1 background ..... \n");
  
  TF1 * fitpol1 = new TF1("fitpol1", "pol1", xMin, -0.3);
  fitpol1->SetParameter(0, gRandom->Rndm());
  fitpol1->SetParameter(1, gRandom->Rndm());
  
  hist->Fit("fitpol1", "Rq");
  
  double x0 = fitpol1->GetParameter(0);
  double x1 = fitpol1->GetParameter(1);
  
  
  int  nPar = 3 * nPeaks + nPols + 1;
  double * para = new double[nPar]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i]/2.;
  }

  para[3*nPeaks+0] = x0;
  para[3*nPeaks+1] = x1;
  
  
  TF1 * fit = new TF1("fit", nGaussPol, xMin, xMax, nPar );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);

  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 1e9); 
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) {
      fit->FixParameter(3*i+2, sigma[i]);
    }else{
      fit->SetParLimits(3*i+2, 0, sigma[i]);
    }
  }
  
  fit->FixParameter(3*nPeaks + 0, x0);
  fit->FixParameter(3*nPeaks + 1, x1);
  
  hist->Fit("fit", "Rq");
  
  //=========== get the polynomial part 
  const Double_t* paraA = fit->GetParameters();
  const Double_t* paraE = fit->GetParErrors();

  TString polExp = "[0]";
  for( int i = 1; i < nPols + 1; i++){
    polExp += Form("+[%d]*TMath::Power(x,%d)", i, i );
  }

  TF1 * bg = new TF1("bg", polExp.Data(), xMin, xMax);
  for( int i = 0; i < nPols + 1; i++){
    bg->SetParameter(i, paraA[3*nPeaks+i]);
  }
  bg->SetNpx(1000);

  for( int i = 0; i < nPols + 1; i++){
    printf("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]);
  }
  printf("====================================\n");
  
  cFitSpecial->cd(1);
  bg->Draw("same");
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(1);

  for( int i = 0; i < nPols + 1; i++){
    text.DrawLatex(0.6, 0.85 - 0.05*i, Form("%4s : %8.4e(%8.4e)\n", Form("p%d", i), paraA[3*nPeaks+i], paraE[3*nPeaks+i]));
  }

  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  text.SetTextSize(0.06);
  text.DrawLatex(0.15, 0.8, Form("#bar{#chi^{2}} : %5.3f", chi2/ndf));

  
  //GoodnessofFit(specS, fit); 
  
  double bw = hist->GetBinWidth(1);

  for(int i = 0; i < nPeaks ; i++){
    printf(" %2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  printf("\n");
  
  
  const int nn = nPeaks;
  TF1 ** gFit = new TF1 *[nn];
  for( int i = 0; i < nPeaks; i++){
      gFit[i] = new TF1(Form("gFit%d", i), "[0] * TMath::Gaus(x,[1],[2], 1)", xMin, xMax);
      gFit[i]->SetParameter(0, paraA[3*i]);
      gFit[i]->SetParameter(1, paraA[3*i+1]);
      gFit[i]->SetParameter(2, paraA[3*i+2]);
      gFit[i]->SetLineColor(i+1);
      gFit[i]->SetNpx(1000);
      gFit[i]->SetLineWidth(1);
      gFit[i]->Draw("same");
  }
  
  cFitSpecial->Update();

  cFitSpecial->cd(2);
  PlotResidual(hist, fit);

  cFitSpecial->cd(3);
  
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.05);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("      %13s, %18s, %18s", "count", "mean", "sigma"));
  
  for( int i = 0; i < nPeaks; i++){
  text.DrawLatex(0.1, 0.8-0.05*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));
  }
  
}


#endif 
