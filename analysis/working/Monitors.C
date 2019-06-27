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
#include <TObjArray.h>
#include <fstream>
using namespace std;

//############################################ User setting
const int numDet = 30;
const int numRow = 6;  

ULong64_t maxNumberEvent = 100000000;

//---histogram setting
int rawEnergyRange[2] = {-500, 8000}; // share with e, ring, xf, xn
int    energyRange[2] = {   0,    8};
int     rdtDERange[2] = {  50,  6000};
int      rdtERange[2] = {  50,  6000};
int      elumRange[2] = { 500, 4000};

double     exRange[3] = {  50, -1, 9}; // bin [keV], low[MeV], high[MeV]


//---Gate
int timeGate[2] = {-20, 20}; // min, max
TString rdtCutFile = "rdtCuts.root";

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

TFile *f, *cutfile;
TCutG *EvsZ_cut;
Bool_t cutfileOpen;


//Int_t n=1; //what is this for?

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
//TH1F* hStat[numDet];
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
TH1F* hexR;

TH2F* hExThetaCM;

//====== TAC
TH1F* htacE;
TH1I* htacArray[numDet];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt
TH2F* hrtac[4];

//======= Recoil
TH2F* hrdtID;
TH1F* hrdt[8]; // single recoil
TH2F* hrdt2D[4];
TH2F* hrdt2Dg[4];

//======= ELUM
TH1F* helum[16];
TH2F* helumID;

//======= EZero
TH2F* he0dee;
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;

