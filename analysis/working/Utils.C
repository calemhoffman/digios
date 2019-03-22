//This file runs after on Monitor.C
//This file is parasite on Monitor.C

int canvasSize[2] = {2000, 1200};

void listDraws(void) {
  printf("------------------- List of Plots -------------------\n");
  printf("  newCanvas() - Creaet a new Canvas\n");
  printf("-----------------------------------------------------\n");
  printf("    HitStat() - Hit statistics for all %d detectors\n", numDet);
  printf("      rawID() - Raw \e[31me\e[0m, \e[31mring\e[0m, \e[31mxf\e[0m, \e[31mxn\e[0m vs detID\n");
  printf("       rawe() - Raw \e[31me\e[0m for all %d detectors\n", numDet);
  printf("    rawring() - Raw \e[31mring\e[0m for all %d detectors\n", numDet);
  printf("      rawxf() - Raw \e[31mxf\e[0m for all %d detectors\n", numDet);
  printf("      rawxn() - Raw \e[31mxn\e[0m for all %d detectors\n", numDet);
  printf("     eVring() - Raw \e[31me\e[0m vs. \e[31mring\e[0m for all %d detectors\n", numDet);
  printf("      xfVxn() - Raw \e[31mxf\e[0m vs. \e[31mxn\e[0m for all %d detectors\n", numDet);
  printf("       eVxs() - Raw \e[31me\e[0m vs. Raw \e[31mxs = xf + xn\e[0m for all %d detectors\n", numDet);
  printf("        eVx() - Raw \e[31me\e[0m vs. RAW \e[31mx\e[0m for all %d detectors\n", numDet);
  printf("-----------------------------------------------------\n");
  printf("    eVxsCal() - Raw \e[31me\e[0m vs. Corrected \e[31mxs\e[0m for all %d detectors\n", numDet);
  printf("       ecal() - Calibrated \e[31me\e[0m for all %d detectors\n", numDet);
  printf("      ecal2() - Calibrated \e[31me\e[0m for all %d detectors (same row or same col)\n", numDet);
  printf("xfCalVxnCal() - Calibrated \e[31mxf\e[0m vs. \e[31mxn\e[0m for all %d detectors\n", numDet);
  printf("-----------------------------------------------------\n");
  printf("  eCalVxCal() - Cal \e[31me\e[0m vs. \e[31mx\e[0m for all %d detectors\n", numDet);
  printf("-----------------------------------------------------\n");
  printf("     recoil() - Raw DE vs. E Recoil spectra\n");
  printf("       elum() - Luminosity Energy Spectra\n");
  printf("-----------------------------------------------------\n");
  printf("     eCalVz() - Energy vs. Z\n");
  printf("  eCalVzRow() - Energy vs. Z for each row\n");
  printf("     excite() - Excitation Energy\n");
  printf("-----------------------------------------------------\n");
  printf("   ShowFitMethod() - Show various fittign methods \n");
}


int xD, yD;
void FindBesCanvasDivision(int nPad){
  for( int i = TMath::Sqrt(nPad); i >= 2 ; i--){
    if( nPad % i == 0 ) {
      yD = i;
      xD = nPad/i;      
      break;
    }
  }
}

int nCanvas=0;
void newCanvas(int sizeX = 800, int sizeY = 600, int posX = 0, int posY = 0){
  TString name; name.Form("cNewCanvas%d", nCanvas);
  TCanvas * cNewCanvas = new TCanvas(name, name, posX, posY, sizeX, sizeY);
  nCanvas++;
}


//TODO set histogram y-axis all the same heigh
void HitStat(void) {
  TCanvas *cStat = (TCanvas *) gROOT->FindObjectAny("cStat");
  if( cStat == NULL ) {
    cStat = new TCanvas("cStat","Hit Statistics",canvasSize[0], canvasSize[1]);
    cStat->Clear();cStat->Divide(numCol,numRow);
    gStyle->SetOptStat("neiou");
    for (Int_t i=0; i<numDet; i++) {
      cStat->cd(i+1); 
      cStat->cd(i+1)->SetGrid();
      cStat->cd(i+1)->SetLogy();
      hStat[i]->Draw("");
    }
  }else{
    cStat->Show();
  }
  
}

void rawID(){
  TCanvas * cRawID = (TCanvas *) gROOT->FindObjectAny("cRawID");
  if( cRawID == NULL ) cRawID = new TCanvas("cRawID", "Raw e, Ring, xf, xn vs ID", canvasSize[0], canvasSize[1]);
  cRawID->Clear();cRawID->Divide(2,2);
  cRawID->cd(1);  cRawID->cd(1)->SetGrid(); heVID->Draw("colz");
  cRawID->cd(2);  cRawID->cd(2)->SetGrid(); hringVID->Draw("colz");
  cRawID->cd(3);  cRawID->cd(3)->SetGrid(); hxfVID->Draw("colz");
  cRawID->cd(4);  cRawID->cd(4)->SetGrid(); hxnVID->Draw("colz");
}

