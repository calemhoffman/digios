/***********************************************************************
 *
 *  MonitorsBZero.C  --  online monitor for B = 0 T, h098 20Ne(a,p)23Na
 *
 *  usage : root -l 'ChainMonitorsBZero.C(RUN)'     or
 *          ../Armory/Process_RUN RUN ... -BZ
 *
 *  detectors (see MonitorsBZero.h)
 *    stub (det 21) : PSD 50 x 10 mm, long axis along X (-25 ~ +25 mm), at Y = -150 mm, Z = +25 mm
 *    ELUM          : S1, 1000 um, 16 rings r = 24-48 mm, z = -50 mm
 *    RDT           : at z = +1000 mm
 *
 *  Ex of 23Na from ELUM : missing mass with (T_p, theta_lab of ring)
 *     P_B = P_beam + P_target - P_p ,  Ex = sqrt(P_B^2) - m(23Na)
 *
 *  after running, at the ROOT prompt : listDrawsBZ()
 *
***********************************************************************/
#ifndef MonitorsBZero_cxx
#define MonitorsBZero_cxx

#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TMath.h>
#include <TString.h>
#include <TLatex.h>
#include <TSystem.h>
#include <TLine.h>
#include <TStopwatch.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <vector>
#include "../Cleopatra/Isotope.h"
#include "GeneralSortMapping.h"

using namespace std;

//############################################ User setting

//---- Reaction : beam(target, light)heavy, inverse kinematics
int    beamA = 20, beamZ = 10;     /// 20Ne
int    targA =  4, targZ =  2;     /// 4He
int   lightA =  1, lightZ = 1;     /// p
int   heavyA = 23, heavyZ = 11;    /// 23Na
double beamEnergyPerU = 6.0;       /// MeV/u
vector<double> exLines = {0.0, 0.440, 2.076, 2.391};  /// Ex [MeV] for the kinematic lines

//---- Stub (det 21) geometry
const int stubID      = 21;
/// stub long axis is along X ; position along the strip -> X, theta = atan2( sqrt(X^2 + Y^2), Z ) ~ 80.5 - 80.7 deg
double stubY          = 150.0;  /// distance from beam axis in Y [mm] (stub at -Y)
double stubZ          =  25.0;  /// Z of the stub plane [mm]
double stubXCenterPos =   0.0;  /// X of the stub center [mm]
double stubLength     =  50.0;  /// along X [mm]
bool   isStubXFPositiveX = true; /// x = 1 (xf side) at +X

//---- ELUM geometry
double elumZ          = -50.0;  /// [mm]
double elumRin        =  24.0;  /// [mm]
double elumRout       =  48.0;  /// [mm]
bool   isElumRing0Inner = true; /// elum[0] is the innermost ring

//---- RDT geometry (info only)
double rdtZ           = 1000.0; /// [mm]

//---- Calibrations
/// alpha sources run 2 : 148Gd 3.183, 228Th 6.288, 6.778, 8.785 MeV
/// E [MeV] = a0 + a1 * ch . NOTE: residual of linear fit ~ +-0.1 MeV (ELUM is slightly non-linear)
double elumCal[16][2] = {
   {0.95851, 0.010756}, // 0
   {0.83502, 0.013989}, // 1
   {0.74435, 0.015034}, // 2
   {0.81024, 0.013649}, // 3
   {0.71285, 0.016669}, // 4
   {0.84813, 0.013170}, // 5
   {0.95078, 0.010525}, // 6
   {0.82415, 0.013655}, // 7
   {0.76935, 0.014159}, // 8
   {0.79429, 0.014711}, // 9
   {0.80700, 0.013753}, // 10
   {0.81650, 0.013804}, // 11
   {0.71932, 0.015741}, // 12
   {0.83791, 0.013044}, // 13
   {0.75955, 0.014721}, // 14
   {0.77956, 0.015440}  // 15
};

double stubECal[2]    = {0.32092, 0.001637}; /// E [MeV] = a0 + a1 * e, run 2 alpha (residual < 50 keV)
/// run 2 alpha, joint LSQ e = a0 + a1*(xf + xnCorr*xn), rms 12.5 ch; per-line xn-xf slopes give xnCorr 0.954-0.962
double stubXnCorr     = 0.95264; /// xn' = xn * stubXnCorr  (gain match xn to xf)
double stubXfxnE[2]   = {83.845, 1.04781}; /// xf' = a1*xf + a0/2, xn' = a1*xnCorr*xn + a0/2  -> xf' + xn' = e
double stubXCenter    = 0.544;   /// x' = (x - center)/scale + 0.5 -> detector edges at x' = 0, 1
double stubXScale     = 0.710;   /// run 2 alpha, half-max edges of gain-matched x at 0.189 and 0.899

//---- histogram ranges
int    rawERange[2]   = {   0, 6000};   /// stub e, xf, xn [ch]
int    elumRawRange[2]= {   0, 1000};   /// [ch]
double eCalRange[2]   = {   0,   15};   /// [MeV]
int    rdtERange[2]   = {   0, 8000};
int    rdtDERange[2]  = {   0, 4000};
double exRange[3]     = {  50,   -2, 12}; /// bin [keV], low [MeV], high [MeV]
int    coinTimeRange[2] = { -300, 300};   /// [ch = 10 ns]

