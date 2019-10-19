#define Monitors_cxx

#include "Monitors.h"
#include <TH2.h>
#include <TH1.h>
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
#include <TBox.h>
#include <TMD5.h>
#include <TObjArray.h>
#include <fstream>
using namespace std;

//############################################ User setting
const int numDet = 30;
const int numRow = 6;  

ULong64_t maxNumberEvent = 1000000000;

//---histogram setting
int rawEnergyRange[2] = { 100, 12000}; // share with e, ring, xf, xn
int    energyRange[2] = {   0,    20}; // in the E-Z plot
int     rdtDERange[2] = {   0,  2000};
int      rdtERange[2] = {   0,  160};
int      elumRange[2] = { 200,  4000};

double     exRange[3] = {  50, -1, 9}; // bin [keV], low[MeV], high[MeV]
int  coinTimeRange[2] = { -40, 40};


double rdtot[4] = {};

//---Gate
int timeGate[2] = {-10, 5}; // min, max, 1 ch = 10 ns
int tacGate[2] = {-8000, 0};
int dEgate[2] = {500,1500};
int Eresgate[2] = {1000,4000};

TString rdtCutFile = "rdtCuts.root";

TString ezCutFile = "ezCut.root";

//TODO switches for histograms on/off
//############################################ end of user setting

int numCol;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar
TStopwatch StpWatch;

//======= Canvas
TCanvas *cCanvas;
TCanvas *cEX;
TString canvasTitle;
int lastRunID;
bool contFlag;

//======= Recoil Cut
TString cutName("cut1");
TCutG* cutG; //!
TObjArray * cutList;
TString cutTag;
Bool_t isCutFileOpen;
int numCut;
vector<int> countFromCut;

//======= Other Cuts
TCutG* EZCut;
Bool_t isEZCutFileOpen;

/***************************************************
 variable and histogram naming rules
 name are case sensitive, so as any C/C++ code
  
 ID is dettector ID
 
 raw data from gen_tree are e, xf, xn, ring.
 the x from raw data is x
 
 xf + xn = xs, s for sum

 calibrated data are  eCal, xfCal, xnCal, ringCal.
 the x from cal data is xCal
  
 xfCal + xnCal = xsCal

 since the z is always from xCal, so it calls z.
 
 Excitation energy calls Ex 
 
 
 TH2D is always using "V" to seperate 2 variables, like eVx
 
histogram with TCutG, add suffix "GC" for Graphical-Cut.
   
***************************************************/
//======== raw data
TH1F* he[numDet];
TH1F* hring[numDet];
TH1F* hxf[numDet];
TH1F* hxn[numDet];

TH2F* heVring[numDet];
TH2F* hxfVxn[numDet];
TH2F* heVxs[numDet];

TH1F* hMultiHit; //TODO

TH2F* heVID;
TH2F* hringVID;
TH2F* hxfVID;
TH2F* hxnVID;

TH2F* heVx[numDet]; // e vs (xf-xn)/e
TH2F* hringVx[numDet]; // ring vs (xf-xn)/e

//====== cal data
TH2F* heVxsCal[numDet]; // raw e vs xf

TH2F* heCalVxCal[numDet]; // eCal vs xCal
TH1F* heCal[numDet];
TH2F* heCalID; // e vs detID
TH2F* hxfCalVxnCal[numDet]; 

TH2F* heCalVz;
TH2F* heCalVzGC;
TH2F* hecalVzRow[numRow];

//====== Ex data
TH1F* hEx;
TH1F* hExi[numDet];

TH2F* hExThetaCM;

//====== TAC
TH1F* htac;
TH1I* htacArray[numDet];
TH2F* hrtac[4];
TH2F* htacEx;
TH2F* htacRecoil[8];
TH2F* htacRecoilsum[4];

//======= Recoil
TH2F* hrdtID;
TH1F* hrdt[8]; // single recoil
TH2F* hrdt2D[4];
TH2F* hrdt2Dsum[4];
TH2F* hrdt2Dg[4];

//======= ELUM
TH1F* helum[16];
TH1F* helumSUM;
TH2F* helumID;

//======= EZero, or IonChamber
TH1F* hic0; //ionChamber ch0
TH1F* hic1;
TH1F* hic2;

TH2F* hic01; //ionChamber ch0-ch1
TH2F* hic02;
TH2F* hic12;

/*
TH2F* he0dee;
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;
*/

//======= multi-Hit
TH2I *hmult;
TH1I *hmultEZ;
TH1I *htdiff;
TH1I *htdiffg;
TH2I *hID2;
TH2I *hID2g;

int count1, count2;

/***************************
 ***************************/
//==== global variables
Float_t x[numDet],z[numDet];
Float_t xcal[numDet],xfcal[numDet],xncal[numDet],eCal[numDet],ezero[10];
Int_t tacA[numDet];

