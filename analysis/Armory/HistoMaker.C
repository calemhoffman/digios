#include "HistoMaker.h"

void HistoMaker(TString rootFile = "root_data/s005_32Si_trace_run135.root") {
   //rootFile="root_data/s005_32Si_trace_run120-129.root";
   const char* treeName="tree";
   TFile *file0 = new TFile (rootFile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);
   //general
   tree->SetBranchAddress("eventID",&eventID);
   tree->SetBranchAddress("run",&run);
   //array
   tree->SetBranchAddress("multiHit",&multiHit);
   tree->SetBranchAddress("e",e);
   tree->SetBranchAddress("x",x);
   tree->SetBranchAddress("xf",xf);
   tree->SetBranchAddress("xn",xn);
   tree->SetBranchAddress("te",te);
   tree->SetBranchAddress("te_t",te_t);
   tree->SetBranchAddress("te_r",te_r);
   tree->SetBranchAddress("z",z);
   //recoil
   tree->SetBranchAddress("rdt",rdt);

   //timing

   //histograms
   //general
   TH1F * heventID = new TH1F("heventID","heventID",1000,0,1e6);
   TH1F * hrunID = new TH1F("hrunID","hrunID",200,0,200);

   //array
   TH2F * hxfxn[24];
   TH2F * hesumx[24];
   TH2F * htetx[24];
   TH2F * htetr[24];
   TH2F * htete[24];
   for (int i=0;i<24;i++) {
      hxfxn[i] = new TH2F(Form("hxfxn%d",i),Form("hxfxn%d; xf; xn",i), 1000,0,2500,1000,0,2500);
      hesumx[i] = new TH2F(Form("hesumx%d",i),Form("hesumx%d; e; xf+xn",i), 1000,0,10,1000,0,2500);
      htetx[i] = new TH2F(Form("htetx%d",i),Form("htetx%d; x; te_t",i), 1000,-1.1,1.1,350,90,125);
      htetr[i] = new TH2F(Form("htetr%d",i),Form("htetr%d; te_r; te_t",i), 1000,0,10,350,90,125);
      htete[i] = new TH2F(Form("htete%d",i),Form("htete%d; e; te_t",i), 1000,0,10,350,90,125);
   }
   //recoil
   //ez
   TH2F * hez = new TH2F("hez","hez; z; e",1000,-600,-100,1000,0,10);
   TH2F * hezg = new TH2F("hezg","gated hez; z; e",1000,-600,-100,1000,0,10);
   //timing

   Int_t nEntries = tree->GetEntries();
   printf("nEntries: %d\n",nEntries);

// for (Int_t entryNumber=0;entryNumber<numElistEntries; entryNumber++) {
// //for (Int_t entryNumber=0;entryNumber<nEntries; entryNumber++) {
// chain->GetEntry(elist_all->GetEntry(entryNumber));
// //chain->GetEntry(entryNumber);
// if (((Float_t)entryNumber/(Float_t)numElistEntries)>counter)
// { printf("^_^_^_%4.1f_^_^_^\n",counter*100);
// counter=counter+0.1; }

   for (Int_t entryNumber=0;entryNumber<=nEntries; entryNumber++) {
      tree->GetEntry(entryNumber);
      //printf("eventID: %d",eventID);
      heventID->Fill(eventID); hrunID->Fill(run);
      for (int i=0;i<multiHit;i++) {
         for (int detID=0;detID<24;detID++) {
            if ( TMath::IsNaN(e[detID]) || e[detID]<0.5 ) continue;
            //"no" gates
            hxfxn[detID]->Fill(xf[detID],xn[detID]);
            hesumx[detID]->Fill(e[detID],xf[detID]+xn[detID]);
            htetx[detID]->Fill(x[detID],te_t[detID]);
            htetr[detID]->Fill(te_r[detID],te_t[detID]);
            htete[detID]->Fill(e[detID],te_t[detID]);
            hez->Fill(z[detID],e[detID]);

            //full gates
            if (rdt[4] > 500) { //de back needs a signal
               hezg->Fill(z[detID],e[detID]);
            } //full gates
         } //detID
      } //multiHit
   } //entryNumber

   TFile * outFile = new TFile("HistMaker.root","RECREATE");
   // outFile->mkdir("array");
   hez->Write(); hezg->Write();
   // outFile->cd("array");
   for (int detID=0;detID<24;detID++) {
      hxfxn[detID]->Write(); hesumx[detID]->Write();
      htetx[detID]->Write(); htete[detID]->Write();
      htetr[detID]->Write();
   }

   TCanvas *ctetx = new TCanvas("ctetx","ctetx",1000,800);
   ctetx->Divide(6,4);
   for (int detID=0;detID<24;detID++) {
      ctetx->cd(detID+1); htetx[detID]->Draw("colz");
   }
   TCanvas *ctete = new TCanvas("ctete","ctete",1000,800);
   ctete->Divide(6,4);
   for (int detID=0;detID<24;detID++) {
      ctete->cd(detID+1); htete[detID]->Draw("colz");
   }

   outFile->Write(); outFile->Close();
} //end