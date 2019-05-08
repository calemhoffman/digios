void recoilcuts(TString ofile)
{
  TCanvas *cCuts=new TCanvas("Recoil Cuts");
  TCutG *cutg;
  TString name;
  TFile *f1=new TFile(ofile,"RECREATE");
  TObjArray *cl1=new TObjArray();
 
  for(Int_t i=0;i<4;i++){
    name.Form("cut%d",i);
    hrdtg[i]->Draw();
    cCuts->Update();
    cutg=(TCutG*) gPad->WaitPrimitive("CUTG","CutG");
    cutg->SetName(name);
    cutg->SetLineColor(i+1);
    cout<<name<<endl;
    cl1->Add(cutg);
  }
cutList->Write("cutList", TObject::kSingleKey);



}
