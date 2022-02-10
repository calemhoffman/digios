#include "HistoMaker.h"

void HistoMaker(TString rootFile = "root_data/s005_32Si_trace_run135.root") {
   //rootFile="root_data/s005_32Si_trace_run120-129.root";
   const char* treeName="tree";
   TFile *file0 = new TFile (rootFile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);

   setTreeBranches(tree);
   defineHistos();
   getCalibrations();

   //Process events
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
            //process
            te_t[detID] = te_t[detID] - te_r[detID]; //correct time

            //fill //"no" gates
            hxfxn[detID]->Fill(xf[detID],xn[detID]);
            hesumx[detID]->Fill(e[detID],xf[detID]+xn[detID]);
            htetx[detID]->Fill(x[detID],te_t[detID]);
            htetr[detID]->Fill(te_r[detID],te_t[detID]);
            htete[detID]->Fill(e[detID],te_t[detID]);
            htet[detID]->Fill(te_t[detID]);
            hez->Fill(z[detID],e[detID]);

            //full gates
            if (  (rdt[4] > 500) && //rdt energy
                  (x[detID]>-0.9 && x[detID]<0.9) // x
               )
            { //de back needs a signal
               hezg->Fill(z[detID],e[detID]);
               htetg[detID]->Fill(te_t[detID]);
            } //full gates
         } //detID
      } //multiHit
   } //entryNumber

//WRITE FILE
   TFile * outFile = new TFile("HistMaker.root","RECREATE");
   // outFile->mkdir("array");
   hez->Write(); hezg->Write();
   // outFile->cd("array");
   for (int detID=0;detID<24;detID++) {
      hxfxn[detID]->Write(); hesumx[detID]->Write();
      htetx[detID]->Write(); htete[detID]->Write();
      htetr[detID]->Write(); htet[detID]->Write();
   }
   cali_tetX.Write("correction_tetX");

//DRAW
   TCanvas *ctetx = new TCanvas("ctetx","ctetx",1000,800);
   TCanvas *ctete = new TCanvas("ctete","ctete",1000,800);
   TCanvas *ctet = new TCanvas("ctet","ctet",1000,800);
   ctetx->Divide(6,4);
   ctete->Divide(6,4);
   ctet->Divide(6,4);
   for (int detID=0;detID<24;detID++) {
      ctetx->cd(detID+1); htetx[detID]->Draw("colz");
      ctete->cd(detID+1); htete[detID]->Draw("colz");
      ctet->cd(detID+1); htetg[detID]->Draw("colz");
   }

   outFile->Write(); outFile->Close();
} //end