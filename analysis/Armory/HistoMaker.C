#include "HistoMaker.h"

void HistoMaker(TString rootFile = "root_data/s005_32Si_trace_run135_short.root") {
   rootFile="root_data/s005_32Si_trace_tp.root";
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
            //correct timing
            Double_t tetXCorrFactor = 0;
            for (int ipwr=0;ipwr<=8;ipwr++) {
               tetXCorrFactor-= tetXCorr[detID][ipwr]*TMath::Power(x[detID],ipwr);
               //printf("%f %d %f\n",te_t[detID],detID,tetXCorrFactor);
            }

            //dt[detID] = 10.0*((float)coin_t + te_t[detID] - trdt_t[4]);//coinTimeUC;
            Int_t tsDiff = (int)(e_t[detID] - rdt_t[4]);
            dt[detID] = 10.0*((float)tsDiff + te_t[detID] - trdt_t[4]) + tetXCorrFactor;//coinTimeUC;
            te_t[detID] = coinTimeUC;
            //fills no gates
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
            hcoinTime->Fill(coinTimeUC);
            hdt[detID]->Fill(dt[detID]);
            hExTot->Fill(Ex);
            hdtx[detID]->Fill(x[detID],dt[detID]);
            hdtr[detID]->Fill(te_r[detID],dt[detID]);
            hdte[detID]->Fill(e[detID],dt[detID]);
      
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
                  (x[detID]>-0.9 && x[detID]<0.9) && //x
                  (rdt[0] > 500 && rdt[0] < 1700) && //recoil energy
                  (dt[detID] < -10.0 && dt[detID]>-33.0)
               )
            { //de back needs a signal
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
      hdt[detID]->Write();
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
   TCanvas *ctetr = new TCanvas("ctetr","ctetr",1000,800);
   TCanvas *ctet = new TCanvas("ctet","ctet",1000,800);
   TCanvas *cdtx = new TCanvas("cdtx","cdtx",1000,800);
   TCanvas *cdte = new TCanvas("cdte","cdte",1000,800);
   TCanvas *cez = new TCanvas("cez","cez",1000,800);
   TCanvas *crdt = new TCanvas("crdt","crdt",1000,800);
   ctetx->Divide(6,4);
   ctete->Divide(6,4);
   ctetr->Divide(6,4);
   ctet->Divide(6,4);
   cdtx->Divide(6,4);
   cdte->Divide(6,4);
   cez->Divide(1,2);
   crdt->Divide(2,5);
   for (int detID=0;detID<24;detID++) {
      ctetx->cd(detID+1); htetx[detID]->Draw("");
      ctete->cd(detID+1); htete[detID]->Draw("");
      ctetr->cd(detID+1); htetr[detID]->Draw("");
      ctet->cd(detID+1); htetg[detID]->Draw("");
      cdtx->cd(detID+1); hdtx[detID]->Draw("");
      cdte->cd(detID+1); hdte[detID]->Draw("");
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

   TCanvas *cc = new TCanvas("cc","cc",800,800);
   cc->Clear(); cc->cd();
} //end