//---- Gates
double elumThreshold  = 1.0;     /// [MeV], below -> ignored (cal offset ~0.8 MeV at ch 0)
double stubThreshold  = 1.0;     /// [MeV] (cal offset 0.32 MeV at ch 0)
double xGate          = 1.0;     /// |x - 0.5| < xGate/2 for stub
bool   isTimeGateOn   = true;
int    timeGate[2]    = { -50, 50}; /// rdt_t - elum_t, [ch = 10 ns]
int    stubTimeGate[2]= { -50, 50}; /// rdt_t - e_t[stub], [ch = 10 ns], (a,a): alpha -> stub (-Y), 20Ne -> RDT (+Y)
TString rdtCutFile    = "rdtCuts.root"; /// from ../Armory/RDTCutCreator.C : TObjArray "cutList", cut-i on x=rdt[2i] (E), y=rdt[2i+1] (dE). ignored if not exist

//############################################ end of user setting

ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; ///Progress bar
TStopwatch StpWatch;

TCanvas *cBZ;
TString canvasTitle;
int lastRunID;
bool contFlag;
double runTime = 0;

TObjArray * cutList = NULL;
bool isCutFileOpen = false;
int numCut = 0;

TLatex text;

//====== reaction constants
double mBeam, mTarg, mLight, mHeavy;
double Etot, Ptot, sInv; /// lab total energy, momentum (along +z), invariant mass^2
double Qvalue;
bool isKinematicsOK = false;

double elumTheta[16];    /// lab angle of each ring [deg]
double elumR[16];        /// ring center radius [mm]

/******************************************************************
*  histograms
******************************************************************/
//======== stub
TH1F* hStubE;
TH1F* hStubXF;
TH1F* hStubXN;
TH2F* hStubXFVXN;
TH2F* hStubEVXS;      // raw e vs xf+xn
TH2F* hStubEVx;       // raw e vs raw x
TH1F* hStubECal;
TH2F* hStubECalVX;
TH2F* hStubECalVTheta;

//======== ELUM
TH1F* helum[NELUM];
TH1F* helumCal[NELUM];
TH2F* helumID;
TH2F* helumCalID;
TH2F* helumCalVTheta;
TH2F* helumCalVThetaG;
TH1I* helumMulti;
TH1F* helumRate;
TH1F* hExElum;
TH1F* hExElumG;
TH1F* hExElumRing[NELUM];
TH2F* hExElumVTheta;

//======== RDT
TH1F* hrdt[NRDT];
TH2F* hrdtID;
TH2F* hrdt2D[NRDT/2];
TH2F* hrdt2Dg[NRDT/2];
TH2F* hrdtMatrix;

//======== coincidence
TH1I* htdiffElum;    // rdt_t - elum_t
TH1I* htdiffStub;    // rdt_t - e_t[stub]
TH2F* htdiffStubVID; // vs RDT channel
TH2F* hStubECalVXG;  // stub eCal vs X, RDT coincident
TH2F* hrdt2DgStub[NRDT/2]; // RDT dE-E, stub coincident
TH2F* htdiffElumVID; // vs ring

/******************************************************************
*  kinematics
******************************************************************/
/// Ex of heavy recoil from light particle kinetic energy T [MeV] and lab angle theta [deg]
double CalEx(double T, double thetaDeg){
   if( !isKinematicsOK || TMath::IsNaN(T) || T <= 0 ) return TMath::QuietNaN();
   double E3 = T + mLight;
   double p3 = TMath::Sqrt(T*T + 2*T*mLight);
   double cs = TMath::Cos(thetaDeg*TMath::DegToRad());
   double m4sq = sInv + mLight*mLight - 2*Etot*E3 + 2*Ptot*p3*cs;
   if( m4sq < 0 ) return TMath::QuietNaN();
   return TMath::Sqrt(m4sq) - mHeavy;
}

/// kinetic energy of the light particle at theta [deg] for Ex; sol = 0 (higher p), 1 (lower p). NaN if not allowed
double CalT(double thetaDeg, double ex, int sol = 0){
   if( !isKinematicsOK ) return TMath::QuietNaN();
   double m4 = mHeavy + ex;
   double c = TMath::Cos(thetaDeg*TMath::DegToRad());
   double K = (sInv + mLight*mLight - m4*m4)/2.;
   double A = Etot*Etot - Ptot*Ptot*c*c;
   double D = K*K - mLight*mLight*A;
   if( D < 0 ) return TMath::QuietNaN();
   double p3 = ( K*Ptot*c + (sol == 0 ? 1 : -1) * Etot * TMath::Sqrt(D) ) / A;
   if( p3 <= 0 ) return TMath::QuietNaN();
   double E3 = TMath::Sqrt(p3*p3 + mLight*mLight);
   if( TMath::Abs(Etot*E3 - Ptot*c*p3 - K) > 1e-6*TMath::Abs(K) ) return TMath::QuietNaN(); /// spurious root from squaring
   return E3 - mLight;
}

TGraph * KinematicLine(double ex, double th1, double th2, int color){
   TGraph * g = new TGraph();
   for( int sol = 0; sol < 2; sol++){
      for( int k = 0; k <= 200; k++){
         double th = th1 + (th2-th1)*k/200.;
         double T = CalT(th, ex, sol);
         if( !TMath::IsNaN(T) ) g->SetPoint(g->GetN(), th, T);
      }
   }
   g->SetLineColor(color); g->SetMarkerColor(color); g->SetMarkerStyle(7);
   return g;
}

/// Isotope uses $HELIOSSYS/analysis/Cleopatra/mass20.txt; fall back to ../Cleopatra if HELIOSSYS is wrong
Isotope GetIsotope(int A, int Z){
   Isotope iso(A, Z);
   if( !(iso.Mass > 1) ){
      iso.dataSource = "../Cleopatra/mass20.txt";
      iso.SetIso(A, Z);
      printf("   -> retry mass table %s : %s mass = %.3f MeV\n", iso.dataSource.c_str(), iso.Name.c_str(), iso.Mass);
   }
   return iso;
}

