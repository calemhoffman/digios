#include <string>
#include "TChain.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TF1.h"

void Cali_xf_xn_single(int detID, double xGate0, double xGate1, std::string fileList){

  TChain * chain = new TChain("gen_tree");
  chain->Add(fileList.c_str());


  TCanvas * canvas = (TCanvas *) gROOT->FindObject("canvas");
  if( canvas == nullptr ) canvas = new TCanvas("canvas", "Cali xf-xn single", 1000, 1000, 1200, 400);
  canvas->Clear();

  canvas->Divide(3,1);

  //*=======================================
  canvas->cd(1);

  TH2F * h1 = (TH2F*) gROOT->FindObjectAny("h1");
  if( h1 == nullptr ) h1 = new TH2F("h1", "h1;xn;xf", 400, 0, 3000, 400, 0, 3000);
  TString haha;
  haha.Form("xf[%d]:xn[%d]>>h1", detID, detID);

  chain->Draw(haha, "", "colz");


  //*=======================================
  canvas->cd(2);

  TH1F * h2 = (TH1F*) gROOT->FindObjectAny("h2");
  if( h2 == nullptr ) h2 = new TH1F("h2", "h2;xn+xf", 400, 0, 3000);
  TString kaka;
  kaka.Form("xf[%d]+xn[%d]>>h2", detID, detID);

  chain->Draw(kaka, "", "colz");


  //*=======================================
  canvas->cd(3);

  TH2F * h3 = (TH2F*) gROOT->FindObjectAny("h3");
  if( h3 == nullptr ) h3 = new TH2F("h3", "h3 (gated);xn;xf", 400, 0, 3000, 400, 0, 3000);
  TString dudu;
  dudu.Form("xf[%d]:xn[%d]>>h3", detID, detID);
  TString gate;
  gate.Form("%f > xf[%d] + xn[%d] && xf[%d] + xn[%d] > %f", xGate1, detID, detID, detID, detID, xGate0 );

  chain->Draw(dudu, gate, "colz");

  TF1 * fit = new TF1("fit", "pol1");

  h3->Fit("fit");

  double para[2];
  fit->GetParameters(para);

  printf("============= Put %.6f into the correction file.\n", -para[1]);

  delete chain;

}