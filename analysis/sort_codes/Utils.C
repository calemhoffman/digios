void listDraws(void) {
  printf("xfxn() - Raw XF vs. XN for all 24 detectors\n");
  printf("eVx() - Raw E vs. X for all 24 detectors\n");
  printf("ecalVxcal() - Cal E vs. X for all 24 detectors\n");
  printf("elum() - Luminosity Energy Spectra\n");
  printf("recoil() - Raw DE vs. E Recoil spectra\n");
  printf("ecalVz() - Energy vs. Z\n");
  printf("excite() - Excitation Energy\n");
}

void xfxn(void) {
  TCanvas *cxfxn = new TCanvas("cxfxn","XFXN",1200,800);
  cxfxn->Clear(); cxfxn->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cxfxn->cd(i+1); hxfxn[i]->Draw("col");
  }
}

void eVx(void) {
  TCanvas *ceVx = new TCanvas("ceVx","EVX",1200,800);
  ceVx->Clear(); ceVx->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    ceVx->cd(i+1); heVx[i]->Draw("col");
  }
}

void ecalVxcal(void) {
  TCanvas *cecalVxcal = new TCanvas("cecalVxcal","ECALVXCAL",1200,800);
  cecalVxcal->Clear(); cecalVxcal->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cecalVxcal->cd(i+1); hecalVxcal[i]->Draw("col");
  } 
}

void elum(void) {
  TCanvas *celum = new TCanvas("celum","ELUM",1000,1000);
  celum->Clear(); celum->Divide(2,2);
  helum[0]->Draw("colz");
}

void recoil(void) {
  TCanvas *crdt = new TCanvas("crdt","RDT",1000,1000);
  crdt->Clear();crdt->Divide(2,2);
  for (Int_t i=0;i<4;i++) {
    crdt->cd(i+1); hrdt[i]->Draw("col");
    //   cutG = (TCutG *)cutList->At(i);
    //   cutG->Draw("same");
  }
}

void ecalVz(void) {
  TCanvas *cecalVz = new TCanvas("cevalVz","ECALVZ",1000,650);
  cecalVz->Clear(); cecalVz->Divide(2,1);
  cecalVz->cd(1);hecalVz->Draw("col");
  cecalVz->cd(2);hecalVzR->Draw("col");
}

void excite(void) {
  TCanvas *cex = new TCanvas("cex","EX",1000,650);
  cex->Clear();cex->Divide(2,1);
  cex->cd(1); hexC->Draw("");
  cex->cd(2); hexR->Draw("");
}
void tac(void) {
  TCanvas *ctac = new TCanvas("ctac","ARRAY-RDT",1000,650);
  ctac->Clear();ctac->SetGrid(0);ctac->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    ctac->cd(i+1); htacArray[i]->Draw("");
    //   cutG = (TCutG *)cutList->At(i);
    //   cutG->Draw("same");
  }
}
