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
Float_t dt[30];
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
   tree->SetBranchAddress("trdt",trdt); 
   tree->SetBranchAddress("trdt_t",trdt_t); 
   tree->SetBranchAddress("trdt_r",trdt_r);
   //final
   tree->SetBranchAddress("coinTime",&coinTime);
   tree->SetBranchAddress("tcoin_t",&tcoin_t);
   tree->SetBranchAddress("Ex",&Ex);
   tree->SetBranchAddress("thetaLab",&thetaLab);
}

TH1F * heventID;
TH1F * hrunID;
TH2F * hxfxn[24];
TH2F * hesumx[24];
TH2F * htetx[24];
TH2F * htetr[24];
TH2F * htete[24];
TH2F * hdtx[24];
TH2F * hdtr[24];
TH2F * hdte[24];
TH1F * htet[24];
TH1F * htetg[24];
TH2F * heVx[24];
TH2F * heVxg[24];
TH1F * hrdt[8];
TH1F * hrdtg[8];
TH2F * hrdt2d[10];// 5 de vs. 2 e
TH2F * hrdt2dg[10];// 5 de vs. 2 e
TH2F * hez;
TH2F * hezg;
TH1F * hEx[24];
TH1F * hExg[24];
TH1F * hExTot;
TH1F * hExgTot;
TH1F * hcoin_t;
TH1F * hcoinTime;
TH1F * hdt;
void defineHistos() {
       //histograms
   //general
   heventID = new TH1F("heventID","heventID",1000,0,1e6);
   hrunID = new TH1F("hrunID","hrunID",200,0,200);
   //array
   for (int i=0;i<24;i++) {
        hxfxn[i] = new TH2F(Form("hxfxn%d",i),Form("hxfxn%d; xf; xn",i), 1000,0,2500,1000,0,2500);
        hesumx[i] = new TH2F(Form("hesumx%d",i),Form("hesumx%d; e; xf+xn",i), 1000,0,10,1000,0,2500);
        htetx[i] = new TH2F(Form("htetx%d",i),Form("htetx%d; x; te_t",i), 1000,-1.1,1.1,350,950,1150);
        htetr[i] = new TH2F(Form("htetr%d",i),Form("htetr%d; te_r; te_t",i), 1000,0,10,350,950,1150);
        htete[i] = new TH2F(Form("htete%d",i),Form("htete%d; e; te_t",i), 1000,0,10,350,950,1150);
        hdtx[i] = new TH2F(Form("hdtx%d",i),Form("hdtx%d; x; dt",i), 1000,-1.1,1.1,300,-100,200);
        hdtr[i] = new TH2F(Form("hdtr%d",i),Form("hdtr%d; te_r; dt",i), 1000,0,10,300,-100,200);
        hdte[i] = new TH2F(Form("hdte%d",i),Form("hdte%d; e; dt",i), 1000,0,10,300,-100,200);
        htet[i] = new TH1F(Form("htet%d",i),Form("htet%d; te_t",i), 350,950,1150);
        htetg[i] = new TH1F(Form("htetg%d",i),Form("htetg%d; te_t",i), 350,950,1150);
        heVx[i] = new TH2F(Form("heVx%d",i),Form("heVx%d; x, e",i), 500, -1.1,1.1, 500,0,10);
        heVxg[i] = new TH2F(Form("heVxg%d",i),Form("heVxg%d; x, e",i), 500, -1.1,1.1, 500,0,10);
        hEx[i] = new TH1F(Form("hExTot%d",i),Form("hExTot%d",i),400,-2,8); 
        hExg[i] = new TH1F(Form("hExTotg%d",i),Form("hExTotg%d",i),400,-2,8); 
   }
   //recoil
   for (int i=0;i<8;i++) {
       if (i<8) {
            hrdt[i] = new TH1F(Form("hrdt%d",i),Form("hrdt%d; rdt",i), 1000,0,5000);
            hrdtg[i] = new TH1F(Form("hrdtg%d",i),Form("hrdtg%d; rdt",i), 1000,0,5000);
       }
       if (i < 5) {
        hrdt2d[i] = new TH2F(Form("hrdt2d%d",i),Form("htet2d%d; rdt[%d]; rdt[%d];",i,i,6),1000,0,5000,1000,0,5000);
        hrdt2d[i+5] = new TH2F(Form("hrdt2d%d",i+5),Form("htet2d%d; rdt[%d]; rdt[%d];",i+5,i,7),1000,0,5000,1000,0,5000);
        hrdt2dg[i] = new TH2F(Form("hrdt2dg%d",i),Form("htet2dg%d; rdt[%d]; rdt[%d];",i,i,6),1000,0,5000,1000,0,5000);
        hrdt2dg[i+5] = new TH2F(Form("hrdt2dg%d",i+5),Form("htet2dg%d; rdt[%d]; rdt[%d];",i+5,i,7),1000,0,5000,1000,0,5000);
       }
   }
   //ez
   hez = new TH2F("hez","hez; z; e",1000,-600,-100,1000,0,10);
   hezg = new TH2F("hezg","gated hez; z; e",1000,-600,-100,1000,0,10);
   hExTot = new TH1F("hExTot","hExTot",400,-2,8);
   hExgTot = new TH1F("hExgTot","hExgTot",400,-2,8);
   hcoin_t = new TH1F("hcoin_t","hcoin_t",400,-200,200);
   hcoinTime = new TH1F("hcoinTime","hcoinTime",400,-200,200);
   hdt = new TH1F("hdt","hdt",400,-50,50);
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