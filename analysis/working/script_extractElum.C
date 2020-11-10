#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TMath.h"


void script_extractElum(){

  //TFile * oldFile = new TFile("../root_data/h073_82Kr_gen_run019-080.root");
  //TTree * oldTree = (TTree *) oldFile->Get("tree");

  TChain * oldTree = new TChain("gen_tree");
  oldTree->Add("../root_data/gen_run01[3-9].root");
  oldTree->Add("../root_data/gen_run0[2-9][0-9].root");
  oldTree->Add("../root_data/gen_run10[0-3].root");
  
  oldTree->SetBranchStatus("*", 0);
  
  oldTree->SetBranchStatus("runID", 1);
  oldTree->SetBranchStatus("elum", 1);
  oldTree->SetBranchStatus("elum_t", 1);
  //oldTree->SetBranchStatus("tac", 1);
  //oldTree->SetBranchStatus("tac_t", 1);
  
  
  /*///========== Method one, simply copy branches 
  TFile *newFile = new TFile("small.root","recreate");
  TTree *newTree = oldTree->CloneTree();

  newTree->Print();
  newFile->Write();
   
  delete oldFile;
  delete newFile;
  
  /*///========== Method two, looping all events, take what is needed. 

  Int_t runID;
  
  Float_t elum_old[32];
  ULong64_t elum_old_t[32];
  
  ///Float_t tac_old[100];
  ///ULong64_t tac_old_t[100];

  oldTree->SetBranchAddress("runID", &runID);
  oldTree->SetBranchAddress("elum", elum_old);
  oldTree->SetBranchAddress("elum_t", elum_old_t);
  ///oldTree->SetBranchAddress("tac", tac_old);
  ///oldTree->SetBranchAddress("tac_t", tac_old_t);

  TFile *newFile = new TFile("small.root","recreate");
  TTree *newTree = new TTree("tree","tree");

  Float_t elum;
  ULong64_t elum_t;
  Int_t multi;
  Int_t elumID;
  
  Float_t tac[2];
  ULong64_t tac_t[2];
  
  newTree->Branch("runID", &runID, "run/I");
  newTree->Branch("multi", &multi, "multi/I");

  newTree->Branch("elumID", &elumID, "elumID/I");
  newTree->Branch("elum", &elum, "elum/F");
  newTree->Branch("elum_t", &elum_t, "elum/l");

  ///newTree->Branch("tac", tac, "tac[2]/F");
  ///newTree->Branch("tac_t", tac_t, "tac_t[2]/l");

  int totEntry = oldTree->GetEntries();

  printf("=============== start\n");

  double Frac = 0;
  int count = 0;
  
  for(int ev = 0; ev < totEntry; ev ++){

    if (ev > totEntry*Frac-1) {
      printf(" %3.0f%% (%d/%d) processed \n", Frac*100, ev, totEntry);
      Frac+=0.1;
    }

    oldTree->GetEntry(ev, 0);

    multi = 0;

    ///if( TMath::IsNaN(elum_old[0]) ) continue;
    ///if( TMath::IsNaN(elum_old[0]) && tac_old_t[1] == 0 ) continue;

    for( int i = 0 ; i < 16; i++){

      if( TMath::IsNaN(elum_old[i]) ) continue;
      multi ++;
      elumID = i;
      elum = elum_old[i];
      elum_t = elum_old_t[i];

    } 

    //tac[0] = tac_old[0];
    //tac[1] = tac_old[1];
    //
    //tac_t[0] = tac_old_t[0];
    //tac_t[1] = tac_old_t[1];

    if ( multi == 0 ) continue;
    count++;
    newTree->Fill();  
    

  }

  newFile->cd(); //set focus on this file
  newTree->Write(); 
  newFile->Close();

  printf("============ end, %d \n", count);

  ///delete oldFile;
  ///delete newFile;
}
