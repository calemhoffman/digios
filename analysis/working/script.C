#include "TChain.h"
#include "TCanvas.h"
#include "TMacro.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TRandom.h"
#include "TObjArray.h"
#include "TCutG.h"

#include <string.h>
#include <fstream> 

#include "../Armory/AnalysisLibrary.h"
#include "Hit.h"

DetGeo detGeo;
ReactionConfig reactionConfig;
double zRange[2];
const int numDet = 24;
const int numSide = 4;
const int numCol = numDet / numSide;
double xnCorr[numDet];
double xfxneCorr[numDet][2];
double xScale[numDet];
double eCorr[numDet][2];

// e_z to Ex_thetaCM
double mass;
double betRel;
double Et;
double massB;
double alpha;
double gamm;
double G;

TCutG * cut[4];

void CalExTheta(Hit & hit);
void LoadDetGeoAndReactionConfigFile();
void LoadXFXNCorr();
void LoadXFXN2ECorr();
void LoadXScaleCorr();
void LoadECorr();
void LoadReactionPars();

double xfxn2e(double x){
  return 67.7618 + 0.311656 * x + 7.37694e-5*x*x - 4.21035e-9 *x*x*x;
}


TFile * transfer = nullptr;
TObjArray * gList  = nullptr ;
TObjArray * fxList = nullptr ;
int nGList = 0;
int nFxList = 0;

void PlotKELines(){
  ///the constant thetaCM line 
  gList->At(0)->Draw("same");
  ///the e-z line for excitation 
  for( int i = 0 ; i < nFxList ; i++){
    ((TF1*)fxList->At(i))->SetLineColor(6);
    fxList->At(i)->Draw("same");
  }
  
}

TChain * chain = nullptr;