#include "MonitorsBZero.h"

/***************************************************************
 * Begin
 ***************************************************************/
void MonitorsBZero::Begin(TTree *tree){

   NumEntries = tree->GetEntries();
   canvasTitle.Form("Runs: ");
   lastRunID = -1;
   contFlag = false;

   printf("####################################################\n");
   printf("##########        MonitorsBZero.C          #########\n");
   printf("####################################################\n");

   //================ reaction
   Isotope iBeam  = GetIsotope(beamA, beamZ);
   Isotope iTarg  = GetIsotope(targA, targZ);
   Isotope iLight = GetIsotope(lightA, lightZ);
   Isotope iHeavy = GetIsotope(heavyA, heavyZ);
   mBeam = iBeam.Mass; mTarg = iTarg.Mass; mLight = iLight.Mass; mHeavy = iHeavy.Mass;
   isKinematicsOK = ( mBeam > 1 && mTarg > 1 && mLight > 1 && mHeavy > 1 );
   printf("   masses [MeV] : beam %.4f, target %.4f, light %.4f, heavy %.4f\n", mBeam, mTarg, mLight, mHeavy);
   double Tbeam = beamEnergyPerU * beamA;
   Etot = Tbeam + mBeam + mTarg;
   Ptot = TMath::Sqrt(Tbeam*Tbeam + 2*Tbeam*mBeam);
   sInv = Etot*Etot - Ptot*Ptot;
   Qvalue = mBeam + mTarg - mLight - mHeavy;
   printf("================ Reaction %s(%s,%s)%s @ %.2f MeV/u = %.1f MeV\n",
           iBeam.Name.c_str(), iTarg.Name.c_str(), iLight.Name.c_str(), iHeavy.Name.c_str(), beamEnergyPerU, Tbeam);
   printf("   Q-value : %.3f MeV,  Ecm : %.3f MeV,  isOK : %d\n", Qvalue, TMath::Sqrt(sInv) - mBeam - mTarg, isKinematicsOK);

   //================ ELUM ring angles
   double dr = (elumRout - elumRin)/NELUM;
   printf("================ ELUM rings (z = %.1f mm)  T_p for Ex = 0 \n", elumZ);
   for( int i = 0; i < NELUM; i++){
      int k = isElumRing0Inner ? i : NELUM - 1 - i;
      elumR[i] = elumRin + dr * (k + 0.5);
      elumTheta[i] = TMath::ATan2(elumR[i], elumZ) * TMath::RadToDeg();
      printf("   ring %2d : r = %5.2f mm, theta = %6.2f deg, T_p(Ex=0) = %6.3f MeV\n", i, elumR[i], elumTheta[i], CalT(elumTheta[i], 0));
   }
   double stubXMaxAbs = TMath::Max(TMath::Abs(stubXCenterPos - stubLength/2.), TMath::Abs(stubXCenterPos + stubLength/2.));
   double stubXMinAbs = ( TMath::Abs(stubXCenterPos) < stubLength/2. ) ? 0 : TMath::Abs(stubXCenterPos) - stubLength/2.;
   double thStub[2] = { TMath::ATan2(TMath::Sqrt(stubXMinAbs*stubXMinAbs + stubY*stubY), stubZ)*TMath::RadToDeg(),
                        TMath::ATan2(TMath::Sqrt(stubXMaxAbs*stubXMaxAbs + stubY*stubY), stubZ)*TMath::RadToDeg() };
   printf("================ stub theta range : %.2f - %.2f deg\n", thStub[0], thStub[1]);

   //================ Recoil cut
   isCutFileOpen = false; numCut = 0; cutList = NULL;
   if( rdtCutFile != "" && !gSystem->AccessPathName(rdtCutFile) ){
      TFile * fCut = new TFile(rdtCutFile);
      isCutFileOpen = fCut->IsOpen();
      if( isCutFileOpen ){
         cutList = (TObjArray *) fCut->FindObjectAny("cutList");
         if( cutList ) numCut = cutList->GetEntries(); else isCutFileOpen = false;
         printf("=========== found %d cutG in %s \n", numCut, rdtCutFile.Data());
         for( int i = 0; i < numCut; i++){
            TCutG * cc = (TCutG*) cutList->At(i);
            printf("   %s : VarX %s, VarY %s, nPoints %d\n", cc->GetName(), cc->GetVarX(), cc->GetVarY(), cc->GetN());
         }
      }else{
         printf("Failed to open rdt-cutfile : %s\n", rdtCutFile.Data());
      }
   }else if( rdtCutFile != "" ){
      printf("=========== no rdt-cutfile %s, no RDT cut. make one with\n", rdtCutFile.Data());
      printf("            RDTCutCreator(\"../root_data/gen_runXXX.root\", \"%s\", %d, %d)\n", rdtCutFile.Data(), rdtERange[1], rdtDERange[1]);
   }

   //================ time range
   bool isTimeOK = FindStartEndTime(tree, "elum_t", NELUM);
   if( !isTimeOK ) isTimeOK = FindStartEndTime(tree, "e_t", NARRAY);
   if( !isTimeOK ) isTimeOK = FindStartEndTime(tree, "rdt_t", NRDT);
   runTime = isTimeOK ? (endTime - startTime)/1e8/60. : 0;
   int timeRange[2] = {0, 1000};
   if( isTimeOK ){
      timeRange[0] = (int) TMath::Floor(startTime/1e8/60.);
      timeRange[1] = (int) TMath::Ceil(endTime/1e8/60.) + 1;
   }
   printf("================ Duration : %.1f min\n", runTime);

   //================ histograms
   gROOT->cd();

   int nEx = (int) ((exRange[2]-exRange[1])/exRange[0]*1000);

   //---- stub
   hStubE      = new TH1F("hStubE",     Form("stub raw e (det %d); e [ch]; count", stubID),  2000, rawERange[0], rawERange[1]);
   hStubXF     = new TH1F("hStubXF",    Form("stub raw xf (det %d); xf [ch]; count", stubID), 500, rawERange[0], rawERange[1]);
   hStubXN     = new TH1F("hStubXN",    Form("stub raw xn (det %d); xn [ch]; count", stubID), 500, rawERange[0], rawERange[1]);
   hStubXFVXN  = new TH2F("hStubXFVXN", "stub raw xf vs xn; xf [ch]; xn [ch]",               500, rawERange[0], rawERange[1], 500, rawERange[0], rawERange[1]);
   hStubEVXS   = new TH2F("hStubEVXS",  "stub raw e vs xf+xn; xf+xn [ch]; e [ch]",           500, rawERange[0], rawERange[1], 500, rawERange[0], rawERange[1]);
   hStubEVx    = new TH2F("hStubEVx",   "stub raw e vs x; x = 0.5(xf-xn)/e+0.5; e [ch]",      500, -0.1, 1.1, 500, rawERange[0], rawERange[1]);
   hStubECal   = new TH1F("hStubECal",  "stub eCal; E [MeV]; count",                          1500, eCalRange[0], eCalRange[1]);
   hStubECalVX = new TH2F("hStubECalVX","stub eCal vs X; X [mm]; E [MeV]",                    400, stubXCenterPos - stubLength/2. - 10, stubXCenterPos + stubLength/2. + 10, 500, eCalRange[0], eCalRange[1]);
   hStubECalVTheta = new TH2F("hStubECalVTheta", "stub eCal vs #theta_{lab}; #theta_{lab} [deg]; E [MeV]", 200, thStub[0]-0.5, thStub[1]+0.5, 500, eCalRange[0], eCalRange[1]);

   //---- ELUM
   for( int i = 0; i < NELUM; i++){
      helum[i]       = new TH1F(Form("helum%d", i),    Form("ELUM-%d raw (#theta=%.1f); [ch]; count", i, elumTheta[i]), 500, elumRawRange[0], elumRawRange[1]);
      helumCal[i]    = new TH1F(Form("helumCal%d", i), Form("ELUM-%d cal (#theta=%.1f); E [MeV]; count", i, elumTheta[i]), 750, eCalRange[0], eCalRange[1]);
      hExElumRing[i] = new TH1F(Form("hExElum%d", i),  Form("Ex ELUM-%d (#theta=%.1f); Ex [MeV]; count / %.0f keV", i, elumTheta[i], exRange[0]), nEx, exRange[1], exRange[2]);
   }
   helumID        = new TH2F("helumID",    "ELUM raw vs ring; ring; [ch]",    NELUM, 0, NELUM, 500, elumRawRange[0], elumRawRange[1]);
   helumCalID     = new TH2F("helumCalID", "ELUM cal vs ring; ring; E [MeV]", NELUM, 0, NELUM, 500, eCalRange[0], eCalRange[1]);
   /// variable bins so that each ring is one bin in theta
   double thEdges[NELUM+1];
   for( int i = 0; i <= NELUM; i++) thEdges[i] = TMath::ATan2(elumRout - (elumRout-elumRin)/NELUM*i, elumZ)*TMath::RadToDeg();
   helumCalVTheta  = new TH2F("helumCalVTheta",  "ELUM E vs #theta_{lab}; #theta_{lab} [deg]; E [MeV]",          NELUM, thEdges, 500, eCalRange[0], eCalRange[1]);
   helumCalVThetaG = new TH2F("helumCalVThetaG", "ELUM E vs #theta_{lab} (RDT gated); #theta_{lab} [deg]; E [MeV]", NELUM, thEdges, 500, eCalRange[0], eCalRange[1]);
   helumMulti = new TH1I("helumMulti", "ELUM multiplicity; multi; count", NELUM+1, 0, NELUM+1);
   helumRate  = new TH1F("helumRate",  "ELUM rate; time [min]; count / min", TMath::Max(1, timeRange[1]-timeRange[0]), timeRange[0], timeRange[1]);
   hExElum    = new TH1F("hExElum",  Form("Ex(^{%d}%s) from ELUM; Ex [MeV]; count / %.0f keV", heavyA, iHeavy.Symbol.c_str(), exRange[0]), nEx, exRange[1], exRange[2]);
   hExElumG   = new TH1F("hExElumG", Form("Ex(^{%d}%s) from ELUM, RDT gated; Ex [MeV]; count / %.0f keV", heavyA, iHeavy.Symbol.c_str(), exRange[0]), nEx, exRange[1], exRange[2]);
   hExElumG->SetLineColor(2);
   hExElumVTheta = new TH2F("hExElumVTheta", "Ex vs #theta_{lab} (ELUM); #theta_{lab} [deg]; Ex [MeV]", NELUM, thEdges, nEx, exRange[1], exRange[2]);

   //---- RDT
   for( int i = 0; i < NRDT; i++){
      if( i % 2 == 0 ) hrdt[i] = new TH1F(Form("hrdt%d", i), Form("RDT-%d E; [ch]; count", i),  500, rdtERange[0],  rdtERange[1]);
      else             hrdt[i] = new TH1F(Form("hrdt%d", i), Form("RDT-%d dE; [ch]; count", i), 500, rdtDERange[0], rdtDERange[1]);
   }
   for( int i = 0; i < NRDT/2; i++){
      hrdt2D[i]  = new TH2F(Form("hrdt2D%d", i),  Form("RDT dE vs E (E=%d, dE=%d); E [ch]; dE [ch]", 2*i, 2*i+1),         500, rdtERange[0], rdtERange[1], 500, rdtDERange[0], rdtDERange[1]);
      hrdt2Dg[i] = new TH2F(Form("hrdt2Dg%d", i), Form("RDT dE vs E, ELUM coin. (E=%d, dE=%d); E [ch]; dE [ch]", 2*i, 2*i+1), 500, rdtERange[0], rdtERange[1], 500, rdtDERange[0], rdtDERange[1]);
   }
   hrdtID     = new TH2F("hrdtID", "RDT vs ID; ID; [ch]", NRDT, 0, NRDT, 500, 0, TMath::Max(rdtERange[1], rdtDERange[1]));
   hrdtMatrix = new TH2F("hrdtMatrix", "RDT ID vs RDT ID", NRDT, 0, NRDT, NRDT, 0, NRDT);

   //---- coincidence
   int nT = coinTimeRange[1] - coinTimeRange[0];
   htdiffElum    = new TH1I("htdiffElum", "rdt_t - elum_t; time [10 ns]; count",   nT, coinTimeRange[0], coinTimeRange[1]);
   htdiffStub    = new TH1I("htdiffStub", "rdt_t - stub e_t; time [10 ns]; count", nT, coinTimeRange[0], coinTimeRange[1]);
   htdiffStubVID = new TH2F("htdiffStubVID", "rdt_t - stub e_t vs RDT ch; RDT ch; time [10 ns]", NRDT, 0, NRDT, nT, coinTimeRange[0], coinTimeRange[1]);
   hStubECalVXG  = new TH2F("hStubECalVXG", "stub eCal vs X, RDT coin.; X [mm]; E [MeV]", 400, stubXCenterPos - stubLength/2. - 10, stubXCenterPos + stubLength/2. + 10, 500, eCalRange[0], eCalRange[1]);
   for( int i = 0; i < NRDT/2; i++){
      hrdt2DgStub[i] = new TH2F(Form("hrdt2DgStub%d", i), Form("RDT dE vs E, stub coin. (E=%d, dE=%d); E [ch]; dE [ch]", 2*i, 2*i+1), 500, rdtERange[0], rdtERange[1], 500, rdtDERange[0], rdtDERange[1]);
   }
   htdiffElumVID = new TH2F("htdiffElumVID", "rdt_t - elum_t vs ring; ring; time [10 ns]", NELUM, 0, NELUM, nT, coinTimeRange[0], coinTimeRange[1]);

   printf("======================================== End of histograms Declaration\n");
   StpWatch.Start();
}

