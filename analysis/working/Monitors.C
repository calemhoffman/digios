#define Monitors_cxx

#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TMath.h>
#include <TMultiGraph.h>
#include <TString.h>
#include <TLatex.h>
#include <TSystem.h>
#include <TMacro.h>
#include <TLine.h>
#include <TStopwatch.h>
#include <TCanvas.h>
#include <TBox.h>
#include <TDatime.h>
#include <TMD5.h>
#include <TObjArray.h>
#include <fstream>
#include <vector>
#include "../Cleopatra/Isotope.h"
#include "GeneralSortMapping.h"

using namespace std;

//############################################ User setting
const int numDet = 30;
const int numRow = 6;  

ULong64_t maxNumberEvent = 1000000000;

//---histogram setting
int rawEnergyRange[2] = {   100,    8000};       /// share with e, ring, xf, xn
int    energyRange[2] = {     0,      10};       /// in the E-Z plot
int     rdtDERange[2] = {     0,     4000}; 
int      rdtERange[2] = {     0,     5000};  
int    apolloRange[2] = {     0,    1000};
int      crdtRange[2] = {     0,    8000};
int      elumRange[2] = {   100,    8000};
int       TACRange[3] = { 300,   2000,   6000};  /// #bin, min, max
int      TAC2Range[3] = { 100,    400,    500};
int   thetaCMRange[2] = {0, 80};

double     exRange[3] = {  100,    -2,     10};  /// bin [keV], low[MeV], high[MeV]

int  coinTimeRange[2] = { -200, 200};
int  timeRangeUser[2] = {0, 99999999}; /// min, use when cannot find time, this set the min and max

int  icRange [3] = {1000, 1000, 500}; /// max of IC0,1,2 

bool isUseArrayTrace = false;
bool isUseRDTTrace = false;

//---Gate
bool isTimeGateOn     = true;
int timeGate[2]       = {-50, 50};             /// min, max, 1 ch = 10 ns
double eCalCut[2]     = {0.2, 50};             /// lower & higher limit for eCal
bool  isTACGate       = false;
int tacGate[2]        = {-8000, -2000};
int dEgate[2]         = {  500,  1500};
int Eresgate[2]       = { 1000,  4000};
double thetaCMGate    = 0;                    /// deg
double xGate          = 0.95;                  ///cut out the edge
vector<int> skipDetID = {11} ;//{2,  11, 17}

TString rdtCutFile1 = "";
TString rdtCutFile2 = "";
TString ezCutFile   = "";//"ezCut.root";

//TODO switches for histograms on/off
//############################################ end of user setting

int numCol;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; ///Progress bar
TStopwatch StpWatch;

//======= Canvas
TCanvas *cCanvas;
TString canvasTitle;
int lastRunID;
bool contFlag;
double runTime=0;

//======= Recoil Cut
TCutG* cutG; //!  //general temeprary pointer to cut

TObjArray * cutList1;
Bool_t isCutFileOpen1;
int numCut1;

TObjArray * cutList2;
Bool_t isCutFileOpen2;
int numCut2;

//======= Other Cuts
TCutG* EZCut;
Bool_t isEZCutFileOpen;

/******************************************************************
*   variable and histogram naming rules                           *
*   name are case sensitive, so as any C/C++ code                 *
*                                                                 *
*   ID is dettector ID                                            *
*                                                                 *
*   raw data from gen_tree are e, xf, xn, ring.                   *
*   the x from raw data is x                                      *
*                                                                 *
*   xf + xn = xs, s for sum                                       *
*                                                                 *
*   calibrated data are  eCal, xfCal, xnCal, ringCal.             *
*   the x from cal data is xCal                                   *
*                                                                 *
*   xfCal + xnCal = xsCal                                         *
*                                                                 *
*   since the z is always from xCal, so it calls z.               *
*                                                                 *
*   Excitation energy calls Ex                                    *
*                                                                 *
*                                                                 *
*   TH2D is always using "V" to seperate 2 variables, like eVx    *
*                                                                 *
*  histogram with TCutG, add suffix "GC" for Graphical-Cut.       *
*                                                                 *
*******************************************************************/
//======== raw data
TH1F* he[numDet];
TH1F* hring[numDet];
TH1F* hxf[numDet];
TH1F* hxn[numDet];

TH2F* heVring[numDet];
TH2F* hxfVxn[numDet];
TH2F* heVxs[numDet];

//TH1F* hMultiHit; //TODO

TH2F* heVID;
TH2F* heVIDG; //gated
TH2F* hringVID;
TH2F* hxfVID;
TH2F* hxnVID;

TH2F* heVx[numDet]; // e vs (xf-xn)/e
TH2F* hringVx[numDet]; // ring vs (xf-xn)/e

//====== cal data
TH2F* heVxsCal[numDet]; // raw e vs xf

TH2F* heCalVxCal[numDet]; // eCal vs xCal
TH2F* heCalVxCalG[numDet]; // eCal vs xCal
TH1F* heCal[numDet];
TH2F* heCalID; // e vs detID
TH2F* hxfCalVxnCal[numDet]; 

TH2F* heCalVz;
TH2F* heCalVzGC;
TH2F* hecalVzRow[numRow];

//====== Ex data
TH1F* hEx;
TH1F* hExi[numDet];
TH2F* hExVxCal[numDet];
TH1F* hExc[numDet/numRow];

TH2F* hExThetaCM;

TH1F* hExCut1;
TH1F* hExCut2;

//====== TAC
TH1F* htac;   // by TAC
TH1F* htac2;  // by timestamp
TH2F* htacEx;
TH2F* htac2Ex;

TH1I* htacArray[numDet];
TH2F* hrtac[4];

TH2F* htacTdiff;
TH2F* htacTdiffg;

TH2F* htacRecoil[8];
TH2F* htacRecoilsum[4];

//======= APOLLO
TH1F* hApollo[20];

//======= Recoil
TH2F* hrdtID;
TH1F* hrdt[NRDT]; // single recoil
TH1F* hrdtg[NRDT]; 

TH2F* hrdt2D[4];
TH2F* hrdt2Dsum[4];
TH2F* hrdt2Dg[4];

TH2F* hrdtMatrix; // coincident between rdt

TH1F* hrdtRate1;
TH1F* hrdtRate2;

//======= Circular Recoil
TH2F* hcrdtID;
TH1F* hcrdt[16];
TH2F* hcrdtPolar;

//======= ELUM
TH1F* helum[NELUM];
TH2F* helumID;

TH1F* helum4D; // elum rate for (d,d)
TH1F* helum4C; // elum rate for (12C, 12C)

TH1F* hBIC; // BIC, beam integrated current

