#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>

double gfit(double * x, double * para){  
  
  double result = 0;
  
  double A = TMath::Log(x[0]/100);
  double B = TMath::Log(x[0]/1000);
  
  double lowEnergy = para[0] + para[1] * A;  
  double highEnergy = para[2] + para[3] * B + para[4]*B*B;  
  
  double a1 = TMath::Power(lowEnergy, -para[5]);
  double a2 = TMath::Power(highEnergy, -para[5]);
  
  result = TMath::Exp( TMath::Power( a1 + a2, -1/para[5]));
  
  return result;
}


void script_eff(){
                    
  //2019-Sept
  //vector<vector<double>> en = {
  //                              { 121.783, 100.00},
  //                              { 244.699,  75.42},
  //                              { 344.281,  56.91},
  //                              { 411.115,  47.50},
  //                           //   { 569.702,  45.69},      //207Bi
  //                           //   { 778.903,  36.43},
  //                              { 867.39 ,  28.14},
  //                              { 964.055,  25.86},
  //                           //   {1063.662,  21.805},    //207Bi
  //                              {1085.842,  19.94},
  //                              {1112.087,  22.05},
  //                              {1408.022,  17.63},
  //                           //   {1770.237,  14.802}   //207Bi
  //                            };
                              
  //2020-Oct
  vector<vector<double>> en = {
                                { 121.783, 100.00},
                                { 244.699,  88.90},
                                { 344.281,  65.98},
                                { 443.965,  61.14},
                                { 778.903,  34.51},
                                { 964.055,  30.32},
                                {1085.842,  26.39},
                                {1112.087,  28.09},
                                {1408.022,  23.11}
                              };
  
  static int numPt = en.size(); 
  
  double en1[numPt], en2[numPt];
  for( int i = 0; i < numPt; i++){
    en1[i] = en[i][0];
    en2[i] = en[i][1];
  }
  
  TGraph * gEff = new TGraph(numPt, en1, en2);
  gEff->SetTitle("2020-Oct");
  gEff->Draw("A*");
  gEff->GetYaxis()->SetRangeUser(0, 120);
  
  TF1 * fit = new TF1("fit", gfit, 50, 7500, 6);
  fit->SetNpx(1000);
  fit->SetParameter(0, 4.6);
  fit->SetParameter(1, 1.1);
  fit->SetParameter(2, 3.3);
  fit->SetParameter(3, -0.7);
  fit->SetParameter(4, 0.03);
  fit->SetParameter(5, 11);
  
  fit->SetParLimits(1, 0, 2);
  
  gEff->Fit("fit");
  
  double * para = fit->GetParameters();
  const double * paraE = fit->GetParErrors();
  
  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  
  printf("======= NDF : %d, chi2 : %f | reduced-chi2 : %f \n", ndf, chi2, chi2/ndf);
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
  text.DrawLatex(0.3, 0.80, Form("#chi^{2} = %.0f, ndf = %d, #chi^{2}/ndf = %.3f ", chi2, ndf, chi2/ndf));
  text.DrawLatex(0.6, 0.60, Form(" A = %.3f #pm %.3f ", para[0], paraE[0]));
  text.DrawLatex(0.6, 0.55, Form(" B = %.3f #pm %.3f ", para[1], paraE[1]));
  text.DrawLatex(0.6, 0.50, Form(" D = %.3f #pm %.3f ", para[2], paraE[2]));
  text.DrawLatex(0.6, 0.45, Form(" E = %.3f #pm %.3f ", para[3], paraE[3]));
  text.DrawLatex(0.6, 0.40, Form(" F = %.3f #pm %.3f ", para[4], paraE[4]));
  text.DrawLatex(0.6, 0.35, Form(" G = %.3f #pm %.3f ", para[5], paraE[5]));
  
  fit->Draw("same");
  
  
  TCanvas * c2 = new TCanvas("c2", "c2", 0, 600, 1000, 300);
  
  //TF1 * fit2 = new TF1("fit2", gfit, 0, 7500, 6);
  //fit2->SetParameters(para);
  //fit2->SetNpx(1000);
  //fit2->Draw();
  fit->Draw("");
  
  gEff->Draw("*same");

  
  printf("=====================\n");
  printf("eff( 120 keV) = %.3f \n",fit->Eval( 120));
  printf("eff( 570 keV) = %.3f \n",fit->Eval( 570));
  printf("eff(1064 keV) = %.3f \n",fit->Eval(1064));
  printf("eff(1770 keV) = %.3f \n",fit->Eval(1770));
  printf("eff(1720 keV) = %.3f \n",fit->Eval(1720));
  printf("eff(2231 keV) = %.3f \n",fit->Eval(2231));
  printf("eff(2742 keV) = %.3f \n",fit->Eval(2742));
  printf("eff(5108 keV) = %.3f \n",fit->Eval(5180));
  printf("eff(5619 keV) = %.3f \n",fit->Eval(5619));
  printf("eff(6130 keV) = %.3f \n",fit->Eval(6130));
  printf("eff(6095 keV) = %.3f \n",fit->Eval(6095));
  printf("eff(6606 keV) = %.3f \n",fit->Eval(6606));
  printf("eff(7117 keV) = %.3f \n",fit->Eval(7117));
  
  
  
}
/**************/

