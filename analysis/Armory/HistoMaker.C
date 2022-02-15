#include "HistoMaker.h"

void HistoMaker(TString rootFile = "root_data/s005_32Si_trace_tp.root") {
   //rootFile="root_data/s005_32Si_trace_run093-157.root";
   //rootFile="root_data/s005_32Si_trace_dp.root";
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
      //flip recoil
       for (int recID=0;recID<8;recID++) {
         if ( TMath::IsNaN(rdt[recID]) ) continue;
         if ( (recID<4) || (recID==6) ) rdt[recID] = -1.0*rdt[recID];
       }
      //printf("eventID: %d",eventID);
      heventID->Fill(eventID); hrunID->Fill(run);
      for (int i=0;i<multiHit;i++) {
         for (int detID=0;detID<24;detID++) {
            if ( TMath::IsNaN(e[detID]) || e[detID]<0.5 ) continue;
            //process
            te_t[detID] = te_t[detID]*10.0 - te_r[detID]*0.0; //correct time
            dt[detID] = (te_t[detID] - trdt_t[4]*10.0); //de back only
            //fill //"no" gates
            hxfxn[detID]->Fill(xf[detID],xn[detID]);
            hesumx[detID]->Fill(e[detID],xf[detID]+xn[detID]);
            htetx[detID]->Fill(x[detID],te_t[detID]);
            htetr[detID]->Fill(te_r[detID],te_t[detID]);
            htete[detID]->Fill(e[detID],te_t[detID]);
            htet[detID]->Fill(te_t[detID]);
            heVx[detID]->Fill(x[detID],e[detID]);
            hEx[detID]->Fill(Ex);
            hez->Fill(z[detID],e[detID]);
            hcoin_t->Fill(tcoin_t);
            hcoinTime->Fill(coinTime);
            hdt->Fill(dt[detID]);
            hExTot->Fill(Ex);
            for (int recID=0;recID<8;recID++) {
               if ( TMath::IsNaN(rdt[recID]) ) continue;
               hrdt[recID]->Fill(rdt[recID]);
               if ( recID < 5 ) {
                  hrdt2d[recID]->Fill(rdt[6],rdt[recID]);
                  hrdt2d[recID+5]->Fill(rdt[7],rdt[recID]);
               }
            }
            //full gates
            if (  (rdt[4] > 500) && //rdt energy
                  (x[detID]>-0.9 && x[detID]<0.9) //&& //x
               )
            { //de back needs a signal
               hdtx[detID]->Fill(x[detID],dt[detID]);
               hdtr[detID]->Fill(te_r[detID],dt[detID]);
               hdte[detID]->Fill(e[detID],dt[detID]);
               hezg->Fill(z[detID],e[detID]);
               htetg[detID]->Fill(te_t[detID]);
               heVxg[detID]->Fill(x[detID],e[detID]);
               hExg[detID]->Fill(Ex);
               hExgTot->Fill(Ex);
               for (int recID=0;recID<8;recID++) {
                  if ( TMath::IsNaN(rdt[recID]) ) continue;
                  hrdtg[recID]->Fill(rdt[recID]);
                  if ( recID < 5 ) {
                     hrdt2dg[recID]->Fill(rdt[6],rdt[recID]);
                     hrdt2dg[recID+5]->Fill(rdt[7],rdt[recID]);
                  }
               }
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
      heVx[detID]->Write(); heVxg[detID]->Write();
      hEx[detID]->Write(); hExg[detID]->Write();
   }
   hcoin_t->Write(); hcoinTime->Write();
   hExTot->Write(); hExgTot->Write();

   for (int recID=0;recID<8;recID++) {
      hrdt[recID]->Write();
      if ( recID < 5 ) {
         hrdt2d[recID]->Write();
         hrdt2d[recID+5]->Write();
      }
   }

   cali_tetX.Write("correction_tetX");

//DRAW
   TCanvas *ctetx = new TCanvas("ctetx","ctetx",1000,800);
   TCanvas *ctete = new TCanvas("ctete","ctete",1000,800);
   TCanvas *ctet = new TCanvas("ctet","ctet",1000,800);
   TCanvas *cdtx = new TCanvas("cdtx","cdtx",1000,800);
   TCanvas *cdte = new TCanvas("cdte","cdte",1000,800);
   TCanvas *cez = new TCanvas("cez","cez",1000,800);
   TCanvas *crdt = new TCanvas("crdt","crdt",1000,800);
   ctetx->Divide(6,4);
   ctete->Divide(6,4);
   ctet->Divide(6,4);
   cdtx->Divide(6,4);
   cdte->Divide(6,4);
   cez->Divide(1,2);
   crdt->Divide(2,5);
   for (int detID=0;detID<24;detID++) {
      ctetx->cd(detID+1); htetx[detID]->Draw("colz");
      ctete->cd(detID+1); htete[detID]->Draw("colz");
      ctet->cd(detID+1); htetg[detID]->Draw("colz");
      cdtx->cd(detID+1); hdtx[detID]->Draw("colz");
      cdte->cd(detID+1); hdte[detID]->Draw("colz");
   }
   for (int i=0;i<5;i++) {
      crdt->cd(2*i+1);
      hrdt2dg[i]->Draw("colz");
      crdt->cd(2*i+2);
      hrdt2dg[i+5]->Draw("colz");
   }
   cez->cd(1); hez->Draw("colz");
   cez->cd(2); hezg->Draw("colz");
   outFile->Write(); outFile->Close();
} //end