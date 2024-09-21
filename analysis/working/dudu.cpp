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

void analyzer(){

  TFile * file = new TFile("h088_11C_dp_trace_run012-016_018.root.root");
  TTree * tree = (TTree *) file->Get("tree"); 

  TTreeReader reader(tree);

  TTreeReaderValue<Int_t>   evID = {reader, "eventID"};
  TTreeReaderValue<Int_t>    run = {reader, "run"};
  TTreeReaderArray<float>       e = {reader, "e"}; 
  TTreeReaderArray<float>       z = {reader, "z"}; 
  TTreeReaderArray<float>       x = {reader, "x"}; 
  TTreeReaderArray<int>     detID = {reader, "detID"}; 
  TTreeReaderArray<float>      Ex = {reader, "Ex"}; 
  TTreeReaderArray<float> thetaCM = {reader, "thetaCM"}; 

  TTreeReaderArray<float>     rdt = {reader, "rdt"}; 

  TTreeReaderArray<unsigned long>   e_t = {reader, "e_t"}; 
  TTreeReaderArray<float>   e_r = {reader, "e_r"}; 

  TTreeReaderArray<unsigned long>   rdt_t = {reader, "rdt_t"}; 
  TTreeReaderArray<float>   trdt_r = {reader, "trdt_r"}; 
  TTreeReaderArray<float>   trdt_t = {reader, "trdt_t"}; 
  TTreeReaderArray<float>   trdt   = {reader, "trdt"}; 

  TTreeReaderArray<int>    coin_t = {reader, "coin_t"}; 
  TTreeReaderArray<float> tcoin_t = {reader, "tcoin_t"}; 

  TTreeReaderArray<float> coinTimeUC = {reader, "coinTimeUC"}; 
  TTreeReaderArray<float> coinTime   = {reader, "coinTime"}; 

  //^###########################################################
  //^ * Process
  //^###########################################################

  while(reader.Next()){

    for( int i = 0; i < 24; i++){
      if( TMath::IsNaN(e[i]) ) continue;
      if( e[i] < 0.5 ) continue;
    }

  }  // end while(reader.Next())

  //^###########################################################
  //^ * Plot
  //^###########################################################
  gStyle->SetOptStat(11111111);

  TCanvas *canvas = new TCanvas("canvas", "anlayszer", 1000, 1000);
  canvas->Divide(6,4);

  
} //====================== end script()