//==== correction parameters
Float_t xnCorr[numDet];
Float_t xfxneCorr[numDet][2];
Float_t eCorr[numDet][2];

//==== parameters for Ex and thetaCM calcualtion

double length ; // detector z-length 
double firstPos;
vector<double> pos;
double a ; // perpendicular distance of detector to axis [mm]
double Bfield ;

double Ex, thetaCM;
double q, alpha, Et, beta, gamm, G, massB, mass; //variables for Ex calculation
bool isReaction;

/***************************
 ***************************/

void Monitors::Begin(TTree *tree)
{
   TString option = GetOption();
   
   numCol = numDet/numRow;
   
   NumEntries = tree->GetEntries();
   
   //canvasTitle.Form("#Events:%lld, Runs: ", NumEntries);
   canvasTitle.Form("Runs: ");
   lastRunID = -100;
   contFlag = false;
   
      
   printf("------------ %d \n", A); //testing code for TSelector input
   
   //===================================================== loading parameter
   printf("################## loading parameter files\n"); 

   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   printf("=======================\n");
   printf(" loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   
   double zRange[2] = {-1000, 0}; // zMin, zMax
   
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) Bfield  = atof(x.c_str());
         if( i == 3 )     a   = atof(x.c_str());
         if( i == 5 ) length   = atof(x.c_str());
         if( i == 14 ) firstPos = atof(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < numCol; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < numCol ; i++){
         if( firstPos > 0 ){
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      
      if( firstPos > 0 ){
         zRange[0] = pos[0] - 30;
         zRange[1] = pos[numCol-1] + length + 30;
      }else{
         zRange[0] = pos[numCol-1] -length - 30;
         zRange[1] = pos[0] + 30;
      }
      
      printf("=======================\n");
      
   }else{
      printf("... fail\n");
   }
   
   //========================================= xf = xn correction
   printf(" loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("..... done.\n");
   }else{
      printf("..... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   
   printf(" loading xf/xn-e correction.");
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
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   
   printf(" loading e correction.");
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
      printf("......... done.\n");
      
   }else{
      printf("......... fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
   }
   file.close();
   
   //========================================= reaction parameters
   //check is the transfer.root is using the latest reactionConfig.txt   
   //sicne reaction.dat is generated as a by-product of transfer.root
   TFile * transfer = new TFile("transfer.root");
   TString aaa1 = "";
   TString aaa2 = "";
   if( transfer->IsOpen() ){
      TMacro * reactionConfig = (TMacro *) transfer->FindObjectAny("reactionConfig");
      TMacro presentReactionConfig ("reactionConfig.txt");
      aaa1 = ((TMD5*) reactionConfig->Checksum())->AsString();
      aaa2 = ((TMD5*) presentReactionConfig.Checksum())->AsString();
   }
   //printf("%s\n", aaa1.Data());
   //printf("%s\n", aaa2.Data());

   if( aaa1 != aaa2 ) {
     printf("########################## recalculate transfer.root \n");
     system("../Cleopatra/Transfer");
     printf("########################## transfer.root updated\n");
   }
   printf(" loading reaction parameters");
   file.open("reaction.dat");
   isReaction = false;
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x ){
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) mass = atof(x.c_str());
         if( i == 1 ) q    = atof(x.c_str());
         if( i == 2 ) beta = atof(x.c_str()); 
         if( i == 3 ) Et   = atof(x.c_str()); 
         if( i == 4 ) massB = atof(x.c_str()); 
         i = i + 1;
      }
      printf("... done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.; //MeV/mm
      gamm = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamm * beta * a ;
      printf("\tmass-b    : %f MeV/c2 \n", mass);
      printf("\tcharge-b  : %f \n", q);
      printf("\tE-total   : %f MeV \n", Et);
      printf("\tmass-B    : %f MeV/c2 \n", massB);		 
      printf("\tbeta      : %f \n", beta);
      printf("\tB-field   : %f T \n", Bfield);
      printf("\tslope     : %f MeV/mm \n", alpha * beta);
      printf("\tdet radius: %f mm \n", a);
      printf("\tG-coeff   : %f MeV \n", G);
      printf("=================================\n");

   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();


   //================  Get Recoil cuts;
   TFile * fCut = new TFile(rdtCutFile);	
   isCutFileOpen = fCut->IsOpen();
   if(!isCutFileOpen) printf( "Failed to open cutfile : %s\n" , rdtCutFile.Data());
   numCut = 0 ;
   if( isCutFileOpen ){
      cutList = (TObjArray *) fCut->FindObjectAny("cutList");
      numCut = cutList->GetEntries();
      printf("=========== found %d cutG in %s \n", numCut, fCut->GetName());

      cutG = new TCutG();
      for(int i = 0; i < numCut ; i++){
         printf("cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
            cutList->At(i)->GetName(),
            ((TCutG*)cutList->At(i))->GetVarX(),
            ((TCutG*)cutList->At(i))->GetVarY(),
            ((TCutG*)cutList->At(i))->GetN());
         countFromCut.push_back(0);	
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

   //================  Get other cuts
   //Ryan comment : the following is not so correct, but do the job
   /*
   TFile *cutfile = new TFile("6Li_cuts.root");
   cutfileOpen = cutfile->IsOpen();
   if(!cutfileOpen) cout<<"Failed to open cutfile"<<endl;
   cutfile->ls();
   EvsZ_cut=(TCutG*) gROOT->FindObject("EvsZ_cut");
   cutfile->Close();
   */

   printf("======================================== Histograms declaration\n");
   
   gROOT->cd();
   
   //========================= Generate all of the histograms needed for drawing later on
   heVID    = new TH2F("heVID",    "Raw e vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hringVID = new TH2F("hringVID", "Raw Ring vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxfVID   = new TH2F("hxfVID",   "Raw xf vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxnVID   = new TH2F("hxnVID",   "Raw xn vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);

   for(Int_t j=0;j<4;j++){
     hrtac[j]=new TH2F(Form("hrtac%d",j),Form("Array-Recoil tac for recoil %d",j),numDet,0,numDet,500,-500,500);
   }

   for (Int_t i=0;i<numDet;i++) {//array loop
      
      he[i] = new TH1F(Form("he%d", i), 
                       Form("Raw e (ch=%d); e (channel); count", i),
                       200, rawEnergyRange[0], rawEnergyRange[1]);
                       
      hring[i] = new TH1F(Form("hring%d", i), 
                       Form("Raw ring (ch=%d); ring (channel); count", i),
                       200, rawEnergyRange[0], rawEnergyRange[1]);
                       
      hxf[i] = new TH1F(Form("hxf%d", i), 
                       Form("Raw xf (ch=%d); xf (channel); count", i),
                       200, rawEnergyRange[0], rawEnergyRange[1]);
                       
      hxn[i] = new TH1F(Form("hxn%d", i), 
                       Form("Raw xn (ch=%d); xn (channel); count", i),
                       200, rawEnergyRange[0], rawEnergyRange[1]);
      
      heVring[i] = new TH2F(Form("heVring%d",i),
                           Form("Raw e vs. ring (ch=%d);ring (channel);e (channel)",i),
                           500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);
                           
      hxfVxn[i] = new TH2F(Form("hxfVxn%d",i),
                           Form("Raw xf vs. xn (ch=%d);xf (channel);xn (channel)",i),
                           500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);
      
      heVxs[i] = new TH2F(Form("heVxs%d", i), 
                       Form("Raw e vs xf+xn (ch=%d); xf+xn (channel); e (channel)", i),
                       500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);
      
      heCal[i] = new TH1F(Form("heCal%d", i), 
                       Form("Corrected e (ch=%d); e (MeV); count", i),
                       200, energyRange[0], energyRange[1]);
      
      hxfCalVxnCal[i] = new TH2F(Form("hxfCalVxnCal%d",i),
                           Form("Corrected XF vs. XN (ch=%d);XF (channel);XN (channel)",i),
                           500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);      
                           
      heVxsCal[i] = new TH2F(Form("heVxsCal%d", i), 
                       Form("Raw e vs Corrected xf+xn (ch=%d); corrected xf+xn (channel); Raw e (channel)", i),
                       500,rawEnergyRange[0], rawEnergyRange[1], 500, rawEnergyRange[0], rawEnergyRange[1]);           
      
      heVx[i] = new TH2F(Form("heVx%d",i),
                           Form("Raw PSD E vs. X (ch=%d);X (channel);E (channel)",i),
                           500,-0.1,1.1,500,rawEnergyRange[0], rawEnergyRange[1]);
                           
      hringVx[i] = new TH2F(Form("hringVx%d",i),
                           Form("Ring vs, X (ch=%d);X (channel);Ring (channel)",i),
                           500,-0.1,1.1,500,rawEnergyRange[0], rawEnergyRange[1]);
                           
      heCalVxCal[i] = new TH2F(Form("heCalVxCal%d",i),
                           Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),
                           500,-02.5,52.5,500,energyRange[0], energyRange[1]);
   }
   
   heCalID = new TH2F("heCalID", "Corrected E vs detID; detID; E / 10 keV", numDet, 0, numDet, 500, energyRange[0], energyRange[1]);
   
   //====================== E-Z plot
   heCalVz   = new TH2F("heCalVz",  "E vs. Z;Z (mm);E (MeV)"      , 400, zRange[0], zRange[1], 400, energyRange[0], energyRange[1]);
   heCalVzGC = new TH2F("heCalVzGC","E vs. Z gated;Z (mm);E (MeV)", 400, zRange[0], zRange[1], 400, 0, energyRange[1]);
   
   for( int i = 0; i < numRow; i++){
      hecalVzRow[i] = new TH2F(Form("heCalVzRow%d", i),
                               Form("E vs. Z (ch=%d-%d); Z (cm); E (MeV)", numCol*i, numCol*(i+1)-1),
                               500, zRange[0], zRange[1], 500, energyRange[0], energyRange[1]);
   }

   //===================== Recoils
   for (Int_t i=0;i<8;i++) {
      if( i % 2 == 0 ) hrdt[i] = new TH1F(Form("hrdt%d",i),Form("Raw Recoil E(ch=%d); E (channel)",i), 500,rdtERange[0],rdtERange[1]);
      if( i % 2 == 1 ) hrdt[i] = new TH1F(Form("hrdt%d",i),Form("Raw Recoil DE(ch=%d); DE (channel)",i), 500,rdtDERange[0],rdtDERange[1]);
      
      //dE vs E      
      if( i % 2 == 0 ) {
         int tempID = i / 2;
         hrdt2D[tempID] = new TH2F(Form("hrdt2D%d",tempID), Form("Raw Recoil DE vs Eres (dE=%d, E=%d); Eres (channel); DE (channel)", i+1, i), 500,rdtERange[0],rdtERange[1],500,rdtDERange[0],rdtDERange[1]);
         hrdt2Dsum[tempID] = new TH2F(Form("hrdt2Dsum%d",tempID), Form("Raw Recoil DE vs Eres+DE (dE=%d, E=%d); Eres+DE (channel); DE (channel)", i+1, i), 500,rdtERange[0],rdtERange[1]+rdtDERange[1],500,rdtDERange[0],rdtDERange[1]);
         hrdt2Dg[tempID] = new TH2F(Form("hrdt2Dg%d",tempID), Form("Gated Raw Recoil DE vs Eres (dE=%d, E=%d); Eres (channel); DE (channel)",i+1, i), 500,rdtERange[0],rdtERange[1],500,rdtDERange[0], rdtDERange[1]);
      }
      
   }
   hrdtID = new TH2F("hrdtID", "RDT vs ID; ID; energy [ch]", 8, 0, 8, 500, TMath::Min(rdtERange[0], rdtDERange[0]), TMath::Max(rdtERange[1], rdtDERange[1])); 
   
   //===================== multiplicity
   hmult   = new TH2I("hmult","Array Multiplicity vs Recoil Multiplicity; Array ; Recoil",10,0,10,10,0,10);
   hmultEZ = new TH1I("hmultEZ","Filled EZ with coinTime and recoil",10,0,10);
   hID2    = new TH2I("hID2", "Array ID vs Recoil ID; Array ID; Recoil ID",30,0,30,8,0,8);
   hID2g   = new TH2I("hID2g","Array ID vs Recoil ID / g; Array ID; Recoil ID",30,0,30,8,0,8);

   //===================== coincident time 
   htdiff  = new TH1I("htdiff" ,"Coincident time (array, recoil); time [ch = 10ns]; count", coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);   
   htdiffg = new TH1I("htdiffg","Coincident time (array, recoil) w/ recoil gated; time [ch = 10ns]; count", coinTimeRange[1] - coinTimeRange[0], coinTimeRange[0], coinTimeRange[1]);
 
   //===================== TAC
   htac = new TH1F("htac","Array-RF TAC; kind of time diff [a.u.]; Counts", 4000, -5000, -1000);

   for (Int_t i=0;i<numDet;i++) {
      htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
   }
   
   htacEx = new TH2F("htacEx", "Ex - TAC ; TAC [a.u.]; Ex [MeV]", 200, -5000, -1000, (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);

   for (Int_t i=0; i < 8; i++){
      htacRecoil[i] = new TH2F(Form("htacRecoil%d", i), Form("RDT-%d - TAC; TAC ; RDT ", i), 200, -5000 , -1000 , 200, 0, 4000);
      
      if( i % 2 == 0 ) {
         int tempID2 = i / 2;
         htacRecoilsum[tempID2] = new TH2F(Form("htacRecoilsum%d", tempID2), Form("(RDT-%d+RDT-%d) - TAC; TAC ; Eres+dE ", i, i+1), 200, -2500 , -1000 , 200, 0, 8000);
      }
   }

   //===================== energy spectrum
   hEx  = new TH1F("hEx",Form("excitation spectrum w/ goodFlag; Ex [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   for(int i = 0 ; i < numDet; i++){
      hExi[i] = new TH1F(Form("hExi%02d", i), Form("Ex (det=%i) w/goodFlag; Ex [MeV]; Count / %4.0f keV",i, exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   }

   hExThetaCM = new TH2F("hExThetaCM", "Ex vs ThetaCM; ThetaCM [deg]; Ex [MeV]", 200, 0, 50,  (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);

   //===================== ELUM
   for( int i = 0; i < 16; i++){
      helum[i] = new TH1F(Form("helum%d", i), Form("Elum-%d", i), 200, elumRange[0], elumRange[1]);
   }
   helumID = new TH2F("helumID", "Elum vs ID", 16, 0 , 16, 200, elumRange[0], elumRange[1]);
   helumSUM = new TH1F("helumSUM", "ElumSUM", 200, elumRange[0], elumRange[1]);
   
   
   //===================== EZERO
   
   hic0 = new TH1F("hic0", "IC0; IC-0 [ch]; count", 500, 0, 1000);
   hic1 = new TH1F("hic1", "IC1; IC-1 [ch]; count", 500, 0, 1000);
   hic2 = new TH1F("hic2", "IC2; IC-2 [ch]; count", 500, 0, 1000);
   
   hic01 = new TH2F("hic01", "IC0 - IC1; IC-1 [ch]; IC-0[ch]", 1000, 0, 1000, 1000, 0, 1000);
   hic02 = new TH2F("hic02", "IC0 - IC2; IC-2 [ch]; IC-0[ch]", 500, 0, 1000, 500, 0, 1000);
   hic12 = new TH2F("hic12", "IC1 - IC2; IC-2 [ch]; IC-1[ch]", 500, 0, 1000, 500, 0, 1000);
   
   /*
   he0dee = new TH2F("he0dee","EZERO DE-E; E [ch]; DE [ch]",500,0,8000,500,0,8000);//ezero
   he0det = new TH2F("he0det","EZERO DE-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   he0et = new TH2F("he0et","EZERO E-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   h0detet = new TH1F("h0detet","EZERO DE Time - E Time; DET-ET [ch]",500,-250,250);//
   h0dettact = new TH1F("h0dettact","EZERO DE Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0ettact = new TH1F("h0ettact","EZERO E Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0de = new TH1F("h0de","EZERO DE ; DE [ch]",500,50,4050);//
   h0e = new TH1F("h0e","EZERO - E; E [ch]",500,50,4050);//
   h0tac = new TH1F("h0tac","EZERO RF; RF [ch]",500,50,4050);//
   */
   printf("======================================== End of histograms Declaration\n");
   StpWatch.Start();
   
   count1 = 0;
   count2 = 0;
}

void Monitors::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

double solid_angle( double th ) {
   
   double ang = 2. * TMath::Pi() * TMath::Sin( th*TMath::DegToRad() );
   
   return 1.0 / ( ang * TMath::RadToDeg() );
   
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
   b_RDT->GetEntry(entry);
   b_TAC->GetEntry(entry);
   b_ELUM->GetEntry(entry);
   b_EZERO->GetEntry(entry);
   b_EnergyTimestamp->GetEntry(entry);
   b_RDTTimestamp->GetEntry(entry);
   b_TACTimestamp->GetEntry(entry);
   b_ELUMTimestamp->GetEntry(entry);
   b_EZEROTimestamp->GetEntry(entry);
    
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
    
    //initization
    for( int i = 0 ; i < numDet; i++){
       z[i] = TMath::QuietNaN();
       x[i] = TMath::QuietNaN();
       xcal[i] = TMath::QuietNaN();
       eCal[i] = TMath::QuietNaN();
    }
    
    /*********** TAC ************************************************/ 
    htac->Fill(tac[0]);
   
    //if( TMath::IsNaN(tac[0]) ) return kTRUE;
    //if( !(-1800 < tac[0] &&  tac[0] < -800) ) return kTRUE;
    
    /*********** ELUM ************************************************/
    for( int i = 0; i < 16; i++){
       helum[i]->Fill(elum[i]);
       helumID->Fill(i, elum[i]);
       helumSUM->Fill(elum[i]);
    }

    /*********** Array ************************************************/ 
    //Do calculations and fill histograms
    Int_t recoilMulti = 0;
    Int_t arrayMulti = 0;
    Int_t multiEZ = 0;
    bool rdtgate = false;
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
      if( !(tacGate[0] < tac[0] &&  tac[0] < tacGate[1]) ) continue; 
      if( TMath::IsNaN(e[detID]) ) continue ; 
      if( ring[detID] < -100 || ring[detID] > 100 ) continue; 
      //if( ring[detID] > 300 ) continue; 
      if( TMath::IsNaN(xn[detID]) &&  TMath::IsNaN(xf[detID]) ) continue ; 

      //==================== Calibrations go here
      xfcal[detID] = xf[detID] * xfxneCorr[detID][1] + xfxneCorr[detID][0];
      xncal[detID] = xn[detID] * xnCorr[detID] * xfxneCorr[detID][1] + xfxneCorr[detID][0];
      eCal[detID] = e[detID] / eCorr[detID][0] + eCorr[detID][1];

      if( eCal[detID] < 0.5 ) continue;

      //===================== fill Calibrated  data
      heCal[detID]->Fill(eCal[detID]);
      heCalID->Fill(detID, eCal[detID]);
      hxfCalVxnCal[detID]->Fill(xfcal[detID], xncal[detID]);
      heVxsCal[detID]->Fill(xncal[detID] + xfcal[detID], e[detID]);
      
      //===================== calculate X
      if( (xf[detID] > 0 || !TMath::IsNaN(xf[detID])) && ( xn[detID]>0 || !TMath::IsNaN(xn[detID])) ) {
        //x[detID] = 0.5*((xf[detID]-xn[detID]) / (xf[detID]+xn[detID]))+0.5;
        x[detID] = 0.5*((xf[detID]-xn[detID]) / e[detID])+0.5;
      }
      
      // range of x is (0, 1)
      if  ( !TMath::IsNaN(xf[detID]) && !TMath::IsNaN(xn[detID]) ) xcal[detID] = 0.5 + 0.5 * (xfcal[detID] - xncal[detID] ) / e[detID];
      if  ( !TMath::IsNaN(xf[detID]) &&  TMath::IsNaN(xn[detID]) ) xcal[detID] = xfcal[detID]/ e[detID];
      if  (  TMath::IsNaN(xf[detID]) && !TMath::IsNaN(xn[detID]) ) xcal[detID] = 1.0 - xncal[detID]/ e[detID];
      //if  (  TMath::IsNaN(xn[detID]) &&  TMath::IsNaN(xf[detID]) ) xcal[detID] = TMath::QuietNaN();
      
      /*
      if  ( !TMath::IsNaN(xf[detID]) && !TMath::IsNaN(xn[detID]) ) {
        if (xfcal[detID]>0.5*e[detID]) {
          xcal[detID] = xfcal[detID]/e[detID];
        }else if {
          xcal[detID] = 1.0 - xncal[detID]/e[detID];
        }
      }*/
      
      //==================== calculate Z
      if( firstPos > 0 ) {
        z[detID] = length*(1.0-xcal[detID]) + pos[detID%numCol];
      }else{
        z[detID] = length*(xcal[detID]-1.0) + pos[detID%numCol];
      }

      //===================== multiplicity
      arrayMulti++; // multi-hit when both e, xf, xn are not NaN

      //=================== Array fill
      heVx[detID]->Fill(x[detID],e[detID]);
      hringVx[detID]->Fill(x[detID],ring[detID]);
     
      heCalVxCal[detID]->Fill(xcal[detID]*length,eCal[detID]);
      heCalVz->Fill(z[detID],eCal[detID]);

      int iRow = detID/numCol;
      hecalVzRow[iRow] -> Fill( z[detID], eCal[detID]);

      //=================== Recoil Gate
      if( isCutFileOpen ){
        for(int i = 0 ; i < numCut ; i++ ){
          cutG = (TCutG *)cutList->At(i) ;
          if(cutG->IsInside(rdt[2*i],rdt[2*i+1])) {
            rdtgate= true;
            break; // only one is enough
          }
        }
      }else{
        rdtgate = true;
      }      
      
      //================ coincident with Recoil when z is calculated.
      if( !TMath::IsNaN(z[detID]) ) { 
        for( int j = 0; j < 8 ; j++){
          if( TMath::IsNaN(rdt[j]) ) continue; 
	 
          int tdiff = rdt_t[j] - e_t[detID];
	 
          if(j==1||j==3||j==5||j==7) hrtac[j/2]->Fill(detID,tdiff);
        
          htdiff->Fill(tdiff);
          if(rdtgate && eCal[detID]>0) htdiffg->Fill(tdiff);

          hID2->Fill(detID, j); 
	 
          if( timeGate[0] < tdiff && tdiff < timeGate[1] ) {
            if ((tacGate[0] < tac[0] &&  tac[0] < tacGate[1])) {
               if(j % 2 == 0 ) hrdt2Dg[j/2]->Fill(rdt[j],rdt[j+1]);
               hID2g->Fill(detID, j); 
               //if( rdtgate) hID2g->Fill(detID, j); 
               coinFlag = true;
            }
          }
        }
      }
      
      //================ E-Z gate
      if( isEZCutFileOpen ) {
         
         if( EZCut->IsInside(z[detID], eCal[detID])  ){
            ezGate = true;
         }
         
      }else{
         ezGate = true;
      }
      

      if( coinFlag && rdtgate && ezGate) {
        heCalVzGC->Fill( z[detID] , eCal[detID] );
        multiEZ ++;
        isGoodEventFlag = true;
        count2++;
      }
      
    }//end of array loop
    
    if( !isEZCutFileOpen ) ezGate = true;
    
   /*********** RECOILS ************************************************/    
   for( int i = 0; i < 8 ; i++){
      hrdtID->Fill(i, rdt[i]);
      hrdt[i]->Fill(rdt[i]);
      
      if( i % 2 == 0 && tacGate[0] < tac[0] &&  tac[0] < tacGate[1] ){
         recoilMulti++; // when both dE and E are hit
         rdtot[i/2] = rdt[i]+rdt[i+1];
         htacRecoilsum[i/2]->Fill(tac[0],rdtot[i/2]);
         hrdt2D[i/2]->Fill(rdt[i],rdt[i+1]);
         hrdt2Dsum[i/2]->Fill(rdtot[i/2],rdt[i+1]);

         htacRecoil[i]->Fill(tac[0],rdt[i]);
         htacRecoil[i+1]->Fill(tac[0],rdt[i+1]);
      }
   }
   
   /******************* Multi-hit *************************************/
   hmultEZ->Fill(multiEZ);
   hmult->Fill(recoilMulti,arrayMulti);


   /*********** EZERO ************************************************/ 
   
   //if( isGoodEventFlag ) {
   if( ezGate ) {
      hic0->Fill(ezero[0]);
      hic1->Fill(ezero[1]);
      hic2->Fill(ezero[2]);
      
      hic01->Fill(ezero[1], ezero[0]);
      hic02->Fill(ezero[2], ezero[0]);
      hic12->Fill(ezero[2], ezero[1]);
   }
   
   //he0dee->Fill(ezero[1],ezero[0]);
   //he0det->Fill(TMath::Abs(tac[0]),ezero[0]);
   //he0et->Fill(TMath::Abs(tac[0]),ezero[1]);
   //h0detet->Fill((int)(ezero_t[0]-ezero_t[1]));
   //h0dettact->Fill((int)(ezero_t[0]-tac_t[0]));
   //h0ettact->Fill((int)(ezero_t[1]-tac_t[0]));
   //h0de->Fill(ezero[0]);
   //h0e->Fill(ezero[1]);
   //h0tac->Fill(TMath::Abs(tac[0]));
 

   /*********** Ex and thetaCM ************************************************/ 
   
   if( !isGoodEventFlag ) return kTRUE;
   
   for(Int_t detID = 0; detID < numDet ; detID++){
     	
     if( TMath::IsNaN(e[detID]) ) continue ; 
     if( TMath::IsNaN(z[detID]) ) continue ; 
     if( eCal[detID] < 1) continue;

     if( isReaction ){
       //======== Ex calculation by Ryan 
       double y = eCal[detID] + mass; // to give the KE + mass of proton;
       double Z = alpha * gamm * beta * z[detID];
       double H = TMath::Sqrt(TMath::Power(gamm * beta,2) * (y*y - mass * mass) ) ;
			  
       if( TMath::Abs(Z) < H ) {				 
			//using Newton's method to solve 0 ==	H * sin(phi) - G * tan(phi) - Z = f(phi) 
			double tolerrence = 0.001;
			double phi = 0; //initial phi = 0 -> ensure the solution has f'(phi) > 0
			double nPhi = 0; // new phi

			int iter = 0;
			do{
           phi = nPhi;
           nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G /TMath::Power( TMath::Cos(phi), 2));					 
           iter ++;
           if( iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
			}while( TMath::Abs(phi - nPhi ) > tolerrence);
			phi = nPhi;

			// check f'(phi) > 0
			double Df = H * TMath::Cos(phi) - G / TMath::Power( TMath::Cos(phi),2);
			if( Df > 0 && TMath::Abs(phi) < TMath::PiOver2()  ){
           double K = H * TMath::Sin(phi);
           double x = TMath::ACos( mass / ( y * gamm - K));
           double momt = mass * TMath::Tan(x); // momentum of particel b or B in CM frame
           double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(momt*momt + mass * mass));
           Ex = EB - massB;
				
           double hahaha1 = gamm* TMath::Sqrt(mass * mass + momt * momt) - y;
           double hahaha2 = gamm* beta * momt;
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
     //ungated excitation energy
     hEx->Fill(Ex);
     hExi[detID]->Fill(Ex);
     hExThetaCM->Fill(thetaCM, Ex);
     htacEx->Fill(tac[0], Ex);

   }
  
   return kTRUE;
}

void Monitors::SlaveTerminate()
{
  
}

void Monitors::Draw2DHist(TH2F * hist){
   
   if( hist->Integral() < 1000 ){
      hist->SetMarkerStyle(20);
      hist->SetMarkerSize(0.1);
      hist->Draw("");
   }else{
      hist->Draw("colz"); 
   }
}

void Monitors::Terminate()
{
   printf("============ finishing.\n");

   gROOT->cd();

   //############################################ User is free to edit this section
   
   int strLen = canvasTitle.Sizeof();
   canvasTitle.Remove(strLen-3);
   
   cCanvas  = new TCanvas("cCanvas",canvasTitle,1250,1300);
   cCanvas->Modified(); cCanvas->Update();
   
   cCanvas->cd(); cCanvas->Divide(2,4);
   
   gStyle->SetOptStat("neiou");
   
   TFile * transfer = new TFile("transfer.root");
   TObjArray * gList  = NULL ;
   TObjArray * fxList = NULL ;
   int nGList = 0;
   int nFxList = 0;
   if( transfer->IsOpen() ) {
     gList  = (TObjArray *) transfer->FindObjectAny("gList");
     nGList = gList->GetLast() + 1;
     fxList = (TObjArray *) transfer->FindObjectAny("fxList");
     nFxList = fxList->GetLast() +1 ;
   }

   ///----------------------------------- Canvas - 1
   cCanvas->cd(1);
   heCalVz->Draw("colz");

   ///----------------------------------- Canvas - 2
   cCanvas->cd(2); 
   double yMax = htdiff->GetMaximum()*1.2;
   htdiff->GetYaxis()->SetRangeUser(0, yMax);
   htdiff->Draw();
   htdiffg->SetLineColor(2);
   htdiffg->Draw("same");
   
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);
   text.DrawLatex(0.15, 0.8, "with Recoil");
   
   TBox * box = new TBox (timeGate[0], 0, timeGate[1], yMax);
   box->SetFillColorAlpha(kGreen, 0.2);
   box->Draw();

   ///----------------------------------- Canvas - 3
   cCanvas->cd(3);
   Draw2DHist(heCalVzGC);

   text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1]));
   text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   text.DrawLatex(0.15, 0.6, "with Recoil");
   
   //the constant thetaCM line
   if( transfer->IsOpen() ) gList->At(0)->Draw("same");
   
   //the e-z line for excitation 
   if( transfer->IsOpen() ){
      for( int i = 0 ; i < nFxList ; i++){
         fxList->At(i)->Draw("same");
      }
   }
   
   ///----------------------------------- Canvas - 4
   //cCanvas->cd(4); hmult->Draw("colz");
   //cCanvas->cd(4); hID2->Draw("colz");
   cCanvas->cd(4); 
   //Draw2DHist(hExThetaCM);
   Draw2DHist(htacEx);
   
   ///----------------------------------- Canvas - 5
   cCanvas->cd(5); 
   //cCanvas->cd(5)->SetLogy();
   //hic0->Draw();
   
   Draw2DHist(hrdt2Dg[0]);
   text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   if( isCutFileOpen && numCut > 0 ) {cutG = (TCutG *)cutList->At(0) ; cutG->Draw("same");}
   
   ///----------------------------------- Canvas - 6
   cCanvas->cd(6); 
   //cCanvas->cd(6)->SetLogy();
   //hic1->Draw();
   //Draw2DHist(hic02);
   
   Draw2DHist(hrdt2Dg[1]);
   text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   if( isCutFileOpen && numCut > 1) {cutG = (TCutG *)cutList->At(1) ; cutG->Draw("same");}
   
   ///----------------------------------- Canvas - 7
   cCanvas->cd(7); 
   //Draw2DHist(hic01);
   
   Draw2DHist(hrdt2Dg[2]);
   text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   if( isCutFileOpen && numCut > 2) {cutG = (TCutG *)cutList->At(2) ; cutG->Draw("same");}
   
   ///----------------------------------- Canvas - 8
   cCanvas->cd(8); 
   
   Draw2DHist(hrdt2Dg[3]);
   text.DrawLatex(0.15, 0.8, Form("%d < coinTime < %d", timeGate[0], timeGate[1])); 
   text.DrawLatex(0.15, 0.7, Form("%d < TAC < %d", tacGate[0], tacGate[1]));
   if( isCutFileOpen && numCut > 3) {cutG = (TCutG *)cutList->At(3) ; cutG->Draw("same");}
   
   /************************* Save histograms to root file*/
   
   gROOT->cd();
   TString outFileName = canvasTitle;
   outFileName.ReplaceAll(" - ", "-").ReplaceAll(" ", "_").ReplaceAll(":", "").ReplaceAll(",","");
   //gROOT->GetList()->SaveAs(outFileName + ".root");
   
   //cCanvas->SaveAs("Canvas_"+outFileName + ".png");
   
   /************************************/
   
   StpWatch.Start(kFALSE);
   
   gROOT->ProcessLine(".L Utils.C");
   //gROOT->ProcessLine(Form("FindBesCanvasDivision(%d)", numDet));
   printf("=============== loaded Utils.C\n");
   gROOT->ProcessLine(".L ../Armory/AutoFit.C");
   printf("=============== loaded Armory/AutoFit.C\n");
   gROOT->ProcessLine(".L ../Armory/RDTCutCreator.C");
   printf("=============== loaded Armory/RDTCutCreator.C\n");
   gROOT->ProcessLine("listDraws()");
   
   gROOT->ProcessLine("recoils()");
   //gROOT->ProcessLine("rawID()");
   
   
   printf("count1: %d , count2: %d \n", count1, count2);

   
   
}