/***************************************************************
 * Process
 ***************************************************************/
Bool_t MonitorsBZero::Process(Long64_t entry){

   ProcessedEntries++;
   if (ProcessedEntries > NumEntries*Frac - 1) {
      printf(" %3.0f%% (%llu/%llu k) processed in %6.1f sec | expect %6.1f sec\n\033[A\r",
         Frac*100, ProcessedEntries/1000, NumEntries/1000, StpWatch.RealTime(), StpWatch.RealTime()/Frac);
      StpWatch.Start(kFALSE);
      Frac += 0.1;
   }

   b_runID->GetEntry(entry);
   if( isArrayExist ){
      b_Energy->GetEntry(entry);  b_EnergyTimestamp->GetEntry(entry);
      b_XF->GetEntry(entry);      b_XFTimestamp->GetEntry(entry);
      b_XN->GetEntry(entry);      b_XNTimestamp->GetEntry(entry);
   }
   if( isRDTExist  ){ b_RDT->GetEntry(entry);  b_RDTTimestamp->GetEntry(entry);  }
   if( isELUMExist ){ b_ELUM->GetEntry(entry); b_ELUMTimestamp->GetEntry(entry); }

   //=========== canvas title
   if( entry == 0 ) {
      if( runID == lastRunID + 1 ) {
         int len = canvasTitle.Sizeof();
         if( contFlag == false) { canvasTitle.Remove(len-3); canvasTitle += " - "; }
         if( contFlag == true ) { canvasTitle.Remove(len-6); }
         contFlag = true;
      }
      if( runID > lastRunID + 1 ) contFlag = false;
      canvasTitle += Form("%03d, ", runID );
      lastRunID = runID;
   }

   /*********** RDT ****************************************************/
   bool rdtCutGate = !isCutFileOpen; /// true if no cut
   int  rdtHit = 0;
   for( int i = 0; i < NRDT; i++){
      if( TMath::IsNaN(rdt[i]) ) continue;
      rdtHit++;
      hrdt[i]->Fill(rdt[i]);
      hrdtID->Fill(i, rdt[i]);
      for( int j = 0; j < NRDT; j++) if( !TMath::IsNaN(rdt[j]) ) hrdtMatrix->Fill(i, j);
   }
   for( int i = 0; i < NRDT/2; i++){
      hrdt2D[i]->Fill(rdt[2*i], rdt[2*i+1]);
      if( isCutFileOpen && i < numCut && ((TCutG*)cutList->At(i))->IsInside(rdt[2*i], rdt[2*i+1]) ) rdtCutGate = true;
   }

   /*********** ELUM ***************************************************/
   int elumMulti = 0;
   bool anyElumCoin = false;
   for( int i = 0; i < NELUM; i++){
      if( TMath::IsNaN(elum[i]) ) continue;
      helum[i]->Fill(elum[i]);
      helumID->Fill(i, elum[i]);

      double eC = elumCal[i][0] + elumCal[i][1] * elum[i];
      if( eC < elumThreshold ) continue;
      elumMulti++;

      helumCal[i]->Fill(eC);
      helumCalID->Fill(i, eC);
      helumCalVTheta->Fill(elumTheta[i], eC);
      if( elum_t[i] > 0 ) helumRate->Fill(elum_t[i]/1e8/60.);

      double ex = CalEx(eC, elumTheta[i]);
      hExElum->Fill(ex);
      hExElumRing[i]->Fill(ex);
      hExElumVTheta->Fill(elumTheta[i], ex);

      //---- coincidence with RDT
      bool coin = false;
      for( int j = 0; j < NRDT; j++){
         if( TMath::IsNaN(rdt[j]) || rdt_t[j] == 0 ) continue;
         int tdiff = (int) ((Long64_t) rdt_t[j] - (Long64_t) elum_t[i]);
         htdiffElum->Fill(tdiff);
         htdiffElumVID->Fill(i, tdiff);
         if( !isTimeGateOn || (timeGate[0] < tdiff && tdiff < timeGate[1]) ) coin = true;
      }
      if( coin ) anyElumCoin = true;
      if( coin && rdtCutGate ){
         helumCalVThetaG->Fill(elumTheta[i], eC);
         hExElumG->Fill(ex);
      }
   }
   helumMulti->Fill(elumMulti);

   if( anyElumCoin ){
      for( int i = 0; i < NRDT/2; i++) hrdt2Dg[i]->Fill(rdt[2*i], rdt[2*i+1]);
   }

   /*********** stub (det 21) ******************************************/
   double sE = e[stubID], sXF = xf[stubID], sXN = xn[stubID];
   if( !TMath::IsNaN(sE) ){
      hStubE->Fill(sE);
      if( !TMath::IsNaN(sXF) ) hStubXF->Fill(sXF);
      if( !TMath::IsNaN(sXN) ) hStubXN->Fill(sXN);
      if( !TMath::IsNaN(sXF) && !TMath::IsNaN(sXN) ){
         hStubXFVXN->Fill(sXF, sXN);
         hStubEVXS->Fill(sXF + sXN, sE);
      }

      if( !TMath::IsNaN(sXF) && !TMath::IsNaN(sXN) && sE > 0 ) hStubEVx->Fill(0.5*(sXF - sXN)/sE + 0.5, sE);

      double eC = stubECal[0] + stubECal[1] * sE;
      if( eC > stubThreshold && !(TMath::IsNaN(sXF) && TMath::IsNaN(sXN)) ){

         hStubECal->Fill(eC);

         //---- coincidence with RDT, per telescope
         bool stubCoin = false;
         bool telCoin[NRDT/2] = {false};
         for( int j = 0; j < NRDT; j++){
            if( TMath::IsNaN(rdt[j]) || rdt_t[j] == 0 ) continue;
            int tdiff = (int) ((Long64_t) rdt_t[j] - (Long64_t) e_t[stubID]);
            htdiffStub->Fill(tdiff);
            htdiffStubVID->Fill(j, tdiff);
            if( !isTimeGateOn || (stubTimeGate[0] < tdiff && tdiff < stubTimeGate[1]) ){
               stubCoin = true;
               telCoin[j/2] = true;
            }
         }
         for( int i = 0; i < NRDT/2; i++) if( telCoin[i] ) hrdt2DgStub[i]->Fill(rdt[2*i], rdt[2*i+1]);

         double xfC = sXF * stubXfxnE[1] + stubXfxnE[0]/2.;
         double xnC = sXN * stubXnCorr * stubXfxnE[1] + stubXfxnE[0]/2.;
         double xC  = TMath::QuietNaN();
         if( !TMath::IsNaN(sXF) && !TMath::IsNaN(sXN) ) xC = 0.5 + 0.5 * (xfC - xnC) / sE;
         if( !TMath::IsNaN(sXF) &&  TMath::IsNaN(sXN) ) xC = xfC / sE;
         if(  TMath::IsNaN(sXF) && !TMath::IsNaN(sXN) ) xC = 1.0 - xnC / sE;
         xC = (xC - stubXCenter)/stubXScale + 0.5;

         if( TMath::Abs(xC - 0.5) < xGate/2. ){
            double xx = isStubXFPositiveX ? stubXCenterPos + (xC - 0.5)*stubLength : stubXCenterPos - (xC - 0.5)*stubLength;
            double th = TMath::ATan2(TMath::Sqrt(xx*xx + stubY*stubY), stubZ) * TMath::RadToDeg();
            hStubECalVX->Fill(xx, eC);
            hStubECalVTheta->Fill(th, eC);
            if( stubCoin ) hStubECalVXG->Fill(xx, eC);
         }
      }
   }

   return kTRUE;
}

