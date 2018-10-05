void listDraws(void) {
  printf("----------------- List of Plots -------------------\n");
  printf("---------------------------------------------------\n");
  printf("     eRaw() - Raw E for all 24 detectors\n");
  printf("     xfxn() - Raw XF vs. XN for all 24 detectors\n");
  printf("    xfxne() - Raw E vs. XF+XN for all 24 detectors\n");
  printf("---------------------------------------------------\n");
  printf("       eC() - Corrected E for all 24 detectors\n");
  printf("    eCall() - Corrected E for all 24 detectors (at same plot)\n");
  printf("    xfxnC() - Corrected XF vs. XN for all 24 detectors\n");
  printf("   xfxneC() - Raw E vs. Corrected XF+XN for all 24 detectors\n");
  printf("---------------------------------------------------\n");
  printf("      eVx() - Raw E vs. X for all 24 detectors\n");
  printf("ecalVxcal() - Cal E vs. X for all 24 detectors\n");
  printf("---------------------------------------------------\n");
  printf("   recoil() - Raw DE vs. E Recoil spectra\n");
  printf("     elum() - Luminosity Energy Spectra\n");
  printf("---------------------------------------------------\n");
  printf("   ecalVz() - Energy vs. Z\n");
  printf("ecalVzRow() - Energy vs. Z for each row\n");
  printf("   excite() - Excitation Energy\n");
  printf("fitExcite() - Auto-Fit the Excitation Energy\n");  
}

void eRaw(void) {
  TCanvas *cERaw = new TCanvas("cERaw","E raw",1200,800);
  cERaw->Clear();cERaw->Divide(6,4);
  for (Int_t i=0; i<24; i++) {
    cERaw->cd(i+1); 
    cERaw->cd(i+1)->SetGrid();
    he[i]->Draw("");
  }
}


void xfxn(void) {
  TCanvas *cxfxn = new TCanvas("cxfxn","XF vs. XN",1200,800);
  cxfxn->Clear(); cxfxn->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cxfxn->cd(i+1); 
    cxfxn->cd(i+1)->SetGrid(); 
    hxfxn[i]->Draw("col");
  }
}

void xfxne(void) {
  TCanvas *cxfxne = new TCanvas("cxfxne","E - XF+XN",1200,800);
  cxfxne->Clear(); cxfxne->Divide(6,4);
  TLine line(0,0, 4000, 4000); line.SetLineColor(2);
  for (Int_t i=0;i<24;i++) {
    cxfxne->cd(i+1); 
    cxfxne->cd(i+1)->SetGrid(); 
    hxfxne[i]->Draw("col");
    line.Draw("same");
  }
}

void eC(void) {
  TCanvas *cEC = new TCanvas("cEC","E corrected",1200,800);
  cEC->Clear();cEC->Divide(6,4);
  for (Int_t i=0; i<24; i++) {
    cEC->cd(i+1); 
    cEC->cd(i+1)->SetGrid();
    heC[i]->Draw("");
  }
}

void eCall(void) {
  TCanvas *cECall = new TCanvas("cECall","E corrected",1200,800);
  cECall->Clear();cECall->Divide(6,2);
  //plot same position
  for(int i = 0; i < 6 ; i++){
     cECall->cd(i+1);
     cECall->cd(i+1)->SetGrid();
     heC[i]->SetLineColor(1); heC[i]->Draw("");
     for(int j = 1 ; j < 4; j++){
        heC[6*j + i]->SetLineColor(j+1); 
        heC[6*j + i]->Draw("same"); 
     }
  }
  //plot same side
  TH1F * heC2[24];
  for (Int_t i = 0; i< 4; i++) {
     cECall->cd(i+7);
     cECall->cd(i+7)->SetGrid();
     heC2[6*i] = (TH1F* )heC[6*i]->Clone();
     heC2[6*i]->SetLineColor(1); heC2[6*i]->Draw("");
     for( int j = 1; j < 6; j++){
         heC2[6*i+j] = (TH1F* )heC[6*i+j]->Clone();
         heC2[6*i+j]->SetLineColor(j+1);
         heC2[6*i+j]->Draw("same");
     }
  }
}


void xfxnC(void) {
  TCanvas *cxfxnC = new TCanvas("cxfxnC","XF vs XN corrected",1200,800);
  cxfxnC->Clear(); cxfxnC->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cxfxnC->cd(i+1); 
    cxfxnC->cd(i+1)->SetGrid(); 
    hxfxnC[i]->Draw("col");
  }
}

