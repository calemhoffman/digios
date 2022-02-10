#include "TROOT.h"
#include "TStyle.h"
#include <TChain.h>
#include <TString.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph2D.h>
#include <TGraph.h>
#include <TCutG.h>
#include <TRandom.h>
#include <string>
#include <fstream>
#include <math.h>
#include <TRandom.h>
#include <TDatime.h>
#include <TObjArray.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TFile.h>
#include <TEventList.h>

//tree  
Int_t eventID;
Int_t run;
Float_t eC[30];
Float_t xfC[30];
Float_t xnC[30];
ULong64_t eC_t[30];
Float_t x[30]; // 
Float_t z[30]; 
int det;    //
int hitID[30]; // 
int multiHit; // 
Float_t thetaCM;
Float_t Ex;   
Float_t thetaLab;
Float_t rdtC[8];
ULong64_t rdtC_t[8];
int rdtID; // rdt hit ID
int rdtdEMultiHit;
Int_t   coin_t;
Float_t tcoin_t;
Float_t coinTimeUC; 
Float_t coinTime;
// Declaration of leaf types
Int_t           runID;
Float_t         e[100];
ULong64_t       e_t[100];
ULong64_t       EBIS_t;
Float_t         xf[100];
ULong64_t       xf_t[100];
Float_t         xn[100];
ULong64_t       xn_t[100];
Float_t         ring[100];
ULong64_t       ring_t[100];
Float_t         rdt[8];
ULong64_t       rdt_t[8];
Float_t         tac[10];
ULong64_t       tac_t[10];
Float_t         elum[50];
ULong64_t       elum_t[50];
Float_t         ezero[50];
ULong64_t       ezero_t[50];
//Trace
Float_t         te[100];
Float_t         te_r[100];
Float_t         te_t[100];
Float_t         trdt[8];
Float_t         trdt_t[8];
Float_t         trdt_r[8];

void setTreeBranches(TTree * tree) {
   //general
   tree->SetBranchAddress("eventID",&eventID);
   tree->SetBranchAddress("run",&run);
   //array
   tree->SetBranchAddress("multiHit",&multiHit);
   tree->SetBranchAddress("e",e);
   tree->SetBranchAddress("x",x);
   tree->SetBranchAddress("xf",xf);
   tree->SetBranchAddress("xn",xn);
   tree->SetBranchAddress("te",te);
   tree->SetBranchAddress("te_t",te_t);
   tree->SetBranchAddress("te_r",te_r);
   tree->SetBranchAddress("z",z);
   //recoil
   tree->SetBranchAddress("rdt",rdt); 
}

TH1F * heventID;
TH1F * hrunID;
TH2F * hxfxn[24];
TH2F * hesumx[24];
TH2F * htetx[24];
TH2F * htetr[24];
TH2F * htete[24];
TH1F * htet[24];
TH1F * htetg[24];
TH2F * hez;
TH2F * hezg;
void defineHistos() {
       //histograms
   //general
   heventID = new TH1F("heventID","heventID",1000,0,1e6);
   hrunID = new TH1F("hrunID","hrunID",200,0,200);
   //array
   for (int i=0;i<24;i++) {
        hxfxn[i] = new TH2F(Form("hxfxn%d",i),Form("hxfxn%d; xf; xn",i), 1000,0,2500,1000,0,2500);
        hesumx[i] = new TH2F(Form("hesumx%d",i),Form("hesumx%d; e; xf+xn",i), 1000,0,10,1000,0,2500);
        htetx[i] = new TH2F(Form("htetx%d",i),Form("htetx%d; x; te_t",i), 1000,-1.1,1.1,350,90,125);
        htetr[i] = new TH2F(Form("htetr%d",i),Form("htetr%d; te_r; te_t",i), 1000,0,10,350,90,125);
        htete[i] = new TH2F(Form("htete%d",i),Form("htete%d; e; te_t",i), 1000,0,10,350,90,125);
        htet[i] = new TH1F(Form("htet%d",i),Form("htet%d; te_t",i), 350,90,125);
        htetg[i] = new TH1F(Form("htetg%d",i),Form("htetg%d; te_t",i), 350,90,125);
   }
   //recoil
   //ez
   hez = new TH2F("hez","hez; z; e",1000,-600,-100,1000,0,10);
   hezg = new TH2F("hezg","gated hez; z; e",1000,-600,-100,1000,0,10);
   //timing
}

Float_t tetXCorr[30][10];
ifstream file;
TMacro cali_tetX("correction_tetX.dat");
void getCalibrations() {
    file.open("correction_tetX.dat");
    if( file.is_open() ){
        printf("loading tetX correction parameters.");    
        int d;
        double a0, a1, a2, a3, a4, a5, a6, a7, a8;
        int i = 0;
        while( file >>  d >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 /* >> a7 >> a8*/){
            if( i >= 29) break;
            tetXCorr[i][0] = a0;tetXCorr[i][1] = a1;tetXCorr[i][2] = a2;
            tetXCorr[i][3] = a3;tetXCorr[i][4] = a4;tetXCorr[i][5] = a5;
            tetXCorr[i][6] = a6;//tetXCorr[i][7] = a7;tetXCorr[i][8] = a8; 
            i = i + 1;
        }
        printf(".... done.\n");
        //TMacro cali_tetX("correction_tetX.dat");
        //cali_tetX.Write("correction_tetX");
    }
    file.close();
}