/***************************************************************
 * Drawing helpers, callable at the ROOT prompt
 ***************************************************************/
void Draw2DHistBZ(TH2F * hist, int colzThreshold = 3000){
   if( hist->Integral() < colzThreshold ){
      hist->SetMarkerStyle(20); hist->SetMarkerSize(0.3); hist->Draw("box");
   }else{
      hist->Draw("colz");
   }
}

/// RDT dE-E of telescope id, with the RDTCutCreator cut-id on top if loaded
void DrawRDTWithCut(TH2F * hist, int id){
   Draw2DHistBZ(hist);
   if( isCutFileOpen && id < numCut ){
      TCutG * cc = (TCutG *) cutList->At(id);
      cc->SetLineWidth(2);
      cc->Draw("same");
   }
}

void DrawExLinesTheta(double th1, double th2){
   int colors[6] = {2, 6, 4, 8, 7, 9};
   for( int k = 0; k < (int) exLines.size(); k++) KinematicLine(exLines[k], th1, th2, colors[k%6])->Draw("L same");
}


TCanvas * GetCanvasBZ(TString name, TString title, int nx, int ny, int w, int h){
   TCanvas * c = (TCanvas *) gROOT->FindObjectAny(name);
   if( c == NULL ) c = new TCanvas(name, title + " | " + canvasTitle, w, h);
   c->Clear(); if( nx*ny > 1 ) c->Divide(nx, ny);
   return c;
}

