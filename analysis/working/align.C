void align(){
  Float_t output[30]={};
  TCanvas *cta=new TCanvas("cta","cta");
  TF1 *g1=new TF1("g1","gaus",-15,10,"");
  TH1D *pjy=new TH1D("pjy","y proj",40,-20,20);
  for(Int_t j=0;j<4;j++){
    
    for(Int_t i=0;i<30;i++){
      hrtac[j]->ProjectionY("pjy",i,i+1,"")->Draw();
      pjy->Fit("g1","QR","",-10,5);
      output[i]=(Float_t)g1->GetParameter(1);
      cta->Update();
      //gPad->WaitPrimitive();
    }
    for(Int_t k=0;k<30;k++){
      if(j==0&&k==0) cout<<"{";
      if(k==0) cout<<"{";
      if(k<29) cout<<output[k]<<",";
      if(k==29&&j<3) cout<<output[k]<<"},"<<endl;;
      if(k==29&&j==3) cout<<output[k]<<"}};"<<endl;;
      output[k]=0;
    }
  }
}