//======= multi-Hit
TH2I *hmult;
TH1I *hmultEZ;
TH1I *htdiff;
TH1I *htdiffg;
TH2I *hID2;
TH2I *hID2g;
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
   lastRunID = -1;
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
         zRange[0] = pos[0] -length - 30;
         zRange[1] = pos[numCol-1] + 30;
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
      printf("============\n");
		printf("mass-b    : %f MeV/c2 \n", mass);
		printf("charge-b  : %f \n", q);
		printf("E-total   : %f MeV \n", Et);
		printf("mass-B    : %f MeV/c2 \n", massB);		 
		printf("beta      : %f \n", beta);
		printf("B-field   : %f T \n", Bfield);
		printf("alpha     : %f MeV/mm \n", alpha);
		printf("det radius: %f mm \n", a);
		printf("G-coeff   : %f MeV \n", G);
		printf("============\n");

   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();


   //================  Get Recoil cuts;
   TFile * fCut = new TFile(rdtCutFile);	
   isCutFileOpen = fCut->IsOpen();
   if(!isCutFileOpen) cout<<"Failed to open cutfile"<<endl;
   numCut = 0 ;
   if( isCutFileOpen ){
      cutList = (TObjArray *) fCut->FindObjectAny("cutList");
      numCut = cutList->GetEntries();
      printf("=========== found %d cutG in %s \n", numCut, fCut->GetName());

      cutG = new TCutG();
      for(int i = 0; i < numCut ; i++){
         printf(" cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
            cutList->At(i)->GetName(),
            ((TCutG*)cutList->At(i))->GetVarX(),
            ((TCutG*)cutList->At(i))->GetVarY(),
            ((TCutG*)cutList->At(i))->GetN());
         countFromCut.push_back(0);	
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

   printf("======================================\n");
   
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
      
      //hStat[i] = new TH1F(Form("hStat%d", i),
      //                    Form("Hit Statistics (ch=%d)", i),
      //                    7, 1, 8); // 0 = no hit, 1 = e, 2 = ring, 3 = xf, 4 = xn, 5 = xf + xn, 6 = xf +xn + e, 7 = xf + xn + e + ring
    
      
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
                       500, energyRange[0], energyRange[1]);
      
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
                           500,-0.25,5.25,500,energyRange[0], energyRange[1]);
   }
   
   heCalID = new TH2F("heCalID", "Corrected E vs detID; detID; E / 10 keV", numDet, 0, numDet, 500, energyRange[0], energyRange[1]);
   
   //====================== E-Z plot
   heCalVz   = new TH2F("heCalVz",  "E vs. Z;Z (mm);E (MeV)"      , 400, zRange[0], zRange[1], 400, energyRange[0], energyRange[1]);
   heCalVzGC = new TH2F("heCalVzGC","E vs. Z gated;Z (mm);E (MeV)", 400, zRange[0], zRange[1], 400, 0, energyRange[1]);
   
   for( int i = 0; i < numRow; i++){
      hecalVzRow[i] = new TH2F(Form("heCalVz%d", i),
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
         hrdt2D[tempID] = new TH2F(Form("hrdt2D%d",tempID), Form("Raw Recoil DE vs Eres (dE=%d, E=%d); Eres (channel); DE (channel)", i+1, i), 500,rdtERange[0],rdtERange[1],500,rdtDERange[0], rdtDERange[1]);
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
   htdiff  = new TH1I("htdiff" ,"Coincident time (array, recoil); time [ch = 10ns]; count", 200,-100,100);   
   htdiffg = new TH1I("htdiffg",Form("Coincident time (array, recoil) w/ %d < coinTime gate < %d; time [ch = 10ns]; count", timeGate[0], timeGate[1]), 200,-100,100);

   //===================== TAC
   htac[0] = new TH1F("htac0","Array-RDT0 TAC; DT [clock ticks]; Counts",2000,-1000,1000);
   htac[1] = new TH1F("htac1","Array-RDT1 TAC; DT [clock ticks]; Counts",2000,-1000,1000);
   htac[2] = new TH1F("htac2","Array-RDT2 TAC; DT [clock ticks]; Counts",2000,-1000,1000);
   htac[3] = new TH1F("htac3","Array-RDT3 TAC; DT [clock ticks]; Counts",2000,-1000,1000);

   htacE = new TH1F("htacE","Array-RF TAC; kind of time diff [a.u.]; Counts", 4000, -8000, 8000);

   for (Int_t i=0;i<numDet;i++) {
      htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
   }

   //===================== energy spectrum
   hEx  = new TH1F("hEx",Form("excitation spectrum w/ goodFlag; E [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   hexR = new TH1F("hexR","excitation spectrum with Recoil",(int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);

   hExThetaCM = new TH2F("hExThetaCM", "Ex vs ThetaCM; ThetaCM [deg]; Ex [MeV]", 200, 0, 50,  (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);


   //===================== ELUM
   for( int i = 0; i < 16; i++){
      helum[i] = new TH1F(Form("helum%d", i), Form("Elum-%d", i), 200, elumRange[0], elumRange[1]);
   }
   helumID = new TH2F("helumID", "Elum vs ID", 16, 0 , 16, 200, elumRange[0], elumRange[1]);

   //===================== EZERO
   he0dee = new TH2F("he0dee","EZERO DE-E; E [ch]; DE [ch]",500,0,8000,500,0,8000);//ezero
   he0det = new TH2F("he0det","EZERO DE-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   he0et = new TH2F("he0et","EZERO E-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   h0detet = new TH1F("h0detet","EZERO DE Time - E Time; DET-ET [ch]",500,-250,250);//
   h0dettact = new TH1F("h0dettact","EZERO DE Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0ettact = new TH1F("h0ettact","EZERO E Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0de = new TH1F("h0de","EZERO DE ; DE [ch]",500,50,4050);//
   h0e = new TH1F("h0e","EZERO - E; E [ch]",500,50,4050);//
   h0tac = new TH1F("h0tac","EZERO RF; RF [ch]",500,50,4050);//
	
   printf("========================================\n");
   StpWatch.Start();
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
    htacE->Fill(tac[0]);
    
    /*********** ELUM ************************************************/
    for( int i = 0; i < 16; i++){
       helum[i]->Fill(elum[i]);
       helumID->Fill(i, elum[i]);
    }

    /*********** Array ************************************************/ 
    //Do calculations and fill histograms
    Int_t recoilMulti = 0;
    Int_t arrayMulti = 0;
    Int_t multiEZ = 0;
    bool rdtgate = false;
    bool coinFlag = false;
    bool isGoodEventFlag = false;
    for (Int_t i = 0; i < numDet; i++) {
      
      //================== Filling raw data
      he[i]->Fill(e[i]);
      hring[i]->Fill(ring[i]);
      hxf[i]->Fill(xf[i]);
      hxn[i]->Fill(xn[i]);
      heVring[i]->Fill(ring[i],e[i]);
      hxfVxn[i]->Fill(xf[i],xn[i]);
      heVxs[i]->Fill(xf[i]+xn[i], e[i]);
      
      heVID->Fill(i, e[i]);
      hringVID->Fill(i, ring[i]);
      hxfVID->Fill(i, xf[i]);
      hxnVID->Fill(i, xn[i]);
      
      //if( !TMath::IsNaN(e[i])  && !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) && !TMath::IsNaN(ring[i]) ) hStat[i]->Fill(7);
      //if( !TMath::IsNaN(e[i])  && !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(6);
      //if( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(5);
      //if( !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(4);
      //if( !TMath::IsNaN(xf[i]) ) hStat[i]->Fill(3);
      //if( !TMath::IsNaN(ring[i]) ) hStat[i]->Fill(2);
      //if( !TMath::IsNaN(e[i])  ) hStat[i]->Fill(1);
      //if( TMath::IsNaN(e[i]) && TMath::IsNaN(ring[i]) && TMath::IsNaN(xf[i]) && TMath::IsNaN(xn[i]) ) hStat[i]->Fill(0);

      //==================== Basic gate
      if( TMath::IsNaN(e[i]) ) continue ; 
      if( ring[i] > 300 ) continue; 
      if( TMath::IsNaN(xn[i]) &&  TMath::IsNaN(xf[i]) ) continue ; 

      //==================== Calibrations go here
      xfcal[i] = xf[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      xncal[i] = xn[i]*xnCorr[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      eCal[i] = e[i]/eCorr[i][0]+eCorr[i][1];

      if( eCal[i] < 0.5 ) continue;

      //===================== fill Calibrated  data
      heCal[i]->Fill(eCal[i]);
      heCalID->Fill(i, eCal[i]);
      hxfCalVxnCal[i]->Fill(xfcal[i], xncal[i]);
      heVxsCal[i]->Fill(xncal[i] + xfcal[i], e[i]);
      
      //===================== calculate X
      if( (xf[i] > 0 || !TMath::IsNaN(xf[i])) && ( xn[i]>0 || !TMath::IsNaN(xn[i])) ) {
        //x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;
        x[i] = 0.5*((xf[i]-xn[i]) / e[i])+0.5;
      }
      
      // range of x is (0, 1)
      if  ( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) xcal[i] = 0.5 + 0.5 * (xfcal[i] - xncal[i] ) / e[i];
      if  ( !TMath::IsNaN(xf[i]) &&  TMath::IsNaN(xn[i]) ) xcal[i] = xfcal[i]/ e[i];
      if  (  TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) xcal[i] = 1.0 - xncal[i]/ e[i];
      //if  (  TMath::IsNaN(xn[i]) &&  TMath::IsNaN(xf[i]) ) xcal[i] = TMath::QuietNaN();
      
      /*
      if  ( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) {
        if (xfcal[i]>0.5*e[i]) {
          xcal[i] = xfcal[i]/e[i];
        }else if {
          xcal[i] = 1.0 - xncal[i]/e[i];
        }
      }*/
      
      
      //==================== calculate Z
      if( firstPos > 0 ) {
        z[i] = length*(xcal[i]) + pos[i%numCol];
      }else{
        z[i] = length*(xcal[i]-1.0) + pos[i%numCol];
      }

      //===================== multiplicity
      arrayMulti++; // multi-hit when both e, xf, xn are not NaN

      //=================== Array fill next
      heVx[i]->Fill(x[i],e[i]);
      hringVx[i]->Fill(x[i],ring[i]);
     
      heCalVxCal[i]->Fill(xcal[i]*length,eCal[i]);
      heCalVz->Fill(z[i],eCal[i]);

      //=================== Recoil Gate
      if( isCutFileOpen ){
        for(int i = 0 ; i < 4 ; i++ ){
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
      if( !TMath::IsNaN(z[i]) ) { 
        for( int j = 0; j < 8 ; j++){
          if( TMath::IsNaN(rdt[j]) ) continue; 
	 
          int tdiff = rdt_t[j] - e_t[i];
	 
          if(j==1||j==3||j==5||j==7) hrtac[j/2]->Fill(i,tdiff);
        
          htdiff->Fill(tdiff);
          if(rdtgate && eCal[i]>0) htdiffg->Fill(tdiff);

          hID2->Fill(i, j); 
	 
          if( timeGate[0] < tdiff && tdiff < timeGate[1] )	 {
            if(j % 2 == 0 ) hrdt2Dg[j/2]->Fill(rdt[j],rdt[j+1]);
            hID2g->Fill(i, j); 
            //if( rdtgate) hID2g->Fill(i, j); 
            coinFlag = true;
          }
        }
      }

      if( coinFlag && rdtgate ) {
        heCalVzGC->Fill( z[i] , eCal[i] );
        multiEZ ++;
        isGoodEventFlag = true;
      }	 
      
    }//end of array loop
    
    //=========== fill eCal Vs z for each row
    for( int i = 0; i < numRow; i++){
      for(int j = 0; j < numCol; j++){
         int k = numCol*i+j;
         if( ((xf[k] > 0 || !TMath::IsNaN(xf[k]))  && ( xn[k]>0 || !TMath::IsNaN(xn[k]))) ) hecalVzRow[i] -> Fill( z[k], eCal[k]);
      }
    }
    
   /*********** RECOILS ************************************************/    
   for( int i = 0; i < 8 ; i++){
      hrdtID->Fill(i, rdt[i]);
      hrdt[i]->Fill(rdt[i]);
      
      if( i % 2 == 0  ){
         recoilMulti++; // when both dE and E are hit
         hrdt2D[i/2]->Fill(rdt[i],rdt[i+1]);
      }
      
   }
   
   /******************* Multi-hit *************************************/
   hmultEZ->Fill(multiEZ);
   hmult->Fill(recoilMulti,arrayMulti);


   /*********** EZERO ************************************************/ 
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
     hExThetaCM->Fill(thetaCM, Ex);
	
     // recoil CUTS
     if( isCutFileOpen){
		 for( int k = 0 ; k < numCut; k++ ){
         cutG = (TCutG *)cutList->At(k) ;
         if( cutG->IsInside(rdt[k], rdt[k+1]) ) { 
           hexR->Fill(Ex);
           break; // ensure fill only once
         }
		 }
     }
   }
  
   return kTRUE;
}

void Monitors::SlaveTerminate()
{
  
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
   //TTree * treeT = (TTree *) transfer->FindObjectAny("tree"); 
   TObjArray * gList  = NULL ;
   TObjArray * fxList = NULL ;
   if( transfer->IsOpen() ) {
     gList  = (TObjArray *) transfer->FindObjectAny("gList");
     fxList = (TObjArray *) transfer->FindObjectAny("fxList");
   }

   //cCanvas->cd(1);
   //treeT->Draw("thetaCM >> c0", "hit == 1 && ExID == 0", "");
   //treeT->Draw("thetaCM >> c1", "hit == 1 && ExID == 1", "");
   //treeT->Draw("thetaCM >> c2", "hit == 1 && ExID == 2", "");
   //treeT->Draw("thetaCM >> c3", "hit == 1 && ExID == 3", "");
   
   cCanvas->cd(1);
   heCalVz->Draw("colz");
   //if( transfer->IsOpen() ) fxList->At(0)->Draw("same");
   //if( transfer->IsOpen() ) fxList->At(5)->Draw("same");

   cCanvas->cd(2); 
   htdiff->Draw();
   htdiffg->SetLineColor(2);
   htdiffg->Draw("same");
   
   TLatex text;
   text.SetNDC();
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(2);
   text.DrawLatex(0.15, 0.8, "with Recoil");

   cCanvas->cd(3);
   heCalVzGC->Draw("colz");
   
   //the constant thetaCM line
   if( transfer->IsOpen() ) gList->At(0)->Draw("same");
   
   //the e-z line for excitation 
   if( transfer->IsOpen() ) fxList->At(0)->Draw("same");
   //if( transfer->IsOpen() ) fxList->At(5)->Draw("same");
   
   
   //cCanvas->cd(4); hmult->Draw("colz");
   //cCanvas->cd(4); hID2->Draw("colz");
   cCanvas->cd(4); hExThetaCM->Draw("colz");
   
   //hrdt2Dg[0]->SetMarkerStyle(20);
   //hrdt2Dg[1]->SetMarkerStyle(20);
   //hrdt2Dg[2]->SetMarkerStyle(20);
   //hrdt2Dg[3]->SetMarkerStyle(20);
   
   cCanvas->cd(5); hrdt2Dg[0]->Draw("colz"); if( isCutFileOpen) {cutG = (TCutG *)cutList->At(0) ; cutG->Draw("same");}
   cCanvas->cd(6); hrdt2Dg[1]->Draw("colz"); if( isCutFileOpen) {cutG = (TCutG *)cutList->At(1) ; cutG->Draw("same");}
   cCanvas->cd(7); hrdt2Dg[2]->Draw("colz"); if( isCutFileOpen) {cutG = (TCutG *)cutList->At(2) ; cutG->Draw("same");}
   cCanvas->cd(8); hrdt2Dg[3]->Draw("colz"); if( isCutFileOpen) {cutG = (TCutG *)cutList->At(3) ; cutG->Draw("same");}
   
   /************************* Save histograms to root file*/
   
   gROOT->cd();
   TString outFileName = canvasTitle;
   ((outFileName.ReplaceAll(" - ", "-")).ReplaceAll(" ", "_")).ReplaceAll(":", "");
   //gROOT->GetList()->SaveAs(outFileName + ".root");
   
   cCanvas->SaveAs("Canvas_"+outFileName + ".png");
   cCheckRDT->SaveAs("CheckRDT_"+outFileName + ".png");
   
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
   
}
