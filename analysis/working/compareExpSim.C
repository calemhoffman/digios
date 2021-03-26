{
  //TFile * fSim = new TFile("152Eu_Sim_hist.root");
  //TFile * fExp = new TFile("152Eu.root");
  
  TFile * fSim = new TFile("run306_Ta.root");
  TFile * fExp = new TFile("run308_Al.root");
  
  gStyle->SetOptStat("");

  TH1F * hExp = (TH1F *) fExp->Get("h4g0");
  TH1F * hSim = (TH1F *) fSim->Get("h4g0");

  //hExp->GetYaxis()->SetRangeUser(1, 3000);
  hExp->Draw();
  

  hSim->SetLineColor(4);
  //hSim->Scale(0.2);
  hSim->Draw("same hist");

}