void script(){

  transfer = new TFile("transfer.root");
  if( transfer->IsOpen() ) {
    gList  = (TObjArray *) transfer->FindObjectAny("gList");
    nGList = gList->GetLast() + 1;
    fxList = (TObjArray *) transfer->FindObjectAny("fxList");
    nFxList = fxList->GetLast() +1 ;
  }
  
  chain = new TChain("gen_tree");
  //chain->Add("../root_data/gen_run00[4,5].root"); // alpha

  chain->Add("../root_data/trace_run01[2-8].root"); // > 1.5e5 pps
  chain->Add("../root_data/trace_run019.root"); // approx 2e5 pps
  chain->Add("../root_data/trace_run02[0-9].root"); // approx 2e5 pps
  chain->Add("../root_data/trace_run030.root"); // approx 2e5 pps
  chain->Add("../root_data/trace_run03[2-6].root"); // approx 2e5 pps
  chain->Add("../root_data/trace_run03[8-9].root"); // approx 2e5 pps
  chain->Add("../root_data/trace_run040.root"); // approx 2e5 pps

  printf("\033[0;31m========================================== Number of Files : %2d\n",chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  unsigned long long numTotalEvents = chain->GetEntries();
  printf("================================= total %lld\033[0m\n", numTotalEvents);

  TTreeReader reader(chain);

  TTreeReaderValue<unsigned long long>        evID = {reader, "evID"};
  TTreeReaderValue<int>       runID = {reader, "runID"};
  TTreeReaderArray<float>         e = {reader, "e"};
  TTreeReaderArray<float>        xf = {reader, "xf"};
  TTreeReaderArray<float>        xn = {reader, "xn"};
  TTreeReaderArray<float>       rdt = {reader, "rdt"};
  TTreeReaderArray<ULong64_t> rdt_t = {reader, "rdt_t"};
  TTreeReaderArray<ULong64_t>   e_t = {reader, "e_t"};
  
  TTreeReaderArray<float>    te_t = {reader, "te_t"};
  TTreeReaderArray<float>    te_r = {reader, "te_r"};
  
  TTreeReaderArray<float>    trdt   = {reader, "trdt"};
  TTreeReaderArray<float>    trdt_t = {reader, "trdt_t"};
  TTreeReaderArray<float>    trdt_r = {reader, "trdt_r"};

  //*###################################################

  LoadDetGeoAndReactionConfigFile();
  LoadXFXNCorr();
  LoadXFXN2ECorr();
  LoadXScaleCorr();
  LoadECorr();
  LoadReactionPars();

  //*###################################################

  cut[0] = new TCutG("rdtCut0",7);
  cut[0]->SetPoint(0,4422.68,1760.71);
  cut[0]->SetPoint(1,4158.67,1750.79);
  cut[0]->SetPoint(2,4051.12,1453.36);
  cut[0]->SetPoint(3,4178.23,1126.19);
  cut[0]->SetPoint(4,4667.12,878.331);
  cut[0]->SetPoint(5,4696.46,1602.08);
  cut[0]->SetPoint(6,4422.68,1760.71);

  cut[1] = new TCutG("rdtCut1",7);
  cut[1]->SetPoint(0,4481.34,1750.79);
  cut[1]->SetPoint(1,3777.34,1919.34);
  cut[1]->SetPoint(2,3767.56,1721.05);
  cut[1]->SetPoint(3,4041.34,1304.65);
  cut[1]->SetPoint(4,4637.79,1264.99);
  cut[1]->SetPoint(5,4813.79,1512.85);
  cut[1]->SetPoint(6,4481.34,1750.79);

  cut[2] = new TCutG("rdtCut2",5);
  cut[2]->SetPoint(0,4051.12,2206.85);
  cut[2]->SetPoint(1,4109.79,1562.42);
  cut[2]->SetPoint(2,4931.12,1334.39);
  cut[2]->SetPoint(3,4911.57,2038.31);
  cut[2]->SetPoint(4,4051.12,2206.85);

  cut[3] = new TCutG("rdtCut3",5);
  cut[3]->SetPoint(0,3444.89,3862.55);
  cut[3]->SetPoint(1,3757.78,2791.8);
  cut[3]->SetPoint(2,4686.68,2702.57);
  cut[3]->SetPoint(3,4540.01,3555.2);
  cut[3]->SetPoint(4,3444.89,3862.55);


  //*###################################################
  printf("===================================================== Create histograms...\n");

  // TH2F * heCal_xCal[24];
  // TH1F * heCal[24];
  // TH1F * heCalg[24];
  // for( int i = 0; i < numDet; i++){
  //   heCal_xCal[i] = new TH2F(Form("heCal_xCal%02d", i), Form("det-%02d;xCal;eCal", i), 400, -0.5, 1.5, 400, 0, 13);
  //   heCal[i]  = new TH1F(Form("heCal%02d", i), Form("det-%02d;eCal", i), 400, 0, 10);
  //   heCalg[i] = new TH1F(Form("heCalg%02d", i), Form("det-%02d (gate);eCal; count / 10 keV ", i), 600, 0, 10);
  // }
  TH1F * hArrayMulti = new TH1F("hArrayMulti", "Array Multi", 5, 0, 5);

  TH2F * hez = new TH2F("hez", "e-z; z [mm]; e [MeV]", 400, zRange[0], zRange[1], 400, 0, 14);
  TH2F * hezg = new TH2F("hezg", "e-z (gated); z [mm]; e [MeV]", 400, zRange[0], zRange[1], 400, 0, 14);

  TH2F * hrdt2D[4];
  TH2F * hrdt2Ds[4];
  TH1F * hdE[4];
  TH1F * hdEs[4];
  for(int i = 0; i < 4; i ++ ){
    hrdt2D[i] = new TH2F(Form("hrdt2D%d", i), Form("RDT"), 400, 0, 6000, 400, 0, 6000);
    hrdt2Ds[i] = new TH2F(Form("hrdt2Ds%d", i), Form("RDT"), 400, 0, 6000, 400, 0, 6000);
    hdE[i] = new TH1F(Form("hdE%d", i), Form("dE"), 400, 0, 6000);
    hdEs[i] = new TH1F(Form("hdEs%d", i), Form("dE"), 400, 0, 6000);
  }
  TH1F * hRDTMulti = new TH1F("hRDTMulti", "RDT Multi", 5, 0, 5);
  
  double ExRange[2] = {3, 20};
  double resol = 0.1;
  int ExBin = (ExRange[1] - ExRange[0])/resol;
  TH1F * hEx  = new TH1F("hEx",  Form("Ex; Ex [MeV]; count / %.0f keV", resol* 1000), ExBin, ExRange[0], ExRange[1]); 
  TH1F * hExg = new TH1F("hExg", Form("Ex (gate); Ex [MeV]; count / %.0f keV", resol * 1000), ExBin, ExRange[0], ExRange[1]); 
  hExg->SetLineColor(kRed);

  TH2F * hEx_EE = new TH2F("hEx_EE", ";Ex ; EE", ExBin, ExRange[0], ExRange[1], 400, 0, 6000);
  TH2F * hEx_dE = new TH2F("hEx_dE", ";Ex ; dE", ExBin, ExRange[0], ExRange[1], 400, 0, 5000);


  //*###################################################
  printf("===================================================== Processing....\n");

  std::vector<Hit> hitList;

  while(reader.Next()){

    hitList.clear();
    Hit hit;

    ///##################################################################### Array
    for( int i = 0; i < numDet; i++){
      //@================ gate
      if( TMath::IsNaN(e[i]) ) continue ; 
      if( TMath::IsNaN(xn[i]) &&  TMath::IsNaN(xf[i]) ) continue ; 
      if( i == 10 || i == 11 ) continue;

      hit.e = e[i] / eCorr[i][0] + eCorr[i][1];
      if( i == 2 ) hit.e = (xf[i]+xn[i])/ eCorr[i][0] + eCorr[i][1];

      //@================ gate
      if( hit.e < 0.8) continue;
      
      double xfcal = xf[i] * xfxneCorr[i][1] + xfxneCorr[i][0]/2.;
      double xncal = xn[i] * xnCorr[i] * xfxneCorr[i][1] + xfxneCorr[i][0]/2.;

      //correction for 
      if( 30 >= *runID && *runID >= 24 && i == 8){

        if( *runID == 24 && (1.4e5 > *evID )  ) continue; 
        if( *runID == 25 && (1.95e5 > *evID && *evID > 1.8e5)  ) continue; 

        xfcal = xfxn2e(xf[i]);
        xncal = xfxn2e(xn[i] * xnCorr[i]);

      }

      if( i == 2 ) e[i] = xfcal + xncal;
      // e[i] = xfcal + xncal;

      if  ( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hit.x = 0.5 + 0.5 * (xfcal - xncal ) / e[i];
      // if  ( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hit.x = 0.5 + 0.5 * (xfcal - xncal ) / (xfcal + xncal);
      if  ( !TMath::IsNaN(xf[i]) &&  TMath::IsNaN(xn[i]) ) hit.x = xfcal/ e[i];
      if  (  TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hit.x = 1.0 - xncal/ e[i];
      
      hit.x = (hit.x-0.5)/xScale[i] + 0.5; 

      // if( detGeo.firstPos > 0 ) {
      //   hit.z = detGeo.detLength*(1.0-hit.x) + detGeo.detPos[i%numCol];
      // }else{
      hit.z = detGeo.detLength*(hit.x-1.0) + detGeo.detPos[i%numCol];
      // }

      hitList.push_back(hit);

      // heCal[i]->Fill(hit.e);
      // heCal_xCal[i]->Fill(hit.x, hit.e);
      // hez->Fill(hit.z, hit.e);

      // @================ gate
      // if( abs(hit.x - 0.5) > 1.0/2. ) continue;
      // heCalg[i]->Fill(hit.e);
      // hezg->Fill(hit.z, hit.e);
  
    }

    // @================ gate
    if( hitList.size() == 0 ) continue;

    hArrayMulti->Fill( hitList.size() ) ;

    ///##################################################################### RDT
    if( 12 <= *runID && *runID <= 18 ){
      //dE scale
      rdt[1] = rdt[1] * 4.9;
      rdt[3] = rdt[3] * 5.1;
      rdt[5] = rdt[5] * 4.9;
      rdt[7] = rdt[7] * 4.9;
    }
    // if no dE hit but E hit less then 2000, due to dE is very small and below threshold
    if( rdt[0] < 1700 && TMath::IsNaN(rdt[1]) ) rdt[1] = 120 ; //+ gRandom->Gaus(0, 20);
    if( rdt[2] < 1300 && TMath::IsNaN(rdt[3]) ) rdt[3] = 200 ; //+ gRandom->Gaus(0, 20);
    if( rdt[4] < 1300 && TMath::IsNaN(rdt[5]) ) rdt[5] = 300 ; //+ gRandom->Gaus(0, 20);
    if( rdt[6] < 1300 && TMath::IsNaN(rdt[7]) ) rdt[7] = 400 ; //+ gRandom->Gaus(0, 20);

    //~================ fill Hit
    for( int i = 0; i < 8 ; i++ ){
      if( TMath::IsNaN(rdt[i] ) ) continue;

      for( size_t k = 0; k < hitList.size(); k++ ){
        if( i % 2 == 0 ) hitList[k].EE[i/2] = rdt[i]; 
        if( i % 2 == 1 ) hitList[k].dE[i/2] = rdt[i]; 
      }
    }

    ///##################################################################### Array & RDT
    for( size_t k = 0; k < hitList.size(); k++ ){ 
      hitList[k].CalRDTMultiplicity();

      CalExTheta(hitList[k]);

      for( int i = 0; i < 4; i++ ){
        hrdt2D[i]->Fill( hitList[k].EE[i], hitList[k].dE[i] );
        
        if( hitList[k].dE[i] > 0  && hitList[k].EE[i] > 0  )  {
          hdE[i]->Fill(hitList[k].EE[i]);
          hez->Fill( hitList[k].z, hitList[k].e );
        }

        // if( TMath::IsNaN(hitList[k].dE[i]) && hitList[k].EE[i] > 0 )  {
        //   hdEs[i]->Fill(hitList[k].EE[i]);
        //   hezg->Fill( hitList[k].z, hitList[k].e );
        // }
        // if( 14.7 < hitList[k].Ex && hitList[k].Ex < 15.7 ) {
        //   hrdt2Ds[i]->Fill( hitList[k].EE[i], hitList[k].dE[i] );
        // } 
      
      }

      hRDTMulti->Fill(hitList[k].nRDT);

      hEx->Fill( hitList[k].Ex );

      // int ID = 2;
      // if( cut[ID]->IsInside(hitList[k].EE[ID], hitList[k].dE[ID] ) ) {
      //   hExg->Fill( hitList[k].Ex);
      //   hezg->Fill( hitList[k].z, hitList[k].e );

      //   hEx_EE->Fill( hitList[k].Ex, hitList[k].EE[ID]);
      //   hEx_dE->Fill( hitList[k].Ex, hitList[k].dE[ID]);

      // }

      for( int ID = 0; ID < 4; ID ++ ){

        if( cut[ID]->IsInside(hitList[k].EE[ID], hitList[k].dE[ID] ) ) {
          hExg->Fill( hitList[k].Ex);
          hezg->Fill( hitList[k].z, hitList[k].e );
        }

      }

    }


    //&========================================= this block is for gain match of dE
    // if( !(12 <= *runID && *runID <= 18 )) {

    //   for( size_t k = 0; k < hitList.size(); k++ ){ 
    //     for( int i = 0; i < 4; i++ ){
    //       hrdt2D[i]->Fill( hitList[k].EE[i], hitList[k].dE[i] );

    //       // if( hitList[k].EE[i] > 3200) hdE[i]->Fill( hitList[k].dE[i] ); 
      
    //     }
    //   }

    // }else{

    //   for( size_t k = 0; k < hitList.size(); k++ ){ 
    //     for( int i = 0; i < 4; i++ ){
    //       hrdt2Ds[i]->Fill( hitList[k].EE[i], hitList[k].dE[i] );
    //       // if( hitList[k].EE[i] > 3200) hdEs[i]->Fill( hitList[k].dE[i]);       
    //     }
    //   }

    // }
    //&========================================= 



    // if( reader.GetCurrentEntry() % 10000 == 0 ) printf("..");

  };

  //*###################################################

  int padSize = 400;
  // TCanvas * canvas = new TCanvas("canvas", "canvas", numCol * padSize, numSide * padSize);
  // canvas->Divide(numCol, numSide);

  // for( int i = 0; i < numDet; i++){
  //   canvas->cd(i+1);
  //   heCal_xCal[i]->Draw("colz");

  //   // heCal[i]->Draw("");
  //   // heCalg[i]->SetLineColor(kRed);
  //   // heCalg[i]->Draw("same");
  // }

  TCanvas * canvas1 = new TCanvas("canvas1", "canvas1", 500, 0, 1000, 500);
  // hArrayMulti->Draw();
  // hRDTMulti->Draw();
  // hEx->Draw();
  hExg->Draw("");

  
  TCanvas * canvas2 = new TCanvas("canvas2", "canvas2", 500, 600, 2000, 1000);
  canvas2->Divide(2,1);
  canvas2->cd(1); hez->Draw("colz");
  PlotKELines();
  canvas2->cd(2); hezg->Draw("colz box");
  PlotKELines();

  // TCanvas * canvas3 = new TCanvas("canvas3", "canvas3", 1600, 1600);
  // canvas3->Divide(2,2);
  // canvas3->cd(1); hdE[0]->Draw("hist"); hdEs[0]->SetLineColor(kRed); hdEs[0]->Draw("same");
  // canvas3->cd(2); hdE[1]->Draw("hist"); hdEs[1]->SetLineColor(kRed); hdEs[1]->Draw("same");
  // canvas3->cd(3); hdE[3]->Draw("hist"); hdEs[3]->SetLineColor(kRed); hdEs[3]->Draw("same");
  // canvas3->cd(4); hdE[2]->Draw("hist"); hdEs[2]->SetLineColor(kRed); hdEs[2]->Draw("same");

  // TCanvas * canvas3 = new TCanvas("canvas3", "canvas3", 1600, 1600);
  // canvas3->Divide(2,2);
  // canvas3->cd(1); hrdt2Ds[0]->Draw("colz box");
  // canvas3->cd(2); hrdt2Ds[1]->Draw("colz box");
  // canvas3->cd(3); hrdt2Ds[3]->Draw("colz box");
  // canvas3->cd(4); hrdt2Ds[2]->Draw("colz box");

  TCanvas * canvas4 = new TCanvas("canvas4", "canvas4", 1600, 1600);
  canvas4->Divide(2,2);
  canvas4->cd(1); hrdt2D[0]->Draw("colz"); cut[0]->Draw("same");
  canvas4->cd(2); hrdt2D[1]->Draw("colz"); cut[1]->Draw("same");
  canvas4->cd(3); hrdt2D[3]->Draw("colz"); cut[3]->Draw("same");
  canvas4->cd(4); hrdt2D[2]->Draw("colz"); cut[2]->Draw("same");


  // TCanvas * canvas5 = new TCanvas("canvas5", "canvas5", 100, 0, 1200, 600);
  // canvas5->Divide(2,1);
  // canvas5->cd(1); hEx_EE->Draw("colz box");
  // canvas5->cd(2); hEx_dE->Draw("colz box");

  TCanvas * canvas6 = new TCanvas("canvas6", "canvas6", 100, 0, 1200, 600);
  hArrayMulti->Draw();

}

//^=================================================
//^=================================================

void CalExTheta(Hit & hit){

  double Ex, thetaCM ;

  double y = hit.e + mass; // to give the KE + mass of proton;
  double Z = alpha * gamm * betRel * hit.z;
  double H = TMath::Sqrt(TMath::Power(gamm * betRel,2) * (y*y - mass * mass) ) ;

  if( TMath::Abs(Z) < H ) {
    ///using Newton's method to solve 0 ==	H * sin(phi) - G * tan(phi) - Z = f(phi) 
    double tolerrence = 0.001;
    double phi = 0;  ///initial phi = 0 -> ensure the solution has f'(phi) > 0
    double nPhi = 0; /// new phi

    int iter = 0;
    do{
      phi = nPhi;
      nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G /TMath::Power( TMath::Cos(phi), 2));					 
      iter ++;
      if( iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
    }while( TMath::Abs(phi - nPhi ) > tolerrence);
    phi = nPhi;

    /// check f'(phi) > 0
    double Df = H * TMath::Cos(phi) - G / TMath::Power( TMath::Cos(phi),2);
    if( Df > 0 && TMath::Abs(phi) < TMath::PiOver2()  ){
      double K = H * TMath::Sin(phi);
      double x = TMath::ACos( mass / ( y * gamm - K));
      double momt = mass * TMath::Tan(x); /// momentum of particel b or B in CM frame
      double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(momt*momt + mass * mass));
      Ex = EB - massB;

      double hahaha1 = gamm* TMath::Sqrt(mass * mass + momt * momt) - y;
      double hahaha2 = gamm* betRel * momt;
      thetaCM = TMath::ACos(hahaha1/hahaha2) * TMath::RadToDeg();

    }else{
      Ex = TMath::QuietNaN();
      thetaCM = TMath::QuietNaN();
    }

  }else{
    Ex = TMath::QuietNaN();
    thetaCM = TMath::QuietNaN();
  }

  hit.Ex = Ex;
  hit.thetaCM = thetaCM;
  
}

void LoadDetGeoAndReactionConfigFile(){

  std::string detGeoFileName = "detectorGeo.txt";
  printf("=======================\n");
  printf(" loading detector geometery : %s.", detGeoFileName.c_str());
  TMacro * haha = new TMacro();
  if( haha->ReadFile(detGeoFileName.c_str()) > 0 ) {
    printf("... done.\n");
    detGeo = LoadDetectorGeo(haha);
    PrintDetGeo(detGeo);

    zRange[0] = detGeo.zMin - 50 ;
    zRange[1] = detGeo.zMax + 50 ;
  }else{
    printf("... fail\n");
  }

  std::string reactionConfigFileName = "reactionConfig.txt";
  printf("=======================\n");
  printf(" loading reaction config : %s.", reactionConfigFileName.c_str());
  TMacro * kaka = new TMacro();
  if( kaka->ReadFile(reactionConfigFileName.c_str()) > 0 ) {
    printf("..... done.\n");
    reactionConfig  = LoadReactionConfig(kaka);
    PrintReactionConfig(reactionConfig);
  }else{
    printf("..... fail\n");
  }

}

void LoadXFXNCorr(){
   //========================================= xf = xn correction
   printf(" loading xf-xn correction.");
   std::ifstream file;
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf(".......... done.\n");
   }else{
      printf(".......... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
}

void LoadXFXN2ECorr(){
   //========================================= e = xf + xn correction
   printf(" loading xf/xn-e correction.");
   std::ifstream file;
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("........ done.\n");
   }else{
      printf("........ fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();
} 

void LoadXScaleCorr(){
   //========================================= X-Scale correction
   printf(" loading x-Scale correction.");
   std::ifstream file;
   file.open("correction_scaleX.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xScale[i] = a;  
         i = i + 1;
      }
      printf("........ done.\n");
      
   }else{
      printf("........ fail.\n");
      for( int i = 0; i < numDet ; i++){
         xScale[i] = 1.;
      }
   }
   file.close();
   
}

void LoadECorr(){
   //========================================= e correction
   printf(" loading e correction.");
   std::ifstream file;
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;  // 1/a1
         eCorr[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf(".............. done.\n");
      
   }else{
      printf(".............. fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
   }
   file.close();

}

void LoadReactionPars(){

  double q;
   
  printf(" loading reaction parameters");
  std::ifstream file;
  file.open("reaction.dat");
  if( file.is_open() ){
    std::string x;
    int i = 0;
    while( file >> x ){
        if( x.substr(0,2) == "//" )  continue;
        if( i == 0 ) mass = atof(x.c_str());
        if( i == 1 ) q    = atof(x.c_str());
        if( i == 2 ) betRel = atof(x.c_str()); 
        if( i == 3 ) Et   = atof(x.c_str()); 
        if( i == 4 ) massB = atof(x.c_str()); 
        i = i + 1;
    }
    printf("........ done.\n");

    alpha = 299.792458 * abs(detGeo.Bfield) * q / TMath::TwoPi()/1000.; //MeV/mm
    gamm = 1./TMath::Sqrt(1-betRel*betRel);
    G = alpha * gamm * betRel * detGeo.detPerpDist ;
    printf("\tmass-b    : %f MeV/c2 \n", mass);
    printf("\tcharge-b  : %f \n", q);
    printf("\tE-total   : %f MeV \n", Et);
    printf("\tmass-B    : %f MeV/c2 \n", massB);
    printf("\tbeta      : %f \n", betRel);
    printf("\tB-field   : %f T \n", detGeo.Bfield);
    printf("\tslope     : %f MeV/mm \n", alpha * betRel);
    printf("\tdet radius: %f mm \n", detGeo.detPerpDist);
    printf("\tG-coeff   : %f MeV \n", G);
    printf("=================================\n");

  }else{
    printf("........ fail.\n");
  }
  file.close();
   
}