void stub(){
   TCanvas * c = GetCanvasBZ("cStubBZ", "stub (det 21)", 3, 3, 1500, 1200);
   c->cd(1); hStubE->Draw();
   c->cd(2); hStubXF->Draw();
   c->cd(3); hStubXN->Draw();
   c->cd(4); Draw2DHistBZ(hStubXFVXN);
   c->cd(5); Draw2DHistBZ(hStubEVXS);
   c->cd(6); Draw2DHistBZ(hStubEVx);
   c->cd(7); hStubECal->Draw();
   c->cd(8); Draw2DHistBZ(hStubECalVX);
   c->cd(9); Draw2DHistBZ(hStubECalVTheta);
}

void elum(bool isLogy = false){
   TCanvas * c = GetCanvasBZ("cElumBZ", "ELUM raw", 4, 4, 1600, 1200);
   for( int i = 0; i < NELUM; i++){ c->cd(i+1); if(isLogy) gPad->SetLogy(); helum[i]->Draw(); }
   TCanvas * c2 = GetCanvasBZ("cElumCalBZ", "ELUM cal", 4, 4, 1600, 1200);
   for( int i = 0; i < NELUM; i++){ c2->cd(i+1); if(isLogy) gPad->SetLogy(); helumCal[i]->Draw(); }
   TCanvas * c3 = GetCanvasBZ("cElumIDBZ", "ELUM ID", 2, 2, 1200, 1000);
   c3->cd(1); helumID->Draw("colz");
   c3->cd(2); helumCalID->Draw("colz");
   c3->cd(3); helumMulti->Draw();
   c3->cd(4); helumRate->Draw();
}

