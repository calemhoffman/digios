{
TFile *f[200];
TCanvas *cc0;
cc0 = new TCanvas("cc0","rdt0",2200,1400);
cc0->ToggleEditor();cc0->ToggleToolBar();
cc0->Clear(); cc0->Divide(2,4); cc0->cd();
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
int runList[2] = {133};
for (Int_t run=0;run<1;run++) {
    //cc[runList[run]] = new TCanvas(Form("cc%d",run),Form("rdt%d",run));
    f[runList[run]] = new TFile(Form("h066_31Si_trace_run%d.root",runList[run]));
    TTree *tree = (TTree*)f[runList[run]]->Get("tree");
    TH2F *hrdt[4];
    TH1F *hrdtDE[4];
    TH1F *hrdtE[4];
    TH1F *hrdtTOTE[4];
    TH1F *hrdtTOTE2[4];
    for (Int_t i=0;i<4;i++) {
        hrdtTOTE[i] = new TH1F(Form("hrdtTOTE%d",i),Form("hrdtTOTE%d_%d",i,runList[run]),1000,3500,5600);
        hrdtTOTE2[i] = new TH1F(Form("hrdtTOTE2%d",i),Form("hrdtTOTE2%d_%d",i,runList[run]),1000,3500,5600);
            TString draw_string(Form("rdt[%d]+rdt[%d]>>hrdtTOTE%d",2*i+1,2*i,i));
            TString draw_string2(Form("trdt[%d]+trdt[%d]>>hrdtTOTE2%d",2*i+1,2*i,i));
            cc0->cd(i+1);
            if (run<1)   tree->Draw(draw_string,"","");
            cc0->cd(i+5);
            if (run < 1) tree->Draw(draw_string2,"","");
            // if (run>0) tree->Draw(draw_string,"","same");
            // fit2GaussP1(hrdtTOTE0,4250,40,3650,40,3900,4500);
    }
}
    
}