#include "TFile.h"
#include "TTree.h"
#include "TCutG.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TProfile.h" 

#include "../Armory/AutoFit.C"
#include "../Armory/AnalysisLibrary.h"

void script_12C_2() {

  //^######## Load RDT cuts ########
  TFile * fcut = new TFile("rdtCuts_12C_6.root");
  TList * cutList = (TList *)fcut->Get("cutList");
  TCutG * cutG_rdt[4];
  for( int i = 0 ; i < 4 ; i++ ) {
    cutG_rdt[i] = (TCutG *)cutList->At(i);
  }
  
  float rdtdE[4], rdtE[4];
  std::vector<int> rdtIDs;

  //^######## Load data ########
  TFile * file = TFile::Open("h095_11C_dp_2_trace_run014.root");
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

  TTreeReaderArray<float> te_t(reader, "te_t"); // trace time for e
  TTreeReaderArray<float> te_r(reader, "te_r"); // rise time for e

  TTreeReaderValue<int> nDEHit(reader, "nDEHit");
  TTreeReaderArray<int> rdtID(reader, "rdtID");
  TTreeReaderArray<int> coin_t(reader, "coin_t"); // e_t - rdt_t for dE in tick
  TTreeReaderArray<float> tcoin_t(reader, "tcoin_t"); // tet - trdt_t for dE in tick

  TTreeReaderArray<float> coinTimeUC(reader, "coinTimeUC"); // 10 (coin_t + tcoin_t) in ns

  //^######## Histograms ########
  int exRange[2] = {8, 24};

  TH2F * rdtdEE[4];
  TH2F * Ex_rdtdE[4]; // odd
  TH2F * Ex_rdtE[4]; // even
  for( int i = 0 ; i < 4 ; i++ ) {
    rdtdEE[i]   = new TH2F( Form("rdt_%d", i), Form("rdt_%d", i), 200, 0, 5000, 200, 4500, 9000 );
    Ex_rdtdE[i] = new TH2F( Form("Ex_rdtdE_%d", i), Form("Ex_rdtdE_%d", i), 200, exRange[0], exRange[1], 200, 4500, 9000 );
    Ex_rdtE[i]  = new TH2F( Form("Ex_rdtE_%d", i),  Form("Ex_rdtE_%d", i) , 200, exRange[0], exRange[1], 200, 0, 5000 );
  }

  TH1F * hdT = new TH1F("hdT", "hdT", 50, -10, 40 );
  TH2F * hdT_coin = new TH2F("hdT_coin", "hdT_coin", 200, -30, 50 , 50, -10, 40 );

  TH2F * hExdT = new TH2F("hExdT", "hExdT", 200, exRange[0], exRange[1], 50, -10, 40 );
  TH2F * hExCoin = new TH2F("hExCoin", "hExCoin", 200, exRange[0], exRange[1], 200, -10, 40 );

  TH2F * hIDCoin = new TH2F("hIDCoin", "hIDCoin", 24, 0, 24, 200, -10, 40);

  TH1F * hExID[24];
  TH2F * hCoinTimeX[24];
  TH2F * htet_ter[24];
  TH2F * hCoinTimeX_cor[24];
  TH2F * hCoinEx_corr[24];
  for( int i = 0 ; i < 24 ; i++ ) {
    hExID[i] = new TH1F( Form("hExID_%d", i), Form("hExID_%d", i), 200, exRange[0], exRange[1] );
    hCoinTimeX[i] = new TH2F( Form("hCoinTimeX_%d", i), Form("hCoinTimeX_%d", i), 200, -1.2, 1.2, 200, -10, 40);
    htet_ter[i] = new TH2F( Form("htet_ter_%d", i), Form("htet_ter_%d", i), 200, 0, 10, 200, 95, 120);
    hCoinTimeX_cor[i] = new TH2F( Form("hCoinTimeX_cor_%d", i), Form("hCoinTimeX_cor_%d", i), 200, -1.2, 1.2, 200, -10, 40);
    hCoinEx_corr[i] = new TH2F( Form("hCoinEx_corr_%d", i), Form("hCoinEx_corr_%d", i), 200, exRange[0], exRange[1], 200, -20, 50);
  }

  TH2F * hEZ = new TH2F("hEZ", "hEZ", 200, -600, 0, 200, 0, 12 );

  //^######## correction parameters ########
  std::vector<std::vector<float>> para = LoadFitParameters("coinTimeCorr_para.txt", true);
  TF1 * fcor = nullptr;

  //^######## main loop ########
  int count = 0;
  while( reader.Next() ) {
    //*======== skip detID 11 and 22 ==========
    // if( !detID.IsValid() ) continue; 
    if( *detID < 0 || *detID >= 24 ) continue;
    if( *detID == 11 || *detID == 22 ) continue;

    //*======== skip if no e or e > 20 MeV ==========
    if( e[*detID] <= 0.0 ) continue;
    if( e[*detID] > 20.0 ) continue;

    // printf("Event %d: detID = %d, Ex = %f, thetaCM = %f, e = %f \n", 
          //  count, *detID, *Ex, *thetaCM, e[*detID] );
    

    ///============== apply correction ================
    if( fcor != nullptr ) delete fcor;
    fcor = nullptr;
    
    double coinTime_cor = coinTimeUC[0];
    if( para.size() > static_cast<size_t>(*detID)  ){
      fcor = new TF1( "fcor", Form("pol%d", static_cast<int>(para[*detID].size())-1), 0, 10 );
      for( size_t i = 0 ; i < para[*detID].size() ; i++ ) {
        fcor->SetParameter( i, para[*detID][i] );
      }

      coinTime_cor = coinTimeUC[0] - fcor->Eval( x[*detID] )*10.0;
    }

    // htet_ter[*detID]->Fill(  te_r[*detID], te_t[*detID] );
    // htet_ter[*detID]->Fill(  te_r[*detID], tet_cor);

    // htet_x_cor[*detID]->Fill( x[*detID], tet_cor );
    
    // hCoinTimeX[*detID]->Fill( x[*detID], coinTimeUC[0]/10. );

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
        // if( rdtTime > rdt_t[2*i] ) {
          //   rdtTime = rdt_t[2*i];
          // }
          // printf("rdt[%zu] = %f , rdt[%zu] = %f \n", 2*i, rdt[2*i], 2*i+1, rdt[2*i+1] );
        }
      }
      
    //*======== skip if no RDT ==========
    if( !hasRDT ) continue;
    if( *nDEHit <= 0 ) continue;

    hCoinTimeX[*detID]->Fill( x[*detID], coinTimeUC[0]/10. );
    hCoinTimeX_cor[*detID]->Fill( x[*detID], coinTime_cor/10. );

    htet_ter[*detID]->Fill(  te_r[*detID], te_t[*detID] );


    int dt = 99999;
    if ( rdtTime != (unsigned long long)(-1) ) {
      dt = static_cast<int>( e_t[*detID] - rdtTime );
      hdT->Fill( dt );
      // printf(" detID = %d, e_t = %llu , rdt_t = %llu , dt = %d \n", *detID, e_t[*detID], rdtTime, dt );
    }

    // int dt2 = static_cast<int>( e_t[*detID] - rdt_t[rdtIDs.back()] );
    // hdT2->Fill( dt2 );

    // for( int i = 0 ; i < *nDEHit ; i++ ) {
    //   hdT_coin->Fill( coinTimeUC[i] / 10.0 );
    // }
    // if (*nDEHit > 1 ) hdT_coin1->Fill( coinTimeUC[1] / 10.0 );
    
    //*======== time gate ==========
    if ( dt < 0 || dt > 30 ) continue;
    // if (  -20 < dt && dt < 40 ) continue;
    
    bool isInCut = false;
    for( size_t i = 0 ; i < rdtIDs.size() ; i++ ) {
      int id = rdtIDs[i];
      
      if( !cutG_rdt[id]->IsInside( rdtE[id], rdtdE[id] ) ) continue;
      isInCut = true;
      
      // printf("%d | rdtID = %d, rdtE[%d] = %f , rdtdE[%d] = %f \n", i, id, id, rdtE[id], id, rdtdE[id] );
      Ex_rdtdE[id]->Fill( *Ex, rdtdE[id] );
      Ex_rdtE[id]->Fill( *Ex, rdtE[id] );
      
      rdtdEE[id]->Fill( rdtE[id], rdtdE[id] );  
    }
    
    //*======== skip if not in cut ==========
    if( !isInCut ) continue;
    
    hExdT->Fill( *Ex, dt );
    hdT_coin->Fill( coinTimeUC[0]/10., dt );
    hExCoin->Fill( *Ex, coinTimeUC[0]/10. );

    hIDCoin->Fill(*detID, coinTimeUC[0]/10. );

    hExID[*detID]->Fill( *Ex );
    // hCoinTimeX[*detID]->Fill( x[*detID], coinTimeUC[0]/10. );

    hCoinEx_corr[*detID]->Fill( *Ex, coinTime_cor );

    hEZ->Fill( z[*detID], e[*detID] );

    count++;
    
  }

  //^######## Plotting ########
  // TCanvas * c1 = new TCanvas("c1", "c1", 1200, 800 );
  // c1->Divide(4,2);
  // for( int i = 0 ; i < 4 ; i++ ) {
  //   c1->cd(i+1);
  //   Ex_rdtdE[i]->Draw("box");
  //   c1->cd(i+5);
  //   Ex_rdtE[i]->Draw("box"); 
  // }
  
  // TCanvas * c2 = new TCanvas("c2", "c2", 800, 800 );
  // c2->Divide(2,2);
  // for( int i = 0 ; i < 4 ; i++ ) {
  //   c2->cd(i+1);
  //   rdtdEE[i]->Draw("colz");
  // }
  
  TCanvas * c3 = new TCanvas("c3", "c3", 2400, 1600 );
  c3->Divide(6,4);
  for( int i = 0 ; i < 24 ; i++ ) {
    c3->cd(i+1);
    // hExID[i]->Draw();
    hCoinEx_corr[i]->Draw("box");
    // hCoinTimeX_cor[i]->Draw("box");
    // htet_ter[i]->Draw("colz");
  }
  
  
  // TCanvas * c4 = new TCanvas("c4", "c4", 1200, 600 );
  // c4->Divide(2,1);
  // c4->cd(1);
  // hExdT->Draw("colz");
  // c4->cd(2);
  // hExCoin->Draw("colz");
  
  // TCanvas * c5 = new TCanvas("c5", "c5", 600, 600 );
  // // hdT_coin->Draw("colz");
  // // hIDCoin->Draw("colz");
  // hEZ->Draw("colz");
  
  // TCanvas * c6 = new TCanvas("c6", "c6", 1600, 800 );
  // c6->Divide(2,1);
  // c6->cd(1); hCoinTimeX[1]->Draw("box");
  // c6->cd(2); hCoinTimeX_cor[1]->Draw("box");


}