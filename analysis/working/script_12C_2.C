#include "TFile.h"
#include "TTree.h"
#include "TCutG.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

void script_12C_2() {

  TFile * file = TFile::Open("h095_11C_dp_2_gen_run014.root");
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

  TTreeReaderArray<unsigned long long> e_t(reader, "e_t");
  TTreeReaderArray<unsigned long long> rdt_t(reader, "rdt_t");

  //^######## Histograms ########
  TH2F * rdtdEE[4];
  TH2F * Ex_rdtdE[4]; // odd
  TH2F * Ex_rdtE[4]; // even
  for( int i = 0 ; i < 4 ; i++ ) {
    rdtdEE[i]   = new TH2F( Form("rdt_%d", i), Form("rdt_%d", i), 200, 0, 5000, 200, 4500, 9000 );
    Ex_rdtdE[i] = new TH2F( Form("Ex_rdtdE_%d", i), Form("Ex_rdtdE_%d", i), 200, 8, 22, 200, 4500, 9000 );
    Ex_rdtE[i]  = new TH2F( Form("Ex_rdtE_%d", i),  Form("Ex_rdtE_%d", i) , 200, 8, 22, 200, 0, 5000 );
  }

  TH1F * hdT = new TH1F("hdT", "hdT", 200, -100, 100 );

  //^######## Load RDT cuts ########
  TFile * fcut = new TFile("rdtCuts_12C_6.root");
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

    unsigned long long rdtTime = -1ULL;

    for( size_t i = 0 ; i < 4 ; i++ ) {
      rdtE[i]  = rdt[2*i];
      rdtdE[i] = rdt[2*i+1];
      if( rdtE[i] > 0.0 && rdtdE[i] > 0.0 ) {
        rdtIDs.push_back(i);
        hasRDT = true;

        if( rdtTime > rdt_t[2*i+1] ) {
          rdtTime = rdt_t[2*i+1];
        }
        if( rdtTime > rdt_t[2*i] ) {
          rdtTime = rdt_t[2*i];
        }
        // printf("rdt[%zu] = %f , rdt[%zu] = %f \n", 2*i, rdt[2*i], 2*i+1, rdt[2*i+1] );
      }
    }

    //*======== skip if no RDT ==========
    if( !hasRDT ) continue;

    int dt = 99999;
    if ( rdtTime != (unsigned long long)(-1) ) {
      dt = static_cast<int>( e_t[*detID] - rdtTime );
      hdT->Fill( dt );
      // printf(" detID = %d, e_t = %llu , rdt_t = %llu , dt = %d \n", *detID, e_t[*detID], rdtTime, dt );
    }
    
    if ( dt < -20 || dt > 40 ) continue;
    // if (  -20 < dt && dt < 40 ) continue;

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
    rdtdEE[i]->Draw("colz");
  }

  TCanvas * c3 = new TCanvas("c3", "c3", 600, 600 );
  hdT->Draw();
  
}