void xfxneC(void) {
  TCanvas *cxfxneC = new TCanvas("cxfxneC","Raw E - Corrected XF+XN",1200,800);
  cxfxneC->Clear(); cxfxneC->Divide(6,4);
  TLine line(0,0, 4000, 4000); line.SetLineColor(2);
  for (Int_t i=0;i<24;i++) {
    cxfxneC->cd(i+1); 
    cxfxneC->cd(i+1)->SetGrid(); 
    hxfxneC[i]->Draw("col");
    line.Draw("same");
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
  gStyle->SetOptStat("neiou");
  cecalVz->cd(1);hecalVz->Draw("col");
  cecalVz->cd(2);hecalVzR->Draw("col");
}

void ecalVzRow() {
  TCanvas *cecalVzRow = new TCanvas("cevalVzRow","ECALVZ",1200,800);
  cecalVzRow->Clear(); cecalVzRow->Divide(2,2);
  gStyle->SetOptStat("neiou");
  
  for(int row = 0; row < 4; row ++){
     cecalVzRow->cd(row+1);
     cecalVzRow->cd(row+1)->SetGrid();
     hecalVzRow[row]->Draw("colz");
  }
  
}

void excite(void) {
  TCanvas *cex = new TCanvas("cex","EX",1000,650);
  cex->Clear();cex->Divide(2,1);
  gStyle->SetOptStat("neiou");
  cex->cd(1); hexC->Draw("");
  cex->cd(2); hexR->Draw("");
}
void tac(void) {
  TCanvas *ctac = new TCanvas("ctac","ARRAY-RDT",1000,650);
  ctac->Clear();ctac->SetGrid(0);ctac->Divide(6,4);
  gStyle->SetOptStat("neiou");
  for (Int_t i=0;i<24;i++) {
    ctac->cd(i+1); htacArray[i]->Draw("");
    //   cutG = (TCutG *)cutList->At(i);
    //   cutG->Draw("same");
  }
}

int nPeaks = 16;
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}

void fitExcite(){
  TCanvas *cFitExcite = new TCanvas("cFitExcite","Fitting on Ex", 800, 100, 800,800);
  cFitExcite->Divide(1,2);
  
  gStyle->SetOptStat("neiou");
  cFitExcite->cd(1);
  hexR->Draw();
  
  TH1F * specS = (TH1F*) hexR->Clone();
  double xMin = hexR->GetXaxis()->GetXmin();
  double xMax = hexR->GetXaxis()->GetXmax();
  int xBin = hexR->GetXaxis()->GetNbins();
  
  TString titleH;
  titleH.Form("fitted spectrum; Ex [MeV]; Count / %4.0f keV", (xMax-xMin)*1000./xBin );
  specS->SetTitle(titleH);   
  specS->SetName("specS");
  //specS->GetXaxis()->SetTitleSize(0.06);
  //specS->GetYaxis()->SetTitleSize(0.06);
  //specS->GetXaxis()->SetTitleOffset(0.7);
  //specS->GetYaxis()->SetTitleOffset(0.6);
  
  //=================== find peak and fit
  printf("============= estimate background and find peak\n");
  TSpectrum * peak = new TSpectrum(50);
  nPeaks = peak->Search(hexR, 1, "", 0.1); 
  TH1 * h1 = peak->Background(hexR,10);
  h1->Draw("same");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  cFitExcite->cd(2)->SetGrid();
  cFitExcite->cd(2);

  specS->Add(h1, -1.);
  specS->Sumw2();
  specS->Draw();

  //========== Fitting 
  printf("============= Fit.....");
  printf(" found %d peaks \n", nPeaks);

  double * xpos = peak->GetPositionX();
  double * ypos = peak->GetPositionY();

  int * inX = new int[nPeaks];
  TMath::Sort(nPeaks, xpos, inX, 0 );
  vector<double> energy, height;
  for( int j = 0; j < nPeaks; j++){
    energy.push_back(xpos[inX[j]]);
    height.push_back(ypos[inX[j]]);
  }

  const int  n = 3 * nPeaks;
  double * para = new double[n]; 
  for(int i = 0; i < nPeaks ; i++){
    para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
    para[3*i+1] = energy[i];
    para[3*i+2] = 0.08;
  }

  TF1 * fit = new TF1("fit", fpeaks, xMin, xMax, 3* nPeaks );
  fit->SetLineWidth(1);
  fit->SetLineColor(2);
  fit->SetNpx(1000);
  fit->SetParameters(para);
  specS->Fit("fit", "q");

  printf("============= display\n");   
  const Double_t* paraE = fit->GetParErrors();
  const Double_t* paraA = fit->GetParameters();

  double bw = specS->GetBinWidth(1);

  double * ExPos = new double[nPeaks];
  double * ExSigma = new double[nPeaks];   
  for(int i = 0; i < nPeaks ; i++){
    ExPos[i] = paraA[3*i+1];
    ExSigma[i] = paraA[3*i+2];
    printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
            i, 
            paraA[3*i] / bw,   paraE[3*i] /bw, 
            paraA[3*i+1], paraE[3*i+1],
            paraA[3*i+2], paraE[3*i+2]);
  }
  cFitExcite->Update();
}
