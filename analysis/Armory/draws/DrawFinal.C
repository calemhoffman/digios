{
  /*
  TCanvas *cc = new TCanvas("cc","Energy vs. Z");
  for (Int_t i=19;i<23;i++) {
    psd_tree->Draw(Form("ecal1_%d:z1_%d>>hez1_%d(500,-55,-25,500,0,12)",i,i,i),Form("xf[%d]>xncal_%d&&x1_%d<0.98",i,i,i),"col");
    psd_tree->Draw(Form("ecal2_%d:z2_%d>>hez2_%d(500,-55,-25,500,0,12)",i,i,i),Form("xf[%d]<=xncal_%d&&x2_%d>0.02",i,i,i),"col");
    psd_tree->Draw(Form("ecal1_%d:z1_%d>>hez1g_%d(500,-55,-25,500,0,12)",i,i,i),Form("xf[%d]>xncal_%d&&rdt[1]>5300&&rdt[1]<5850&&x1_%d<0.98&&(e_t[%d]-rdt_t[1])>-6&&(e_t[%d]-rdt_t[1])<6",i,i,i,i,i),"col");
    psd_tree->Draw(Form("ecal2_%d:z2_%d>>hez2g_%d(500,-55,-25,500,0,12)",i,i,i),Form("xf[%d]<=xncal_%d&&rdt[1]>5300&&rdt[1]<5850&&x2_%d>0.02&&(e_t[%d]-rdt_t[1])>-6&&(e_t[%d]-rdt_t[1])<6",i,i,i,i,i),"col");
  }
  
  cc->Clear();cc->Divide(1,2);cc->cd(1);
  TH2F *hez = new TH2F("hez","Energy Vs. Z (Ungated); Z [cm]; Energy [MeV]",500,-55,-25,500,0,12);
  TH2F *hezg = new TH2F("hezg","Energy Vs. Z (Gated); Z [cm]; Energy [MeV]",500,-55,-25,500,0,12);

  hez->Add(hez1_19); hez->Add(hez2_19); hez->Add(hez1_20); hez->Add(hez2_20);
  hez->Add(hez1_21); hez->Add(hez2_21); hez->Add(hez1_22); hez->Add(hez2_22);
  hez->SetMinimum(0);hez->Draw("col");

  cc->cd(2);
  hezg->Add(hez1g_19); hezg->Add(hez2g_19); hezg->Add(hez1g_20); hezg->Add(hez2g_20);
  hezg->Add(hez1g_21); hezg->Add(hez2g_21); hezg->Add(hez1g_22); hezg->Add(hez2g_22);
  hezg->SetMinimum(0);hezg->Draw("col");
*/

  TCanvas *ccc = new TCanvas("ccc","Excitation Energy");
   for (Int_t i=19;i<23;i++) {
     psd_tree->Draw(Form("ex1_%d>>hex1_%d(500,-1,9)",i,i),Form("xf[%d]>xncal_%d&&x1_%d<0.98&&e[%d]>100",i,i,i,i),"col");
     psd_tree->Draw(Form("ex2_%d>>hex2_%d(500,-1,9)",i,i),Form("xf[%d]<=xncal_%d&&x2_%d>0.02&&e[%d]>100",i,i,i,i),"col");
     psd_tree->Draw(Form("ex1_%d>>hex1g_%d(500,-1,9)",i,i),Form("xf[%d]>xncal_%d&&rdt[1]>5300&&rdt[1]<5850&&x1_%d<0.98&&(e_t[%d]-rdt_t[1])>-6&&(e_t[%d]-rdt_t[1])<6&&e[%d]>100",i,i,i,i,i,i),"col");
     psd_tree->Draw(Form("ex2_%d>>hex2g_%d(500,-1,9)",i,i),Form("xf[%d]<=xncal_%d&&rdt[1]>5300&&rdt[1]<5850&&x2_%d>0.02&&(e_t[%d]-rdt_t[1])>-6&&(e_t[%d]-rdt_t[1])<6&&e[%d]>100",i,i,i,i,i,i),"col");
  }
   TH1F *hexu = new TH1F("hexu","Excitation Energy (Ungated); Energy [MeV]; Counts / 20 keV",500,-1,9);
   
   TH1F *hexg = new TH1F("hexg","Excitation Energy (Gated); Energy [MeV]; Counts / 20 keV",500,-1,9);

   
   ccc->Clear(); ccc->Divide(1,2);
   ccc->cd(1);
   hexu->Add(hex1_19); hexu->Add(hex2_19); hexu->Add(hex1_20); hexu->Add(hex2_20);
   hexu->Add(hex1_21); hexu->Add(hex2_21); hexu->Add(hex1_22); hexu->Add(hex2_22);
   hexu->Draw("");
   
   ccc->cd(2);
   hexg->Add(hex1g_19); hexg->Add(hex2g_19); hexg->Add(hex1g_20); hexg->Add(hex2g_20);
   hexg->Add(hex1g_21); hexg->Add(hex2g_21); hexg->Add(hex1g_22); hexg->Add(hex2g_22);
   hexg->Draw("");
   
   




   
}
