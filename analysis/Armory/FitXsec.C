
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TObjArray.h"
#include "TLegend.h"

TGraph * g0;
Double_t func0(Double_t *x, Double_t *para) {
  return para[0] * (g0->Eval(x[0]));
}

TGraph * g1;
Double_t func1(Double_t *x, Double_t * para){
  
  return para[0] * (g0->Eval(x[0])) + para[1] * (g1->Eval(x[0])) ;
}


void FitXsec(TString expXsec, int ID, TString ptolemy, int ID2 = -1, int ID3 = -1){
  
  
  printf("========================================================\n");
  printf("======    Fit Experimental Xsec with Ptolemy Xsec \n");
  printf("======    * the plotting is not prefect....... \n");
  printf("========================================================\n");

  //================ read Exp Xsec
  TFile * fExp = new TFile(expXsec);
  
  TObjArray * xList = (TObjArray*) fExp->FindObjectAny("xList");
  
  const int nX = xList->GetLast() + 1 ; 
  
  if( ID >= nX ) {
    printf("Number of Xsec in %s is %d. You select the %d-th. Out or range.\n", expXsec.Data(), nX, ID);
    return;
  }
  
  TGraphErrors * gX = (TGraphErrors *) xList->At(ID);
  gX->SetMarkerColor(4);
  gX->SetMarkerSize(1.5);
  gX->SetMarkerStyle(4);

  TCanvas * cFitXsec = new TCanvas ("cFitXsec", "Fit X-sec", 0, 0, 800, 600);
  cFitXsec->SetLogy();
  
  TLegend * legend;

  if( ID2 >= 0 ){
    legend = new TLegend( 0.1, 0.9, 0.9, 0.99); 
  }else{
    legend = new TLegend( 0.7, 0.3, 0.9, 0.9); 
  }
  legend->AddEntry(gX, "Exp");
  
  gX->Draw("AP");

  //find yRange and xRange;
  double yRange[2], xRange[2];
  yRange[0] = 1000.0;
  yRange[1] = 0.0001;
  xRange[0] = 0;
  xRange[1] = 0;
  for( int i = 0 ; i < gX->GetN(); i++){
    double x, y;
    gX->GetPoint(i, x, y);
    if( x > xRange[1] ) xRange[1] = x;
    if( x < xRange[0] ) xRange[0] = x;
    if( y > yRange[1] ) yRange[1] = y;
    if( y < yRange[0] ) yRange[0] = y;
  }

  printf("yRange : %f, %f \n", yRange[0], yRange[1]);
  double yMin, yMax, yMean, dy;
  yMean = (yRange[1] + yRange[0])/2.;
  dy = abs(yRange[1] - yRange[0])/2.;
  yMin = TMath::Power(10, TMath::Floor(TMath::Log10(yRange[0])))*0.5;
  yMax = TMath::Power(10, TMath::Ceil(TMath::Log10(yRange[1])))*1.5;

  gX->GetYaxis()->SetRangeUser(yMin, yMax);
  gX->GetXaxis()->SetLimits(0, xRange[1] * 1.1);
  
  gX->GetXaxis()->SetTitle("#theta_{CM} [deg]");
  gX->GetYaxis()->SetTitle("d#sigma/d#Omega [mb/sr]");

  gX->Draw("AP");
  
  cFitXsec->Modified();
  cFitXsec->Update();
  
  //=============== read Therotical Xsec
  TFile * fPtolemy = new TFile(ptolemy);
  
  TObjArray * gList = (TObjArray*) fPtolemy->FindObjectAny("qList");
  int n = gList->GetLast() + 1 ; 
  
  TGraph * gr[n];
  
  int startID = 0;
  int endID = n;
  if( 0 <= ID2 && ID2 < n) {
    startID = ID2;
    endID = ID2+1;
  }
  
  for ( int i = 0 ; i < n ; i++){    
    gr[i] = (TGraph *) gList->At(i);
    gr[i]->SetLineColor(i+1);
  }
  
  //============ Fit 
  double  SF [n];
  double dSF [n];
  double chi [n];
  
  for( int i = startID; i < endID ; i ++){
    g0 = gr[i];
     
    TF1 * fit = new TF1("fit", func0, 0, 50, 1);
    fit->SetParameter(0, 1);
    fit->SetParLimits(0, 0, 10);
    fit->SetLineColor(i+1);
    gX->Fit("fit", "Rnq", "", 0, xRange[1] * 1.1);
    
    const double* paraE = fit->GetParErrors();
    const double* paraA = fit->GetParameters();
    
    SF[i] = paraA[0];
    dSF[i] = paraE[0];
    
    int ndf = fit->GetNDF();
    double chisquared = fit->GetChisquare(); // checked. the chisquared is calculated correctly
    chi[i] = chisquared/ndf;
    printf(" %s | SF = %5.3f(%5.3f), chi2/ndf = %f \n", gr[i]->GetName(), paraA[0], paraE[0], chisquared/ndf);  
    
  }
  printf("!!!!! if the uncertainties of Xsec are ZERO. the chisquare should be as small as possible!!!!!\n");
  
  //============= Scale TGraph with SF
  for( int i = startID; i < endID; i++){
    for(int j = 0; j < gr[i]->GetN(); j++){
      (gr[i]->GetY())[j] *= SF[i];
    }
    
    legend->AddEntry(gr[i], gr[i]->GetName());
    gr[i]->Draw("same");
    
  }
  
  legend->Draw();  
  
  //Draw Latex
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.03);
  
  for( int i = startID ; i < endID; i++){
    TString nlj = gr[i]->GetName();
    int length = nlj.Length();
    nlj.Remove(0, length - 8);
    nlj.Remove(5);
    nlj.Insert(2, "_{");
    nlj.Append("}");
    text.DrawLatex(0.15, 0.8 - 0.05 *i ,Form("%s| SF: %5.3f(%3.0f), #chi^{2}: %5.3e", nlj.Data(), SF[i], dSF[i]*1000, chi[i]));
  }
  
  //================= Fit with 2 
  printf(" ID2 = %d , n = %d \n", ID2, n);
  
  if( ID3 >= 0  || n == 2){
    printf("--------------------------\n");
    if( ID2 < 0 ) {
      ID2 =  0;
      ID3 = 1;
    }
    g0 = gr[ID2];
    g1 = gr[ID3];
    
    TF1 * fit2 = new TF1("fit2", func1, 0, 50, 2);
    fit2->SetParameter(0, 1);
    fit2->SetParameter(1, 1);
    fit2->SetParLimits(0, 0, 10);
    fit2->SetParLimits(1, 0, 10);
    fit2->SetLineColor(1);
    
    //printf(" fit2(16 deg) = %f \n", fit2->Eval(16));
    
    gX->Fit("fit2", "Rnq", "", 0, xRange[1] * 1.1);
    
    const double* paraE = fit2->GetParErrors();
    const double* paraA = fit2->GetParameters();
    
    int ndf = fit2->GetNDF();
    double chisquared = fit2->GetChisquare(); // checked. the chisquared is calculated correctly
    printf("A f1 + B f2 | chi2/ndf = %f\n", chisquared/ndf);  
    printf(" A = %5.3f(%5.3f) \n", paraA[0], paraE[0]);  
    printf(" B = %5.3f(%5.3f) \n", paraA[1], paraE[1]);  
    
    text.DrawLatex(0.15, 0.45 ,Form("A f_{1} + B f_{2} | #chi^2/ndf : %f", chisquared/ndf));
    text.DrawLatex(0.15, 0.40 ,Form("A : %5.3f(%3.0f)", paraA[0], paraE[0]*1000));
    text.DrawLatex(0.15, 0.35 ,Form("B : %5.3f(%3.0f)", paraA[1], paraE[1]*1000));

    
  }
  
}
