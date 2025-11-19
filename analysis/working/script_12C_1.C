#include "TFile.h"
#include "TTree.h"
#include "TCutG.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

void script_12C_1() {

  TFile * file = TFile::Open("h095_11C_dp_2_trace_run009.root");
  TTree * tree = (TTree *)file->Get("tree");

  TTreeReader reader(tree);
  // TTreeReaderValue<int>    runID(reader, "runID");
  // TTreeReaderValue<int>    eventID(reader, "eventID");
  
  TTreeReaderArray<float> rdt(reader, "rdt");
  TTreeReaderArray<float> e(reader, "e");
  TTreeReaderArray<float> x(reader, "x");
  TTreeReaderArray<float> z(reader, "z");
  
  TTreeReaderValue<int>    detID(reader, "detID");
  TTreeReaderValue<float> Ex(reader, "Ex");
  TTreeReaderValue<float> thetaCM(reader, "thetaCM");

  TH2F * rdtdEE[4];
  TH2F * Ex_rdtdE[4]; // odd
  TH2F * Ex_rdtE[4]; // even
  for( int i = 0 ; i < 4 ; i++ ) {
    rdtdEE[i]   = new TH2F( Form("rdt_%d", i), Form("rdt_%d", i), 200, 0, 5000, 200, 0, 9000 );
    Ex_rdtdE[i] = new TH2F( Form("Ex_rdtdE_%d", i), Form("Ex_rdtdE_%d", i), 200, -2, 8, 200, 0, 9000 );
    Ex_rdtE[i]  = new TH2F( Form("Ex_rdtE_%d", i),  Form("Ex_rdtE_%d", i) , 200, -2, 8, 200, 0, 5000 );
  }

  TFile * fcut = new TFile("rdtCuts_12C_3.root");
  TList * cutList = (TList *)fcut->Get("cutList");
  TCutG * cutG_rdt[4];
  for( int i = 0 ; i < 4 ; i++ ) {
    cutG_rdt[i] = (TCutG *)cutList->At(i);
  }
  
  float rdtdE[4], rdtE[4];
  std::vector<int> rdtIDs;
  
  //^######## main loop ########
  int count = 0;
  while( reader.Next() ) {
    //*======== skip detID 11 and 22 ==========
    if( *detID == 11 || *detID == 22 ) continue;

    bool hasRDT = false;
    rdtIDs.clear();
    for( size_t i = 0 ; i < 4 ; i++ ) {
      rdtE[i]  = rdt[2*i];
      rdtdE[i] = rdt[2*i+1];
      if( rdtE[i] > 0.0 && rdtdE[i] > 0.0 ) {
        rdtIDs.push_back(i);
        hasRDT = true;
        // printf("rdt[%zu] = %f , rdt[%zu] = %f \n", 2*i, rdt[2*i], 2*i+1, rdt[2*i+1] );
      }
    }

    //*======== skip if no RDT ==========
    if( !hasRDT ) continue;

    for( size_t i = 0 ; i < rdtIDs.size() ; i++ ) {
      int id = rdtIDs[i];

      if( !cutG_rdt[id]->IsInside( rdtE[id], rdtdE[id] ) ) continue;

      // printf("%d | rdtID = %d, rdtE[%d] = %f , rdtdE[%d] = %f \n", i, id, id, rdtE[id], id, rdtdE[id] );
      Ex_rdtdE[id]->Fill( *Ex, rdtdE[id] );
      Ex_rdtE[id]->Fill( *Ex, rdtE[id] );

      rdtdEE[id]->Fill( rdtE[id], rdtdE[id] );

    }
    // printf("Ex = %f , thetaCM = %f \n", *Ex, *thetaCM );

    count++;
    // if( count > 10 ) break;

  }

  //^######## Plotting ########
  TCanvas * c1 = new TCanvas("c1", "c1", 1200, 800 );
  c1->Divide(4,2);
  for( int i = 0 ; i < 4 ; i++ ) {
    c1->cd(i+1);
    Ex_rdtdE[i]->Draw("box");
    c1->cd(i+5);
    Ex_rdtE[i]->Draw("box"); 
  }

  TCanvas * c2 = new TCanvas("c2", "c2", 800, 800 );
  c2->Divide(2,2);
  for( int i = 0 ; i < 4 ; i++ ) {
    c2->cd(i+1);
    rdtdEE[i]->Draw("box");
  }
  
}