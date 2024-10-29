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
TGraph *tDiffFineCorr[24];
TCutG * tDiffExCut[24];

TCanvas *canvas2;
TH2F *hdudu[24];
TH2F *hEx_tCorr[24];

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

void script() {
  transfer = new TFile("transfer.root");
  if (transfer->IsOpen()) {
    gList = (TObjArray *)transfer->FindObjectAny("gList");
    nGList = gList->GetLast() + 1;
    fxList = (TObjArray *)transfer->FindObjectAny("fxList");
    nFxList = fxList->GetLast() + 1;
  }

  chain = new TChain("gen_tree");
  // chain->Add("../root_data/gen_run00[4,5].root"); // alpha

  chain->Add("../root_data/trace_run01[2-8].root");  // > 1.5e5 pps
  chain->Add("../root_data/trace_run019.root");      // approx 2e5 pps
  chain->Add("../root_data/trace_run02[0-9].root");  // approx 2e5 pps
  chain->Add("../root_data/trace_run030.root");      // approx 2e5 pps
  chain->Add("../root_data/trace_run03[2-6].root");  // approx 2e5 pps
  chain->Add("../root_data/trace_run03[8-9].root");  // approx 2e5 pps
  chain->Add("../root_data/trace_run040.root");      // approx 2e5 pps

  // chain->Add("../root_data/trace_run025.root"); 

  printf("\033[0;31m========================================== Number of Files : %2d\n", chain->GetListOfFiles()->GetEntries());
  chain->GetListOfFiles()->Print();
  unsigned long long numTotalEvents = chain->GetEntries();
  printf("================================= total %lld\033[0m\n", numTotalEvents);

  TTreeReader reader(chain);

  TTreeReaderValue<unsigned long long> evID = {reader, "evID"};
  TTreeReaderValue<int> runID = {reader, "runID"};
  TTreeReaderArray<float> e = {reader, "e"};
  TTreeReaderArray<float> xf = {reader, "xf"};
  TTreeReaderArray<float> xn = {reader, "xn"};
  TTreeReaderArray<float> rdt = {reader, "rdt"};
  TTreeReaderArray<ULong64_t> rdt_t = {reader, "rdt_t"};
  TTreeReaderArray<ULong64_t> e_t = {reader, "e_t"};

  TTreeReaderArray<float> te = {reader, "te"};
  TTreeReaderArray<float> te_t = {reader, "te_t"};
  TTreeReaderArray<float> te_r = {reader, "te_r"};

  TTreeReaderArray<float> trdt = {reader, "trdt"};
  TTreeReaderArray<float> trdt_t = {reader, "trdt_t"};
  TTreeReaderArray<float> trdt_r = {reader, "trdt_r"};

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

  //*################################################### tDiffFine correction

  for (int i = 0; i < 24; i++) {
    tDiffFineCorr[i] = new TGraph();
  }

  tDiffFineCorr[0]->AddPoint(0.00, -9.208);
  tDiffFineCorr[0]->AddPoint(0.10, -4.923);
  tDiffFineCorr[0]->AddPoint(0.20, -1.577);
  tDiffFineCorr[0]->AddPoint(0.30,  0.829);
  tDiffFineCorr[0]->AddPoint(0.40,  2.296);
  tDiffFineCorr[0]->AddPoint(0.50,  2.823);
  tDiffFineCorr[0]->AddPoint(0.60,  2.410);
  tDiffFineCorr[0]->AddPoint(0.70,  1.057);
  tDiffFineCorr[0]->AddPoint(0.80, -1.235);
  tDiffFineCorr[0]->AddPoint(0.90, -4.468);
  tDiffFineCorr[0]->AddPoint(1.00, -8.252);

  tDiffFineCorr[1]->AddPoint(0.00, -7.909);
  tDiffFineCorr[1]->AddPoint(0.10, -4.153);
  tDiffFineCorr[1]->AddPoint(0.20, -1.163);
  tDiffFineCorr[1]->AddPoint(0.30,  1.059);
  tDiffFineCorr[1]->AddPoint(0.40,  2.513);
  tDiffFineCorr[1]->AddPoint(0.50,  3.201);
  tDiffFineCorr[1]->AddPoint(0.60,  3.121);
  tDiffFineCorr[1]->AddPoint(0.70,  2.274);
  tDiffFineCorr[1]->AddPoint(0.80,  0.659);
  tDiffFineCorr[1]->AddPoint(0.90, -4.213);
  tDiffFineCorr[1]->AddPoint(1.00, -6.880);
  tDiffFineCorr[1]->AddPoint(1.10, -9.153);

  tDiffFineCorr[2]->AddPoint(-0.05, -8.134);
  tDiffFineCorr[2]->AddPoint(0.18, -4.998);
  tDiffFineCorr[2]->AddPoint(0.28, -1.312);
  tDiffFineCorr[2]->AddPoint(0.44,  1.354);
  tDiffFineCorr[2]->AddPoint(0.55,  1.158);
  tDiffFineCorr[2]->AddPoint(0.67, -0.528);
  tDiffFineCorr[2]->AddPoint(0.95, -8.369);

  tDiffFineCorr[3]->AddPoint(-0.14, -9.180);
  tDiffFineCorr[3]->AddPoint(0.28,  2.517);
  tDiffFineCorr[3]->AddPoint(0.42,  4.935);
  tDiffFineCorr[3]->AddPoint(0.56,  4.935);
  tDiffFineCorr[3]->AddPoint(0.71,  3.105);
  tDiffFineCorr[3]->AddPoint(0.84, -1.534);
  tDiffFineCorr[3]->AddPoint(0.97, -5.586);
  tDiffFineCorr[3]->AddPoint(1.15, -13.754);

  tDiffFineCorr[4]->AddPoint(-0.25, -15.910);
  tDiffFineCorr[4]->AddPoint(0.05, -7.546);
  tDiffFineCorr[4]->AddPoint(0.16, -3.364);
  tDiffFineCorr[4]->AddPoint(0.32,  1.406);
  tDiffFineCorr[4]->AddPoint(0.41,  3.105);
  tDiffFineCorr[4]->AddPoint(0.51,  3.301);
  tDiffFineCorr[4]->AddPoint(0.62,  3.040);
  tDiffFineCorr[4]->AddPoint(0.73,  0.883);
  tDiffFineCorr[4]->AddPoint(0.77, -1.077);
  tDiffFineCorr[4]->AddPoint(0.95, -6.762);
  tDiffFineCorr[4]->AddPoint(1.20, -15.975);

  tDiffFineCorr[5]->AddPoint(-0.24, -12.839);
  tDiffFineCorr[5]->AddPoint(0.16, -1.796);
  tDiffFineCorr[5]->AddPoint(0.34,  2.386);
  tDiffFineCorr[5]->AddPoint(0.51,  3.562);
  tDiffFineCorr[5]->AddPoint(0.62,  3.040);
  tDiffFineCorr[5]->AddPoint(0.75,  0.949);
  tDiffFineCorr[5]->AddPoint(0.82, -0.816);
  tDiffFineCorr[5]->AddPoint(1.13, -12.447);


  tDiffFineCorr[6]->AddPoint(0.00, -3.660);
  tDiffFineCorr[6]->AddPoint(0.10, -2.845);
  tDiffFineCorr[6]->AddPoint(0.20, -2.202);
  tDiffFineCorr[6]->AddPoint(0.30, -1.732);
  tDiffFineCorr[6]->AddPoint(0.40, -1.435);
  tDiffFineCorr[6]->AddPoint(0.50, -1.310);
  tDiffFineCorr[6]->AddPoint(0.60, -1.358);
  tDiffFineCorr[6]->AddPoint(0.70, -1.579);
  tDiffFineCorr[6]->AddPoint(0.80, -1.972);
  tDiffFineCorr[6]->AddPoint(0.90, -2.538);
  tDiffFineCorr[6]->AddPoint(1.00, -3.351);

  tDiffFineCorr[7]->AddPoint(-0.17, -6.135);
  tDiffFineCorr[7]->AddPoint(0.15, -2.998);
  tDiffFineCorr[7]->AddPoint(0.41, -1.940);
  tDiffFineCorr[7]->AddPoint(0.62, -2.018);
  tDiffFineCorr[7]->AddPoint(0.88, -2.998);
  tDiffFineCorr[7]->AddPoint(1.17, -6.056);

  tDiffFineCorr[8]->AddPoint(-0.05, -1.273);
  tDiffFineCorr[8]->AddPoint(0.36,  1.275);
  tDiffFineCorr[8]->AddPoint(0.54,  1.785);
  tDiffFineCorr[8]->AddPoint(0.73,  1.158);
  tDiffFineCorr[8]->AddPoint(0.90, -0.489);
  tDiffFineCorr[8]->AddPoint(1.17, -4.096);

  tDiffFineCorr[9]->AddPoint(-0.23, -5.128);
  tDiffFineCorr[9]->AddPoint(0.09,  6.634);
  tDiffFineCorr[9]->AddPoint(0.33, 12.841);
  tDiffFineCorr[9]->AddPoint(0.46, 14.148);
  tDiffFineCorr[9]->AddPoint(0.58, 14.083);
  tDiffFineCorr[9]->AddPoint(0.64, 12.515);
  tDiffFineCorr[9]->AddPoint(0.76,  8.137);
  tDiffFineCorr[9]->AddPoint(0.86,  2.713);
  tDiffFineCorr[9]->AddPoint(0.93, -0.946);
  tDiffFineCorr[9]->AddPoint(1.21, -11.075);


  tDiffFineCorr[12]->AddPoint(0.00, -2.637);
  tDiffFineCorr[12]->AddPoint(0.10, -1.853);
  tDiffFineCorr[12]->AddPoint(0.20, -1.221);
  tDiffFineCorr[12]->AddPoint(0.30, -0.742);
  tDiffFineCorr[12]->AddPoint(0.40, -0.414);
  tDiffFineCorr[12]->AddPoint(0.50, -0.238);
  tDiffFineCorr[12]->AddPoint(0.60, -0.214);
  tDiffFineCorr[12]->AddPoint(0.70, -0.343);
  tDiffFineCorr[12]->AddPoint(0.80, -0.623);
  tDiffFineCorr[12]->AddPoint(0.90, -1.200);
  tDiffFineCorr[12]->AddPoint(1.00, -1.900);

  tDiffFineCorr[13]->AddPoint(0.00, -2.687);
  tDiffFineCorr[13]->AddPoint(0.10, -1.447);
  tDiffFineCorr[13]->AddPoint(0.20, -0.447);
  tDiffFineCorr[13]->AddPoint(0.30,  0.313);
  tDiffFineCorr[13]->AddPoint(0.40,  0.831);
  tDiffFineCorr[13]->AddPoint(0.50,  1.110);
  tDiffFineCorr[13]->AddPoint(0.60,  1.147);
  tDiffFineCorr[13]->AddPoint(0.70,  0.944);
  tDiffFineCorr[13]->AddPoint(0.80,  0.500);
  tDiffFineCorr[13]->AddPoint(0.90, -0.184);
  tDiffFineCorr[13]->AddPoint(1.00, -1.312);

  tDiffFineCorr[14]->AddPoint(-0.15, -3.508);
  tDiffFineCorr[14]->AddPoint(0.16, -0.881);
  tDiffFineCorr[14]->AddPoint(0.38, -0.058);
  tDiffFineCorr[14]->AddPoint(0.61, -0.175);
  tDiffFineCorr[14]->AddPoint(0.82, -1.116);
  tDiffFineCorr[14]->AddPoint(0.92, -1.626);
  tDiffFineCorr[14]->AddPoint(1.07, -2.528);
  tDiffFineCorr[14]->AddPoint(1.21, -3.116);

  tDiffFineCorr[15]->AddPoint(-0.07, -2.096);
  tDiffFineCorr[15]->AddPoint(0.26,  0.687);
  tDiffFineCorr[15]->AddPoint(0.42,  1.236);
  tDiffFineCorr[15]->AddPoint(0.58,  1.511);
  tDiffFineCorr[15]->AddPoint(0.75,  0.766);
  tDiffFineCorr[15]->AddPoint(0.93, -0.567);
  tDiffFineCorr[15]->AddPoint(1.10, -2.567);
  tDiffFineCorr[15]->AddPoint(1.22, -3.900);

  tDiffFineCorr[16]->AddPoint(-0.06, -3.351);
  tDiffFineCorr[16]->AddPoint(0.25,  8.097);
  tDiffFineCorr[16]->AddPoint(0.37, 10.293);
  tDiffFineCorr[16]->AddPoint(0.47, 10.803);
  tDiffFineCorr[16]->AddPoint(0.55, 10.528);
  tDiffFineCorr[16]->AddPoint(0.65,  8.881);
  tDiffFineCorr[16]->AddPoint(0.76,  6.451);
  tDiffFineCorr[16]->AddPoint(0.84,  3.353);
  tDiffFineCorr[16]->AddPoint(0.89,  1.119);
  tDiffFineCorr[16]->AddPoint(1.33, -5.899);

  tDiffFineCorr[17]->AddPoint(0.00, -3.834);
  tDiffFineCorr[17]->AddPoint(0.10,  1.706);
  tDiffFineCorr[17]->AddPoint(0.20,  6.048);
  tDiffFineCorr[17]->AddPoint(0.30,  9.191);
  tDiffFineCorr[17]->AddPoint(0.40, 11.136);
  tDiffFineCorr[17]->AddPoint(0.50, 11.881);
  tDiffFineCorr[17]->AddPoint(0.60, 11.428);
  tDiffFineCorr[17]->AddPoint(0.70,  9.775);
  tDiffFineCorr[17]->AddPoint(0.80,  6.924);
  tDiffFineCorr[17]->AddPoint(0.90,  2.875);
  tDiffFineCorr[17]->AddPoint(1.00, -3.037);
  tDiffFineCorr[17]->AddPoint(1.10, -9.049);

  tDiffFineCorr[18]->AddPoint(0.00, -10.431);
  tDiffFineCorr[18]->AddPoint(0.10, -6.201);
  tDiffFineCorr[18]->AddPoint(0.20, -2.873);
  tDiffFineCorr[18]->AddPoint(0.30, -0.448);
  tDiffFineCorr[18]->AddPoint(0.40,  1.074);
  tDiffFineCorr[18]->AddPoint(0.50,  1.693);
  tDiffFineCorr[18]->AddPoint(0.60,  1.410);
  tDiffFineCorr[18]->AddPoint(0.70,  0.224);
  tDiffFineCorr[18]->AddPoint(0.80, -1.865);
  tDiffFineCorr[18]->AddPoint(0.90, -4.857);
  tDiffFineCorr[18]->AddPoint(1.00, -9.637);

  tDiffFineCorr[19]->AddPoint(0.00, -8.442);
  tDiffFineCorr[19]->AddPoint(0.10, -4.342);
  tDiffFineCorr[19]->AddPoint(0.20, -1.100);
  tDiffFineCorr[19]->AddPoint(0.30,  1.285);
  tDiffFineCorr[19]->AddPoint(0.40,  2.813);
  tDiffFineCorr[19]->AddPoint(0.50,  3.484);
  tDiffFineCorr[19]->AddPoint(0.60,  3.297);
  tDiffFineCorr[19]->AddPoint(0.70,  2.254);
  tDiffFineCorr[19]->AddPoint(0.80,  0.353);
  tDiffFineCorr[19]->AddPoint(0.90, -2.405);
  tDiffFineCorr[19]->AddPoint(1.20, -16.171);

  tDiffFineCorr[21]->AddPoint(-0.25, -10.160);
  tDiffFineCorr[21]->AddPoint(0.12, -4.148);
  tDiffFineCorr[21]->AddPoint(0.30, -0.031);
  tDiffFineCorr[21]->AddPoint(0.49,  2.190);
  tDiffFineCorr[21]->AddPoint(0.64,  1.733);
  tDiffFineCorr[21]->AddPoint(0.86, -2.907);
  tDiffFineCorr[21]->AddPoint(1.04, -7.415);
  tDiffFineCorr[21]->AddPoint(1.43, -15.126);

  tDiffFineCorr[22]->AddPoint(-0.32, -11.597);
  tDiffFineCorr[22]->AddPoint(-0.02, -6.958);
  tDiffFineCorr[22]->AddPoint(0.12, -3.299);
  tDiffFineCorr[22]->AddPoint(0.35,  1.145);
  tDiffFineCorr[22]->AddPoint(0.54,  1.537);
  tDiffFineCorr[22]->AddPoint(0.71, -0.424);
  tDiffFineCorr[22]->AddPoint(0.93, -5.063);
  tDiffFineCorr[22]->AddPoint(1.22, -12.316);
  tDiffFineCorr[22]->AddPoint(1.41, -16.825);

  tDiffFineCorr[23]->AddPoint(-0.22, -13.166);
  tDiffFineCorr[23]->AddPoint(0.14, -4.410);
  tDiffFineCorr[23]->AddPoint(0.39, -0.228);
  tDiffFineCorr[23]->AddPoint(0.53, -0.031);
  tDiffFineCorr[23]->AddPoint(0.68, -0.750);
  tDiffFineCorr[23]->AddPoint(0.85, -3.691);
  tDiffFineCorr[23]->AddPoint(0.98, -7.023);
  tDiffFineCorr[23]->AddPoint(1.35, -16.498);

  //*################################################### tDiffFineC - Ex gate

  for (int i = 0; i < 24; i++) {
    tDiffExCut[i] = new TCutG();
  }

  tDiffExCut[0]->AddPoint(1.87,  5.701);
  tDiffExCut[0]->AddPoint(-2.50,  5.613);
  tDiffExCut[0]->AddPoint(-3.86,  3.702);
  tDiffExCut[0]->AddPoint(2.30,  3.813);
  tDiffExCut[0]->AddPoint(2.31,  4.302);
  tDiffExCut[0]->AddPoint(1.87,  5.701);

  tDiffExCut[1]->AddPoint(2.10,  4.968);
  tDiffExCut[1]->AddPoint(-1.14,  5.724);
  tDiffExCut[1]->AddPoint(-1.53,  4.746);
  tDiffExCut[1]->AddPoint(-0.75,  3.680);
  tDiffExCut[1]->AddPoint(2.20,  4.213);
  tDiffExCut[1]->AddPoint(2.10,  4.968);

  tDiffExCut[3]->AddPoint(-0.37, 16.054);
  tDiffExCut[3]->AddPoint(-3.79, 16.232);
  tDiffExCut[3]->AddPoint(-4.02, 14.566);
  tDiffExCut[3]->AddPoint(-1.38, 14.233);
  tDiffExCut[3]->AddPoint(-0.07, 15.321);
  tDiffExCut[3]->AddPoint(-0.37, 16.054);

  tDiffExCut[4]->AddPoint(0.71, 16.077);
  tDiffExCut[4]->AddPoint(-2.03, 16.210);
  tDiffExCut[4]->AddPoint(-2.25, 15.144);
  tDiffExCut[4]->AddPoint(-0.78, 14.544);
  tDiffExCut[4]->AddPoint(1.84, 15.099);
  tDiffExCut[4]->AddPoint(1.51, 15.543);
  tDiffExCut[4]->AddPoint(0.71, 16.077);

  tDiffExCut[5]->AddPoint(1.14, 16.299);
  tDiffExCut[5]->AddPoint(-2.25, 16.210);
  tDiffExCut[5]->AddPoint(-2.69, 15.432);
  tDiffExCut[5]->AddPoint(-0.58, 14.499);
  tDiffExCut[5]->AddPoint(2.04, 15.344);
  tDiffExCut[5]->AddPoint(1.14, 16.299);


  tDiffExCut[6]->AddPoint(0.94,  5.257);
  tDiffExCut[6]->AddPoint(-1.79,  5.724);
  tDiffExCut[6]->AddPoint(-1.28,  4.213);
  tDiffExCut[6]->AddPoint(0.65,  3.680);
  tDiffExCut[6]->AddPoint(1.22,  4.324);
  tDiffExCut[6]->AddPoint(0.94,  5.257);

  tDiffExCut[9]->AddPoint(1.81, 16.099);
  tDiffExCut[9]->AddPoint(-1.51, 16.121);
  tDiffExCut[9]->AddPoint(-1.74, 14.588);
  tDiffExCut[9]->AddPoint(0.67, 13.833);
  tDiffExCut[9]->AddPoint(2.13, 14.966);
  tDiffExCut[9]->AddPoint(1.81, 16.099);

  tDiffExCut[12]->AddPoint(1.10,  4.968);
  tDiffExCut[12]->AddPoint(-0.02,  5.812);
  tDiffExCut[12]->AddPoint(-1.14,  4.479);
  tDiffExCut[12]->AddPoint(-0.01,  3.613);
  tDiffExCut[12]->AddPoint(1.00,  4.213);
  tDiffExCut[12]->AddPoint(1.10,  4.968);

  tDiffExCut[13]->AddPoint(1.27,  5.013);
  tDiffExCut[13]->AddPoint(-1.01,  5.102);
  tDiffExCut[13]->AddPoint(-1.15,  4.235);
  tDiffExCut[13]->AddPoint(0.65,  3.746);
  tDiffExCut[13]->AddPoint(1.44,  4.391);
  tDiffExCut[13]->AddPoint(1.27,  5.013);

  tDiffExCut[15]->AddPoint(1.61, 15.877);
  tDiffExCut[15]->AddPoint(-0.80, 16.499);
  tDiffExCut[15]->AddPoint(-1.56, 15.499);
  tDiffExCut[15]->AddPoint(-0.53, 14.433);
  tDiffExCut[15]->AddPoint(1.78, 15.366);
  tDiffExCut[15]->AddPoint(1.61, 15.877);

  tDiffExCut[16]->AddPoint(2.72, 15.921);
  tDiffExCut[16]->AddPoint(-1.45, 16.854);
  tDiffExCut[16]->AddPoint(-3.80, 15.743);
  tDiffExCut[16]->AddPoint(-2.56, 14.610);
  tDiffExCut[16]->AddPoint(2.99, 14.633);
  tDiffExCut[16]->AddPoint(2.72, 15.921);

  tDiffExCut[17]->AddPoint(1.84, 15.988);
  tDiffExCut[17]->AddPoint(-2.29, 16.232);
  tDiffExCut[17]->AddPoint(-2.48, 14.566);
  tDiffExCut[17]->AddPoint(1.47, 14.366);
  tDiffExCut[17]->AddPoint(2.26, 15.410);
  tDiffExCut[17]->AddPoint(1.84, 15.988);

  tDiffExCut[18]->AddPoint(1.81,  5.057);
  tDiffExCut[18]->AddPoint(-2.14,  5.235);
  tDiffExCut[18]->AddPoint(-2.24,  4.168);
  tDiffExCut[18]->AddPoint(-0.31,  3.835);
  tDiffExCut[18]->AddPoint(2.27,  4.080);
  tDiffExCut[18]->AddPoint(1.81,  5.057);

  tDiffExCut[19]->AddPoint(2.76,  5.168);
  tDiffExCut[19]->AddPoint(-0.07,  5.901);
  tDiffExCut[19]->AddPoint(-2.43,  4.591);
  tDiffExCut[19]->AddPoint(-0.29,  3.591);
  tDiffExCut[19]->AddPoint(3.02,  4.124);
  tDiffExCut[19]->AddPoint(2.76,  5.168);

  tDiffExCut[21]->AddPoint(2.30, 16.277);
  tDiffExCut[21]->AddPoint(-3.40, 16.232);
  tDiffExCut[21]->AddPoint(-3.23, 15.144);
  tDiffExCut[21]->AddPoint(-0.44, 14.433);
  tDiffExCut[21]->AddPoint(2.44, 15.499);
  tDiffExCut[21]->AddPoint(2.30, 16.277);

  tDiffExCut[22]->AddPoint(2.35, 16.077);
  tDiffExCut[22]->AddPoint(-2.44, 16.721);
  tDiffExCut[22]->AddPoint(-4.01, 15.277);
  tDiffExCut[22]->AddPoint(-3.07, 14.477);
  tDiffExCut[22]->AddPoint(2.61, 14.766);
  tDiffExCut[22]->AddPoint(2.35, 16.077);

  tDiffExCut[23]->AddPoint(2.01, 16.077);
  tDiffExCut[23]->AddPoint(-2.97, 16.188);
  tDiffExCut[23]->AddPoint(-2.99, 14.566);
  tDiffExCut[23]->AddPoint(2.20, 14.588);
  tDiffExCut[23]->AddPoint(2.01, 16.077);

  //*###################################################
  printf("===================================================== Create histograms...\n");

  double ExRange[2] = {3, 20};
  double resol = 0.05;
  int ExBin = (ExRange[1] - ExRange[0]) / resol;

  // TH2F * heCal_xCal[24];
  // TH1F * heCal[24];
  // TH1F * heCalg[24];
  TH1F *hEx_id[24];
  TH2F *hTrace_ee[24];
  TH1F *htCorr[24];
  for (int i = 0; i < numDet; i++) {
    //   heCal_xCal[i] = new TH2F(Form("heCal_xCal%02d", i), Form("det-%02d;xCal;eCal", i), 400, -0.5, 1.5, 400, 0, 13);
    //   heCal[i]  = new TH1F(Form("heCal%02d", i), Form("det-%02d;eCal", i), 400, 0, 10);
    //   heCalg[i] = new TH1F(Form("heCalg%02d", i), Form("det-%02d (gate);eCal; count / 10 keV ", i), 600, 0, 10);
    hEx_id[i] = new TH1F(Form("hEx_ID%02d", i), Form("det-%02d (gate);Ex; count / %.0f keV ", i, resol * 1000), ExBin, ExRange[0], ExRange[1]);
    hdudu[i] = new TH2F(Form("hdudu%02d", i), "tDiffFine vs x; x ; tDiffFine", 60, -0.5, 1.5, 130, -20, 30);
    htCorr[i] = new TH1F(Form("htCorr%02d", i), Form("det-%02d (gate);tDiffFine [1 = 10 ns];", i), 50, -2, 3);
    hEx_tCorr[i] = new TH2F(Form("hEx_tCorr%02d", i), Form("det-%02d (gate);tDiffFine [1 = 10 ns];Ex", i), 80, -5, 5, ExBin, ExRange[0], ExRange[1]);

    hTrace_ee[i] = new TH2F(Form("hTrace_ee%02d", i), "te vs e; e ; te", 400, -1, 12, 400, -1, 12);
  }
  TH1F *hArrayMulti = new TH1F("hArrayMulti", "Array Multi", 5, 0, 5);

  TH2F *hez = new TH2F("hez", "e-z; z [mm]; e [MeV]", 400, zRange[0], zRange[1], 400, 0, 14);
  TH2F *hezg = new TH2F("hezg", "e-z (gated); z [mm]; e [MeV]", 400, zRange[0], zRange[1], 400, 0, 14);

  TH2F *hrdt2D[4];
  TH2F *hrdt2Ds[4];
  TH1F *hdE[4];
  TH1F *hdEs[4];
  for (int i = 0; i < 4; i++) {
    hrdt2D[i] = new TH2F(Form("hrdt2D%d", i), Form("RDT"), 400, 0, 6000, 400, 0, 6000);
    hrdt2Ds[i] = new TH2F(Form("hrdt2Ds%d", i), Form("RDT"), 400, 0, 6000, 400, 0, 6000);
    hdE[i] = new TH1F(Form("hdE%d", i), Form("dE"), 400, 0, 6000);
    hdEs[i] = new TH1F(Form("hdEs%d", i), Form("dE"), 400, 0, 6000);
  }
  TH1F *hRDTMulti = new TH1F("hRDTMulti", "RDT Multi", 5, 0, 5);

  TH2F *hArrayRDT = new TH2F("hArrayRDT", "Array - RDT; array; RDT", 24, 0, 24, 4, 0, 4);

  TH1F *hEx = new TH1F("hEx", Form("Ex; Ex [MeV]; count / %.0f keV", resol * 1000), ExBin, ExRange[0], ExRange[1]);
  TH1F *hExg = new TH1F("hExg", Form("Ex (gate); Ex [MeV]; count / %.0f keV", resol * 1000), ExBin, ExRange[0], ExRange[1]);
  hExg->SetLineColor(kBlue);

  // TH2F *hEx_EE = new TH2F("hEx_EE", ";Ex ; EE", ExBin, ExRange[0], ExRange[1], 400, 0, 6000);
  // TH2F *hEx_dE = new TH2F("hEx_dE", ";Ex ; dE", ExBin, ExRange[0], ExRange[1], 400, 0, 5000);

  int tDiffRange[2] = {-50, 50};
  int tDiffBin = (tDiffRange[1] - tDiffRange[0]) / 0.1;
  TH1F *hTDiff = new TH1F("hTDiff", "array_t - dE_t", tDiffBin, tDiffRange[0], tDiffRange[1]);
  TH1F *hTDiffg = new TH1F("hTDiffg", "array_t - dE_t (gated)", tDiffBin, tDiffRange[0], tDiffRange[1]);

  TH2F *hEx_tDiff = new TH2F("hEx_tDiff", ";tDiff;Ex", tDiffBin, tDiffRange[0], tDiffRange[1], ExBin, ExRange[0], ExRange[1]);
  TH2F *hRDT_tDiff = new TH2F("hRDT_tDiff", ";tDiff;rdt ID", tDiffBin, tDiffRange[0], tDiffRange[1], 4, 0, 4);

  TH2F * hExThetaCM = new TH2F("hExThetaCM", "Ex-Theta; Ex; ThetaCM", ExBin, ExRange[0], ExRange[1], 100, 0, 50);

  TH2F *hTDiff_z[4];
  TH2F *hTDiff_zg[4];
  TH1F *hExRDT[4];
  for (int i = 0; i < 4; i++) {
    hTDiff_z[i] = new TH2F(Form("hTDiff_z%d", i), Form("TDiff vs z (%d); z; TDiff", i), 150, zRange[0], zRange[1], 150, -20, 30);
    hTDiff_zg[i] = new TH2F(Form("hTDiff_zg%d", i), Form("TDiff vs z (%d) (gated); z; TDiff", i), 150, zRange[0], zRange[1], 150, -20, 30);

    hExRDT[i] = new TH1F(Form("hExRDT%d", i), Form("Ex (%d)", i), ExBin, ExRange[0], ExRange[1]);
  }

  //*###################################################
  printf("===================================================== Processing....\n");

  std::vector<Hit> hitList;

  while (reader.Next()) {
    hitList.clear();
    Hit hit;

    /// ##################################################################### Array
    for (int i = 0; i < numDet; i++) {
      //@================ gate
      if (TMath::IsNaN(e[i])) continue;
      if (TMath::IsNaN(xn[i]) && TMath::IsNaN(xf[i])) continue;
      if (i == 10 || i == 11) continue;

      hit.e = e[i] / eCorr[i][0] + eCorr[i][1];
      if (i == 2) hit.e = (xf[i] + xn[i]) / eCorr[i][0] + eCorr[i][1];

      //@================ gate
      if (hit.e < 0.8) continue;

      double xfcal = xf[i] * xfxneCorr[i][1] + xfxneCorr[i][0] / 2.;
      double xncal = xn[i] * xnCorr[i] * xfxneCorr[i][1] + xfxneCorr[i][0] / 2.;

      // correction for
      if (30 >= *runID && *runID >= 24 && i == 8) {
        if (*runID == 24 && (1.4e5 > *evID)) continue;
        if (*runID == 25 && (1.95e5 > *evID && *evID > 1.8e5)) continue;

        xfcal = xfxn2e(xf[i]);
        xncal = xfxn2e(xn[i] * xnCorr[i]);
      }

      if (i == 2) e[i] = xfcal + xncal;

      if (!TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i])) hit.x = 0.5 + 0.5 * (xfcal - xncal) / e[i];
      if (!TMath::IsNaN(xf[i]) && TMath::IsNaN(xn[i])) hit.x = xfcal / e[i];
      if (TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i])) hit.x = 1.0 - xncal / e[i];

      hit.x = (hit.x - 0.5) / xScale[i] + 0.5;

      hit.z = detGeo.detLength * (hit.x - 1.0) + detGeo.detPos[i % numCol];
      hit.detID = i;
      hit.e_t = e_t[i];
      hit.te_t = te_t[i];
      hit.te_r = te_r[i];
      hit.te = te[i] / eCorr[i][0] + eCorr[i][1];

      hitList.push_back(hit);
    }

    // @================ gate
    if (hitList.size() == 0) continue;

    hArrayMulti->Fill(hitList.size());

    /// ##################################################################### RDT
    if (12 <= *runID && *runID <= 18) {
      // dE scale
      rdt[1] = rdt[1] * 4.9;
      rdt[3] = rdt[3] * 5.1;
      rdt[5] = rdt[5] * 4.9;
      rdt[7] = rdt[7] * 4.9;
    }
    // if no dE hit but E hit less then 2000, due to dE is very small and below threshold
    if (rdt[0] < 1700 && TMath::IsNaN(rdt[1])) rdt[1] = 120;  //+ gRandom->Gaus(0, 20);
    if (rdt[2] < 1300 && TMath::IsNaN(rdt[3])) rdt[3] = 200;  //+ gRandom->Gaus(0, 20);
    if (rdt[4] < 1300 && TMath::IsNaN(rdt[5])) rdt[5] = 300;  //+ gRandom->Gaus(0, 20);
    if (rdt[6] < 1300 && TMath::IsNaN(rdt[7])) rdt[7] = 400;  //+ gRandom->Gaus(0, 20);

    for (int i = 0; i < 8; i++) {
      if (TMath::IsNaN(rdt[i])) continue;

      for (size_t k = 0; k < hitList.size(); k++) {
        if (i % 2 == 0) {
          hitList[k].EE[i / 2] = rdt[i];
          hitList[k].EE_t[i / 2] = rdt_t[i];
          hitList[k].tEE_t[i / 2] = trdt_t[i];
          hitList[k].tEE_r[i / 2] = trdt_r[i];
        }
        if (i % 2 == 1) {
          hitList[k].dE[i / 2] = rdt[i];
          hitList[k].dE_t[i / 2] = rdt_t[i];
          hitList[k].tdE_t[i / 2] = trdt_t[i];
          hitList[k].tdE_r[i / 2] = trdt_r[i];
        }
      }
    }

    /// ##################################################################### Array & RDT
    for (size_t k = 0; k < hitList.size(); k++) {

      // @============ remove detID-2
      // if( hitList[k].detID == 2 ) continue;

      //&============= deduce the RDt multipicity
      hitList[k].CalRDTMultiplicity();

      //&============= Calculate Ex and ThetaCM, also tDiffFine
      CalExTheta(hitList[k]);

      //&============= tDiffFine correction
      if (tDiffFineCorr[hitList[k].detID]->GetN() > 1) {
        hitList[k].tDiffFineC = hitList[k].tDiffFine - tDiffFineCorr[hitList[k].detID]->Eval(hitList[k].x);
      }

      int sizeID = hitList[k].detID / 6;
      hTDiff_z[sizeID]->Fill(hitList[k].z, hitList[k].tDiffFine);

      // hdudu[hitList[k].detID]->Fill(hitList[k].x, hitList[k].tDiffFine);  

      hTDiff_zg[sizeID]->Fill(hitList[k].z, hitList[k].tDiffFineC);

      //&============= looping recoil
      for (int i = 0; i < 4; i++) {
        //@================== gate, rdt match array
        if (((hitList[k].detID / 6) + 1) % 4 != i) continue;

        hrdt2D[i]->Fill(hitList[k].EE[i], hitList[k].dE[i]);

        //@================== basic RDT gate
        if (hitList[k].dE[i] > 0 && hitList[k].EE[i] > 0) {
          hdE[i]->Fill(hitList[k].EE[i]);
          hTDiff->Fill(hitList[k].tDiffFine);
          hTrace_ee[hitList[k].detID]->Fill(hitList[k].e, hitList[k].te);
        } else {
          continue;
        }

        // hdudu[hitList[k].detID]->Fill(hitList[k].x, hitList[k].tDiffFine);  
        // hEx->Fill(hitList[k].Ex);
        // hez->Fill(hitList[k].z, hitList[k].e);

        //@================== gate
        if (cut[i]->IsInside(hitList[k].EE[i], hitList[k].dE[i])) {
          // hEx_id[hitList[k].detID]->Fill(hitList[k].Ex);

          hArrayRDT->Fill(hitList[k].detID, i);

          hTDiffg->Fill(hitList[k].tDiffFineC);
          hEx_tDiff->Fill(hitList[k].tDiffFineC, hitList[k].Ex);
          hRDT_tDiff->Fill(hitList[k].tDiffFineC, i);


          if (hitList[k].detID % 6 <= 1 && abs(hitList[k].Ex - 4.5) < 0.3) {
            hdudu[hitList[k].detID]->Fill(hitList[k].x, hitList[k].tDiffFine);  
          }

          if (hitList[k].detID % 6 >= 3 && abs(hitList[k].Ex - 15.3) < 0.3) {
            hdudu[hitList[k].detID]->Fill(hitList[k].x, hitList[k].tDiffFine);
          }
          hEx->Fill(hitList[k].Ex);
          hez->Fill(hitList[k].z, hitList[k].e);

          hEx_tCorr[hitList[k].detID]->Fill(hitList[k].tDiffFineC, hitList[k].Ex);
          htCorr[hitList[k].detID]->Fill(hitList[k].tDiffFineC);

          // hTDiff_zg[sizeID]->Fill(hitList[k].z, hitList[k].tDiffFineC);

          // // //@=============== gate, rought time gate
          // if (-15 < hitList[k].tDiff && hitList[k].tDiff < 20) {
          //   hezg->Fill(hitList[k].z, hitList[k].e);
          //   hExg->Fill(hitList[k].Ex);
          //   hEx_tCorr[hitList[k].detID]->Fill(hitList[k].tDiffFineC, hitList[k].Ex);
          // }

          // //@=============== gate, trace time gate
          // if (!(-4 < hitList[k].tDiffFineC && hitList[k].tDiffFineC < 2)) {
          //   hezg->Fill(hitList[k].z, hitList[k].e);
          //   hExg->Fill(hitList[k].Ex);
          //   hEx_tCorr[hitList[k].detID]->Fill(hitList[k].tDiffFineC, hitList[k].Ex);
          // }

          //@=============== gate, trace time - Ex gate
          if ( (tDiffExCut[hitList[k].detID]->GetN() > 1) && tDiffExCut[hitList[k].detID]->IsInside(hitList[k].tDiffFineC, hitList[k].Ex)  ) {
            hezg->Fill(hitList[k].z, hitList[k].e);
            hExg->Fill(hitList[k].Ex);
            
            hExThetaCM->Fill( hitList[k].Ex, hitList[k].thetaCM);

            hEx_id[hitList[k].detID]->Fill(hitList[k].Ex);
            // hEx_tCorr[hitList[k].detID]->Fill(hitList[k].tDiffFineC, hitList[k].Ex);
          }

        }
      }

      hRDTMulti->Fill(hitList[k].nRDT);

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

  //*################################################### Draw hist

  int padSize = 400;

  // TCanvas *canvas = new TCanvas("canvas", "canvas", numCol * padSize, numSide * padSize);
  // canvas->Divide(numCol, numSide);

  // for (int i = 0; i < numDet; i++) {
  //   canvas->cd(i + 1);
  //   // heCal_xCal[i]->Draw("colz");

  //   // heCal[i]->Draw("");
  //   // heCalg[i]->SetLineColor(kRed);
  //   // heCalg[i]->Draw("same");
  //   hEx_id[i]->Draw("");
  //   // hTrace_ee[i]->Draw("colz box");
  //   // hdudu[i]->Draw("colz");

  //   // if( tDiffFineCorr[i]->GetN() > 1 ){
  //   //   tDiffFineCorr[i]->Draw("same");
  //   // }
  // }

  // TCanvas *canvasA = new TCanvas("canvasA", "canvasA", numCol * padSize, numSide * padSize);
  // canvasA->Divide(numCol, numSide);

  // for (int i = 0; i < numDet; i++) {
  //   canvasA->cd(i + 1);
  //   // htCorr[i]->Draw("");
  //   hEx_tCorr[i]->Draw("colz box");

  //   if( tDiffExCut[i]->GetN() > 1 ){
  //     tDiffExCut[i]->Draw("same");
  //   }
  // }

  TCanvas *canvasB = new TCanvas("canvasB", "canvasB", numCol * padSize, padSize);
  canvasB->Divide(numCol, 1);

  for (int i = 0; i < numDet; i++) {
    canvasB->cd( i % 6 + 1) ;
    // htCorr[i]->Draw("");
    if( i / 6 == 0 ) { 

      double yMax = 0;
      for( int j = 0; j < 4; j++ ){
        double haha = hEx_id[i + 6*j]->GetMaximum();
        if( haha > yMax ) yMax = haha;
      }
      hEx_id[i]->SetMaximum(yMax*1.5);

      if( i < 2 ) hEx_id[i]->GetXaxis()->SetRangeUser(3.8, 5.0);
      if( i > 2 ) hEx_id[i]->GetXaxis()->SetRangeUser(14.7, 15.65);

      hEx_id[i]->Draw(""); 
    }else{
      if( i/6 == 1 ) hEx_id[i]->SetLineColor(kRed);
      if( i/6 == 2 ) hEx_id[i]->SetLineColor(kBlue);
      if( i/6 == 3 ) hEx_id[i]->SetLineColor(kGreen + 2);
      hEx_id[i]->Draw("same"); 
    }

  }

  TCanvas *canvas1 = new TCanvas("canvas1", "canvas1", 500, 0, 1000, 500);
  // hArrayMulti->Draw();
  // hRDTMulti->Draw();
  hEx->Draw();
  hExg->Draw("same");
  // hExg->Draw("");

  canvas2 = new TCanvas("canvas2", "canvas2", 500, 600, 2000, 1000);
  canvas2->Divide(2, 1);
  canvas2->cd(1);
  // hez->Draw("colz");
  hExThetaCM->Draw("colz box");
  // PlotKELines();
  canvas2->cd(2);
  hezg->Draw("colz box");
  PlotKELines();

  // TCanvas * canvas3 = new TCanvas("canvas3", "canvas3", 1600, 1600);
  // canvas3->Divide(2,2);
  // canvas3->cd(1); hdE[0]->Draw("hist"); hdEs[0]->SetLineColor(kRed); hdEs[0]->Draw("same");
  // canvas3->cd(2); hdE[1]->Draw("hist"); hdEs[1]->SetLineColor(kRed); hdEs[1]->Draw("same");
  // canvas3->cd(3); hdE[3]->Draw("hist"); hdEs[3]->SetLineColor(kRed); hdEs[3]->Draw("same");
  // canvas3->cd(4); hdE[2]->Draw("hist"); hdEs[2]->SetLineColor(kRed); hdEs[2]->Draw("same");

  // TCanvas *canvas3 = new TCanvas("canvas3", "canvas3", 1600, 1600);
  // canvas3->Divide(2, 2);
  // canvas3->cd(1); hrdt2Ds[0]->Draw("colz box"); cut[0]->Draw("same");
  // canvas3->cd(2); hrdt2Ds[1]->Draw("colz box"); cut[1]->Draw("same");
  // canvas3->cd(3); hrdt2Ds[3]->Draw("colz box"); cut[3]->Draw("same");
  // canvas3->cd(4); hrdt2Ds[2]->Draw("colz box"); cut[2]->Draw("same");

  TCanvas *canvas4 = new TCanvas("canvas4", "canvas4", 1600, 1600);
  canvas4->Divide(2, 2);
  canvas4->cd(1); hrdt2D[0]->Draw("colz"); cut[0]->Draw("same");
  canvas4->cd(2); hrdt2D[1]->Draw("colz"); cut[1]->Draw("same");
  canvas4->cd(3); hrdt2D[3]->Draw("colz"); cut[3]->Draw("same");
  canvas4->cd(4); hrdt2D[2]->Draw("colz"); cut[2]->Draw("same");

  // TCanvas * canvas5 = new TCanvas("canvas5", "canvas5", 100, 0, 1200, 600);
  // canvas5->Divide(2,1);
  // canvas5->cd(1); hEx_EE->Draw("colz box");
  // canvas5->cd(2); hEx_dE->Draw("colz box");

  // TCanvas *canvas6 = new TCanvas("canvas6", "canvas6", 100, 0, 1200, 600);
  // hArrayMulti->Draw();
  // hArrayRDT->Draw("colz");
  // hTDiff->Draw();
  // hTDiffg->SetLineColor(kRed);
  // hTDiffg->Draw("same");

  // TCanvas * canvas7 = new TCanvas("canvas7", "canvas7", 2000, 0, 600, 600);
  // hEx_tDiff->Draw("colz");
  // hRDT_tDiff->Draw("colz");

//   TCanvas *canvas8 = new TCanvas("canvas8", "canvas8", 2000, 0, 1200, 1200);
//   canvas8->Divide(2, 2);
//   for (int i = 0; i < 4; i++) {
//     canvas8->cd(i + 1);
//     hTDiff_z[i]->Draw("colz");
//   }

//   TCanvas *canvas9 = new TCanvas("canvas9", "canvas9", 3200, 0, 1200, 1200);
//   canvas9->Divide(2, 2);
//   for (int i = 0; i < 4; i++) {
//     canvas9->cd(i + 1);
//     hTDiff_zg[i]->Draw("colz");
//   }
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

  hit.Ex = Ex - 0.16;
  hit.thetaCM = thetaCM;

  // 12C Ex correction
  if( hit.detID ==  4 ) hit.Ex -= 0.05;
  if( hit.detID ==  5 ) hit.Ex -= 0.05;
  if( hit.detID == 15 ) hit.Ex -= 0.05;
  if( hit.detID == 16 ) hit.Ex += 0.05;
  if( hit.detID == 17 ) hit.Ex += 0.10;
  if( hit.detID == 19 ) hit.Ex += 0.15;
  if( hit.detID == 22 ) hit.Ex += 0.15;
  if( hit.detID == 23 ) hit.Ex -= 0.05;


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

void Check(int id) {
  canvas2->cd(2);
  if( !canvas2->GetShowEventStatus() ) canvas2->ToggleEventStatus();
  if( !canvas2->GetShowToolBar() ) canvas2->ToggleToolBar();

  hdudu[id]->Draw("");

  if( tDiffFineCorr[id]->GetN() > 1 ){
    tDiffFineCorr[id]->Draw("same");
  }

  // hdudu[id]->Fit("pol2");

  // printf("========================================== \n");

  // for (float kk = 0; kk <= 1.05; kk += 0.1) {
  //   printf("  tDiffFineCorr[%d]->AddPoint(%4.2f, %6.3f);\n", id, kk, hdudu[id]->GetFunction("pol2")->Eval(kk));
  // }
}

void DrawGraph(int id){

  Check(id);

  canvas2->Modified();
  canvas2->Update();

  gPad->WaitPrimitive();

  TGraph * graph = (TGraph *) gROOT->FindObject("Graph");

  if( graph == nullptr ) return;

  for( int i = 0; i < graph->GetN(); i++){
    double x = graph->GetPointX(i);
    double y = graph->GetPointY(i);
    printf("  tDiffFineCorr[%d]->AddPoint(%4.2f, %6.3f);\n", id, x, y);
  }

}

void DrawCut(int id){

  canvas2->cd(2);
  if( !canvas2->GetShowEventStatus() ) canvas2->ToggleEventStatus();
  if( !canvas2->GetShowToolBar() ) canvas2->ToggleToolBar();

  hEx_tCorr[id]->Draw("");

  canvas2->Modified();
  canvas2->Update();

  gPad->WaitPrimitive();

  TGraph * cutG = (TGraph *) gROOT->FindObject("CUTG");

  if( cutG == nullptr ) return;

  for( int i = 0; i < cutG->GetN(); i++){
    double x = cutG->GetPointX(i);
    double y = cutG->GetPointY(i);
    printf("  tDiffExCut[%d]->AddPoint(%4.2f, %6.3f);\n", id, x, y);
  }


}