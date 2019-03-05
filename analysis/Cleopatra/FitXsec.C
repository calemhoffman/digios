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


void FitXsec(TString expXsec, int ID, TString ptolemy, int ID2 = -1){
  
  
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
  yRange[0] = 0;
  yRange[1] = 0;
  xRange[0] = 0;
  xRange[1] = 0;
  for( int i = 0 ; i < gX->GetN(); i++){
    double x, y;
    gX->GetPoint(i, x, y);
    if( x > xRange[1] ) xRange[1] = x;
    if( x < xRange[0] ) xRange[0] = x;
    if( y > yRange[1] ) yRange[1] = x;
    if( y < yRange[0] ) yRange[0] = x;
  }

  gX->GetYaxis()->SetRangeUser(0.1, 10);
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

  if( ID2 >= 0 &&  0 <= ID2 && ID2 < n ) {
    n = 1;
  }else{
    ID2= 0;
  }

  TGraph * gr[n];
  
  for ( int i = 0 ; i < n ; i++){
    
    gr[i] = (TGraph *) gList->At(i + ID2);
    gr[i]->SetLineColor(i+1);
    
  }
  
  //============ Fit 
  double  SF [n];
  double dSF [n];
  double chi [n];
  
  for( int i = 0; i < n ; i ++){
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
    double chisquared = fit->GetChisquare();

    //printf("chi2 = %f , ndf = %d \n", chisquared, ndf);

    chi[i] = chisquared/ndf;
    
    printf(" %s | SF = %5.3f(%5.3f), chi2 = %f \n", gr[i]->GetName(), paraA[0], paraE[0], chisquared/ndf);  
    
  }
  
  //============= Scale TGraph with SF
  for( int i = 0; i < n; i++){
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
  
  for( int i = 0 ; i < n; i++){
    TString nlj = gr[i]->GetName();
    int length = nlj.Length();
    nlj.Remove(0, length - 8);
    nlj.Remove(5);
    nlj.Insert(2, "_{");
    nlj.Append("}");
    text.DrawLatex(0.15, 0.8 - 0.05 *i ,Form("%s| SF: %5.3f(%3.0f), #chi^{2}: %5.3f", nlj.Data(), SF[i], dSF[i]*1000, chi[i]));
  }
  
}
