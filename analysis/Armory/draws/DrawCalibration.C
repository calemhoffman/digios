void DrawCalibration(Int_t param=0){
  TTree *psd_tree;
  
  if (param==0) {
  TCanvas *cc = new TCanvas("cc","xf eq xn",1200,1000);
  cc->Clear();
  Int_t i=0;
  TString cut="";
  //cc->Divide(6,4);
  //for (Int_t i=0;i<24;i++) {
  //  cc->cd(i+1);
    psd_tree->Draw(Form("xf[%d]:xn[%d]>>hxfxn%d(1000,50,2000,1000,50,2000)",i,i,i),cut,Form(""));
    printf(Form("XFXN Det %d\n",i));
  } else {

    // TCanvas *cc = new TCanvas("cc","xf eq xn",1200,1000);
  Int_t ii=0;
  TString cut1="CUTG";
  //cc->Divide(6,4);
  //for (Int_t i=0;i<24;i++) {
  //  cc->cd(i+1);
  psd_tree->Draw(Form("xf[%d]:xn[%d]>>hxfxn%dg(1000,50,2000,1000,50,2000)",ii,ii,ii),cut1,Form(""));
  printf(Form("XFXNg Det %d\n",ii));
  }
}
