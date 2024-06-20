#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TMath.h"
#include "TCanvas.h"

void script(){
	
  TFile * file  = new TFile("../root_data/gen_run010.root");
  
  TTree * tree = (TTree *) file->Get("gen_tree");
  
  unsigned long totEntries = tree->GetEntries();

	float crdt[16];

  tree->SetBranchAddress("crdt", crdt);

  TH2F * hcrdtID = new TH2F("hcrdtID", "Circular Recoil ID; Angular ID; Radial ID;", 8, 0, 8, 8, 0, 8);
  TH2F * hcrdtPolar = new TH2F("hcrdtPolar", "Polar ID", 8, -TMath::Pi(), TMath::Pi(),8, 10, 50);

  for( unsigned long ev = 0; ev < totEntries; ev++){

    tree->GetEntry(ev);

    for( int i = 0; i < 8; i ++ ){
      if( TMath::IsNaN(crdt[i]) ) continue;
      for( int j = 8; j < 16; j++){
        if( TMath::IsNaN(crdt[j]) ) continue;

        hcrdtID->Fill(i, j-8);

        double rho = 10. + 5*i;
        double theta = (j-8)*45.*TMath::DegToRad();

        
        hcrdtPolar->Fill( theta-TMath::Pi(), rho);

      }
    }


  }
	
  TCanvas * cCanvas = new TCanvas("canvas", "canvas", 1000, 500);
  cCanvas->Divide(2,1);
  cCanvas->cd(1);
  hcrdtID->Draw("colz");
  cCanvas->cd(2)->DrawFrame(-50, -50, 50, 50);
  hcrdtPolar->Draw("same colz pol");

	
}
