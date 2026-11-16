/***********************************************************************
 *
 *  MonitorsBZero.C  --  online monitor for B = 0 T, h098 20Ne(a,p)23Na
 *
 *  usage : root -l 'ChainMonitorsBZero.C(RUN)'     or
 *          ../Armory/Process_RUN RUN ... -BZ
 *
 *  detectors (see MonitorsBZero.h)
 *    stub (det 21) : PSD 50 x 10 mm, center at Y = -150 mm, Z = +25 mm, only the long axis is position sensitive
 *                    stubLongAxis = 0 : long axis along Z -> theta_lab measured, phi unresolved
 *                    stubLongAxis = 1 : long axis along X -> phi_lab measured, theta unresolved (~80.5 +- 1.9 deg)
 *    ELUM          : S1, 1000 um, 16 rings r = 24-48 mm, z = -50 mm
 *    RDT           : at z = +1000 mm
 *
 *  Ex of 23Na from ELUM : missing mass with (T_p, theta_lab of ring)
 *     P_B = P_beam + P_target - P_p ,  Ex = sqrt(P_B^2) - m(23Na)
 *
 *  trace mode : trace_runXXX.root (Process_RUN RUN 2 10 -BZ), auto-detected from the branches
 *     energy : te/txf/txn (stub), trdt, telum (EventBuilder, NELUM > 0) replace e/xf/xn, rdt, elum,
 *              stub / ELUM via stubTraceScale / elumTraceScale ({0,1} now : same calibrations as non-trace),
 *              RDT = trdt * POLARITY_RDT (no scale), so all calibrations / cuts apply unchanged
 *     time   : timestamp + trace T0 when both sides of a coincidence have a trace
 *     fit dropped (NaN) when rise time outside traceRiseGate
 *     Terminate prints the trace -> firmware scale table ; trace() draws the checks
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
#include <TChain.h>
#include <TChainElement.h>
#include <TFile.h>
#include <vector>
#include <fstream>
#include "../Cleopatra/Isotope.h"
#include "mapping.h"

using namespace std;

//############################################ User setting

//---- Reaction : beam(target, light)heavy, inverse kinematics
int    beamA = 20, beamZ = 10;     /// 20Ne
int    targA =  4, targZ =  2;     /// 
int   lightA =  1, lightZ = 1;     /// 
int   heavyA = beamA + targA - lightA, heavyZ = beamZ + targZ - lightZ;    /// 23Na
int   targ2A = 28, targ2Z = 14;     /// for the beam(targ2, targ2)beam elastic line on hStubECalVPos / VTheta, e.g. 12C in CD2
double beamEnergyPerU = 6.0;       /// MeV/u
vector<double> exLines = {0.0, 4.0};  /// Ex [MeV] for the kinematic lines

//---- Stub (det 21) geometry
const int stubID      = 21;
/// only the long axis is position sensitive ; the short axis (width) is not resolved
///   stubLongAxis = 0 : long axis along Z            -> position gives theta_lab = atan2( sqrt(X^2 + Y^2), Z ), phi fixed (unresolved)
///   stubLongAxis = 1 : long axis along X (XY plane) -> position gives phi_lab   = atan2( Y, X ),  theta fixed (unresolved, from width along Z)
int    stubLongAxis   =   0;
double stubX          =   0.0;  /// center of the stub [mm]
double stubY          = -150.0; /// center of the stub [mm] (signed, stub at -Y)
double stubZ          =  50.0;  /// center of the stub [mm]
double stubLength     =  50.0;  /// long axis, position sensitive [mm]
double stubWidth      =  10.0;  /// short axis, not position sensitive [mm]
bool   isStubXFPositive = true; /// x = 1 (xf side) toward +Z (mode 0) or +X (mode 1)

//---- ELUM geometry
double elumZ          = -80.0;  /// [mm]
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
int    rawERange[2]   = {  100, 6000};   /// stub e, xf, xn [ch]
int    elumRawRange[2]= {  100, 1000};   /// [ch]
double eCalRange[2]   = {   0.5,  15};   /// [MeV]
int    rdtERange[2]   = {   0, 5000};
int    rdtDERange[2]  = {   0, 5000};
double exRange[3]     = {  50,   -10, 12}; /// bin [keV], low [MeV], high [MeV]
int    coinTimeRange[2] = { -100, 100};   /// [ch = 10 ns]

//---- Gates
double elumThreshold  = 1.0;     /// [MeV], below -> ignored (cal offset ~0.8 MeV at ch 0)
double stubThreshold  = 1.0;     /// [MeV] (cal offset 0.32 MeV at ch 0)
double xGate          = 1.0;     /// |x - 0.5| < xGate/2 for stub
bool   isTimeGateOn   = true;
int    timeGate[2]    = { -50, 50}; /// rdt_t - elum_t, [ch = 10 ns]
int    stubTimeGate[2]= { -50, 50}; /// rdt_t - e_t[stub], [ch = 10 ns], (a,a): alpha -> stub (-Y), 20Ne -> RDT (+Y)
TString rdtCutFile    = "rdtCuts.root"; /// from ../Armory/RDTCutCreator.py (or .C) : TObjArray "cutList", cut-i on x=rdt[2i] (E), y=rdt[2i+1] (dE). ignored if not exist

//---- Trace (trace_runXXX.root). used only when the trace branches exist
bool   isUseArrayTrace = true;  /// stub e, xf, xn  <- te, txf, txn
bool   isUseRDTTrace   = true;  /// rdt  <- trdt
bool   isUseELUMTrace  = true;  /// elum <- telum (EventBuilder trace analysis, NELUM > 0)
bool   isUseTraceTime  = true;  /// coincidence time = timestamp + trace T0, when both sides have a trace
double traceRiseGate[2] = {0.5, 50}; /// [sample], rise time outside -> fit dropped (no hit)
/// trace amplitude -> firmware scale : x = s0 + s1 * (trace * POLARITY), {s0, s1}
/// for the moment {0, 1} : trace uses the same calibrations as non-trace (stubECal, stubXfxnE, elumCal)
/// a fit of firmware vs trace per channel is printed at Terminate, if a rescale is needed later
/// RDT : no scale, rdt = trdt * POLARITY_RDT (same RDT cuts)
double stubTraceScale[3][2] = { {0, 1}, {0, 1}, {0, 1} }; /// e, xf, xn of the stub
double elumTraceScale[NELUM][2] = {
   {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
   {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}
};
int    traceTimeBinPerCh = 10;          /// tdiff bins per 10 ns with trace time
double traceTimeGate[2]     = {-50, 50}; /// rdt - elum, trace time [10 ns]
double traceStubTimeGate[2] = {-50, 50}; /// rdt - stub, trace time [10 ns]
double traceScaleFitMin = 200;          /// firmware value > this [ch] used in the scale fit

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
TFile * fCutFile = NULL;      /// file of cutList
TString processedFiles = "";  /// files of the chain, default input of RDTCutCreator()

/// (re)load the RDT cuts from rdtCutFile : TObjArray "cutList", cut-i on x = rdt[2i] (E), y = rdt[2i+1] (dE)
void LoadRDTCuts(){
   isCutFileOpen = false; numCut = 0; cutList = NULL;
   if( fCutFile ){ fCutFile->Close(); delete fCutFile; fCutFile = NULL; } /// cutList read from it is not owned by the file
   if( rdtCutFile != "" && !gSystem->AccessPathName(rdtCutFile) ){
      fCutFile = new TFile(rdtCutFile);
      isCutFileOpen = fCutFile->IsOpen();
      if( isCutFileOpen ){
         cutList = (TObjArray *) fCutFile->FindObjectAny("cutList");
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
      printf("=========== no rdt-cutfile %s, no RDT cut. make one at the ROOT prompt (after the monitor) with RDTCutCreator()\n", rdtCutFile.Data());
   }
}

TLatex text;

//====== reaction constants
double mBeam, mTarg, mLight, mHeavy;
double mTarg2; /// for the (targ2, targ2) elastic line
double Etot, Ptot, sInv; /// lab total energy, momentum (along +z), invariant mass^2
double Qvalue;
bool isKinematicsOK = false;

//====== trace mode, set in Begin from the bound branches
bool isStubFineTime = false;  /// stub-RDT tdiff uses trace T0 on both sides
bool isElumFineTime = false;  /// ELUM-RDT tdiff uses trace T0 on both sides
double gateElum[2], gateStub[2]; /// active time gates

/// linear LSQ sums for the trace -> firmware scale, y = s0 + s1 * x
struct ScaleSum {
   double n = 0, sx = 0, sy = 0, sxx = 0, sxy = 0;
   void Add(double x, double y){ n++; sx += x; sy += y; sxx += x*x; sxy += x*y; }
   bool Get(double &s0, double &s1){
      double d = n*sxx - sx*sx;
      if( n < 10 || d == 0 ) return false;
      s1 = (n*sxy - sx*sy)/d; s0 = (sy - s1*sx)/n; return true;
   }
};
ScaleSum sumStub[3], sumElum[NELUM];

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
TH2F* hStubECalVPos;    // stub eCal vs position along the long axis (Z or X)
TH2F* hStubECalVTheta;  // stub eCal vs resolved angle : theta_lab (mode 0) or phi_lab (mode 1), name kept

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
TH2F* hrdt2D_timeG[NRDT/2]; // RDT dE-E, this telescope (E or dE) in rdt_t - elum_t gate with any ELUM hit
TH2F* hrdtMatrix;

//======== coincidence
TH1F* htdiffElum;    // rdt_t - elum_t  (+ trace T0 with trace time)
TH1F* htdiffStub;    // rdt_t - e_t[stub] (+ trace T0 with trace time)
TH2F* htdiffStubVID; // vs RDT channel
TH2F* hStubECalVPosG; // stub eCal vs position, RDT coincident
TH2F* hrdt2DgStub[NRDT/2]; // RDT dE-E, stub coincident
TH2F* htdiffElumVID; // vs ring

//======== trace checks (firmware vs trace*POLARITY, before rescale)
TH2F* hTrStub[3];     // e, xf, xn
TH2F* hTrRdtRatio;    // fw / trace vs ID
TH2F* hTrElumRatio;   // fw / trace vs ring
TH1F* hTrStubRise;
TH1F* hTrStubT0;
TH2F* hTrRdtRise;
TH2F* hTrRdtT0;
TH2F* hTrElumRise;
TH2F* hTrElumT0;

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

/// (X, X) elastics : beam(X, X)beam with X = targ or targ2, e.g. 20Ne(d,d)20Ne, 20Ne(12C,12C)20Ne
/// T [MeV] of the recoiling X (mass mX [MeV]) at theta_lab [deg]; p3 = 2 m E P cos / (E^2 - P^2 cos^2). NaN if theta >= 90 deg
TString elasticName, elasticName2; /// (targ, targ), (targ2, targ2), set in Begin
double CalTElastic(double thetaDeg, double mX){
   if( !(mBeam > 1 && mX > 1) ) return TMath::QuietNaN();
   double c = TMath::Cos(thetaDeg*TMath::DegToRad());
   if( c <= 0 ) return TMath::QuietNaN();
   double Tb = beamEnergyPerU * beamA;
   double E  = Tb + mBeam + mX;
   double P  = TMath::Sqrt(Tb*Tb + 2*Tb*mBeam);
   double p3 = 2*mX*E*P*c/(E*E - P*P*c*c);
   return TMath::Sqrt(p3*p3 + mX*mX) - mX;
}

/// stub hit position [mm] from s = signed position along the long axis, measured from the stub center
void StubHitPos(double s, double &X, double &Y, double &Z){
   X = stubX; Y = stubY; Z = stubZ;
   if( stubLongAxis == 0 ) Z += s; else X += s;
}

/// the angle resolved by the stub [deg] : theta_lab (long axis along Z) or phi_lab (long axis along X)
double StubAngle(double X, double Y, double Z){
   if( stubLongAxis == 0 ) return TMath::ATan2(TMath::Sqrt(X*X + Y*Y), Z) * TMath::RadToDeg();
   return TMath::ATan2(Y, X) * TMath::RadToDeg();
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

/// (X, X) elastic line : T vs position along the long axis (Z or X) for hStubECalVPos,
/// or T vs StubAngle (theta_lab or phi_lab) for hStubECalVTheta (isAngle); theta_lab from the hit position
TGraph * ElasticLineStub(double mX, int color = kRed, bool isAngle = false){
   TGraph * g = new TGraph();
   for( int k = 0; k <= 200; k++){
      double s = -stubLength/2. - 10 + (stubLength + 20)*k/200.;
      double X, Y, Z;
      StubHitPos(s, X, Y, Z);
      double th = TMath::ATan2(TMath::Sqrt(X*X + Y*Y), Z) * TMath::RadToDeg();
      double T  = CalTElastic(th, mX);
      if( TMath::IsNaN(T) ) continue;
      double xx = isAngle ? StubAngle(X, Y, Z) : (stubLongAxis == 0 ? Z : X);
      g->SetPoint(g->GetN(), xx, T);
   }
   g->SetLineColor(color); g->SetLineWidth(2);
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
   Isotope iTarg2 = GetIsotope(targ2A, targ2Z);
   mTarg2 = iTarg2.Mass;
   elasticName.Form("%s(%s,%s)",  iBeam.Name.c_str(), iTarg.Name.c_str(),  iTarg.Name.c_str());
   elasticName2.Form("%s(%s,%s)", iBeam.Name.c_str(), iTarg2.Name.c_str(), iTarg2.Name.c_str());
   printf("   elastic lines on hStubECalVPos / VTheta : %s (red), %s (blue, targ2 mass %.4f MeV)\n", elasticName.Data(), elasticName2.Data(), mTarg2);

   //================ ELUM ring angles
   double dr = (elumRout - elumRin)/NELUM;
   printf("================ ELUM rings (z = %.1f mm)  T_p for Ex = 0 \n", elumZ);
   for( int i = 0; i < NELUM; i++){
      int k = isElumRing0Inner ? i : NELUM - 1 - i;
      elumR[i] = elumRin + dr * (k + 0.5);
      elumTheta[i] = TMath::ATan2(elumR[i], elumZ) * TMath::RadToDeg();
      printf("   ring %2d : r = %5.2f mm, theta = %6.2f deg, T_p(Ex=0) = %6.3f MeV\n", i, elumR[i], elumTheta[i], CalT(elumTheta[i], 0));
   }
   //---- stub angles : resolved (along long axis) and unresolved (across width)
   double angStub[2];  /// range of the resolved angle, for histogram axis
   {
      double s[3] = {-stubLength/2., 0, stubLength/2.};
      double w[2] = {-stubWidth/2., stubWidth/2.};
      double a[3], u[2];
      for( int i = 0; i < 3; i++){ double X, Y, Z; StubHitPos(s[i], X, Y, Z); a[i] = StubAngle(X, Y, Z); }
      /// unresolved angle : center of the long axis, edges of the width
      if( stubLongAxis == 0 ){ /// width along X (perpendicular to Y offset) -> phi
         for( int i = 0; i < 2; i++) u[i] = TMath::ATan2(stubY, stubX + w[i]) * TMath::RadToDeg();
      }else{                   /// width along Z -> theta
         for( int i = 0; i < 2; i++) u[i] = TMath::ATan2(TMath::Sqrt(stubX*stubX + stubY*stubY), stubZ + w[i]) * TMath::RadToDeg();
      }
      angStub[0] = TMath::Min(a[0], a[2]); angStub[1] = TMath::Max(a[0], a[2]);
      const char * res = stubLongAxis == 0 ? "theta" : "phi";
      const char * unr = stubLongAxis == 0 ? "phi"   : "theta";
      printf("================ stub long axis along %s : %s_lab measured %.2f - %.2f deg (center %.2f)\n",
             stubLongAxis == 0 ? "Z" : "X", res, angStub[0], angStub[1], a[1]);
      printf("                 %s_lab NOT resolved : %.2f - %.2f deg (center %.2f +- %.2f, from %.0f mm width)\n",
             unr, TMath::Min(u[0], u[1]), TMath::Max(u[0], u[1]), 0.5*(u[0]+u[1]), 0.5*TMath::Abs(u[1]-u[0]), stubWidth);
      for( int el = 0; el < 2; el++){
         printf("                 %s elastics :", (el == 0 ? elasticName : elasticName2).Data());
         for( int i = 0; i < 3; i++){
            double X, Y, Z; StubHitPos(s[i], X, Y, Z);
            double th = TMath::ATan2(TMath::Sqrt(X*X + Y*Y), Z) * TMath::RadToDeg();
            printf(" %s = %.1f mm (theta %.1f) T = %.3f MeV%s", stubLongAxis == 0 ? "Z" : "X", stubLongAxis == 0 ? Z : X, th,
                   CalTElastic(th, el == 0 ? mTarg : mTarg2), i < 2 ? " |" : "\n");
         }
      }
   }

   //================ Recoil cut
   LoadRDTCuts();

   //================ files of the chain (default input of RDTCutCreator())
   processedFiles = "";
   TChain * chainIn = dynamic_cast<TChain *>(tree);
   if( chainIn ){
      TIter next(chainIn->GetListOfFiles());
      while( TChainElement * el = (TChainElement *) next() ) processedFiles += TString(el->GetTitle()) + " ";
   }else if( tree && tree->GetCurrentFile() ){
      processedFiles = tree->GetCurrentFile()->GetName();
   }
   processedFiles = processedFiles.Strip(TString::kBoth);

   //================ trace mode (Init is called after Begin, so detect from the branches here)
   DetectTrace(tree);
   isStubFineTime = isUseTraceTime && isArrayTrace && isRDTTrace;
   isElumFineTime = isUseTraceTime && isELUMTrace  && isRDTTrace;
   for( int k = 0; k < 2; k++){
      gateStub[k] = isStubFineTime ? traceStubTimeGate[k] : stubTimeGate[k];
      gateElum[k] = isElumFineTime ? traceTimeGate[k]     : timeGate[k];
   }
   for( int k = 0; k < 3; k++) sumStub[k] = ScaleSum();
   for( int i = 0; i < NELUM; i++) sumElum[i] = ScaleSum();
   printf("================ Trace : stub %s, RDT %s, ELUM %s | tdiff stub-RDT %s, ELUM-RDT %s\n",
          isArrayTrace ? "ON" : "off", isRDTTrace ? "ON" : "off", isELUMTrace ? "ON" : "off",
          isStubFineTime ? "trace time" : "timestamp", isElumFineTime ? "trace time" : "timestamp");
   if( isArrayTrace || isRDTTrace || isELUMTrace ){
      printf("                 rise-time gate %.1f - %.1f sample, stub / ELUM x = s0 + s1 * trace * POL (xxxTraceScale), RDT = trdt * POL\n", traceRiseGate[0], traceRiseGate[1]);
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
   TString stubAx  = stubLongAxis == 0 ? "Z" : "X";
   TString stubAng = stubLongAxis == 0 ? "#theta_{lab}" : "#phi_{lab}";
   double  stubPos0 = (stubLongAxis == 0 ? stubZ : stubX);
   hStubECalVPos   = new TH2F("hStubECalVPos", Form("stub eCal vs %s; %s [mm]; E [MeV]", stubAx.Data(), stubAx.Data()), 400, stubPos0 - stubLength/2. - 10, stubPos0 + stubLength/2. + 10, 500, eCalRange[0], eCalRange[1]);
   hStubECalVTheta = new TH2F("hStubECalVTheta", Form("stub eCal vs %s; %s [deg]; E [MeV]", stubAng.Data(), stubAng.Data()), 200, angStub[0]-1, angStub[1]+1, 500, eCalRange[0], eCalRange[1]);

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
      hrdt2D_timeG[i] = new TH2F(Form("hrdt2D%d_timeG", i),
                                 Form("RDT dE vs E, %g < rdt_t - elum_t < %g%s (E=%d, dE=%d); E [ch]; dE [ch]", gateElum[0], gateElum[1], isElumFineTime ? " (trace time)" : "", 2*i, 2*i+1),
                                 500, rdtERange[0], rdtERange[1], 500, rdtDERange[0], rdtDERange[1]);
   }
   hrdtID     = new TH2F("hrdtID", "RDT vs ID; ID; [ch]", NRDT, 0, NRDT, 500, 0, TMath::Max(rdtERange[1], rdtDERange[1]));
   hrdtMatrix = new TH2F("hrdtMatrix", "RDT ID vs RDT ID", NRDT, 0, NRDT, NRDT, 0, NRDT);

   //---- coincidence
   int nT = coinTimeRange[1] - coinTimeRange[0];
   int nTS = nT * (isStubFineTime ? traceTimeBinPerCh : 1);
   int nTE = nT * (isElumFineTime ? traceTimeBinPerCh : 1);
   const char * tagS = isStubFineTime ? " (trace time)" : "";
   const char * tagE = isElumFineTime ? " (trace time)" : "";
   htdiffElum    = new TH1F("htdiffElum", Form("rdt_t - elum_t%s; time [10 ns]; count", tagE),   nTE, coinTimeRange[0], coinTimeRange[1]);
   htdiffStub    = new TH1F("htdiffStub", Form("rdt_t - stub e_t%s; time [10 ns]; count", tagS), nTS, coinTimeRange[0], coinTimeRange[1]);
   htdiffStubVID = new TH2F("htdiffStubVID", Form("rdt_t - stub e_t vs RDT ch%s; RDT ch; time [10 ns]", tagS), NRDT, 0, NRDT, nTS, coinTimeRange[0], coinTimeRange[1]);
   hStubECalVPosG = new TH2F("hStubECalVPosG", Form("stub eCal vs %s, RDT coin.; %s [mm]; E [MeV]", stubAx.Data(), stubAx.Data()), 400, stubPos0 - stubLength/2. - 10, stubPos0 + stubLength/2. + 10, 500, eCalRange[0], eCalRange[1]);
   for( int i = 0; i < NRDT/2; i++){
      hrdt2DgStub[i] = new TH2F(Form("hrdt2DgStub%d", i), Form("RDT dE vs E, stub coin. (E=%d, dE=%d); E [ch]; dE [ch]", 2*i, 2*i+1), 500, rdtERange[0], rdtERange[1], 500, rdtDERange[0], rdtDERange[1]);
   }
   htdiffElumVID = new TH2F("htdiffElumVID", Form("rdt_t - elum_t vs ring%s; ring; time [10 ns]", tagE), NELUM, 0, NELUM, nTE, coinTimeRange[0], coinTimeRange[1]);

   //---- trace checks : firmware vs trace * POLARITY (before rescale), rise time, T0
   const char * stubName[3] = {"e", "xf", "xn"};
   for( int k = 0; k < 3; k++){
      hTrStub[k] = new TH2F(Form("hTrStub%s", stubName[k]), Form("stub %s : firmware vs trace; t%s * POLARITY [ch]; %s [ch]", stubName[k], stubName[k], stubName[k]),
                            400, rawERange[0], rawERange[1], 400, rawERange[0], rawERange[1]);
   }
   hTrRdtRatio  = new TH2F("hTrRdtRatio",  "RDT firmware / (trdt * POLARITY); ID; ratio",   NRDT,  0, NRDT,  400, 0, 2);
   hTrElumRatio = new TH2F("hTrElumRatio", "ELUM firmware / (telum * POLARITY); ring; ratio", NELUM, 0, NELUM, 400, 0, 2);
   hTrStubRise  = new TH1F("hTrStubRise", "stub te rise time; rise time [sample]; count", 500, 0, 100);
   hTrStubT0    = new TH1F("hTrStubT0",   "stub te T0; T0 [sample]; count",               500, 0, 250);
   hTrRdtRise   = new TH2F("hTrRdtRise",  "RDT trdt rise time; ID; rise time [sample]",   NRDT,  0, NRDT,  250, 0, 100);
   hTrRdtT0     = new TH2F("hTrRdtT0",    "RDT trdt T0; ID; T0 [sample]",                 NRDT,  0, NRDT,  250, 0, 250);
   hTrElumRise  = new TH2F("hTrElumRise", "ELUM telum rise time; ring; rise time [sample]", NELUM, 0, NELUM, 250, 0, 100);
   hTrElumT0    = new TH2F("hTrElumT0",   "ELUM telum T0; ring; T0 [sample]",               NELUM, 0, NELUM, 250, 0, 250);

   printf("======================================== End of histograms Declaration\n");
   StpWatch.Start();
}

/***************************************************************
 * trace : drop bad fit, fill checks, rescale to firmware scale (in place)
 ***************************************************************/
