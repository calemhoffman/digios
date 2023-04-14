#include "HistoMaker.h"

void HistoMaker(TString rootFile = "../../../data/s005_32Si_tp.root") {
   //rootFile="root_data/s005_32Si_tp.root";
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
            if ( TMath::IsNaN(e[detID]) || e[detID]<0.5) continue;
            //process
            //correct timing
            Double_t tetXCorrFactor = 0;
            for (int ipwr=0;ipwr<8;ipwr++) {
               tetXCorrFactor-= tetXCorr[detID][ipwr]*TMath::Power(x[detID],ipwr);
               //printf("%f %d %f\n",te_t[detID],detID,tetXCorrFactor);
            }
            tetXCorrFactor-=tetXCorr[detID][8];
            //dt[detID] = 10.0*((float)coin_t + te_t[detID] - trdt_t[4]);//coinTimeUC;
            Int_t tsDiff = (int)(e_t[detID] - rdt_t[4]);
            dt[detID] = 10.0*((float)tsDiff + te_t[detID] - trdt_t[4]) + tetXCorrFactor;//coinTimeUC;
            //dt[detID] -= (rdt[4]-565.)*0.0413;
            //te_t[detID] = coinTimeUC;
            //fills only x range gate
            if (x[detID]>-0.95 && x[detID]<0.95) {
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
               
               hdtr[detID]->Fill(te_r[detID],dt[detID]);
               hdte[detID]->Fill(e[detID],dt[detID]);
               hdtEx[detID]->Fill(Ex,dt[detID]);
               hdtrdt[detID]->Fill(rdt[4],dt[detID]);
               for (int recID=0;recID<8;recID++) {
                  if ( TMath::IsNaN(rdt[recID]) ) continue;
                  hrdt[recID]->Fill(rdt[recID]);
                  if ( recID < 5 ) {
                     hrdt2d[recID]->Fill(rdt[6],rdt[recID]);
                     hrdt2d[recID+5]->Fill(rdt[7],rdt[recID]);
                  }
               }
            }
            //full gates
            if (  (rdt[4] > 1050 && rdt[4] <1500 && rdt[7]>50 && rdt[7]<550) && //rdt energy
                  (x[detID]>-0.95 && x[detID]<0.95) && //x
                  (dt[detID]>0 && dt[detID]<25)               )
            { //de back needs a signal
            hdtx[detID]->Fill(x[detID],dt[detID]);
               hezg->Fill(z[detID],e[detID]);
               htetg[detID]->Fill(te_t[detID]);
               heVxg[detID]->Fill(x[detID],e[detID]);
               hExg[detID]->Fill(Ex);
               hExgTot->Fill(Ex);
               hdtg[detID]->Fill(dt[detID]);
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
      hdt[detID]->Write(); hdtr[detID]->Write();
      hdtx[detID]->Write();hdte[detID]->Write();
      hdtrdt[detID]->Write();
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
   TCanvas *cdtr = new TCanvas("cdtr","cdtr",1000,800);
   TCanvas *cdtx = new TCanvas("cdtx","cdtx",1000,800);
   TCanvas *cdte = new TCanvas("cdte","cdte",1000,800);
   TCanvas *cez = new TCanvas("cez","cez",1000,800);
   cdtr->Divide(6,4);
   cdtx->Divide(6,4);
   cdte->Divide(6,4);
   cez->Divide(1,2);
   for (int detID=0;detID<24;detID++) {
     cdtr->cd(detID+1); hdtrdt[detID]->Draw("");
      cdtx->cd(detID+1); hdtx[detID]->Draw("");
      cdte->cd(detID+1); hdt[detID]->Draw("");hdtg[detID]->Draw("same");
   }

   cez->cd(1); hez->Draw("colz");
   cez->cd(2); hezg->SetMarkerStyle(7);hezg->Draw("");
   outFile->Write(); outFile->Close();

   TCanvas *cc = new TCanvas("cc","cc",800,800);
   cc->Clear(); cc->cd();
   hExgTot->Draw();
} //end