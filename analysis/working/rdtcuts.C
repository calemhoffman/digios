void recoilcuts(TString ofile)
{
  TCanvas *cCuts=new TCanvas("Recoil Cuts");
  TCutG *cutpoint;
  TString name;
  TFile *f1=new TFile(ofile,"RECREATE");
  TObjArray *cl1=new TObjArray();
 
  // for(Int_t i=0;i<4;i++){
  //   name.Form("cut%d",i);
  //   hrdt[i]->Draw("colz");
  //   //hrdtg[i]->Draw("colz");
  //   cCuts->Update();
  //   cutpoint=(TCutG*) gPad->WaitPrimitive("CUTG","CutG");
  //   cutpoint->SetName(name);
  //   cutpoint->SetLineColor(i+1);
  //   cout<<name<<endl;
  //   cl1->Add(cutpoint);
  // }
  //   for(Int_t i=0;i<4;i++){
  //   name.Form("cut%d",i+4);
  //   hrdt[i]->Draw("colz");
  //   //hrdtg[i]->Draw("colz");
  //   cCuts->Update();
  //   cutpoint=(TCutG*) gPad->WaitPrimitive("CUTG","CutG");
  //   cutpoint->SetName(name);
  //   cutpoint->SetLineColor(i+1);
  //   cout<<name<<endl;
  //   cl1->Add(cutpoint);
  // }
    name=("ezcut");
    heCalVz->Draw();
    heCalVzGC->SetMarkerStyle(20);
    heCalVzGC->Draw("same");
    cCuts->Update();
    cutpoint->SetName(name);
    cutpoint->SetLineColor(2);
    cout<<name<<endl;
    cl1->Add(cutpoint);

    cutpoint=(TCutG*) gPad->WaitPrimitive("CUTG","CutG");
    cl1->Write("cutList", TObject::kSingleKey);



}
