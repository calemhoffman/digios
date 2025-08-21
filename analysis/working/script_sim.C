#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"

void script_sim(){

  gStyle->SetOptStat(0);

  TFile *file0 = TFile::Open("transfer_50.root");
  TFile *file1 = TFile::Open("transfer_500.root");

  TFile *file2 = TFile::Open("EZ_260.root");
  TFile *file3 = TFile::Open("EZ_260_alpha.root");

  TTree *tree0 = (TTree*)file0->Get("tree");
  TTree *tree1 = (TTree*)file1->Get("tree");

  gStyle->SetOptStat(0);

  TH2F* h2 = (TH2F*)file2->Get("hezg");
  TH2F* h3 = (TH2F*)file3->Get("hez");

  gStyle->SetOptStat(0);

  TH2F * h0 = new TH2F("h0", "haha;z [mm];proton energy [MeV]", 600, -900, 0, 400, 0, 14); h0->SetMarkerColor(2);
  TH2F * h1 = new TH2F("h1", "haha;z [mm];proton energy [MeV]", 600, -900, 0, 400, 0, 14);

  tree0->Draw("e:z>>h0", "loop==1 && thetaCM > 10 && hit==1 && rho < 160 ", "scat" ,  10000);
  tree1->Draw("e:z>>h1", "loop==1 && thetaCM > 10 && hit==1 && rho < 160 ", "scat same", 30000);
  h2->Draw("colz same");
  h3->Draw("colz same");


}