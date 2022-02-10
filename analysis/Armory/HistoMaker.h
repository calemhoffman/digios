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