//======= EZero, or IonChamber when recoil also use
TH1F* hic0; //ionChamber ch0
TH1F* hic1;
TH1F* hic2;

TH2F* hic01; //ionChamber ch0-ch1
TH2F* hic02;
TH2F* hic12;

//======= multi-Hit
TH2I *hmult;
TH1I *hmultEZ;
TH2I *hArrayRDTMatrix;
TH2I *hArrayRDTMatrixG;

//======= ARRAY-RDT time diff
TH1I *htdiff;
TH1I *htdiffg;

/***************************
 ***************************/
//==== global variables
Float_t x[numDet],z[numDet];
Float_t xcal[numDet],xfcal[numDet],xncal[numDet],eCal[numDet];
Int_t tacA[numDet];

//==== correction parameters
Float_t xnCorr[numDet];
Float_t xfxneCorr[numDet][2];
Float_t xScale[numDet];
Float_t eCorr[numDet][2];
Float_t rdtCorr[8][2];
//==== parameters for Ex and thetaCM calcualtion

double zRange[2] = {-1000, 0}; // zMin, zMax

double Ex, thetaCM;
double q, alpha, Et, betRel, gamm, G, massB, mass; //variables for Ex calculation
bool isReaction;

int padID = 0;

TLatex text;

#include "Monitors.h"

/***************************
 ***************************/