void Ex(){
   TCanvas * c = GetCanvasBZ("cExRingBZ", "Ex per ELUM ring", 4, 4, 1600, 1200);
   for( int i = 0; i < NELUM; i++){ c->cd(i+1); hExElumRing[i]->Draw(); }
   TCanvas * c2 = GetCanvasBZ("cExBZ", "Ex", 2, 2, 1200, 1000);
   c2->cd(1); hExElum->Draw(); hExElumG->Draw("same");
   c2->cd(2); hExElumVTheta->Draw("colz");
   c2->cd(3); Draw2DHistBZ(helumCalVTheta); DrawExLinesTheta(helumCalVTheta->GetXaxis()->GetXmin(), helumCalVTheta->GetXaxis()->GetXmax());
   c2->cd(4); helumID->Draw("colz");
}

void rdt(bool isLogz = false){
   TCanvas * c = GetCanvasBZ("cRdtBZ", "RDT dE-E", 2, 2, 1000, 1000);
   int padOrder[4] = {0, 1, 3, 2}; /// physical quadrants
   for( int k = 0; k < NRDT/2; k++){ c->cd(k+1); if(isLogz) gPad->SetLogz(); DrawRDTWithCut(hrdt2D[padOrder[k]], padOrder[k]); }
   TCanvas * c2 = GetCanvasBZ("cRdtSBZ", "RDT single", 2, NRDT/2, 1000, 1000);
   for( int i = 0; i < NRDT; i++){ c2->cd(i+1); hrdt[i]->Draw(); }
   TCanvas * c3 = GetCanvasBZ("cRdtIDBZ", "RDT ID", 2, 1, 1000, 500);
   c3->cd(1); hrdtID->Draw("colz");
   c3->cd(2); hrdtMatrix->Draw("colz");
}

void DrawGateLines(TH1 * h, int * gate){
   if( !isTimeGateOn ) return;
   double yMax = h->GetMaximum();
   TLine * l1 = new TLine(gate[0], 0, gate[0], yMax); l1->SetLineColor(2); l1->Draw();
   TLine * l2 = new TLine(gate[1], 0, gate[1], yMax); l2->SetLineColor(2); l2->Draw();
}

void coin(){
   /// row 1 : ELUM - RDT, row 2 : stub - RDT ((a,a): alpha in stub, 20Ne in RDT)
   TCanvas * c = GetCanvasBZ("cCoinBZ", "coincidence", 3, 2, 1500, 1000);
   c->cd(1); htdiffElum->Draw(); DrawGateLines(htdiffElum, timeGate);
   c->cd(2); htdiffElumVID->Draw("colz");
   c->cd(3); Draw2DHistBZ(helumCalVThetaG);
   c->cd(4); htdiffStub->Draw(); DrawGateLines(htdiffStub, stubTimeGate);
   c->cd(5); htdiffStubVID->Draw("colz");
   c->cd(6); Draw2DHistBZ(hStubECalVXG);

   /// RDT gated by ELUM / stub, pads follow the physical quadrants (0 1 / 3 2)
   int padOrder[4] = {0, 1, 3, 2};
   TCanvas * c2 = GetCanvasBZ("cCoinRdtStubBZ", "RDT dE-E, stub coin.", 2, 2, 1000, 1000);
   for( int k = 0; k < NRDT/2; k++){ c2->cd(k+1); DrawRDTWithCut(hrdt2DgStub[padOrder[k]], padOrder[k]); }
   TCanvas * c3 = GetCanvasBZ("cCoinRdtElumBZ", "RDT dE-E, ELUM coin.", 2, 2, 1000, 1000);
   for( int k = 0; k < NRDT/2; k++){ c3->cd(k+1); DrawRDTWithCut(hrdt2Dg[padOrder[k]], padOrder[k]); }
}