/// val : trace amplitude (in), trace * polarity rescaled or NaN (out). fw : firmware value. returns true if kept
/// scale = NULL / sum = NULL : no rescale / no scale fit (RDT)
bool ApplyTrace(Float_t &val, double rise, int polarity, double * scale, double fw, ScaleSum * sum){
   double raw = val * polarity;
   if( TMath::IsNaN(raw) || !(traceRiseGate[0] < rise && rise < traceRiseGate[1]) ){ val = TMath::QuietNaN(); return false; }
   if( sum && !TMath::IsNaN(fw) && fw > traceScaleFitMin ) sum->Add(raw, fw);
   val = scale ? scale[0] + scale[1] * raw : raw;
   return true;
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

   /*********** trace : e/xf/xn, rdt, elum hold the trace amplitudes (b_Energy etc. bound to te ...) */
   if( isArrayTrace ){
      b_TE_T->GetEntry(entry);  b_TE_R->GetEntry(entry);
      b_TXF_R->GetEntry(entry); b_TXN_R->GetEntry(entry);
      b_FW_E->GetEntry(entry);  b_FW_XF->GetEntry(entry); b_FW_XN->GetEntry(entry);
      /// only the stub is used
      int id = stubID;
      if( !TMath::IsNaN(e[id])  ){ hTrStubRise->Fill(te_r[id]); hTrStubT0->Fill(te_t[id]); hTrStub[0]->Fill(e[id]*POLARITY_E, fw_e[id]); }
      if( !TMath::IsNaN(xf[id]) ) hTrStub[1]->Fill(xf[id]*POLARITY_XFXN, fw_xf[id]);
      if( !TMath::IsNaN(xn[id]) ) hTrStub[2]->Fill(xn[id]*POLARITY_XFXN, fw_xn[id]);
      ApplyTrace(e[id],  te_r[id],  POLARITY_E,    stubTraceScale[0], fw_e[id],  &sumStub[0]);
      ApplyTrace(xf[id], txf_r[id], POLARITY_XFXN, stubTraceScale[1], fw_xf[id], &sumStub[1]);
      ApplyTrace(xn[id], txn_r[id], POLARITY_XFXN, stubTraceScale[2], fw_xn[id], &sumStub[2]);
   }
   if( isRDTTrace ){
      b_TRDT_T->GetEntry(entry); b_TRDT_R->GetEntry(entry); b_FW_RDT->GetEntry(entry);
      for( int i = 0; i < NRDT; i++){
         if( TMath::IsNaN(rdt[i]) ) continue;
         hTrRdtRise->Fill(i, trdt_r[i]); hTrRdtT0->Fill(i, trdt_t[i]);
         if( rdt[i] != 0 ) hTrRdtRatio->Fill(i, fw_rdt[i]/(rdt[i]*POLARITY_RDT));
         ApplyTrace(rdt[i], trdt_r[i], POLARITY_RDT, NULL, fw_rdt[i], NULL); /// no rescale
      }
   }
   if( isELUMTrace ){
      b_TELUM_T->GetEntry(entry); b_TELUM_R->GetEntry(entry); b_FW_ELUM->GetEntry(entry);
      for( int i = 0; i < NELUM; i++){
         if( TMath::IsNaN(elum[i]) ) continue;
         hTrElumRise->Fill(i, telum_r[i]); hTrElumT0->Fill(i, telum_t[i]);
         if( elum[i] != 0 ) hTrElumRatio->Fill(i, fw_elum[i]/(elum[i]*POLARITY_ELUM));
         ApplyTrace(elum[i], telum_r[i], POLARITY_ELUM, elumTraceScale[i], fw_elum[i], &sumElum[i]);
      }
   }

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
      for( int j = 0; j < NRDT; j++) if( j != i && !TMath::IsNaN(rdt[j]) ) hrdtMatrix->Fill(i, j); /// no self-fill : rdt[] keeps 1 hit per channel
   }
   for( int i = 0; i < NRDT/2; i++){
      hrdt2D[i]->Fill(rdt[2*i], rdt[2*i+1]);
      if( isCutFileOpen && i < numCut && ((TCutG*)cutList->At(i))->IsInside(rdt[2*i], rdt[2*i+1]) ) rdtCutGate = true;
   }

   /*********** ELUM ***************************************************/
   int elumMulti = 0;
   bool anyElumCoin = false;
   bool elumTelCoin[NRDT/2] = {false}; /// telescope (E or dE) in rdt_t - elum_t gate with any ELUM hit
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
         double tdiff = (double) ((Long64_t) rdt_t[j] - (Long64_t) elum_t[i]);
         if( isElumFineTime ) tdiff += trdt_t[j] - telum_t[i];
         htdiffElum->Fill(tdiff);
         htdiffElumVID->Fill(i, tdiff);
         if( !isTimeGateOn || (gateElum[0] < tdiff && tdiff < gateElum[1]) ){
            coin = true;
            elumTelCoin[j/2] = true;
         }
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
   for( int i = 0; i < NRDT/2; i++) if( elumTelCoin[i] ) hrdt2D_timeG[i]->Fill(rdt[2*i], rdt[2*i+1]);

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
            double tdiff = (double) ((Long64_t) rdt_t[j] - (Long64_t) e_t[stubID]);
            if( isStubFineTime ) tdiff += trdt_t[j] - te_t[stubID];
            htdiffStub->Fill(tdiff);
            htdiffStubVID->Fill(j, tdiff);
            if( !isTimeGateOn || (gateStub[0] < tdiff && tdiff < gateStub[1]) ){
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
            double s = (isStubXFPositive ? 1 : -1) * (xC - 0.5) * stubLength;
            double hX, hY, hZ;
            StubHitPos(s, hX, hY, hZ);
            double pos = (stubLongAxis == 0 ? hZ : hX);
            hStubECalVPos->Fill(pos, eC);
            hStubECalVTheta->Fill(StubAngle(hX, hY, hZ), eC);
            if( stubCoin ) hStubECalVPosG->Fill(pos, eC);
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

/// stub eCal vs position (or vs angle, isAngle) + (targ, targ) elastic line (red) + (targ2, targ2) elastic line (blue)
void DrawStubECalVPosBZ(TH2F * hist, bool isAngle = false){
   Draw2DHistBZ(hist);
   double  mX[2]   = {mTarg, mTarg2};
   int     col[2]  = {kRed, kBlue};
   TString name[2] = {elasticName, elasticName2};
   TLatex t; t.SetNDC(); t.SetTextFont(82); t.SetTextSize(0.04);
   for( int k = 0; k < 2; k++){
      TGraph * g = ElasticLineStub(mX[k], col[k], isAngle);
      if( g->GetN() == 0 ) continue;
      g->Draw("L same");
      t.SetTextColor(col[k]);
      t.DrawLatex(0.15, 0.85 - 0.05*k, name[k] + " elastic");
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
   c->cd(8); DrawStubECalVPosBZ(hStubECalVPos);
   c->cd(9); DrawStubECalVPosBZ(hStubECalVTheta, true);
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
   TCanvas * c4 = GetCanvasBZ("cRdtTimeGBZ", Form("RDT dE-E, %g < rdt_t - elum_t < %g", gateElum[0], gateElum[1]), 2, 2, 1000, 1000);
   for( int k = 0; k < NRDT/2; k++){ c4->cd(k+1); if(isLogz) gPad->SetLogz(); DrawRDTWithCut(hrdt2D_timeG[padOrder[k]], padOrder[k]); }
}

void DrawGateLines(TH1 * h, double * gate){
   if( !isTimeGateOn ) return;
   double yMax = h->GetMaximum();
   TLine * l1 = new TLine(gate[0], 0, gate[0], yMax); l1->SetLineColor(2); l1->Draw();
   TLine * l2 = new TLine(gate[1], 0, gate[1], yMax); l2->SetLineColor(2); l2->Draw();
}

void coin(){
   /// row 1 : ELUM - RDT, row 2 : stub - RDT ((a,a): alpha in stub, 20Ne in RDT)
   TCanvas * c = GetCanvasBZ("cCoinBZ", "coincidence", 3, 2, 1500, 1000);
   c->cd(1); htdiffElum->Draw(); DrawGateLines(htdiffElum, gateElum);
   c->cd(2); htdiffElumVID->Draw("colz");
   c->cd(3); Draw2DHistBZ(helumCalVThetaG);
   c->cd(4); htdiffStub->Draw(); DrawGateLines(htdiffStub, gateStub);
   c->cd(5); htdiffStubVID->Draw("colz");
   c->cd(6); Draw2DHistBZ(hStubECalVPosG);

   /// RDT gated by ELUM / stub, pads follow the physical quadrants (0 1 / 3 2)
   int padOrder[4] = {0, 1, 3, 2};
   TCanvas * c2 = GetCanvasBZ("cCoinRdtStubBZ", "RDT dE-E, stub coin.", 2, 2, 1000, 1000);
   for( int k = 0; k < NRDT/2; k++){ c2->cd(k+1); DrawRDTWithCut(hrdt2DgStub[padOrder[k]], padOrder[k]); }
   TCanvas * c3 = GetCanvasBZ("cCoinRdtElumBZ", "RDT dE-E, ELUM coin.", 2, 2, 1000, 1000);
   for( int k = 0; k < NRDT/2; k++){ c3->cd(k+1); DrawRDTWithCut(hrdt2Dg[padOrder[k]], padOrder[k]); }
}

/// trace checks : firmware vs trace (for xxxTraceScale), rise time (for traceRiseGate), T0
void trace(){
   TCanvas * c = GetCanvasBZ("cTraceBZ", "trace checks", 4, 3, 1600, 1200);
   c->cd(1);  Draw2DHistBZ(hTrStub[0]);
   c->cd(2);  Draw2DHistBZ(hTrStub[1]);
   c->cd(3);  Draw2DHistBZ(hTrStub[2]);
   c->cd(4);  gPad->SetLogy(); hTrStubRise->Draw(); DrawGateLines(hTrStubRise, traceRiseGate);
   c->cd(5);  hTrRdtRatio->Draw("colz");
   c->cd(6);  hTrRdtRise->Draw("colz");
   c->cd(7);  hTrRdtT0->Draw("colz");
   c->cd(8);  hTrStubT0->Draw();
   c->cd(9);  hTrElumRatio->Draw("colz");
   c->cd(10); hTrElumRise->Draw("colz");
   c->cd(11); hTrElumT0->Draw("colz");
   c->cd(12); htdiffStub->Draw(); DrawGateLines(htdiffStub, gateStub);
}

/// trace -> firmware scale from the LSQ sums, printed ready to paste into MonitorsBZero.C
void PrintTraceScale(){
   double nTot = 0;
   for( int k = 0; k < 3; k++)     nTot += sumStub[k].n;
   for( int i = 0; i < NELUM; i++) nTot += sumElum[i].n;
   if( nTot == 0 ){ printf("=============== trace -> firmware scale : no entries\n"); return; }
   double s0, s1;
   printf("=============== trace -> firmware scale, fw = s0 + s1 * trace * POLARITY (fw > %.0f), paste into MonitorsBZero.C\n", traceScaleFitMin);
   printf("   (current values are applied AFTER this fit, the fit is always trace raw vs firmware)\n");
   printf("double stubTraceScale[3][2] = {");
   for( int k = 0; k < 3; k++){
      if( sumStub[k].Get(s0, s1) ) printf(" {%.4f, %.6f}%s", s0, s1, k < 2 ? "," : "");
      else                         printf(" {%g, %g}%s", stubTraceScale[k][0], stubTraceScale[k][1], k < 2 ? "," : "");
   }
   printf(" }; /// e, xf, xn  (n = %.0f, %.0f, %.0f)\n", sumStub[0].n, sumStub[1].n, sumStub[2].n);
   printf("double elumTraceScale[NELUM][2] = {\n");
   for( int i = 0; i < NELUM; i++){
      if( sumElum[i].Get(s0, s1) ) printf("   {%.4f, %.6f}%s // %d, n = %.0f\n", s0, s1, i < NELUM-1 ? "," : " ", i, sumElum[i].n);
      else printf("   {%g, %g}%s // %d, n = %.0f, no fit\n", elumTraceScale[i][0], elumTraceScale[i][1], i < NELUM-1 ? "," : " ", i, sumElum[i].n);
   }
   printf("};\n");
}

/// draw the RDT cuts with Armory/RDTCutCreator.py (matplotlib window), then reload them
///   files    : root files (space separated, glob ok), "" = the processed runs
///   outFile  : "" = rdtCutFile, can be changed in the window
///   extraArg : more options for the script, e.g. "--gate \"rdt[0]>100\""
/// the ROOT prompt waits until the window is closed. range / logz / output are editable in the window
void RDTCutCreator(TString files = "", TString outFile = "", bool isLogz = false, TString extraArg = ""){
   if( files == "" ) files = processedFiles;
   if( files == "" ){ printf("RDTCutCreator : no file, give the root file(s)\n"); return; }
   if( outFile == "" ) outFile = rdtCutFile;

   TString report = gSystem->TempDirectory() + TString("/RDTCutCreator_report.txt");
   gSystem->Unlink(report);

   TString cmd;
   cmd.Form("python3 ../Armory/RDTCutCreator.py %s -o %s --emin %d --emax %d --demin %d --demax %d%s --report %s %s",
            files.Data(), outFile.Data(), rdtERange[0], rdtERange[1], rdtDERange[0], rdtDERange[1],
            isLogz ? " --logz" : "", report.Data(), extraArg.Data());
   printf("%s\n", cmd.Data());
   gSystem->Exec(cmd);

   //---- saved ? the script writes the saved path into the report file
   std::ifstream fr(report.Data());
   std::string saved;
   if( !fr.is_open() || !std::getline(fr, saved) || saved.empty() ){
      printf("=============== RDTCutCreator : no cut saved, %s unchanged\n", rdtCutFile.Data());
      return;
   }
   fr.close();
   gSystem->Unlink(report);

   TString savedPath = saved.c_str();
   TString cwd = gSystem->WorkingDirectory();
   if( savedPath.BeginsWith(cwd + "/") ) savedPath.Remove(0, cwd.Length() + 1);
   if( savedPath != rdtCutFile ){
      printf("=============== RDT cut file of this session : %s -> %s (rdtCutFile in MonitorsBZero.C unchanged)\n", rdtCutFile.Data(), savedPath.Data());
      rdtCutFile = savedPath;
   }
   LoadRDTCuts();
   printf("=============== new cuts drawn by rdt() / coin(). gated histograms (hExElumG, helumCalVThetaG, ...) use them after the next run of the monitor\n");
}

void listDrawsBZ(){
   printf("------------------- MonitorsBZero draw functions\n");
   printf("  stub()           - stub (det %d) raw, x, eCal vs pos, eCal vs theta or phi ((a,a) elastics)\n", stubID);
   printf("  elum(isLogy)     - ELUM raw / cal per ring, ID, multi, rate\n");
   printf("  Ex()             - Ex per ring, Ex vs theta, ELUM E vs theta + (a,p) lines\n");
   printf("  rdt(isLogz)      - RDT dE-E, singles, ID, dE-E gated by rdt_t - elum_t (hrdt2DX_timeG)\n");
   printf("  coin()           - ELUM-RDT & stub-RDT time diff, gated spectra, RDT dE-E gated by stub / ELUM\n");
   printf("  trace()          - trace checks : firmware vs trace, rise time, T0 (trace_runXXX.root)\n");
   printf("  PrintTraceScale()- trace -> firmware scale table\n");
   printf("-------------------------------\n");
   printf("  RDTCutCreator(files, outFile, isLogz, extraArg) - draw 4 RDT cuts (x=E, y=dE), Armory/RDTCutCreator.py\n");
   printf("                   files = \"\" : the processed runs (%s)\n", processedFiles.Data());
   printf("                   double-click closes, drag vertices to edit, x accepts, saved after 4, cuts reloaded\n");
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
   if( isArrayTrace || isRDTTrace || isELUMTrace ){
      TString tr = " | trace:";
      if( isArrayTrace ) tr += " stub";
      if( isRDTTrace   ) tr += " RDT";
      if( isELUMTrace  ) tr += " ELUM";
      canvasTitle += tr;
   }

   gStyle->SetOptStat("neiou");
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);

   int padSize = 500;
   cBZ = new TCanvas("cBZ", "B=0 | " + canvasTitle, 3*padSize, 3*padSize);
   cBZ->Divide(3, 3);

   /// layout (RDT pads follow the physical quadrant arrangement, as in Monitors.C)
   ///   hStubECalVPos | helumCalVTheta | htdiffElum
   ///   hExElum     | hrdt2D0        | hrdt2D1
   ///   hExElumG    | hrdt2D3        | hrdt2D2

   ///---- 1 stub eCal vs position along long axis, (a,a) elastics
   cBZ->cd(1); DrawStubECalVPosBZ(hStubECalVPos);

   ///---- 2 ELUM E vs theta + (a,p) kinematic lines
   cBZ->cd(2); Draw2DHistBZ(helumCalVThetaG);
   DrawExLinesTheta(helumCalVThetaG->GetXaxis()->GetXmin(), helumCalVThetaG->GetXaxis()->GetXmax());
   text.DrawLatex(0.15, 0.85, Form("lines : Ex ="));
   for( int k = 0; k < (int) exLines.size(); k++) text.DrawLatex(0.15, 0.80 - 0.05*k, Form("%.3f", exLines[k]));

   ///---- 3 tdiff ELUM - RDT
   cBZ->cd(3); htdiffElum->Draw(); DrawGateLines(htdiffElum, gateElum);

   ///---- 4 Ex from ELUM
   cBZ->cd(4); hExElum->Draw();

   ///---- 5, 6 RDT dE-E
   cBZ->cd(5); DrawRDTWithCut(hrdt2D[0], 0);
   cBZ->cd(6); DrawRDTWithCut(hrdt2D[1], 1);

   ///---- 7 Ex from ELUM, RDT gated
   cBZ->cd(7); hExElumG->Draw();
   if( isTimeGateOn ) text.DrawLatex(0.15, 0.85, Form("%g < tdiff < %g", gateElum[0], gateElum[1]));
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

   gROOT->ProcessLine(".L ../Armory/Check_rdtGate.C");
   printf("=============== loaded Armory/Check_rdtGate.C\n");

   if( isArrayTrace || isRDTTrace || isELUMTrace ) PrintTraceScale();

   listDrawsBZ();
}

#endif
