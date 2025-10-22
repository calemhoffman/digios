#include <string.h>

#include <fstream>

#include "../Armory/AnalysisLibrary.h"
#include "Hit.h"

#include <TROOT.h>
#include "TCanvas.h"
#include "TChain.h"
#include "TGraph.h"
#include "TCutG.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMacro.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"

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

TCutG *cut[4];

void CalExTheta(Hit &hit);
void LoadDetGeoAndReactionConfigFile();
void LoadXFXNCorr();
void LoadXFXN2ECorr();
void LoadXScaleCorr();
void LoadECorr();
void LoadReactionPars();

double xfxn2e(double x) {
  return 67.7618 + 0.311656 * x + 7.37694e-5 * x * x - 4.21035e-9 * x * x * x;
}

TFile *transfer = nullptr;
TObjArray *gList = nullptr;
TObjArray *fxList = nullptr;
int nGList = 0;
int nFxList = 0;

void PlotKELines() {
  /// the constant thetaCM line
  gList->At(0)->Draw("same");
  /// the e-z line for excitation
  for (int i = 0; i < nFxList; i++) {
    ((TF1 *)fxList->At(i))->SetLineColor(6);
    fxList->At(i)->Draw("same");
  }
}

TChain *chain = nullptr;

//^############################################################
void analysis_12C() {
  transfer = new TFile("transfer.root");
  if (transfer->IsOpen()) {
    gList = (TObjArray *)transfer->FindObjectAny("gList");
    nGList = gList->GetLast() + 1;
    fxList = (TObjArray *)transfer->FindObjectAny("fxList");
    nFxList = fxList->GetLast() + 1;
  }

  chain = new TChain("gen_tree");
  // chain->Add("../root_data/trace_run005.root");
  chain->Add("../root_data/trace_run024.root");

  printf("\033[0;31m========================================== Number of Files : %2d\n", chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  unsigned long long numTotalEvents = chain->GetEntries();
  printf("================================= total %lld\033[0m\n", numTotalEvents);

  TTreeReader reader(chain);

  TTreeReaderValue<unsigned int> evID = {reader, "evID"};
  TTreeReaderValue<unsigned short> runID = {reader, "runID"};
  TTreeReaderArray<float> e = {reader, "e"};
  TTreeReaderArray<float> xf = {reader, "xf"};
  TTreeReaderArray<float> xn = {reader, "xn"};
  TTreeReaderArray<float> rdt = {reader, "rdt"};
  TTreeReaderArray<ULong64_t> rdt_t = {reader, "rdt_t"};
  TTreeReaderArray<ULong64_t> e_t = {reader, "e_t"};

  TTreeReaderArray<float> te = {reader, "te"};
  TTreeReaderArray<float> te_t = {reader, "te_t"};
  TTreeReaderArray<float> te_r = {reader, "te_r"};
  TTreeReaderArray<float> te_chi2 = {reader, "te_chi2"};

  TTreeReaderArray<float> txf = {reader, "txf"};
  TTreeReaderArray<float> txf_t = {reader, "txf_t"};
  TTreeReaderArray<float> txf_r = {reader, "txf_r"};
  TTreeReaderArray<float> txf_chi2 = {reader, "txf_chi2"};

  TTreeReaderArray<float> txn = {reader, "txn"};
  TTreeReaderArray<float> txn_t = {reader, "txn_t"};
  TTreeReaderArray<float> txn_r = {reader, "txn_r"};
  TTreeReaderArray<float> txn_chi2 = {reader, "txn_chi2"};

  TTreeReaderArray<float> trdt = {reader, "trdt"};
  TTreeReaderArray<float> trdt_t = {reader, "trdt_t"};
  TTreeReaderArray<float> trdt_r = {reader, "trdt_r"};
  TTreeReaderArray<float> trdt_chi2 = {reader, "trdt_chi2"};

  // TTreeReaderValue<TClonesArray>   trace = {reader, "trace"}; // very slow

  //*###################################################

  LoadDetGeoAndReactionConfigFile();
  LoadXFXNCorr();
  LoadXFXN2ECorr();
  LoadXScaleCorr();
  LoadECorr();
  LoadReactionPars();

  //*################################################### RDT Cut

  //@=============== 12C cut
  cut[0] = new TCutG("rdtCut0", 7);
  cut[0]->SetPoint(0, 4422.68, 1760.71);
  cut[0]->SetPoint(1, 4158.67, 1750.79);
  cut[0]->SetPoint(2, 4051.12, 1453.36);
  cut[0]->SetPoint(3, 4178.23, 1126.19);
  cut[0]->SetPoint(4, 4667.12, 878.331);
  cut[0]->SetPoint(5, 4696.46, 1602.08);
  cut[0]->SetPoint(6, 4422.68, 1760.71);

  cut[1] = new TCutG("rdtCut1", 7);
  cut[1]->SetPoint(0, 4481.34, 1750.79);
  cut[1]->SetPoint(1, 3777.34, 1919.34);
  cut[1]->SetPoint(2, 3767.56, 1721.05);
  cut[1]->SetPoint(3, 4041.34, 1304.65);
  cut[1]->SetPoint(4, 4637.79, 1264.99);
  cut[1]->SetPoint(5, 4813.79, 1512.85);
  cut[1]->SetPoint(6, 4481.34, 1750.79);

  cut[2] = new TCutG("rdtCut2", 5);
  cut[2]->SetPoint(0, 4051.12, 2206.85);
  cut[2]->SetPoint(1, 4109.79, 1562.42);
  cut[2]->SetPoint(2, 4931.12, 1334.39);
  cut[2]->SetPoint(3, 4911.57, 2038.31);
  cut[2]->SetPoint(4, 4051.12, 2206.85);

  cut[3] = new TCutG("rdtCut3", 5);
  cut[3]->SetPoint(0, 3444.89, 3862.55);
  cut[3]->SetPoint(1, 3630.67, 3188.37);
  cut[3]->SetPoint(2, 4647.57, 3019.83);
  cut[3]->SetPoint(3, 4540.01, 3555.2);
  cut[3]->SetPoint(4, 3444.89, 3862.55);

  //*###################################################
  printf("===================================================== Create histograms...\n");

  double ExRange[2] = {3, 20};
  double resol = 0.05;
  int ExBin = (ExRange[1] - ExRange[0]) / resol;

  int eRange[2] = {1000, 2500};
  int xRange = 9000;

  TH1F *he[numDet];
  TH1F *hxf[numDet];
  TH1F *hxn[numDet];

  TH1F *hte[numDet];
  TH1F *htxf[numDet];
  TH1F *htxn[numDet];

  TH2F * he_xs[numDet]; //xs = xf + xn
  TH2F * hte_xs[numDet];

  TH2F * hxf_xn[numDet];
  TH2F * htxf_txn[numDet];

  for (int i = 0; i < numDet; i++) {
    he[i] = new TH1F(Form("he_det%02d", i + 1),  Form("e distribution det %02d", i + 1),  500, eRange[0], eRange[1]);
    hte[i]  = new TH1F(Form("hte_det%02d", i + 1),  Form("te distribution det %02d", i + 1),  500, eRange[0], eRange[1]);

    hxf[i]  = new TH1F(Form("hxf_det%02d", i + 1),  Form("xf distribution det %02d", i + 1),  500, 0, xRange);
    htxf[i] = new TH1F(Form("htxf_det%02d", i + 1), Form("txf distribution det %02d", i + 1), 500, 0, xRange);

    hxn[i]  = new TH1F(Form("hxn_det%02d", i + 1),  Form("xn distribution det %02d", i + 1),  500, 0, xRange);
    htxn[i] = new TH1F(Form("htxn_det%02d", i + 1), Form("txn distribution det %02d", i + 1), 500, 0, xRange);

    he_xs[i] = new TH2F(Form("he_xs_det%02d", i + 1), Form("e vs xs det %02d", i + 1), 500, 0, eRange[1], 500, 0, eRange[1]);
    hte_xs[i] = new TH2F(Form("hte_xs_det%02d", i + 1), Form("te vs xs det %02d", i + 1), 500, 0, eRange[1], 500, 0, eRange[1]);

    hxf_xn[i]   = new TH2F(Form("hxf_xn_det%02d", i + 1),     Form("xf vs xn det %02d", i + 1), 500, 0, xRange, 500, 0, xRange);
    htxf_txn[i] = new TH2F(Form("htxf_txn_det%02d", i + 1), Form("txf vs txn det %02d", i + 1), 500, 0, xRange, 500, 0, xRange);
  }

  //*################################################### Process loop
  // std::vector<Hit> hitList;

  while (reader.Next()) {
    // hitList.clear();
    // Hit hit;

    /// ##################################################################### Array
    for (int i = 0; i < numDet; i++) {
      //@================ gate
      if (TMath::IsNaN(e[i])) continue;
      if (TMath::IsNaN(xn[i]) && TMath::IsNaN(xf[i])) continue;
      if (i == 10 || i == 11 ) continue; 

      if (txf[i] < 200 ) continue;
      if (txn[i] < 200 ) continue;

      he[i]->Fill(e[i]);
      hxf[i]->Fill(xf[i]);
      hxn[i]->Fill(xn[i]);

      he_xs[i]->Fill(xf[i] + xn[i], e[i]);
      hxf_xn[i]->Fill(xf[i], xn[i]);

      hte[i]->Fill(te[i]);
      htxf[i]->Fill(txf[i]);
      htxn[i]->Fill(txn[i]);

      hte_xs[i]->Fill(xf[i] + xn[i], te[i]);
      htxf_txn[i]->Fill(txf[i], txn[i]);

    }

    /// ##################################################################### RDT


    /// ##################################################################### Array & RDT

    // simple progress bar
    static unsigned long long __evtCount = 0;
    __evtCount++;

    if (numTotalEvents > 0) {
      const int barWidth = 50;
      // update every 1000 events or on first/last event
      unsigned long long updateInterval = 1000;
      if (__evtCount == 1 || __evtCount == numTotalEvents || (__evtCount % updateInterval) == 0) {
        double frac = (double)__evtCount / (double)numTotalEvents;
        int pos = (int)(barWidth * frac);
        printf("\r[");
        for (int b = 0; b < barWidth; ++b) {
          if (b < pos) printf("=");
          else if (b == pos) printf(">");
          else printf(" ");
        }
        printf("] %6.2f%% (%llu/%llu)", frac * 100.0, __evtCount, numTotalEvents);
        fflush(stdout);
        if (__evtCount == numTotalEvents) printf("\n");
      }
    }

  };

  //*################################################### Draw hist

  int padSize = 400;

  TCanvas *canvas = new TCanvas("canvas", "canvas", numCol * padSize, numSide * padSize);
  canvas->Divide(numCol, numSide);

  for (int i = 0; i < numDet; i++) {
    canvas->cd(i + 1);
    // he[i]->Draw();
    // hte[i]->SetLineColor(kRed);
    // hte[i]->Draw("same");

    // hxf[i]->Draw();
    // htxf[i]->SetLineColor(kRed); 
    // htxf[i]->Draw("same");

    // hxn[i]->Draw();
    // htxn[i]->SetLineColor(kRed);
    // htxn[i]->Draw("same");

    // he_xs[i]->Draw("colz");

    hxf_xn[i]->Draw("colz");
  }

  TCanvas *canvas2 = new TCanvas("canvas2", "canvas2", numCol * padSize, numSide * padSize);
  canvas2->Divide(numCol, numSide);

  for (int i = 0; i < numDet; i++) {
    canvas2->cd(i + 1);
    hte_xs[i]->Draw("colz");

    htxf_txn[i]->Draw("colz");
  }

}

//^=================================================
//^=================================================

void CalExTheta(Hit &hit) {
  double Ex, thetaCM;

  double y = hit.e + mass;  // to give the KE + mass of proton;
  double Z = alpha * gamm * betRel * hit.z;
  double H = TMath::Sqrt(TMath::Power(gamm * betRel, 2) * (y * y - mass * mass));

  if (TMath::Abs(Z) < H) {
    /// using Newton's method to solve 0 ==	H * sin(phi) - G * tan(phi) - Z = f(phi)
    double tolerrence = 0.001;
    double phi = 0;   /// initial phi = 0 -> ensure the solution has f'(phi) > 0
    double nPhi = 0;  /// new phi

    int iter = 0;
    do {
      phi = nPhi;
      nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G / TMath::Power(TMath::Cos(phi), 2));
      iter++;
      if (iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
    } while (TMath::Abs(phi - nPhi) > tolerrence);
    phi = nPhi;

    /// check f'(phi) > 0
    double Df = H * TMath::Cos(phi) - G / TMath::Power(TMath::Cos(phi), 2);
    if (Df > 0 && TMath::Abs(phi) < TMath::PiOver2()) {
      double K = H * TMath::Sin(phi);
      double x = TMath::ACos(mass / (y * gamm - K));
      double momt = mass * TMath::Tan(x);  /// momentum of particel b or B in CM frame
      double EB = TMath::Sqrt(mass * mass + Et * Et - 2 * Et * TMath::Sqrt(momt * momt + mass * mass));
      Ex = EB - massB;

      double hahaha1 = gamm * TMath::Sqrt(mass * mass + momt * momt) - y;
      double hahaha2 = gamm * betRel * momt;
      thetaCM = TMath::ACos(hahaha1 / hahaha2) * TMath::RadToDeg();

    } else {
      Ex = TMath::QuietNaN();
      thetaCM = TMath::QuietNaN();
    }

  } else {
    Ex = TMath::QuietNaN();
    thetaCM = TMath::QuietNaN();
  }

  hit.Ex = Ex ;
  hit.thetaCM = thetaCM;

}

void LoadDetGeoAndReactionConfigFile() {
  std::string detGeoFileName = "detectorGeo.txt";
  printf("=======================\n");
  printf(" loading detector geometery : %s.", detGeoFileName.c_str());
  TMacro *haha = new TMacro();
  if (haha->ReadFile(detGeoFileName.c_str()) > 0) {
    printf("... done.\n");
    detGeo = LoadDetectorGeo(haha);
    PrintDetGeo(detGeo);

    zRange[0] = detGeo.zMin - 50;
    zRange[1] = detGeo.zMax + 50;
  } else {
    printf("... fail\n");
  }

  std::string reactionConfigFileName = "reactionConfig.txt";
  printf("=======================\n");
  printf(" loading reaction config : %s.", reactionConfigFileName.c_str());
  TMacro *kaka = new TMacro();
  if (kaka->ReadFile(reactionConfigFileName.c_str()) > 0) {
    printf("..... done.\n");
    reactionConfig = LoadReactionConfig(kaka);
    PrintReactionConfig(reactionConfig);
  } else {
    printf("..... fail\n");
  }
}

void LoadXFXNCorr() {
  //========================================= xf = xn correction
  printf(" loading xf-xn correction.");
  std::ifstream file;
  file.open("correction_xf_xn.dat");
  if (file.is_open()) {
    double a;
    int i = 0;
    while (file >> a) {
      if (i >= numDet) break;
      xnCorr[i] = a;
      i = i + 1;
    }

    printf(".......... done.\n");
  } else {
    printf(".......... fail.\n");

    for (int i = 0; i < numDet; i++) {
      xnCorr[i] = 1;
    }
  }
  file.close();
}

void LoadXFXN2ECorr() {
  //========================================= e = xf + xn correction
  printf(" loading xf/xn-e correction.");
  std::ifstream file;
  file.open("correction_xfxn_e.dat");
  if (file.is_open()) {
    double a, b;
    int i = 0;
    while (file >> a >> b) {
      if (i >= numDet) break;
      xfxneCorr[i][0] = a;
      xfxneCorr[i][1] = b;
      i = i + 1;
    }
    printf("........ done.\n");
  } else {
    printf("........ fail.\n");
    for (int i = 0; i < numDet; i++) {
      xfxneCorr[i][0] = 0;
      xfxneCorr[i][1] = 1;
    }
  }
  file.close();
}

void LoadXScaleCorr() {
  //========================================= X-Scale correction
  printf(" loading x-Scale correction.");
  std::ifstream file;
  file.open("correction_scaleX.dat");
  if (file.is_open()) {
    double a, b;
    int i = 0;
    while (file >> a) {
      if (i >= numDet) break;
      xScale[i] = a;
      i = i + 1;
    }
    printf("........ done.\n");

  } else {
    printf("........ fail.\n");
    for (int i = 0; i < numDet; i++) {
      xScale[i] = 1.;
    }
  }
  file.close();
}

void LoadECorr() {
  //========================================= e correction
  printf(" loading e correction.");
  std::ifstream file;
  file.open("correction_e.dat");
  if (file.is_open()) {
    double a, b;
    int i = 0;
    while (file >> a >> b) {
      if (i >= numDet) break;
      eCorr[i][0] = a;  // 1/a1
      eCorr[i][1] = b;  //  a0 , e' = e * a1 + a0
      // printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
      i = i + 1;
    }
    printf(".............. done.\n");

  } else {
    printf(".............. fail.\n");
    for (int i = 0; i < numDet; i++) {
      eCorr[i][0] = 1.;
      eCorr[i][1] = 0.;
    }
  }
  file.close();
}

void LoadReactionPars() {
  double q;

  printf(" loading reaction parameters");
  std::ifstream file;
  file.open("reaction.dat");
  if (file.is_open()) {
    std::string x;
    int i = 0;
    while (file >> x) {
      if (x.substr(0, 2) == "//") continue;
      if (i == 0) mass = atof(x.c_str());
      if (i == 1) q = atof(x.c_str());
      if (i == 2) betRel = atof(x.c_str());
      if (i == 3) Et = atof(x.c_str());
      if (i == 4) massB = atof(x.c_str());
      i = i + 1;
    }
    printf("........ done.\n");

    alpha = 299.792458 * abs(detGeo.Bfield) * q / TMath::TwoPi() / 1000.;  // MeV/mm
    gamm = 1. / TMath::Sqrt(1 - betRel * betRel);
    G = alpha * gamm * betRel * detGeo.detPerpDist;
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

  } else {
    printf("........ fail.\n");
  }
  file.close();
}

//^################################### Auxillary functions