void listDrawsBZ(){
   printf("------------------- MonitorsBZero draw functions\n");
   printf("  stub()           - stub (det %d) raw, x, eCal vs X, eCal vs theta ((a,a) elastics)\n", stubID);
   printf("  elum(isLogy)     - ELUM raw / cal per ring, ID, multi, rate\n");
   printf("  Ex()             - Ex per ring, Ex vs theta, ELUM E vs theta + (a,p) lines\n");
   printf("  rdt(isLogz)      - RDT dE-E, singles, ID\n");
   printf("  coin()           - ELUM-RDT & stub-RDT time diff, gated spectra, RDT dE-E gated by stub / ELUM\n");
   printf("-------------------------------\n");
   printf("  RDTCutCreator(\"../root_data/gen_runXXX.root\", \"%s\", %d, %d)  - draw 4 RDT cuts (x=E, y=dE), save, quit\n", rdtCutFile.Data(), rdtERange[1], rdtDERange[1]);
   printf("  Check_rdtGate(\"../root_data/gen_runXXX.root\", \"%s\")  - check the cuts\n", rdtCutFile.Data());
   printf("  RDT cut file : %s (%s)\n", rdtCutFile.Data(), isCutFileOpen ? Form("%d cuts loaded", numCut) : "not found, no RDT cut");
   printf("-------------------------------\n");
   printf("  CalEx(T, theta)  - Ex [MeV] from light T [MeV] at theta_lab [deg]\n");
   printf("  CalT(theta, Ex)  - light T [MeV] at theta_lab [deg] for Ex [MeV]\n");
   printf("-------------------------------------------------\n");
}

/***************************************************************
 * Terminate
 ***************************************************************/
void MonitorsBZero::Terminate(){
   printf("\n============================== finishing.\n");
   gROOT->cd();

   int strLen = canvasTitle.Sizeof();
   canvasTitle.Remove(strLen-3);
   if( runTime > 0. ) canvasTitle += Form(" | %.0f min", runTime);

   gStyle->SetOptStat("neiou");
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);

   int padSize = 500;
   cBZ = new TCanvas("cBZ", "B=0 | " + canvasTitle, 3*padSize, 3*padSize);
   cBZ->Divide(3, 3);

   /// layout (RDT pads follow the physical quadrant arrangement, as in Monitors.C)
   ///   hStubECalVX | helumCalVTheta | htdiffElum
   ///   hExElum     | hrdt2D0        | hrdt2D1
   ///   hExElumG    | hrdt2D3        | hrdt2D2

   ///---- 1 stub eCal vs X, (a,a) elastics
   cBZ->cd(1); Draw2DHistBZ(hStubECalVX);

   ///---- 2 ELUM E vs theta + (a,p) kinematic lines
   cBZ->cd(2); Draw2DHistBZ(helumCalVTheta);
   DrawExLinesTheta(helumCalVTheta->GetXaxis()->GetXmin(), helumCalVTheta->GetXaxis()->GetXmax());
   text.DrawLatex(0.15, 0.85, Form("lines : Ex ="));
   for( int k = 0; k < (int) exLines.size(); k++) text.DrawLatex(0.15, 0.80 - 0.05*k, Form("%.3f", exLines[k]));

   ///---- 3 tdiff ELUM - RDT
   cBZ->cd(3); htdiffElum->Draw();
   if( isTimeGateOn ){
      double yMax = htdiffElum->GetMaximum();
      TLine * l1 = new TLine(timeGate[0], 0, timeGate[0], yMax); l1->SetLineColor(2); l1->Draw();
      TLine * l2 = new TLine(timeGate[1], 0, timeGate[1], yMax); l2->SetLineColor(2); l2->Draw();
   }

   ///---- 4 Ex from ELUM
   cBZ->cd(4); hExElum->Draw();

   ///---- 5, 6 RDT dE-E
   cBZ->cd(5); DrawRDTWithCut(hrdt2D[0], 0);
   cBZ->cd(6); DrawRDTWithCut(hrdt2D[1], 1);

   ///---- 7 Ex from ELUM, RDT gated
   cBZ->cd(7); hExElumG->Draw();
   if( isTimeGateOn ) text.DrawLatex(0.15, 0.85, Form("%d < tdiff < %d", timeGate[0], timeGate[1]));
   if( isCutFileOpen ) text.DrawLatex(0.15, 0.80, "with RDT cut");

   ///---- 8, 9 RDT dE-E
   cBZ->cd(8); DrawRDTWithCut(hrdt2D[3], 3);
   cBZ->cd(9); DrawRDTWithCut(hrdt2D[2], 2);

   cBZ->Modified(); cBZ->Update();

   gStyle->GetAttDate()->SetTextSize(0.02);
   gStyle->SetOptDate(1);
   gStyle->SetDateX(0);
   gStyle->SetDateY(0);

   StpWatch.Start(kFALSE);

   gROOT->ProcessLine(".L ../Armory/RDTCutCreator.C");
   printf("=============== loaded Armory/RDTCutCreator.C\n");
   gROOT->ProcessLine(".L ../Armory/Check_rdtGate.C");
   printf("=============== loaded Armory/Check_rdtGate.C\n");

   listDrawsBZ();
}

#endif
