{
TFile *f[200];
TCanvas *cc0;
cc0 = new TCanvas("cc0","rdt0",2200,1400);
cc0->Clear(); cc0->Divide(2,2); cc0->cd();
// TCanvas *cc1 = new TCanvas("cc1","rdt1",2200,1400);
// cc1->Clear(); cc1->Divide(6,5); cc1->cd();
// TCanvas *cc2 = new TCanvas("cc2","rdt2",2200,1400);
// cc2->Clear(); cc2->Divide(6,5); cc2->cd();
// TCanvas *cc3 = new TCanvas("cc3","rdt3",2200,1400);
// cc3->Clear(); cc3->Divide(6,5); cc3->cd();
// int runList[numRuns] = {123,127,129,131,133,
//                   135,137,139,141,143,
//                   145,153,157,159,161,
//                   163,165,167,169,171,
//                   173,175,177,181,183,
//                   185,187,189,191,195};
int runList[2] = {133,175};
for (Int_t run=0;run<1;run++) {
    //cc[runList[run]] = new TCanvas(Form("cc%d",run),Form("rdt%d",run));
    f[runList[run]] = new TFile(Form("../root_data/h066_31Si_trace_run%d.root",runList[run]));
    TTree *tree = (TTree*)f[runList[run]]->Get("tree");
    TH2F *hrdt[4];
    TH1F *hrdtDE[4];
    TH1F *hrdtE[4];
    TH1F *hrdtTOTE[4];
    for (Int_t i=0;i<4;i++) {
        cc0->cd(i+1);
        hrdtTOTE[i] = new TH1F(Form("hrdtTOTE%d",i),Form("hrdtTOTE%d_%d",i,runList[run]),1000,1000,5600);
        for (int j=0;j<1;j++) {
            TString draw_string(Form("rdt[%d]+rdt[%d]>>hrdtTOTE%d",2*i+1,2*i,i));
            tree->Draw(draw_string,"","");
            fit2GaussP1(hrdtTOTE0,4250,40,3650,40,3900,4500);
        }
    }
}
    
}