/*************************************************** finding count for each peak 
void script(){
  
  TFile * file = new TFile("Ge_data.root");
  TTree * tree = (TTree*) file->FindObjectAny("tree");
  gStyle->SetOptStat("");
  
  int mean = 1770;
  int range[2] = {mean - 30, mean + 30};
  
  TCanvas * c1 = new TCanvas("c1", "c1", 800, 400);
  c1->cd();
  TH1F * h0 = new TH1F("h0", "energy 152-Eu; energy [keV] ; count/keV", 1500, 0, 1500);
  tree->Draw("eT>>h0", "", "");
  
  TCanvas * c2 = new TCanvas("c2", "c2", 800, 600);
  c2->cd();
  TH1F * h1 = new TH1F("h1", "energy 152-Eu; energy [keV] ; count/keV", range[1]-range[0], range[0], range[1]);
  
  tree->Draw("eT>>h1", "", "");
  
  TF1 * fit = new TF1("fit", "[0]*TMath::Gaus(x, [1], [2],1) + [3] + [4]*x", range[0], range[1]);
  fit->SetNpx(1000);
  fit->SetParameter(0, 10000);
  fit->SetParameter(1, mean);
  fit->SetParameter(2, 3);
  fit->SetParameter(3, 10);
  fit->SetParameter(4, -1);
  
  fit->SetParLimits(0, 0, 1000000);
  fit->SetParLimits(2, 0, 10);
  
  h1->Fit("fit");
  
  double * para = fit->GetParameters();
  const double * paraE = fit->GetParErrors();
  
  double chi2 = fit->GetChisquare();
  int ndf = fit->GetNDF();
  
  //estimage BG sigma 
  //TGraph* gSigmaEst = new TGraph(h1);
  //double sigma2 = TMath::Power(gSigmaEst->GetRMS(2),2);
  
  printf("======= NDF : %d, chi2 : %f | reduced-chi2 : %f \n", ndf, chi2, chi2/ndf);
  
  TF1 * g1 = new TF1("g1", "[0]*TMath::Gaus(x, [1], [2],1)", range[0], range[1]);
  g1->SetNpx(1000);
  g1->SetParameter(0, para[0]);
  g1->SetParameter(1, para[1]);
  g1->SetParameter(2, para[2]);
  g1->SetLineColor(4);
  
  
  TF1 * g2 = new TF1("g2", "[0] + [1]*x", range[0], range[1]);
  g2->SetNpx(1000);
  g2->SetParameter(0, para[3]);
  g2->SetParameter(1, para[4]);
  g2->SetLineColor(1);
  
  //g1->Draw("same");
  g2->Draw("same");
  fit->Draw("same");
  
  
  printf("count : %.0f (%.0f)\n", para[0], paraE[0]);
  printf(" mean : %.2f (%.2f)\n", para[1], paraE[1]);
  printf("sigma : %.2f (%.2f)\n", para[2], paraE[2]);
  
  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
  text.DrawLatex(0.15, 0.60, Form("count = %.0f #pm %.0f ", para[0], paraE[0]));
  text.DrawLatex(0.15, 0.55, Form(" mean = %.1f #pm %.1f ", para[1], paraE[1]));
  text.DrawLatex(0.15, 0.50, Form("sigma = %.1f #pm %.1f ", para[2], paraE[2]));
  
}
/**************/

/*** scrpt for fitting 6.12 MeV decay
double gfit(double * x, double * para){
  
  // para[0] = timeZero
  // para[1] = constant
  // para[2] = time constant 
  
  double result = 0;
  
  if( x[0] < para[0]){
    
    result = para[1];
    
  }else{
    
    result = para[1]*TMath::Exp(-(x[0]-para[0])/para[2]);
    
  }
  
  return result;
  
}

void script(){

  TFile * file = new TFile("Ge_data.root");
  
  TTree * tree = (TTree*) file->FindObjectAny("tree");
  
  gStyle->SetOptStat("");
  
  TH1F * h1 = new TH1F("h1", "time for 6.12 MeV; time [sec] ;count/sec", 100, 360, 560);
  TH1F * h2 = new TH1F("h2", "time for 0.12 MeV; time [sec] ;count/sec", 100, 360, 560);
  h2->SetLineColor(1);
  
  tree->Draw("tsec>>h2", "118< eT && eT < 122");
  tree->Draw("tsec>>h1", "6110 < eT && eT < 6130", "same");
  
  TF1 * fit = new TF1("fit", gfit, 360, 560, 3);
  fit->SetParameter(0, 470);
  fit->SetParameter(1, 70);
  fit->SetParameter(2, 8);
  
  h1->Fit("fit");

  TLatex text;
  text.SetNDC();
  text.SetTextFont(82);
  text.SetTextSize(0.04);
  text.SetTextColor(2);
  
  double t0 = fit->GetParameter(0);
  double A = fit->GetParameter(1);
  double tau = fit->GetParameter(2);
  
  double t0_e = fit->GetParError(0);
  double A_e = fit->GetParError(1);
  double tau_e = fit->GetParError(2);
  
  text.DrawLatex(0.2, 0.4, Form("f(t) = Ae^{-(#frac{t-t_{0}}{#tau})}"));
  text.DrawLatex(0.2, 0.3, Form("A = %.1f #pm %.1f /sec", A, A_e));
  text.DrawLatex(0.2, 0.25, Form("t_{0} = %.1f #pm %.1f sec", t0, t0_e));
  text.DrawLatex(0.2, 0.2, Form("#tau = %.1f #pm %.1f sec", tau, tau_e));

}
****/