void rawe(Bool_t isLogy = false) {
  TCanvas *cRawE = (TCanvas *) gROOT->FindObjectAny("cRawE");
  if( cRawE == NULL ) cRawE = new TCanvas("cRawE","E raw",canvasSize[0], canvasSize[1]);
  cRawE->Clear();cRawE->Divide(numCol,numRow);
  for (Int_t i=0; i<numDet; i++) {
    cRawE->cd(i+1); 
    cRawE->cd(i+1)->SetGrid();
    if( isLogy ) cRawE->cd(i+1)->SetLogy();
    he[i]->Draw("");
  }
}

void rawring(Bool_t isLogy = false) {
  TCanvas *cRawRing = (TCanvas *) gROOT->FindObjectAny("cRawRing");
  if( cRawRing == NULL ) cRawRing = new TCanvas("cRawRing","Ring raw",canvasSize[0], canvasSize[1]);
  cRawRing->Clear();cRawRing->Divide(numCol,numRow);
  for (Int_t i=0; i<numDet; i++) {
    cRawRing->cd(i+1); 
    cRawRing->cd(i+1)->SetGrid();
    if( isLogy ) cRawRing->cd(i+1)->SetLogy();
    hring[i]->Draw("");
  }
}

void rawxf(Bool_t isLogy = false) {
  TCanvas *cRawXf = (TCanvas *) gROOT->FindObjectAny("cRawXf");
  if( cRawXf == NULL ) cRawXf = new TCanvas("cRawXf","Xf raw",canvasSize[0], canvasSize[1]);
  cRawXf->Clear();cRawXf->Divide(numCol,numRow);
  for (Int_t i=0; i<numDet; i++) {
    cRawXf->cd(i+1); 
    cRawXf->cd(i+1)->SetGrid();
    if( isLogy ) cRawXf->cd(i+1)->SetLogy();
    hxf[i]->Draw("");
  }
}

void rawxn(Bool_t isLogy = false) {
  TCanvas *cRawXn = (TCanvas *) gROOT->FindObjectAny("cRawXn");
  if( cRawXn == NULL ) cRawXn = new TCanvas("cRawXn","Xn raw",canvasSize[0], canvasSize[1]);
  cRawXn->Clear();cRawXn->Divide(numCol,numRow);
  for (Int_t i=0; i<numDet; i++) {
    cRawXn->cd(i+1); 
    cRawXn->cd(i+1)->SetGrid();
    if( isLogy ) cRawXn->cd(i+1)->SetLogy();
    hxn[i]->Draw("");
  }
}

void eVring(void) {
  TCanvas *cRawERing = (TCanvas *) gROOT->FindObjectAny("cRawERing");
  if( cRawERing == NULL ) cRawERing = new TCanvas("cRawERing","XF vs. XN",canvasSize[0], canvasSize[1]);
  cRawERing->Clear(); cRawERing->Divide(numCol,numRow);
  for (Int_t i=0;i<numDet;i++) {
    cRawERing->cd(i+1); 
    cRawERing->cd(i+1)->SetGrid(); 
    heVring[i]->Draw("col");
  }
}

void xfVxn(void) {
  TCanvas *cxfxn = (TCanvas *) gROOT->FindObjectAny("cxfxn");
  if( cxfxn == NULL ) cxfxn = new TCanvas("cxfxn","XF vs. XN",canvasSize[0], canvasSize[1]);
  cxfxn->Clear(); cxfxn->Divide(numCol,numRow);
  for (Int_t i=0;i<numDet;i++) {
    cxfxn->cd(i+1); 
    cxfxn->cd(i+1)->SetGrid(); 
    hxfVxn[i]->Draw("col");
  }
}

void eVxs(void) {
  TCanvas *cxfxne = new TCanvas("cxfxne","E - XF+XN",canvasSize[0], canvasSize[1]);
  cxfxne->Clear(); cxfxne->Divide(numCol,numRow);
  TLine line(0,0, 4000, 4000); line.SetLineColor(2);
  for (Int_t i=0;i<numDet;i++) {
    cxfxne->cd(i+1); 
    cxfxne->cd(i+1)->SetGrid(); 
    heVxs[i]->Draw("col");
    line.Draw("same");
  }
}

void ecal(void) {
  TCanvas *cEC = new TCanvas("cEC","E corrected",canvasSize[0], canvasSize[1]);
  cEC->Clear();cEC->Divide(numCol,numRow);
  for (Int_t i=0; i<numDet; i++) {
    cEC->cd(i+1); 
    cEC->cd(i+1)->SetGrid();
    heCal[i]->Draw("");
  }
}

