{

TFile *f[200];
#define numRuns 30
//TCanvas *cc[numRuns];
TCanvas *cc0 = new TCanvas("cc0","rdt0",2200,1400);
cc0->Clear(); cc0->Divide(6,5); cc0->cd();
// TCanvas *cc1 = new TCanvas("cc1","rdt1",2200,1400);
// cc1->Clear(); cc1->Divide(6,5); cc1->cd();
// TCanvas *cc2 = new TCanvas("cc2","rdt2",2200,1400);
// cc2->Clear(); cc2->Divide(6,5); cc2->cd();
// TCanvas *cc3 = new TCanvas("cc3","rdt3",2200,1400);
// cc3->Clear(); cc3->Divide(6,5); cc3->cd();
int runList[numRuns] = {123,127,129,131,133,
                  135,137,139,141,143,
                  145,153,157,159,161,
                  163,165,167,169,171,
                  173,175,177,181,183,
                  185,187,189,191,195};
int i = 0;
for (Int_t run=0;run<numRuns;run++) {
    //cc[runList[run]] = new TCanvas(Form("cc%d",run),Form("rdt%d",run));
    cc0->cd(run+1);
    f[runList[run]] = new TFile(Form("../root_data/gen_run%d.root",runList[run]));
    TH2F *hrdt[4];
    TH1F *hrdtDE[4];
    TH1F *hrdtE[4];
    TH1F *hrdtTOTE[4];
    hrdt[i] = new TH2F(Form("hrdt%d",i),Form("hrdt%d_%d",i,runList[run]),1000,1000,4500,500,500,1100);
    hrdtDE[i] = new TH1F(Form("hrdtDE%d",i),Form("hrdtDE%d_%d",i,runList[run]),500,500,1100);
    hrdtE[i] = new TH1F(Form("hrdtE%d",i),Form("hrdtE%d_%d",i,runList[run]),1000,1000,4500);
    hrdtTOTE[i] = new TH1F(Form("hrdtTOTE%d",i),Form("hrdtTOTE%d_%d",i,runList[run]),1000,1000,5600);
    gen_tree->Draw("rdt[1]:rdt[0]>>hrdt0","","col");
    gen_tree->Draw("rdt[0]>>hrdtE0");
    TString nam="rdt[1]+rdt[0]>>hrdtTOTE";
    TString+=run;
    gen_tree->Draw(nam,"","");
    if (run>0) hrdtTOTE0->Draw("same");
}

}