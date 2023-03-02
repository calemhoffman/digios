#define NewMacroTraces_cxx
#include "NewMacroTraces.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void NewMacroTraces::Loop()
{
//   In a ROOT session, you can do:
//      root> .L NewMacroTraces.C
//      root> NewMacroTraces t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   TH1F *h1 = new TH1F("he","e", 5000, 0, 20);
   TH2F *h2 = new TH2F("hez","ez", 1000, -1000, 0, 10000, 0, 20);
   TH2F *htid = new TH2F("htid","tid", 20, 0, 19, 1000, -1000, 1000);
   TH2F *htidtr = new TH2F("htidtr","tidtr", 20, 0, 19, 1000, -1000, 1000);
   TH2F *helabr = new TH2F("helabr","elabr", 1000, 0, 20, 2000, 0, 10000);
   TH2F *hecsi = new TH2F("hecsi","ecsi", 1000, 0, 20, 2000, 0, 10000);
   TH2F *helabr_ring1_dc = new TH2F("helabr_ring1_dc","elabr_ring1_dc", 1000, 0, 20, 2000, 0, 10000);
   TH2F *hecsi_ring2_dc = new TH2F("hecsi_ring2_dc","ecsi_ring2_dc", 1000, 0, 20, 2000, 0, 10000);
   TH2F *hecsi_ring3_dc = new TH2F("hecsi_ring3_dc","ecsi_ring3_dc", 1000, 0, 20, 2000, 0, 10000);
   TH2F *hgammaid_dc = new TH2F("hgammaid_dc","gammaid_dc", 20, 0, 20, 2000, 0, 10000);
   TH2F *hgammaid = new TH2F("hgammaid","gammaid", 20, 0, 20, 2000, 0, 10000);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      h1->Fill(e[detID]);
      h2->Fill(z[detID],e[detID]);
      for (int iid=0; iid<20; iid++) if (rdt_t[iid]>0) {
        htid->Fill(iid, (Long64_t)rdt_t[iid]-(Long64_t)e_t[detID]);
        htidtr->Fill(iid, (Long64_t)rdt_t[iid]+trdt_t[iid]-(Long64_t)e_t[detID]-te_t[detID]);
      }

      //LaBr
      for (int iid=0; iid<5; iid++) if (rdt_t[iid]>0) {
        float time = (Long64_t)rdt_t[iid]+trdt_t[iid]-(Long64_t)e_t[detID]-trdt_t[detID];
        if (time > -100 && time < -94) {
		helabr->Fill(e[detID], rdt[iid]);
		helabr_ring1_dc->Fill(e[detID], rdt[iid]/1.1449);
		if ( e[detID]>3.0 ) {
			hgammaid_dc->Fill(iid,rdt[iid]/1.1449);
			hgammaid->Fill(iid,rdt[iid]);
		}
	}
      }

      //CsI
      for (int iid=5; iid<20; iid++) if (rdt_t[iid]>0) {
        float time = (Long64_t)rdt_t[iid]+trdt_t[iid]-(Long64_t)e_t[detID]-trdt_t[detID];
        if (time > -100 && time < -94) hecsi->Fill(e[detID], rdt[iid]);
	if (time > -100 && time < -94 && iid>5 && iid<15) {
		hecsi_ring2_dc->Fill(e[detID], rdt[iid]/1.0928); 
		if ( e[detID]>3.0 ) {
			hgammaid_dc->Fill(iid,rdt[iid]/1.0928);
			hgammaid->Fill(iid,rdt[iid]);
		}
	}
        if (time > -100 && time < -94 && ((iid>14 && iid<20) || iid==5) ) { 
		hecsi_ring3_dc->Fill(e[detID], rdt[iid]/1.0571);
	        if ( e[detID]>3.0 ) {
			hgammaid_dc->Fill(iid,rdt[iid]/1.0571);	
			hgammaid->Fill(iid,rdt[iid]); 
		}
	}
      }


   }

   TFile ff("new_histos.root","recreate");
   h1->Write();
   h2->Write();
   htid->Write();
   htidtr->Write();
   helabr->Write();
   hecsi->Write();
   helabr_ring1_dc->Write();
   hecsi_ring2_dc->Write();
   hecsi_ring3_dc->Write();
   hgammaid_dc->Write();
   hgammaid->Write();
   ff.Close();
}
