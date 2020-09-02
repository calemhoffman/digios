#ifndef AutoFit_C
#define AutoFit_C

#include <TF1.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <vector>


void ShowFitMethod(){
  printf("----------------- Method of Fitting ---------------\n");
  printf("---------------------------------------------------\n");
  printf("     fitAuto() - estimate BG, find peak, and fit n-Gauss \n");
  printf("    fitGauss() - fit 1 Gauss\n");
  printf("  fitGaussP1() - fit 1 Gauss + Pol1 \n");
  printf(" fit2GaussP1() - fit 2 Gauss + Pol1 \n");
  printf("   fitNGauss() - fit n-Gauss, need input\n");
  printf("  fitNGauss2() - fit n-Guass, fit the estimate BG with Pol1\n");
  printf(" fitNGaussP1() - fit n-Gauss + Pol1 \n");
  printf("---------------------------------------------------\n");
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
         mean + y;
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

Double_t nGaussP1(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1); // normalized Gaussian
   }
   
   result += par[3* nPeaks] + par[3*nPeaks+1] * x[0];
   
   return result;
}


vector<double> energy, height, sigma, lowE, highE ;
vector<int> energyFlag, sigmaFlag;

bool loadFitParameters(TString fitParaFile){
  
  energy.clear(); energyFlag.clear();
  sigma.clear();  sigmaFlag.clear();
  
  lowE.clear(); highE.clear();
  
  height.clear();
  
  bool paraRead = false;

  printf("================================================= \n");
  printf("----- loading fit parameters from : %s", fitParaFile.Data());
  ifstream file;
  file.open(fitParaFile.Data());
  int i = 0;
  if( file.is_open() ){
    string x;
    while( file >> x){
      //printf("%d, %s \n", i,  x.c_str());
      
      if( x.substr(0,2) == "//" )  continue;
      
      if( x.substr(0,2) == "##" ) break;
      
      if( i%7 == 0 ) energy.push_back(atof(x.c_str()));
      if( i%7 == 1 ) lowE.push_back(atof(x.c_str()));
      if( i%7 == 2 ) highE.push_back(atof(x.c_str()));
      if( i%7 == 3 ) energyFlag.push_back(atoi(x.c_str()));         
      if( i%7 == 4 ) sigma.push_back(atof(x.c_str()));
      if( i%7 == 5 ) sigmaFlag.push_back(atoi(x.c_str()));
      if( i%7 == 6 ) height.push_back(atof(x.c_str()));
      
      i = i + 1;
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
      
  }else{
    printf("... fail.\n"); 
  }
  
  printf("================================================= \n");
  
  return  paraRead;
  
}

//########################################
//########################################
//########################################
void fitGauss(TH1F * hist, double mean, double sigma, double xMin, double xMax, TString optStat = ""){
  
  
  if( gROOT->FindObjectAny("cFitGauss") == NULL ){
    TCanvas * cFitGauss = new TCanvas("cFitGauss", "fit Gauss", 800, 400);
  }
  gStyle->SetOptStat(optStat);
  
  hist->Draw();
  
  TF1 * fit = new TF1("fit", "[0] * TMath::Gaus(x, [1], [2], 1)", xMin, xMax);
  
  
  double * para = new double[3]; 
  para[0] = 100 * 0.05 * TMath::Sqrt(TMath::TwoPi());
  para[1] = mean;
  para[2] = sigma;

  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  hist->Fit("fit", "Rq");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  double chisquare = fit->GetChisquare();
  int ndf = fit->GetNDF();
  double bw = hist->GetBinWidth(1);

  printf("%s ====== count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            hist->GetName(),
            paraA[0] / bw,   paraE[0] /bw, 
            paraA[1], paraE[1],
            paraA[2], paraE[2]);
            
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
    
   text.DrawLatex(0.15, 0.65, Form("count : %5.0f(%5.0f)", paraA[0]/bw, paraE[0]/bw));
   text.DrawLatex(0.15, 0.60, Form(" mean : %5.3f(%5.3f) MeV", paraA[1], paraE[1]));
   text.DrawLatex(0.15, 0.55, Form("sigma : %5.3f(%5.3f) MeV", paraA[2], paraE[2]));
   text.DrawLatex(0.15, 0.50, Form(" FWHM : %5.3f(%5.3f) MeV", paraA[2] *2.355, paraE[2]*2.355));
   
   
   text.DrawLatex(0.15, 0.40, Form("#chi^2/ndf : %5.3f", chisquare/ndf));
   
   GoodnessofFit(hist, fit);
   
}

//########################################
//########################################
//########################################
void fitGaussP1(TH1F * hist, double mean, double sigma, double xMin, double xMax, TString optStat = ""){
  
  
  if( gROOT->FindObjectAny("cFitGaussP1") == NULL ){
    TCanvas * cFitGaussP1 = new TCanvas("cFitGaussP1", "fit Gauss & P1", 800, 400);
  }
  gStyle->SetOptStat(optStat);
  
  hist->Draw();
  
  TF1 * fit = new TF1("fit", "[0] * TMath::Gaus(x, [1], [2], 1) + [3] + [4]*x", xMin, xMax);
  
  double * para = new double[5]; 
  para[0] = 100 * 0.05 * TMath::Sqrt(TMath::TwoPi());
  para[1] = mean;
  para[2] = sigma;
  para[3] = 1;
  para[4] = 0;

  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  hist->Fit("fit", "R");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  double bw = hist->GetBinWidth(1);

  printf("%s ====== count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            hist->GetName(),
            paraA[0] / bw,   paraE[0] /bw, 
            paraA[1], paraE[1],
            paraA[2], paraE[2]);
            
            
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
  text.DrawLatex(0.15, 0.7, Form("Line: %4.2f(%3.2f) + %4.2f(%3.2f)x  ", 
                                    paraA[3],   paraE[3],
                                    paraA[4], paraE[4]));
  GoodnessofFit(hist, fit);
  
}

//########################################
//########################################
//########################################
vector<double> fit2GaussP1(TH1F * hist, double mean1, double sigma1, 
                                       double mean2, double sigma2, 
                           double xMin, double xMax, TString optStat = "", bool newCanvas = false){
  
  vector<double> output;
  output.clear();
  
  if( newCanvas &&  gROOT->FindObjectAny("cFit2GaussP1") == NULL ){
    TCanvas * cFit2GaussP1 = new TCanvas("cFit2GaussP1", "fit Gauss & P1", 800, 400);
  }
  gStyle->SetOptStat(optStat);
  
  hist->Draw();
  
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
  return output;
}

//########################################
//########################################
//########################################
vector<double> fitAuto(TH1 * hist, int bgEst = 10, double peakThreshold = 0.1, TString optStat = "", bool newPlot = true, double sigmaGuess = 0.08){

  TCanvas *cFitAuto = NULL;
  if( newPlot ){
    cFitAuto = new TCanvas("cFitAuto","Auto Fitting", 100, 100, 800,800);
    cFitAuto->Divide(1,2);
    
    gStyle->SetOptStat(optStat);
    cFitAuto->cd(1);
    hist->Draw();
  }
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitted spectrum (BG=%d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  //specS->GetXaxis()->SetTitleSize(0.06);
  //specS->GetYaxis()->SetTitleSize(0.06);
  //specS->GetXaxis()->SetTitleOffset(0.7);
  //specS->GetYaxis()->SetTitleOffset(0.6);
  
  //=================== find peak and fit
  gStyle->SetOptFit(0);
  TSpectrum * peak = new TSpectrum(50);
  nPeaks = peak->Search(hist, 2, "", peakThreshold); 
  
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
  
  const int  n = 3 * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigmaGuess;
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(2);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  specS->Fit("fit", "q");
  
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  //======== calculate reduce chi-squared
  if( newPlot ) GoodnessofFit(specS, fit);
  
  /**
  double chisquare = 0;
  for( int iBin = 1; iBin <= specS->GetNbinsX(); iBin ++){
      double x = specS->GetBinCenter(iBin);
      double yf = fit->Eval(x);
      double y  = specS->GetBinContent(iBin);
      chisquare += TMath::Power( y - yf , 2);
  }
  
  int ndf = fit->GetNDF();
  
  int bin1 = specS->FindBin(paraA[1] - 3 * paraA[2]);
  //int bin2 = specS->FindBin(paraA[3*(nPeaks-1)+1] + 3 * paraA[3*(nPeaks-1)+2]);
  int bin2 = specS->GetNbinsX();
  TGraph * gSigmaEst = new TGraph( specS);
  
  for( int iBin = bin2; iBin >= bin1; iBin--){
    gSigmaEst->RemovePoint(iBin);
  }
  double sigma2 = TMath::Power(gSigmaEst->GetRMS(2),2);
  
  printf("==== Chi-Sq : %f , NDF : %d, Sigma2 : %f \n", chisquare, ndf, sigma2);
  printf("============= Fit Result: reduced Chi-squared = %f\n", chisquare/ndf/sigma2);  
  */
  
  double bw = specS->GetBinWidth(1);

  double * ExPos = new double[nPeaks];
  double * ExSigma = new double[nPeaks];   
  vector<double> exPos;
  for(int i = 0; i < nPeaks ; i++){
    exPos.push_back(paraA[3*i+1]);
    ExPos[i] = paraA[3*i+1];
    ExSigma[i] = paraA[3*i+2];
    if( newPlot){
    printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
    }
  }
  if( newPlot ) cFitAuto->Update();

  //draw the indivual fit
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
  
  return exPos;
  
}

//########################################
//########################################
//########################################
void fitNGauss(TH1 * hist, int bgEst = 10, TString optStat = "", TString fitFile = "AutoFit_para.txt"){

  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }
  nPeaks = energy.size();

  TCanvas *cFitNGauss = new TCanvas("cFitNGauss","Fitting on Ex", 600,600);
  cFitNGauss->Divide(1,3);
  if(! cFitNGauss->GetShowEventStatus() ) cFitNGauss->ToggleEventStatus();
  
  gStyle->SetOptStat(optStat);
  cFitNGauss->cd(1);
  hist->Draw();
  
  TH1F * specS = (TH1F*) hist->Clone();
  specS->Sumw2();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitNGauss (BG = %2d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  //=================== find peak and fi
  
  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  cFitNGauss->cd(2)->SetGrid();
  cFitNGauss->cd(2);
  
  if( bgEst > 0 ) {
    printf("============= estimating background...\n");
    TSpectrum * peak = new TSpectrum(50);
    TH1 * h1 = peak->Background(hist, bgEst);
    cFitNGauss->cd(1);
    h1->Draw("same");
    cFitNGauss->cd(2);
    printf("============= substracting the linear background...\n");
    specS->Sumw2();
    specS->Add(h1, -1.);
  }
  
  specS->Draw("hist");

  //========== Fitting 
  printf("============= Fitting..... \n");
  
  const int  n = 3 * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i];
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 20000); 
    
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) fit->FixParameter(3*i+2, sigma[i]);
  }
  
  specS->Fit("fit", "q");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  //======== calculate reduce chi-squared
  
  /**
  double chisquare = 0;
  for( int iBin = 1; iBin <= specS->GetNbinsX(); iBin ++){
      double x = specS->GetBinCenter(iBin);
      double yf = fit->Eval(x);
      double y  = specS->GetBinContent(iBin);
      chisquare += TMath::Power( y - yf , 2);
  }
  
  int ndf = fit->GetNDF();
  
  int bin1 = specS->FindBin(paraA[1] - 3 * paraA[2]);
  //int bin2 = specS->FindBin(paraA[3*(nPeaks-1)+1] + 3 * paraA[3*(nPeaks-1)+2]);
  int bin2 = specS->GetNbinsX();
  TGraph * gSigmaEst = new TGraph( specS);
  
  for( int iBin = bin2; iBin >= bin1; iBin--){
    gSigmaEst->RemovePoint(iBin);
  }
  double sigma2 = TMath::Power(gSigmaEst->GetRMS(2),2);
  printf("\n ==== Historgam : %s, FitMethod: fitNGauss, BG = %2d \n", hist->GetName(), bgEst);
  printf(" ==== Chi-Sq : %f , NDF : %d, BG-Sigma2 : %f \n", chisquare, ndf, sigma2);
  printf(" ============= Fit Result: reduced Chi-squared = %f\n", chisquare/ndf/sigma2);  
  */ 
  GoodnessofFit(specS, fit);
  
  double bw = specS->GetBinWidth(1);

  double * ExPos = new double[nPeaks];
  double * ExSigma = new double[nPeaks];   
  for(int i = 0; i < nPeaks ; i++){
    ExPos[i] = paraA[3*i+1];
    ExSigma[i] = paraA[3*i+2];
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
  
  cFitNGauss->cd(3);
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.06);
  text.SetTextColor(2);
  
  text.DrawLatex(0.1, 0.9, Form("     %13s, %18s, %18s", "count", "mean", "sigma"));
  
  for( int i = 0; i < nPeaks; i++){
  text.DrawLatex(0.1, 0.8-0.1*i, Form(" %2d, %8.0f(%3.0f), %8.4f(%8.4f), %8.4f(%8.4f)\n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]));
  }
  
  
  
  cFitNGauss->Update();
}


//########################################
//########################################
//########################################
void fitNGauss2(TH1 * hist, int bgEst = 10, TString optStat = "", TString fitFile = "AutoFit_para.txt"){
   
  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  nPeaks = energy.size();

  TCanvas *cFitNGauss2 = new TCanvas("cFitNGauss2","Fitting on Ex (fixed width)", 600,600);
  cFitNGauss2->Divide(1,2);
  
  if(! cFitNGauss2->GetShowEventStatus() ) cFitNGauss2->ToggleEventStatus();
  
  gStyle->SetOptStat(optStat);
  cFitNGauss2->cd(1);
  hist->Draw();
  
  TH1F * specS = (TH1F*) hist->Clone();
  double xMin = hist->GetXaxis()->GetXmin();
  double xMax = hist->GetXaxis()->GetXmax();
  int xBin = hist->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitNGauss2 (replace BG with linear BG) (BG = %2d); Ex [MeV]; Count / %4.0f keV", bgEst, (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  //=================== find peak and fit
  printf("============= estimating background...\n");
  TSpectrum * peak = new TSpectrum(50);
  TH1 * h1 = peak->Background(hist, bgEst);
  
  printf("============= fit the est-background with a linear function...\n");
  TF1 * bg = new TF1 ("bg", "pol2", xMin, xMax);
  bg->SetParameter(0, 50);
  bg->SetParameter(0, 0);
  bg->SetLineColor(2);
  bg->SetNpx(1000);
  h1->Fit("bg", "R", "", -1.5, 2);
  
  hist->Draw();
  bg->Draw("same");
  
  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  cFitNGauss2->cd(2)->SetGrid();
  cFitNGauss2->cd(2);

  printf("============= substracting the linear background...\n");
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
    para[3*i+2] = sigma[i];
  }

  TF1 * fit = new TF1("fit", nGauss, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 20000); 
    
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) fit->FixParameter(3*i+2, sigma[i]);
  }
  
  specS->Fit("fit", "q");
  
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();
  
  GoodnessofFit(specS, fit);  
  
  double bw = specS->GetBinWidth(1);

  double * ExPos = new double[nPeaks];
  double * ExSigma = new double[nPeaks];   
  for(int i = 0; i < nPeaks ; i++){
    ExPos[i] = paraA[3*i+1];
    ExSigma[i] = paraA[3*i+2];
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
  
  cFitNGauss2->Update();
}



