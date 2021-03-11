{
  TFile * fSim = new TFile("152Eu_Sim_hist.root");

  TFile * fExp = new TFile("152Eu.root");

  gStyle->SetOptStat("");

  TH1F * hExp = (TH1F *) fExp->Get("h1g0");
  TH1F * hSim = (TH1F *) fSim->Get("h0");

  hExp->GetYaxis()->SetRangeUser(1, 3000);
  hExp->Draw();
  

  hSim->SetLineColor(2);
  hSim->Scale(0.2);
  hSim->Draw("same hist");

}
