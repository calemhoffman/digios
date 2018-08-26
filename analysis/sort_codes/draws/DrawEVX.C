{
  /*
  TCanvas *cc = new TCanvas("cc","xfxn",1200,1000);
  cc->Clear();
  cc->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cc->cd(i+1);
    psd_tree->Draw(Form("xf[%d]:xn[%d]>>hxfxn%d(1000,50,2000,1000,50,2000)",i,i,i),Form(""),Form("col"));
    printf(Form("XFXN Det %d\n",i));

  }
  */
 TCanvas *cc2 = new TCanvas("cc2","e vs x",1200,1000);
  cc2->Clear();
  cc2->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cc2->cd(i+1);
    //    psd_tree->SetAlias(Form("x%d",i),Form("(xf[%d]-xn[%d])/(xf[%d]+xn[%d])",i,i,i,i));
    psd_tree->Draw(Form("e[%d]:(xf[%d]-xn[%d])/(xf[%d]+xn[%d])>>heVx%d(1000,-1.5,1.5,1000,50,2000)",i,i,i,i,i,i),Form(""),Form("col"));
 printf(Form("E vs X Det %d\n",i));

  }





}
