{

Int_t k = 1;
Int_t ab = 0;

const char * AB = "a";
if (ab==1) {AB = "b";}
Int_t cutYN = 0;

TFile *file0 = new TFile ("../root_data/h066_31Si_trace_run025.root", "read"); 
TTree *tree = (TTree*)file0->Get("tree");
printf("=====> is loaded. Total #Entry: %10lld \n", tree->GetEntries());

Float_t p1 = 1.0;
Float_t p2 = 0;

TCanvas *cc = new TCanvas("cc","cc",1400,800);cc->ToggleEditor();cc->ToggleToolBar();
cc->Clear();
TH1F * h[60];
TH1F * hSum;
hSum = new TH1F("hSum","hSum",400,-1,11);
for (i=k;i<k+1;i++) {
    for (int j=0;j<2;j++) {
        TString nameis;
        AB = "a";
        if (j==1) {AB = "b";}
        h[i+i*j] = new TH1F(Form("hEx%d%s",i,AB),Form("hEx%d%s",i,AB),400,-1,11);
        tree->SetAlias("ep",Form("(e[%d]*%f + %f)",i,p1,p2));
        h[i+i*j]->SetLineColor(i + i*j+ 1);
        if (j==0) {
            tree->Draw(Form("Ex>>hEx%d%s",i,AB),Form("x < 0 && x > -1 && x < 1 && thetaCM>12 && detID == %d",i),"");
            fitNGauss(h[i+i*j],50);
        } else {
            tree->Draw(Form("Ex>>hEx%d%s",i,AB),Form("x > 0 && x > -1 && x < 1 && thetaCM>12 && detID == %d",i),"same");
            h[i+i*j]->Rebin();
            fitNGauss(h[i+i*j],50);
        }
        hSum->Add(h[i+i*j]);
    }
}
// cc->Clear(); 
// hSum->SetLineColor(kBlue);hSum->Draw("");
// tree->Draw("Ex>>hEX(400,-1,11)",Form("x > -1 && x < 1 && (detID == 20 || detID ==21 || detID ==22 || detID ==23 || detID==24)"),"same");

}