void Monitors::Begin(TTree *tree)
{
   TString option = GetOption();
   numCol = numDet/numRow;
   NumEntries = tree->GetEntries();
   
   canvasTitle.Form("Runs: ");
   lastRunID = -1;
   contFlag = false;
   
   printf("#################################################\n");
   printf("##########         Monitors.C           #########\n");
   printf("#################################################\n");
   
   printf("printControlID = %d \n", printControlID);
   
   //===================================================== loading parameter
   printf("################## loading parameter files\n"); 
   
   LoadDetGeoAndReactionConfigFile();
   LoadXFXNCorr();
   LoadXFXN2ECorr();
   LoadXScaleCorr();
   LoadECorr();
   LoadRDTCorr();
   LoadReactionPars();

   //================  Get Recoil cuts;
   cutG = new TCutG();

   TFile * fCut1 = new TFile(rdtCutFile1);
   isCutFileOpen1 = fCut1->IsOpen();
   if(!isCutFileOpen1) {
      printf( "Failed to open rdt-cutfile 1 : %s\n" , rdtCutFile1.Data());
      rdtCutFile1 = "";
   }
   numCut1 = 0 ;

   if( isCutFileOpen1 ){
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
   
   TFile * fCut2 = new TFile(rdtCutFile2);
   isCutFileOpen2 = fCut2->IsOpen();
   if(!isCutFileOpen2) {
      printf( "Failed to open rdt-cutfile 2 : %s\n" , rdtCutFile2.Data());
      rdtCutFile2 = "";
   }
   numCut2 = 0 ;
   if( isCutFileOpen2 ){
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
   
   //================  Get EZ cuts;
   TFile * fCutez = new TFile(ezCutFile);
   isEZCutFileOpen = fCutez->IsOpen(); 
   if( !isEZCutFileOpen)  printf( "Failed to open E-Z cutfile : %s\n" , ezCutFile.Data());
   if( isEZCutFileOpen ) {
      
      EZCut = (TCutG *) fCutez->FindObjectAny("cutEZ");
      if( EZCut != NULL ) {
         printf("Found EZ cut| name : %s, VarX: %s, VarY: %s, numPoints: %d \n", 
                  EZCut->GetName(),
                  EZCut->GetVarX(),
                  EZCut->GetVarY(),
                  EZCut->GetN()
            );
      }
   }
   
   //=============== Get Time stamp range
   TString branch = FindStartEndTime(tree, "e_t");
   if( (startTime == 0 && endTime == 0 ) || (startTime == endTime)  ) branch = FindStartEndTime(tree, "rdt_t");
   if( (startTime == 0 && endTime == 0 ) || (startTime == endTime)  ) branch = FindStartEndTime(tree, "ezero_t");
   if( (startTime == 0 && endTime == 0 ) || (startTime == endTime)  ) branch = FindStartEndTime(tree, "elum_t");
   
   runTime = (endTime-startTime)/1e8/60.;
   
   printf("================ Time Range in %s timestamp branch, total Entries : %lld \n", branch.Data(),  tree->GetEntries());
   printf("start time : %llu = %.1f sec = %.0f min\n", startTime, startTime/1e8, startTime/1e8/60.);
   printf("  end time : %llu = %.1f sec = %.0f min\n",  endTime, endTime/1e8, endTime/1e8/60.);
   printf("  Duration : %llu = %.1f sec = %.0f min\n",  endTime - startTime, (endTime-startTime)/1e8, runTime);
   
   int timeRange[2];
   timeRange[0] = TMath::Max((int) TMath::Floor(startTime/1e8/60.), timeRangeUser[0]);
   timeRange[1] = TMath::Min((int) TMath::Ceil(endTime/1e8/60.)   , timeRangeUser[1]);
   
   int dTime = timeRange[1] - timeRange[0];
   
   if( dTime > 0 ){
      timeRange[0] = timeRange[0] - dTime*0.1;
      timeRange[1] = timeRange[1] + dTime*0.1;  
   }else{
      dTime = 1000;
      timeRange[0] = 0;
      timeRange[1] = 1000;
   }
   
   //========================= Generate all of the histograms needed for drawing later on
   printf("======================================== Histograms declaration\n");

   gROOT->cd();
   
   heVID    = new TH2F("heVID",    "Raw e vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   heVIDG   = new TH2F("heVIDG",   "Raw e vs channel (gated)", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hringVID = new TH2F("hringVID", "Raw Ring vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxfVID   = new TH2F("hxfVID",   "Raw xf vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxnVID   = new TH2F("hxnVID",   "Raw xn vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);

   for(Int_t j=0;j<4;j++){
     hrtac[j]=new TH2F(Form("hrtac%d",j),Form("Array-Recoil tac for recoil %d",j),numDet,0,numDet,500,-500,500);
   }

   for (Int_t i=0;i<numDet;i++) {//array loop
      
      he[i]      = new TH1F(Form("he%d", i),     Form("Raw e (ch=%d); e (channel); count", i),                    2000, rawEnergyRange[0], rawEnergyRange[1]);
      hring[i]   = new TH1F(Form("hring%d", i),  Form("Raw ring (ch=%d); ring (channel); count", i),              200, rawEnergyRange[0], rawEnergyRange[1]);
      hxf[i]     = new TH1F(Form("hxf%d", i),    Form("Raw xf (ch=%d); xf (channel); count", i),                  200, rawEnergyRange[0], rawEnergyRange[1]);
      hxn[i]     = new TH1F(Form("hxn%d", i),    Form("Raw xn (ch=%d); xn (channel); count", i),                  200, rawEnergyRange[0], rawEnergyRange[1]);
      heVring[i] = new TH2F(Form("heVring%d",i), Form("Raw e vs. ring (ch=%d);ring (channel);e (channel)",i),     500, rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);
      hxfVxn[i]  = new TH2F(Form("hxfVxn%d",i),  Form("Raw xf vs. xn (ch=%d);xf (channel);xn (channel)",i),       500,                 0, rawEnergyRange[1], 500,                 0, rawEnergyRange[1]);
      heVxs[i]   = new TH2F(Form("heVxs%d", i),  Form("Raw e vs xf+xn (ch=%d); xf+xn (channel); e (channel)", i), 500, rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);
      
      heCal[i]        = new TH1F(Form("heCal%d", i),       Form("Corrected e (ch=%d); e (MeV); count", i),                                          2000,   energyRange[0], energyRange[1]);
      hxfCalVxnCal[i] = new TH2F(Form("hxfCalVxnCal%d",i), Form("Corrected XF vs. XN (ch=%d);XF (channel);XN (channel)",i),                         500,                0, rawEnergyRange[1], 500,                 0, rawEnergyRange[1]);      
      heVxsCal[i]     = new TH2F(Form("heVxsCal%d", i),    Form("Raw e vs Corrected xf+xn (ch=%d); corrected xf+xn (channel); Raw e (channel)", i), 500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);           
      heVx[i]         = new TH2F(Form("heVx%d",i),         Form("Raw PSD E vs. X (ch=%d);X (channel);E (channel)",i),                               500,             -0.1,               1.1, 500, rawEnergyRange[0], rawEnergyRange[1]);
      hringVx[i]      = new TH2F(Form("hringVx%d",i),      Form("Ring vs, X (ch=%d);X (channel);Ring (channel)",i),                                 500,             -0.1,               1.1, 500, rawEnergyRange[0], rawEnergyRange[1]);
      heCalVxCal[i]   = new TH2F(Form("heCalVxCal%d",i),   Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),                                        500,             -2.5,  detGeo.detLength + 2.5, 500,    energyRange[0],    energyRange[1]);
      heCalVxCalG[i]  = new TH2F(Form("heCalVxCalG%d",i),  Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),                                        500,             -2.5,  detGeo.detLength + 2.5, 500,    energyRange[0],    energyRange[1]);
      hExVxCal[i]     = new TH2F(Form("hExVxCal%d",i),     Form("Ex vs X (ch=%d); X (cm); Ex (MeV)", i),                                            500,             -0.1,               1.1, (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   }
   
   heCalID = new TH2F("heCalID", "Corrected E vs detID; detID; E / 10 keV", numDet, 0, numDet, 2000, energyRange[0], energyRange[1]);
   
   //====================== E-Z plot
   heCalVz   = new TH2F("heCalVz",  "E vs. Z;Z (mm);E (MeV)"      , 400, zRange[0], zRange[1], 400, energyRange[0], energyRange[1]);
   heCalVzGC = new TH2F("heCalVzGC","E vs. Z gated;Z (mm);E (MeV)", 400, zRange[0], zRange[1], 400, 0, energyRange[1]);
   
   for( int i = 0; i < numRow; i++){
      hecalVzRow[i] = new TH2F(Form("heCalVzRow%d", i), Form("E vs. Z (ch=%d-%d); Z (cm); E (MeV)", numCol*i, numCol*(i+1)-1), 500, zRange[0], zRange[1], 500, energyRange[0], energyRange[1]);
   }

   //===================== Recoils
   for (Int_t i=0;i<=NRDT;i++) {
      if( i % 2 == 0 ) hrdt[i] = new TH1F(Form("hrdt%d",i),Form("Raw Recoil E(ch=%d); E (channel)",i), 500,rdtERange[0],rdtERange[1]);
      if( i % 2 == 0 ) hrdtg[i] = new TH1F(Form("hrdt%dg",i),Form("Raw Recoil E(ch=%d) gated; E (channel)",i), 500,rdtERange[0],rdtERange[1]);
      if( i % 2 == 1 ) hrdt[i] = new TH1F(Form("hrdt%d",i),Form("Raw Recoil DE(ch=%d); DE (channel)",i), 500,rdtDERange[0],rdtDERange[1]);
      if( i % 2 == 1 ) hrdtg[i] = new TH1F(Form("hrdt%dg",i),Form("Raw Recoil DE(ch=%d) gated; DE (channel)",i), 500,rdtDERange[0],rdtDERange[1]);
      
      ///dE vs E      
      if( i % 2 == 0 ) {
         int tempID = i / 2;
         hrdt2D[tempID] = new TH2F(Form("hrdt2D%d",tempID), Form("Raw Recoil DE vs Eres (dE=%d, E=%d); Eres (channel); DE (channel)", i+1, i),             500,rdtERange[0],rdtERange[1],500,rdtDERange[0],rdtDERange[1]);
         hrdt2Dsum[tempID] = new TH2F(Form("hrdt2Dsum%d",tempID), Form("Raw Recoil DE vs Eres+DE (dE=%d, E=%d); Eres+DE (channel); DE (channel)", i+1, i), 500,rdtERange[0],rdtERange[1]+rdtDERange[1],500,rdtDERange[0],rdtDERange[1]);
         hrdt2Dg[tempID] = new TH2F(Form("hrdt2Dg%d",tempID), Form("Gated Raw Recoil DE vs Eres (dE=%d, E=%d); Eres (channel); DE (channel)",i+1, i),      500,rdtERange[0],rdtERange[1],500,rdtDERange[0], rdtDERange[1]);
      }
   }
   hrdtID = new TH2F("hrdtID", "RDT vs ID; ID; energy [ch]", 8, 0, 8, 500, TMath::Min(rdtERange[0], rdtDERange[0]), TMath::Max(rdtERange[1], rdtDERange[1])); 
   
   hrdtMatrix = new TH2F("hrdtMatrix", "RDT ID vs RDT ID", 16 , 0, 8, 16, 0, 8);
   
   hrdtRate1 = new TH1F("hrdtRate1", "recoil rate 1 / min; min; count / 1 min", timeRange[1] - timeRange[0], timeRange[0], timeRange[1]);
   hrdtRate1->SetLineColor(2);
   
   hrdtRate2 = new TH1F("hrdtRate2", "recoil rate 2 / min; min; count / 1 min", timeRange[1] - timeRange[0], timeRange[0], timeRange[1]);
   hrdtRate2->SetLineColor(4);

   //===================== APOLLO
   for (Int_t i=0;i<20;i++) {
      hApollo[i] = new TH1F(Form("hApollo%d",i),Form("Raw Apollo E(ch=%d); E (channel)",i), 250,apolloRange[0],apolloRange[1]);
   }
   
   //===================== Circular Recoil
   hcrdtID = new TH2F("hcrdtID", "Circular Recoil ID; Angular ID; Radial ID;", 8, 0, 8, 8, 0, 8);
   hcrdtPolar = new TH2F("hcrdtPolar", "Polar ID", 8, -TMath::Pi(), TMath::Pi(),8, 10, 50);

   for( int i = 0; i < 16; i++){
      hcrdt[i] = new TH1F(Form("hcrdt%d", i), Form("Raw Circular Recoil-%d", i), 500, crdtRange[0], crdtRange[1] );
   }

   //===================== multiplicity
   hmult   = new TH2I("hmult","Array Multiplicity vs Recoil Multiplicity; Array ; Recoil",10,0,10,10,0,10);
   hmultEZ = new TH1I("hmultEZ","Filled EZ with coinTime and recoil",10,0,10);
   hArrayRDTMatrix    = new TH2I("hArrayRDTMatrix", "Array ID vs Recoil ID; Array ID; Recoil ID",30,0,30,8,0,8);
   hArrayRDTMatrixG   = new TH2I("hArrayRDTMatrixG","Array ID vs Recoil ID / g; Array ID; Recoil ID",30,0,30,8,0,8);

   //===================== coincident time 
   htdiff  = new TH1I("htdiff" ,"Coincident time (recoil-dE - array); time [ch = 10ns]; count", coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);   
   htdiffg = new TH1I("htdiffg","Coincident time (recoil-dE - array) w/ recoil gated; time [ch = 10ns]; count", coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);
 
   //===================== TAC
   htac  = new TH1F("htac","Array-RF TAC; kind of time diff [a.u.]; Counts", TACRange[0], TACRange[1], TACRange[2]);
   htac2 = new TH1F("htac2","Array-RF TAC2; time diff [10 ns]; Counts", TAC2Range[0], TAC2Range[1], TAC2Range[2]);

   for (Int_t i=0;i<numDet;i++) {
      htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
   }
   
   htacEx  = new TH2F("htacEx", "Ex - TAC ; TAC [a.u.]; Ex [MeV]", TACRange[0], TACRange[1], TACRange[2], (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   htac2Ex = new TH2F("htac2Ex", "Ex - TAC2 ; TAC [10ns]; Ex [MeV]", TAC2Range[0], TAC2Range[1], TAC2Range[2], (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   
   htacTdiff  = new TH2F("htacTdiff", "TDiff vs TAC; TAC [a.u.]; tDiff [ch=10ns]", TACRange[0], TACRange[1], TACRange[2], coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);
   htacTdiffg = new TH2F("htacTdiffg", "TDiff vs TAC (recoil gate); TAC [a.u.]; tDiff [ch=10ns]", TACRange[0], TACRange[1], TACRange[2], coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);

   for (Int_t i=0; i < 8; i++){
      htacRecoil[i] = new TH2F(Form("htacRecoil%d", i), Form("RDT-%d - TAC; TAC ; RDT ", i), TACRange[0], TACRange[1] , TACRange[2], 200, 0, 4000);
      
      if( i % 2 == 0 ) {
         int tempID2 = i / 2;
         htacRecoilsum[tempID2] = new TH2F(Form("htacRecoilsum%d", tempID2), Form("(RDT-%d+RDT-%d) - TAC; TAC ; Eres+dE ", i, i+1), TACRange[0], TACRange[1], TACRange[2], 200, 0, 8000);
      }
   }
   
   //===================== energy spectrum
   hEx    = new TH1F("hEx",Form("excitation spectrum w/ goodFlag; Ex [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   
   hExCut1  = new TH1F("hExCut1",Form("excitation spectrum w/ goodFlag; Ex [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   hExCut1->SetLineColor(2);
   
   hExCut2  = new TH1F("hExCut2",Form("excitation spectrum w/ goodFlag; Ex [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   hExCut2->SetLineColor(4);
   
   for(int i = 0 ; i < numDet; i++){
      hExi[i] = new TH1F(Form("hExi%02d", i), Form("Ex (det=%i) w/goodFlag; Ex [MeV]; Count / %4.0f keV",i, exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   }
   
   for(int i = 0 ; i < numCol; i++){
       hExc[i] = new TH1F(Form("hExc%d", i), Form("Ex (col=%d) w/goodFlag; Ex [MeV]; Count / %4.0f keV", i, exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   }

   hExThetaCM = new TH2F("hExThetaCM", "Ex vs ThetaCM; ThetaCM [deg]; Ex [MeV]", 200, thetaCMRange[0], thetaCMRange[1],  (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);

   //===================== ELUM
   for( int i = 0; i < NELUM; i++){
      helum[i] = new TH1F(Form("helum%d", i), Form("Elum-%d", i), 500, elumRange[0], elumRange[1]);
   }
   helumID = new TH2F("helumID", "Elum vs ID", NELUM, 0 , NELUM, 500, elumRange[0], elumRange[1]);

   helum4D = new TH1F("helum4d", "Elum rate for Z = 1; time [min]; count / min", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]); // elum rate for (d,d)
   helum4C = new TH1F("helum4C", "Elum rate for carbon; time [min]; count / min", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]); // elum rate for (12C, 12C)
   hBIC = new TH1F("hBIC", "BIC rate ; time [min]; count / min", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]); // elum rate for (d,d)
   
   //===================== EZERO
   hic0 = new TH1F("hic0", "IC0; IC-0 [ch]; count", 500,  0, icRange[0]);
   hic1 = new TH1F("hic1", "IC1; IC-1 [ch]; count", 500,  0, icRange[1]);
   hic2 = new TH1F("hic2", "IC2; IC-2 [ch]; count", 500,  0, icRange[2]);

   hic01 = new TH2F("hic01", "IC0 - IC1; IC-1 [ch]; IC-0[ch]", 500,  0, icRange[1], 500, 0, icRange[0]);
   hic02 = new TH2F("hic02", "IC0 vs IC0+IC1; IC-2 [ch]; IC-0[ch]", 500, 0, icRange[1]+icRange[0], 500, 0, icRange[0]);
   hic12 = new TH2F("hic12", "IC1 - IC2; IC-2 [ch]; IC-1[ch]", 500, 0, icRange[1], 500, 0, icRange[2]);
   
   printf("======================================== End of histograms Declaration\n");
   StpWatch.Start();

}

/*###########################################################
 * Process
###########################################################*/
Bool_t Monitors::Process(Long64_t entry)
{

   if( ProcessedEntries > maxNumberEvent ) return kTRUE;
   ProcessedEntries++;
   
   /*********** Progress Bar ******************************************/ 
   if (ProcessedEntries>NumEntries*Frac-1) {
      TString msg; msg.Form("%llu", NumEntries/1000);
      int len = msg.Sizeof();
      printf(" %3.0f%% (%*llu/%llu k) processed in %6.1f sec | expect %6.1f sec\n",
               Frac*100, len, ProcessedEntries/1000,NumEntries/1000,StpWatch.RealTime(), StpWatch.RealTime()/Frac);
      StpWatch.Start(kFALSE);
      Frac+=0.1;
   }

   b_runID->GetEntry(entry);
   b_Energy->GetEntry(entry);
   b_XF->GetEntry(entry);
   b_XN->GetEntry(entry);
   b_Ring->GetEntry(entry);
   b_EnergyTimestamp->GetEntry(entry);

   if( isRDTExist ){
      b_RDT->GetEntry(entry);
      b_RDTTimestamp->GetEntry(entry);
   }

   if( isCRDTExist ){
      b_CRDT->GetEntry(entry);
      b_CRDTTimestamp->GetEntry(entry);
   }
   
   if( isAPOLLOExist ){
      b_APOLLO->GetEntry(entry);
      b_APOLLOTimestamp->GetEntry(entry);
   }
   
   if( isTACExist ){
      b_TAC->GetEntry(entry);
      b_TACTimestamp->GetEntry(entry);
   }
   
   if( isELUMExist ){
      b_ELUM->GetEntry(entry);
      b_ELUMTimestamp->GetEntry(entry);
   }
   
   if( isEZEROExist ) {
      b_EZERO->GetEntry(entry);
      b_EZEROTimestamp->GetEntry(entry);
   }   
   
   if( isArrayTraceExist ) {
      ///b_Trace_Energy->GetEntry(entry);
      b_Trace_Energy_RiseTime->GetEntry(entry);
      b_Trace_Energy_Time->GetEntry(entry);
   }
   
   if( isRDTTraceExist ){
      ///b_Trace_RDT->GetEntry(entry); 
      b_Trace_RDT_Time->GetEntry(entry);
      b_Trace_RDT_RiseTime->GetEntry(entry);
   }

   /*********** forming canvas Title **********************************/ 
    if( entry == 0 ) {
       if( runID == lastRunID + 1 ) {
          int len = canvasTitle.Sizeof();
          if( contFlag == false) {
             canvasTitle.Remove(len-3);
             canvasTitle += " - ";
          }
          if( contFlag == true){
             canvasTitle.Remove(len-6);
          }
          contFlag = true;
       }
       if( runID > lastRunID + 1 ) contFlag = false;
       canvasTitle += Form("%03d, ", runID );
       lastRunID = runID;
    }
    
    /*********** initization ******************************************/
    for( int i = 0 ; i < numDet; i++){
       z[i] = TMath::QuietNaN();
       x[i] = TMath::QuietNaN();
       xcal[i] = TMath::QuietNaN();
       eCal[i] = TMath::QuietNaN();
    }
    
    double rdtot[4] = {TMath::QuietNaN(), TMath::QuietNaN(), TMath::QuietNaN(), TMath::QuietNaN()};
    
    /*********** TAC **************************************************/ 
    htac->Fill(tac[2]);
   
    //if( TMath::IsNaN(tac[0]) ) return kTRUE;
    //if( !(tacGate[0] < tac[0] &&  tac[0] < tacGate[1]) ) {isTACGate=true; return kTRUE;}
    
    /*********** ELUM *************************************************/
    for( int i = 0; i < NELUM; i++){
       helum[i]->Fill(elum[i]);
       helumID->Fill(i, elum[i]);
    }
      
    if( 800 < elum[0]  && elum[0] < 1200 ) helum4D->Fill(elum_t[0]/1e8/60.); 
    
    if( !TMath::IsNaN(elum[1]) ) hBIC->Fill(elum_t[1]/1e8/60.);
    
    int tac2 = tac_t[1]-elum_t[0];        
    htac2->Fill(tac2);
    
    ///if( 4000 < elum[0]  && elum[0] < 6000 ) helum4C->Fill(elum_t[0]/1e8/60.); 
    
    /*********** Apply Recoil correction here *************************/
    
    for( int i = 0 ; i < NRDT; i++){
       rdt[i] = rdt[i]*rdtCorr[i][0] + rdtCorr[i][1];
    }
    
    /*********** Array ************************************************/ 
    //Do calculations and fill histograms
    Int_t recoilMulti = 0;
    Int_t arrayMulti = 0;
    Int_t multiEZ = 0;
    bool rdtgate1 = false;
    bool rdtgate2 = false;
    bool coinFlag = false;
    bool ezGate = false;
    bool isGoodEventFlag = false;

    for (Int_t detID = 0; detID < numDet; detID++) {
      
      //================== Filling raw data
      he[detID]->Fill(e[detID]);
      hring[detID]->Fill(ring[detID]);
      hxf[detID]->Fill(xf[detID]);
      hxn[detID]->Fill(xn[detID]);
      heVring[detID]->Fill(ring[detID],e[detID]);
      hxfVxn[detID]->Fill(xf[detID],xn[detID]);
      heVxs[detID]->Fill(xf[detID]+xn[detID], e[detID]);
      
      heVID->Fill(detID, e[detID]);
      hringVID->Fill(detID, ring[detID]);
      hxfVID->Fill(detID, xf[detID]);
      hxnVID->Fill(detID, xn[detID]);
      
            
      //==================== Basic gate
      if( TMath::IsNaN(e[detID]) ) continue ; 
      ///if( ring[detID] < -100 || ring[detID] > 100 ) continue; 
      ///if( ring[detID] > 300 ) continue; 
      if( TMath::IsNaN(xn[detID]) &&  TMath::IsNaN(xf[detID]) ) continue ; 
      
      //==================== Skip detector 
      bool skipFlag = false;
      for( unsigned int kk = 0; kk < skipDetID.size() ; kk++){
         if( detID == skipDetID[kk] ) {
            skipFlag = true;
            break;
         }
      }
      if (skipFlag ) continue;

      //==================== Basic gate
      if( TMath::IsNaN(e[detID]) ) continue ; 
      ///if( ring[detID] < -100 || ring[detID] > 100 ) continue; 
      ///if( ring[detID] > 300 ) continue; 
      if( TMath::IsNaN(xn[detID]) &&  TMath::IsNaN(xf[detID]) ) continue ; 

      //==================== Calibrations go here
      xfcal[detID] = xf[detID] * xfxneCorr[detID][1] + xfxneCorr[detID][0];
      xncal[detID] = xn[detID] * xnCorr[detID] * xfxneCorr[detID][1] + xfxneCorr[detID][0];
      eCal[detID] = e[detID] / eCorr[detID][0] + eCorr[detID][1];

      if( eCal[detID] < eCalCut[0] ) continue;
      if( eCal[detID] > eCalCut[1] ) continue;
      
      //===================== fill Calibrated  data
      heCal[detID]->Fill(eCal[detID]);
      heCalID->Fill(detID, eCal[detID]);
      hxfCalVxnCal[detID]->Fill(xfcal[detID], xncal[detID]);
      heVxsCal[detID]->Fill(xncal[detID] + xfcal[detID], e[detID]);
      
      //===================== calculate X
      if( (xf[detID] > 0 || !TMath::IsNaN(xf[detID])) && ( xn[detID]>0 || !TMath::IsNaN(xn[detID])) ) {
        ///x[detID] = 0.5*((xf[detID]-xn[detID]) / (xf[detID]+xn[detID]))+0.5;
        x[detID] = 0.5*((xf[detID]-xn[detID]) / e[detID])+0.5;
      }
      
      /// range of x is (0, 1)
      if  ( !TMath::IsNaN(xf[detID]) && !TMath::IsNaN(xn[detID]) ) xcal[detID] = 0.5 + 0.5 * (xfcal[detID] - xncal[detID] ) / e[detID];
      if  ( !TMath::IsNaN(xf[detID]) &&  TMath::IsNaN(xn[detID]) ) xcal[detID] = xfcal[detID]/ e[detID];
      if  (  TMath::IsNaN(xf[detID]) && !TMath::IsNaN(xn[detID]) ) xcal[detID] = 1.0 - xncal[detID]/ e[detID];
      
      //======= Scale xcal from (0,1)      
      xcal[detID] = (xcal[detID]-0.5)/xScale[detID] + 0.5; /// if include this scale, need to also inclused in Cali_littleTree
      
      if( abs(xcal[detID] - 0.5) > xGate/2. ) continue; 
      
      //==================== calculate Z
      if( detGeo.firstPos > 0 ) {
        z[detID] = detGeo.detLength*(1.0-xcal[detID]) + detGeo.detPos[detID%numCol];
      }else{
        z[detID] = detGeo.detLength*(xcal[detID]-1.0) + detGeo.detPos[detID%numCol];
      }

      //===================== multiplicity
      arrayMulti++; /// multi-hit when both e, xf, xn are not NaN

      //=================== Array fill
      heVx[detID]->Fill(x[detID],e[detID]);
      hringVx[detID]->Fill(x[detID],ring[detID]);
     
      heCalVxCal[detID]->Fill(xcal[detID]*detGeo.detLength,eCal[detID]);
      heCalVz->Fill(z[detID],eCal[detID]);

      //=================== Recoil Gate
      if( isRDTExist && (isCutFileOpen1 || isCutFileOpen2)){
        for(int i = 0 ; i < numCut1 ; i++ ){
            cutG = (TCutG *)cutList1->At(i) ;
            if(cutG->IsInside(rdt[2*i],rdt[2*i+1])) {
            // if(cutG->IsInside(rdt[2*i] + rdt[2*i+1],rdt[2*i+1])) {

            rdtgate1= true;
            break; /// only one is enough
          }
        }
        
        for(int i = 0 ; i < numCut2 ; i++ ){
          cutG = (TCutG *)cutList2->At(i) ;
          if(cutG->IsInside(rdt[2*i],rdt[2*i+1])) {
          //if(cutG->IsInside(rdt[2*i]+ rdt[2*i+1],rdt[2*i+1])) {

            rdtgate2= true;
            break; /// only one is enough
          }
        }
        
      }else{
        rdtgate1 = true;
        rdtgate2 = true;
      }
      
      //================ coincident with Recoil when z is calculated.
      if( !TMath::IsNaN(z[detID]) ) { 
        for( int j = 0; j < NRDT ; j++){
          if( TMath::IsNaN(rdt[j]) ) continue; 
   
          int tdiff = rdt_t[j] - e_t[detID];
   
          if( j%2 == 1) {
             hrtac[j/2]->Fill(detID,tdiff);
             htdiff->Fill(tdiff);
             htacTdiff->Fill( tac[0], tdiff);
             if((rdtgate1 || rdtgate2) && (eCalCut[1] > eCal[detID] && eCal[detID]>eCalCut[0])) {
                htdiffg->Fill(tdiff);
                htacTdiffg->Fill( tac[0], tdiff);
             }
          }

          hArrayRDTMatrix->Fill(detID, j); 
   
          if( isTimeGateOn && timeGate[0] < tdiff && tdiff < timeGate[1] ) {
            if (isTACGate && !(tacGate[0] < tac[0] &&  tac[0] < tacGate[1])) continue;
            if(j % 2 == 0 ) hrdt2Dg[j/2]->Fill(rdt[j],rdt[j+1]); /// x=E, y=dE
            ///if(j % 2 == 0 ) hrdt2Dg[j/2]->Fill(rdt[j+1],rdt[j]); /// x=dE, y=E
            hArrayRDTMatrixG->Fill(detID, j); 
            ///if( rdtgate1) hArrayRDTMatrixG->Fill(detID, j); 
            
            hrdtg[j]->Fill(rdt[j]);
            coinFlag = true;
            
          }
        }
      }
      
      if( !isTimeGateOn ) coinFlag = true;
      
      //================ E-Z gate
      if( isEZCutFileOpen ) {
         
         if( EZCut->IsInside(z[detID], eCal[detID])  ){
            ezGate = true;
         }
         
      }else{
         ezGate = true;
      }
      
      if( coinFlag && (rdtgate1 || rdtgate2) && ezGate){ 
         heCalVzGC->Fill( z[detID] , eCal[detID] );
        
         heCalVxCalG[detID]->Fill(xcal[detID]*detGeo.detLength,eCal[detID]);
         heVIDG->Fill(detID, e[detID]);
      
         multiEZ ++;
         isGoodEventFlag = true;

      }
      
    }//end of array loop
    
    if( !isEZCutFileOpen ) ezGate = true;
    
    //=========== fill eCal Vs z for each row
    for( int i = 0; i < numRow; i++){
      for(int j = 0; j < numCol; j++){
         int k = numCol*i+j;
         hecalVzRow[i] -> Fill( z[k], eCal[k]);
      }
    }
    
   /*********** RECOILS ***********************************************/    
   for( int i = 0; i < NRDT ; i++){
      hrdtID->Fill(i, rdt[i]);
      hrdt[i]->Fill(rdt[i]);
      
      for( int j = 0; j < NRDT ; j++){
         if( rdt[i] > 0 && rdt[j] > 0 )  hrdtMatrix->Fill(i, j);
      }
      
      if( i % 2 == 0  ){        
        if ( isTACGate && !(tacGate[0] < tac[0] &&  tac[0] < tacGate[1]) ) continue;        
         recoilMulti++; // when both dE and E are hit
         rdtot[i/2] = rdt[i]+rdt[i+1];
         htacRecoilsum[i/2]->Fill(tac[0],rdtot[i/2]);
         hrdt2D[i/2]->Fill(rdt[i],rdt[i+1]); //E-dE
         hrdt2Dsum[i/2]->Fill(rdtot[i/2],rdt[i+1]);//dE-(dE+E)

         htacRecoil[i]->Fill(tac[0],rdt[i]);
         htacRecoil[i+1]->Fill(tac[0],rdt[i+1]);
      }
   }
   
   /*********** Apollo ***********************************************/    
   
   for( int i = 0; i < NAPOLLO ; i++){
      hApollo[i]->Fill(apollo[i]);
   }
   
   ///if( rdt_t[4] > 0 ){
   ///   if( abs(rdt[4] - 1658) < 40) hrdtRate1->Fill(rdt_t[4]/1e8/60.);
   ///   if( abs(rdt[4] - 1783) < 40) hrdtRate2->Fill(rdt_t[4]/1e8/60.);
   ///}

   /******************* Circular Recoil *******************************/
   ///======= 0 -  7 is angular 
   ///======= 8 - 15 is radial
   
   for( int i = 0; i < 8 ; i++){
     if( TMath::IsNaN(crdt[i]) ) continue;
     hcrdt[i]->Fill(crdt[i]);
     
     for( int j = 8; j < 16; j++){
      hcrdtID->Fill(i, j);

      double theta = -TMath::Pi() + 2*TMath::Pi()/8.*(i+0.5);
      double rho   = 10.+40./8.*(j+0.5);

      hcrdtPolar->Fill( theta, rho );
    }
   }
   
   /******************* Multi-hit *************************************/
   hmultEZ->Fill(multiEZ);
   hmult->Fill(recoilMulti,arrayMulti);


   /*********** EZERO *************************************************/ 
   //if( ezGate ) {
   // hic0->Fill(ezero[0]);
   // hic1->Fill(ezero[1]);
   // hic2->Fill(ezero[2]);
   //
   // hic01->Fill(ezero[1], ezero[0]);
   // hic02->Fill(ezero[1]+ezero[0], ezero[0]);
   // hic12->Fill(ezero[2], ezero[1]);
   //   
   //}
   
   /*********** Good event Gate ***************************************/ 
   if( !isGoodEventFlag ) return kTRUE;
   
   /*********** Ex and thetaCM ****************************************/ 
   for(Int_t detID = 0; detID < numDet ; detID++){
     	
     if( TMath::IsNaN(e[detID]) ) continue ; 
     if( TMath::IsNaN(z[detID]) ) continue ;
     if( eCal[detID] <  eCalCut[0] ) continue ;
     if( eCal[detID] >  eCalCut[1] ) continue ;

     if( isReaction ){
       ///======== Ex calculation by Ryan 
       double y = eCal[detID] + mass; // to give the KE + mass of proton;
       double Z = alpha * gamm * betRel * z[detID];
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
     }else{
       Ex = TMath::QuietNaN();
       thetaCM = TMath::QuietNaN();
     }
     
     htacEx->Fill(tac[2], Ex);
     htac2Ex->Fill(tac_t[1]-e_t[detID], Ex);
     
     //if( thetaCM > thetaCMGate ) {

         hEx->Fill(Ex);

         hExThetaCM->Fill(thetaCM, Ex);
         
         if( rdtgate1 ) {
            hExCut1->Fill(Ex);
            hExThetaCM->Fill(thetaCM, Ex);
         }
         if( rdtgate2 ) {
            hExCut2->Fill(Ex);
            hExThetaCM->Fill(thetaCM, Ex);
         }
         
         hExi[detID]->Fill(Ex);
         hExVxCal[detID]->Fill(xcal[detID], Ex);
         hExc[detID%numCol]->Fill(Ex);
         
    //  }
   }
  
   return kTRUE;
}


void Monitors::Terminate()
{
   printf("============================== finishing.\n");

   gROOT->cd();
   
   int strLen = canvasTitle.Sizeof();
   canvasTitle.Remove(strLen-3);
   
   TString runTimeStr = "";
   if( runTime > 0. ) {
      runTimeStr = Form("%.0f min", runTime);
      canvasTitle += " | " + runTimeStr;
   }

   //############################################ User is free to edit this section
   //--- Canvas Size
   int canvasXY[2] = {1200 , 800} ;// x, y
   int canvasDiv[2] = {3,2};
   cCanvas  = new TCanvas("cCanvas",canvasTitle + " | " + rdtCutFile1,canvasXY[0],canvasXY[1]);
   cCanvas->Modified(); cCanvas->Update();
   cCanvas->cd(); cCanvas->Divide(canvasDiv[0],canvasDiv[1]);

   gStyle->SetOptStat("neiou");
      
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);

   double yMax = 0;

   Isotope hRecoil(reactionConfig.recoilHeavyA, reactionConfig.recoilHeavyZ);
   double Sn = hRecoil.CalSp(0,1);
   double Sp = hRecoil.CalSp(1,0);
   double Sa = hRecoil.CalSp2(4,2);
   
   //TODO, Module each plot
   ///----------------------------------- Canvas - 1
   PlotEZ(1); /// raw EZ
      
   ///----------------------------------- Canvas - 2
   PlotEZ(0); ///gated EZ

   ///----------------------------------- Canvas - 3
   PlotTDiff(1, 1); ///with Gated Tdiff, isLog
   
   ///----------------------------------- Canvas - 4
   padID++; cCanvas->cd(padID); 
   
   //hEx->Draw();
   hExCut1->Draw("");
   hExCut2->Draw("same");
   DrawLine(hEx, Sn);
   DrawLine(hEx, Sa);
   
   if(isTimeGateOn)text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   if( xGate < 1 ) text.DrawLatex(0.15, 0.75, Form("with |x-0.5|<%.4f", xGate/2.));
   if( isCutFileOpen1 ) text.DrawLatex(0.15, 0.7, "with recoil gated"); 

   ///----------------------------------- Canvas - 5
   padID++; cCanvas->cd(padID); 
   
   //Draw2DHist(hExThetaCM);
   //heVIDG->Draw();
   //text.DrawLatex(0.15, 0.75, Form("#theta_{cm} > %.1f deg", thetaCMGate));

   Draw2DHist(hrdt2D[0]);
//      Draw2DHist(hrdt2Dsum[0]);

   if( isCutFileOpen1 && numCut1 > 0 ) {cutG = (TCutG *)cutList1->At(0) ; cutG->Draw("same");}
   if( isCutFileOpen2 && numCut2 > 0 ) {cutG = (TCutG *)cutList2->At(0) ; cutG->Draw("same");}


   //helum4D->Draw();
   //text.DrawLatex(0.25, 0.3, Form("gated from 800 to 1200 ch\n"));
   
   ///----------------------------------- Canvas - 6
   PlotRDT(0,0);
   
  // padID++; cCanvas->cd(padID); 
 //  Draw2DHist(hrdtExGated);
   
   //padID++; cCanvas->cd(padID); 
   //Draw2DHist(htacEx);
   
   ///------------------------------------- Canvas - 7
   //PlotRDT(0, 0);
   
   ///----------------------------------- Canvas - 8
   //PlotRDT(1, 0);

   ///yMax = hic2->GetMaximum()*1.2;
   ///hic2->GetYaxis()->SetRangeUser(0, yMax);
   ///hic2->Draw();
   ///TBox * box14N = new TBox (-10, 0, -2, yMax);
   ///box14N->SetFillColorAlpha(2, 0.1);
   ///box14N->Draw();
   ///
   ///TBox * box14C = new TBox (8, 0, 16, yMax);
   ///box14C->SetFillColorAlpha(4, 0.1);
   ///box14C->Draw();
   ///
   ///text.SetTextColor(2); text.DrawLatex(0.38, 0.50, "14N");
   ///text.SetTextColor(4); text.DrawLatex(0.6, 0.45, "14C");
   ///text.SetTextColor(2);
   ///----------------------------------- Canvas - 9
   //padID++; cCanvas->cd(padID);  
   
   //Draw2DHist(hic01);

   ///----------------------------------- Canvas - 10
   //PlotRDT(3,0);
   
   //TH1F * helumDBIC = new TH1F("helumDBIC", "elum(d)/BIC; time [min]; count/min", timeRange[1]-timeRange[0], timeRange[0], timeRange[1]);
   //helumDBIC = (TH1F*) helum4D->Clone();
   //helumDBIC->SetTitle("elum(d)/BIC; time [min]; count/min");
   //helumDBIC->SetName("helumDBIC");
   //helumDBIC->SetLineColor(2);
   
   //helumDBIC->Divide(hBIC);
   
   //yMax = helumDBIC->GetMaximum();
   //if( yMax < hBIC->GetMaximum() ) yMax = hBIC->GetMaximum();
   
   //helumDBIC->SetMaximum(yMax * 1.2);
   //hBIC->SetMaximum(yMax * 1.2);
   
   //hBIC->Draw();
   //helumDBIC->Draw("same");
   
   //text.DrawLatex(0.15, 0.5, Form("Elum(D) / BIC \n"));
   
   ///----------------------------------- Canvas - 11
   //PlotRDT(2,0);
   
   ///----------------------------------- Canvas - 12
   //padID++; cCanvas->cd(padID);
   //htac->Draw();

   
   /*
   ///----------------------------------- Canvas - 13
   padID++; cCanvas->cd(padID);
   
   ///hicT14N->Draw("");
   ///hicT14C->Draw("same");
   ///
   ///text.SetTextColor(2); text.DrawLatex(0.15, 0.60, "14N");
   ///text.SetTextColor(4); text.DrawLatex(0.15, 0.25, "14C");
   ///text.SetTextColor(2);
   
   ///----------------------------------- Canvas - 14
   padID++; cCanvas->cd(padID);
   
   ///hrdtRate1->Draw("");
   ///hrdtRate2->Draw("same");
   
   ///----------------------------------- Canvas - 15
   padID++; cCanvas->cd(padID);  
      
   ///----------------------------------- Canvas - 16
   padID++; cCanvas->cd(padID); 
   


   ///----------------------------------- Canvas - 17
   padID++; cCanvas->cd(padID);    
   

   ///----------------------------------- Canvas - 18
   padID++; cCanvas->cd(padID);


   ///----------------------------------- Canvas - 19
   padID++; cCanvas->cd(padID);
   

   
   ///----------------------------------- Canvas - 20
   padID++; cCanvas->cd(padID);
   
   htac->Draw();
   */
   
   /************************************/
   gStyle->GetAttDate()->SetTextSize(0.02);
   gStyle->SetOptDate(1);
   gStyle->SetDateX(0);
   gStyle->SetDateY(0);
   
   /************************************/
   StpWatch.Start(kFALSE);
   
   gROOT->ProcessLine(".L ../Armory/Monitors_Util.C");
   //gROOT->ProcessLine(Form("FindBesCanvasDivision(%d)", numDet));
   printf("=============== loaded Monitors_Utils.C\n");
   gROOT->ProcessLine(".L ../Armory/AutoFit.C");
   printf("=============== loaded Armory/AutoFit.C\n");
   gROOT->ProcessLine(".L ../Armory/RDTCutCreator.C");
   printf("=============== loaded Armory/RDTCutCreator.C\n");
   gROOT->ProcessLine(".L ../Armory/Check_rdtGate.C");
   printf("=============== loaded Armory/Check_rdtGate.C\n");
   gROOT->ProcessLine(".L ../Armory/readTrace.C");
   printf("=============== loaded Armory/readTrace.C\n");
   gROOT->ProcessLine(".L ../Armory/readRawTrace.C");
   printf("=============== loaded Armory/readRawTrace.C\n");
   gROOT->ProcessLine("listDraws()");
   
   /************************* Save histograms to root file*/
   
   gROOT->cd();
   //TString outFileNameTemp = canvasTitle;
   //outFileNameTemp.ReplaceAll(" - ", "-").ReplaceAll(" ", "_").ReplaceAll(":", "").ReplaceAll(",","");
   //gROOT->GetList()->SaveAs(outFileName + ".root");
   
   if( printControlID == 0 ) {
      TDatime dateTime;
      TString outFileName;
      outFileName.Form("Canvas_%d%02d%02d_%06d.png", dateTime.GetYear(), dateTime.GetMonth(), dateTime.GetDay(), dateTime.GetTime());
      
      cCanvas->SaveAs("Canvas/"+outFileName);
      
      /// run puhs_to_websrv.sh
      TString cmd;
      printf("|%s|\n", canvasTitle.Data());
      
      cmd.Form(".! ../Armory/push_to_websrv.sh %s %s", outFileName.Data(), canvasTitle.ReplaceAll(" ", "").ReplaceAll("|","_").Data()); 
      gROOT->ProcessLine(cmd);
       
      ///======================== exit after 
      gROOT->ProcessLine(".q");
   }
   
   
   /************************************/
   //gROOT->ProcessLine("recoils()");


}