//########################################
//########################################
//########################################
void fitNGaussP1(TH1 * hist, TString optStat = "", TString fitFile = "AutoFit_para.txt", double xMin = 0, double xMax = 0){
   
  bool isParaRead = loadFitParameters(fitFile);
  if( !isParaRead ) {
    printf("Please provide a valid input file\n");
    return;
  }

  nPeaks = energy.size();

  TCanvas *cFitNGaussP1 = new TCanvas("cFitNGaussP1","Fitting on Ex (fixed width)", 600,600);
  cFitNGaussP1->Divide(1,2);  
  if(! cFitNGaussP1->GetShowEventStatus() ) cFitNGaussP1->ToggleEventStatus();
  
  gStyle->SetOptStat(optStat);
  cFitNGaussP1->cd(1);
  int maxBin = hist->GetMaximumBin();
  double ymax = hist->GetBinContent(maxBin);
  hist->GetYaxis()->SetRangeUser(0, 1.1 * ymax); 
  hist->Draw();

  if( xMin == xMax){ 
    xMin = hist->GetXaxis()->GetXmin();
    xMax = hist->GetXaxis()->GetXmax();
  }
  int xBin = hist->GetXaxis()->GetNbins();
  
  //========== find the linear background
  printf("============= find the linear background ..... \n");
  
  int  n = 3 * nPeaks + 2;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = sigma[i];
  }
  
  para[3*nPeaks+0] = 100;
  para[3*nPeaks+1] = 0.1;
  
  TF1 * fit = new TF1("fit", nGaussP1, xMin, xMax, n );
  fit->SetLineWidth(3);
  fit->SetLineColor(1);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fit->SetParLimits(3*i  ,       0, 20000); 
    
    if( energyFlag[i] == 1 ) {
      fit->FixParameter(3*i+1, energy[i]);
    }else{
      fit->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) fit->FixParameter(3*i+2, sigma[i]);
  }
  
  hist->Fit("fit", "nq");
  
  //=========== get the linear part and substract
  const Double_t* paraAt = fit->GetParameters();
  
  TF1 * bg = new TF1("bg", "pol1", xMin, xMax);
  bg->SetParameter(0, paraAt[3*nPeaks+0]);
  bg->SetParameter(1, paraAt[3*nPeaks+1]);
  bg->SetNpx(1000);
  
  printf("===== linear: y = %f x + %f \n", paraAt[3*nPeaks+1], paraAt[3*nPeaks+2]);
  
  cFitNGaussP1->cd(1);
  bg->Draw("same");
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
  text.DrawLatex(0.6, 0.15, Form("%5.3f + %5.3fx", paraAt[3*nPeaks+0], paraAt[3*nPeaks+1]));

  TH1F * specS = (TH1F*) hist->Clone();
  TString titleH;
  titleH.Form("fitNGaussP1; Ex [MeV]; Count / %4.0f keV", (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  
  //=================== find peak and fit
  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  cFitNGaussP1->cd(2)->SetGrid();
  cFitNGaussP1->cd(2);
  
  printf("============= substracting the linear background...\n");
  specS->Sumw2();
  specS->Add(bg, -1.);
  specS->Draw("hist");

  //======= fit again
  printf("============= fitting the  ..... \n");
  
  n = 3* nPeaks;
  TF1 * fita = new TF1("fita", nGauss, xMin, xMax, n );
  fita->SetLineWidth(3);
  fita->SetLineColor(1);
  fita->SetNpx(1000);
  fita->SetParameters(para);
  
  //fixing parameters
  for( int i = 0; i < nPeaks; i++){
    fita->SetParLimits(3*i  ,       0, 20000); 
    
    if( energyFlag[i] == 1 ) {
      fita->FixParameter(3*i+1, energy[i]);
    }else{
      fita->SetParLimits(3*i+1, lowE[i], highE[i]); 
    }
    if( sigmaFlag[i]  == 1 ) fita->FixParameter(3*i+2, sigma[i]);
  }
  
  specS->Fit("fita", "q");
  
  const Double_t* paraE = fita->GetParErrors();
  const Double_t* paraA = fita->GetParameters();
  
  GoodnessofFit(specS, fit); 
  
  double bw = specS->GetBinWidth(1);

  double * ExPos = new double[nPeaks];
  double * ExSigma = new double[nPeaks];   
  for(int i = 0; i < nPeaks ; i++){
    ExPos[i] = paraA[3*i+1];
    ExSigma[i] = paraA[3*i+2];
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
  
  cFitNGaussP1->Update();
}





#endif 
