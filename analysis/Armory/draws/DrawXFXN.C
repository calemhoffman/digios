{

  TCanvas *cc = new TCanvas("cc","xfxn",1200,1000);
  cc->Clear();
  cc->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cc->cd(i+1);
    psd_tree->Draw(Form("xf[%d]:xn[%d]>>hxfxn%d(1000,50,2000,1000,50,2000)",i,i,i),Form(""),Form("col"));
    printf(Form("XFXN Det %d\n",i));

  }




}
