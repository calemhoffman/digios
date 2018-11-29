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


void FitXsec(TString expXsec, int ID, TString ptolemy){
  
  
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
  
  TLegend * legend = new TLegend( 0.6, 0.2, 0.9, 0.4); 
  
  legend->AddEntry(gX, "Exp");
  
  gX->Draw("AP");
  gX->GetYaxis()->SetRangeUser(0.1, 10);
  gX->GetXaxis()->SetLimits(0, 50);

  //gX->GetXaxis()->SetTitle("theta_CM [deg]");
  //gX->GetYaxis()->SetTitle("dsigma/dOmega [mb/sr]");
  
  gX->GetXaxis()->SetTitle("#theta_{CM} [deg]");
  gX->GetYaxis()->SetTitle("d#sigma/d#Omega [mb/sr]");

  gX->Draw("AP");
  
  cFitXsec->Modified();
  cFitXsec->Update();
  
  //=============== read Therotical Xsec
  TFile * fPtolemy = new TFile(ptolemy);
  
  TObjArray * gList = (TObjArray*) fPtolemy->FindObjectAny("gList");
  
  const int n = gList->GetLast() + 1 ; 
  
  TGraph * gr[n];
  
  for ( int i = 0; i < n ; i++){
    
    gr[i] = (TGraph *) gList->At(i);
    gr[i]->SetLineColor(i+1);
    
    //legend->AddEntry(gr[i], gr[i]->GetName());
    //gr[i]->Draw("same");
    
    //if( i == 0 ){
    //  gr[i]->Draw();
    //  gr[i]->GetYaxis()->SetRangeUser(0.1, 10);
    //}else{
    //  gr[i]->Draw("same");
    //}
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
    gX->Fit("fit", "Rnq", "", 10, 50);
    
    const double* paraE = fit->GetParErrors();
    const double* paraA = fit->GetParameters();
    
    SF[i] = paraA[0];
    dSF[i] = paraE[0];
    
    int ndf = fit->GetNDF();
    double chisquared = fit->GetChisquare();
    
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
    nlj.Remove(0, 9);
    nlj.Remove(5);
    nlj.Insert(2, "_{");
    nlj.Append("}");
    text.DrawLatex(0.15, 0.8 - 0.05 *i ,Form("%s| SF: %5.3f(%3.0f), #chi^{2}: %5.3f", nlj.Data(), SF[i], dSF[i]*1000, chi[i]));
  }
  
}
