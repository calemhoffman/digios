void recoilcuts(TString ofile)
{
  TCanvas *cCuts=new TCanvas("Recoil Cuts");
  TCutG *cutpoint;
  TString name;
  TFile *f1=new TFile(ofile,"RECREATE");
  TObjArray *cl1=new TObjArray();
 
  for(Int_t i=0;i<4;i++){
    name.Form("cut%d",i);
    hrdtg[i]->Draw();
    cCuts->Update();
    cutpoint=(TCutG*) gPad->WaitPrimitive("CUTG","CutG");
    cutpoint->SetName(name);
    cutpoint->SetLineColor(i+1);
    cout<<name<<endl;
    cl1->Add(cutpoint);
  }
cl1->Write("cutList", TObject::kSingleKey);



}