void ecal2(void) {
  TCanvas *cECall = new TCanvas("cECall","E corrected",canvasSize[0], canvasSize[1]);
  
  int maxRC = TMath::Max(numRow, numCol);
  cECall->Clear();cECall->Divide(maxRC,2);
  //plot same position
  for(int i = 0; i < numCol ; i++){
     cECall->cd(i+1);
     cECall->cd(i+1)->SetGrid();
     heCal[i]->SetLineColor(1); heCal[i]->Draw("");
     for(int j = 1 ; j < numRow; j++){
        heCal[numCol*j + i]->SetLineColor(j+1); 
        heCal[numCol*j + i]->Draw("same"); 
     }
  }
  //plot same side
  TH1F * heC2[numDet];
  for (Int_t i = 0; i< numRow; i++) {
     cECall->cd(i+maxRC+1);
     cECall->cd(i+maxRC+1)->SetGrid();
     heC2[numCol*i] = (TH1F* )heCal[numCol*i]->Clone();
     heC2[numCol*i]->SetLineColor(1); heC2[numCol*i]->Draw("");
     for( int j = 1; j < numCol; j++){
         heC2[numCol*i+j] = (TH1F* )heCal[numCol*i+j]->Clone();
         heC2[numCol*i+j]->SetLineColor(j+1);
         heC2[numCol*i+j]->Draw("same");
     }
  }
}


void xfCalVxnCal(void) {
  TCanvas *cxfxnC = new TCanvas("cxfxnC","XF vs XN corrected",canvasSize[0], canvasSize[1]);
  cxfxnC->Clear(); cxfxnC->Divide(numCol,numRow);
  for (Int_t i=0;i<numDet;i++) {
    cxfxnC->cd(i+1); 
    cxfxnC->cd(i+1)->SetGrid(); 
    hxfCalVxnCal[i]->Draw("col");
  }
}

void eVxsCal(void) {
  TCanvas *cxfxneC = new TCanvas("cxfxneC","Raw E - Corrected XF+XN",canvasSize[0], canvasSize[1]);
  cxfxneC->Clear(); cxfxneC->Divide(numCol,numRow);
  TLine line(0,0, 4000, 4000); line.SetLineColor(2);
  for (Int_t i=0;i<numDet;i++) {
    cxfxneC->cd(i+1); 
    cxfxneC->cd(i+1)->SetGrid(); 
    heVxsCal[i]->Draw("col");
    line.Draw("same");
  }
}

void eVx(void) {
  TCanvas *ceVx = new TCanvas("ceVx","EVX",canvasSize[0], canvasSize[1]);
  ceVx->Clear(); ceVx->Divide(numCol,numRow);
  for (Int_t i=0;i<numDet;i++) {
    ceVx->cd(i+1); heVx[i]->Draw("col");
  }
}

void eCalVxCal(void) {
  TCanvas *cecalVxcal = new TCanvas("cecalVxcal","ECALVXCAL",canvasSize[0], canvasSize[1]);
  cecalVxcal->Clear(); cecalVxcal->Divide(numCol,numRow);
  for (Int_t i=0;i<numDet;i++) {
    cecalVxcal->cd(i+1); heCalVxCal[i]->Draw("col");
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

void eCalVz(void) {
  TCanvas *cecalVz = new TCanvas("cevalVz","ECALVZ",1000,650);
  cecalVz->Clear(); cecalVz->Divide(2,1);
  gStyle->SetOptStat("neiou");
  cecalVz->cd(1);heCalVz->Draw("col");
  cecalVz->cd(2);heCalVzGC->Draw("col");
}

void eCalVzRow() {
  TCanvas *cecalVzRow = new TCanvas("cevalVzRow","ECALVZ",canvasSize[0], canvasSize[1]);
  FindBesCanvasDivision(numRow);
  cecalVzRow->Clear(); cecalVzRow->Divide(xD,yD);
  gStyle->SetOptStat("neiou");
  
  for(int row = 0; row < numRow; row ++){
     cecalVzRow->cd(row+1);
     cecalVzRow->cd(row+1)->SetGrid();
     hecalVzRow[row]->Draw("colz");
  }
  
}

void excite(void) {
  TCanvas *cex = new TCanvas("cex","EX",1000,650);
  cex->Clear();cex->Divide(2,1);
  gStyle->SetOptStat("neiou");
  cex->cd(1); hEx->Draw("");
  cex->cd(2); hexR->Draw("");
}

void tac(void) {
  TCanvas *ctac = new TCanvas("ctac","ARRAY-RDT",1000,650);
  ctac->Clear();ctac->SetGrid(0);ctac->Divide(numCol,numRow);
  gStyle->SetOptStat("neiou");
  for (Int_t i=0;i<numDet;i++) {
    ctac->cd(i+1); htacArray[i]->Draw("");
    //   cutG = (TCutG *)cutList->At(i);
    //   cutG->Draw("same");
  }
}

