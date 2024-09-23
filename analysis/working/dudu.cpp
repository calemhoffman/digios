#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TMath.h"
#include "TChain.h"
#include <cstdlib>
#include <iostream>
#include <string>

TH2F * hrt[8];
TH2F * hrdt[4];

TH2F * hCoinTime_x[24];
TH2F * hCoinTime_Ex[24];
TH2F * hCoinTime_z[4];

TH2F * hez;

TH1F * hEx1;
TH1F * hEx2;

void dudu(){
  //^###########################################################
  //^ * Histogram
  //^###########################################################

  for( int i = 0; i < 8; i++){
    hrt[i] = new TH2F(Form("hrt_%d", i), Form("hrt %d", i), 200, 80, 120, 200, 0, 100);

    if( i % 2 == 0 ){
      hrdt[i/2] = new TH2F(Form("hrdt_%d", i/2),Form("hrdt %d", i/2), 400, 0, 6000, 400, 0, 6000);
    }
  }

  for( int i = 0; i < 24; i++){
    hCoinTime_x[i] = new TH2F(Form("hCT_x_%d", i), Form("CT_x %d", i), 200, -1.5, 1.5, 400, -200, 300);
    hCoinTime_Ex[i] = new TH2F(Form("hCT_Ex_%d", i), Form("CT_Ex %d", i), 200, 2, 20, 400, -200, 300);
  }

  for( int i = 0; i < 4; i++){
    hCoinTime_z[i] = new TH2F(Form("hCT_z_%d", i), Form("CT_z %d", i), 500, -700, -200, 400, -200, 300);
  }

  hez = new TH2F("hez", "EZ", 600, -700, -200, 600, 0, 12);

  hEx1 = new TH1F("hEx1", "Ex 12C", 180, 2, 20);
  hEx2 = new TH1F("hEx2", "Ex alpha", 180, 2, 20);

  //^###########################################################
  //^ * root tree
  //^###########################################################

  TFile * file = new TFile("h088_11C_dp_trace_run012-025.root");
  TTree * tree = (TTree *) file->Get("tree"); 

  TTreeReader reader(tree);

  TTreeReaderValue<Int_t>   evID = {reader, "eventID"};
  TTreeReaderValue<Int_t>  runID = {reader, "run"};

  TTreeReaderArray<float>       e = {reader, "e"}; 
  TTreeReaderArray<float>       z = {reader, "z"}; 
  TTreeReaderArray<float>       x = {reader, "x"}; 

  TTreeReaderValue<int>     detID = {reader, "detID"}; 
  TTreeReaderValue<float>      Ex = {reader, "Ex"}; 
  TTreeReaderValue<float> thetaCM = {reader, "thetaCM"}; 

  TTreeReaderArray<float>     rdt = {reader, "rdt"}; 

  TTreeReaderArray<unsigned long long>   e_t = {reader, "e_t"}; 
  TTreeReaderArray<float>   te_r = {reader, "te_r"}; 

  TTreeReaderArray<unsigned long long>   rdt_t = {reader, "rdt_t"}; 
  TTreeReaderArray<float>   trdt_r = {reader, "trdt_r"}; 
  TTreeReaderArray<float>   trdt_t = {reader, "trdt_t"}; 
  TTreeReaderArray<float>   trdt   = {reader, "trdt"}; 

  TTreeReaderValue<int>    coin_t = {reader, "coin_t"}; 
  TTreeReaderValue<float> tcoin_t = {reader, "tcoin_t"}; 

  TTreeReaderValue<float> coinTimeUC = {reader, "coinTimeUC"}; 
  TTreeReaderValue<float> coinTime   = {reader, "coinTime"}; 

  //^###########################################################
  //^ * Cuts
  //^###########################################################

  TString rdtCutFile1 = "rdtCuts_3.root";
  TObjArray * cutList1 = nullptr;

  TFile * fCut1 = new TFile(rdtCutFile1);
  if(!fCut1->IsOpen()) {
    printf( "Failed to open rdt-cutfile 1 : %s\n" , rdtCutFile1.Data());
    rdtCutFile1 = "";
  }
  int numCut1 = 0 ;

  if( fCut1->IsOpen() ){
    cutList1 = (TObjArray *) fCut1->FindObjectAny("cutList");
    numCut1 = cutList1->GetEntries();
    printf("=========== found %d cutG in %s \n", numCut1, fCut1->GetName());

    for(int i = 0; i < numCut1 ; i++){
      printf("cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
        cutList1->At(i)->GetName(),
        ((TCutG*)cutList1->At(i))->GetVarX(),
        ((TCutG*)cutList1->At(i))->GetVarY(),
        ((TCutG*)cutList1->At(i))->GetN());
    }
  }


  TString rdtCutFile2 = "rdtCuts_alpha_2.root";
  TObjArray * cutList2 = nullptr;

  TFile * fCut2 = new TFile(rdtCutFile2);
  if(!fCut2->IsOpen()) {
    printf( "Failed to open rdt-cutfile 1 : %s\n" , rdtCutFile2.Data());
    rdtCutFile2 = "";
  }
  int numCut2 = 0 ;

  if( fCut2->IsOpen() ){
    cutList2 = (TObjArray *) fCut2->FindObjectAny("cutList");
    numCut2 = cutList2->GetEntries();
    printf("=========== found %d cutG in %s \n", numCut2, fCut2->GetName());

    for(int i = 0; i < numCut2 ; i++){
      printf("cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
        cutList2->At(i)->GetName(),
        ((TCutG*)cutList2->At(i))->GetVarX(),
        ((TCutG*)cutList2->At(i))->GetVarY(),
        ((TCutG*)cutList2->At(i))->GetN());
    }
  }

  //^###########################################################
  //^ * Process
  //^###########################################################

  while(reader.Next()){

    // if( 12 <= *runID && *runID <= 18 ){
    //   rdt[1] = rdt[1] * 4.6;
    //   rdt[3] = rdt[3] * 4.8;
    //   rdt[5] = rdt[5] * 4.7;
    //   rdt[7] = rdt[7] * 4.5;

    //   if( rdt[0] < 2000 && TMath::IsNaN(rdt[1]) ) rdt[1] = 120;
    //   if( rdt[2] < 2000 && TMath::IsNaN(rdt[3]) ) rdt[3] = 200;
    //   if( rdt[4] < 2000 && TMath::IsNaN(rdt[5]) ) rdt[5] = 300;
    //   if( rdt[6] < 2000 && TMath::IsNaN(rdt[7]) ) rdt[7] = 400;

    // }

    //*==============
    // if( !( 6 < *Ex && *Ex < 14) ) continue;

    //*==============
    // if( !( -30 < *coin_t && *coin_t < 30) ) continue;

    bool rdtFlag = false;
    for( int i = 0; i < 8; i++){
      hrt[i]->Fill(trdt_t[i], trdt_r[i]);
      if( i % 2 == 0 ){
        if( (trdt_r[i] < 100 && trdt_r[i+1] < 100 )){
          // hrdt[i/2]->Fill(trdt[i], trdt[i+1]);
          hrdt[i/2]->Fill(rdt[i], rdt[i+1]);
          rdtFlag = true;
        }
      }
    }

    //*==============
    // if( rdtFlag == false ) continue;

    hEx1->Fill(*Ex);

    bool rdtgate1 = false;
    for(int i = 0 ; i < numCut1 ; i++ ){
      TCutG * cutG = (TCutG *)cutList1->At(i) ;
      if(cutG->IsInside(rdt[2*i],rdt[2*i+1]) && trdt_r[2*i] < 150 && trdt_r[2*i + 1] < 150) {
        rdtgate1 = true;
        break; /// only one is enough
      }
    }

    bool rdtgate2 = false;
    for(int i = 0 ; i < numCut2 ; i++ ){
      TCutG * cutG = (TCutG *)cutList2->At(i) ;
      if(cutG->IsInside(rdt[2*i],rdt[2*i+1])) {
        rdtgate2 = true;
        break; /// only one is enough
      }
    }

    //*==============
    // if( rdtgate1 == false && rdtgate2 == false) continue;
    // if( rdtgate1 == false ) continue;

    hEx2->Fill(*Ex);

    int id = *detID;


    hCoinTime_x[*detID]->Fill(x[*detID], *coinTimeUC) ;
    hCoinTime_Ex[*detID]->Fill(*Ex, *coinTimeUC) ;
    hCoinTime_z[*detID/6]->Fill(z[i], *coinTimeUC);

    hez->Fill(z[i], e[i]);

    

  }  // end while(reader.Next())

  //^###########################################################
  //^ * Plot
  //^###########################################################
  gStyle->SetOptStat(11111111);

  TCanvas *canvas = new TCanvas("canvas", "anlayszer", 6*300, 4*300);
  canvas->Divide(6,4);

  for( int i = 0; i < 24; i++ ){
    canvas->cd(i+1); 
    hCoinTime_x[i]->Draw("box");
  }

  TCanvas *canvas1 = new TCanvas("canvas1", "anlayszer", 1200, 600);
  canvas1->Divide(1,2);

  canvas1->cd(1); hEx1->Draw();
  canvas1->cd(2); hEx2->Draw();

  TCanvas *canvas2 = new TCanvas("canvas2", "anlayszer", 1200, 600);
  canvas2->Divide(4,2);

  for( int i = 0; i < 8; i++){
    canvas2->cd(i+1); hrt[i]->Draw("colz");
  }
  
  TCanvas *canvas3 = new TCanvas("canvas3", "anlayszer", 1200, 1200);
  canvas3->Divide(2,2);

  for( int i = 0; i < 4; i++){
    canvas3->cd(i+1); hrdt[i]->Draw("colz box");
  }  

  TCanvas *canvas4 = new TCanvas("canvas4", "anlayszer", 1000, 1000);
  canvas4->cd(1); hez->Draw("colz box");
  
  TCanvas *canvas5 = new TCanvas("canvas5", "anlayszer", 6*300, 4*300);
  canvas5->Divide(6,4);

  for( int i = 0; i < 24; i++ ){
    canvas5->cd(i+1); 
    hCoinTime_Ex[i]->Draw("box");
  }  

  TCanvas *canvas6 = new TCanvas("canvas5", "anlayszer", 1200, 4*300);
  canvas6->Divide(1,4);

  for( int i = 0; i < 4; i++ ){
    canvas6->cd(i+1); 
    hCoinTime_z[i]->Draw("box");
  }

} //====================== end script()

