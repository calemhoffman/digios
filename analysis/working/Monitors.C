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
#include <TSystem.h>
#include <TObjArray.h>
#include <fstream>
using namespace std;

/****  ======================= User setting */
const int numDet = 30;
const int numRow = 6;  

ULong64_t maxNumberEvent = 1000000;

//---histogram seeting
int rawEnergyRange[2] = {-500, 5000}; // share with e, ring, xf, xn
int energyRange[2]= {500, 5000};

//TODO switch for histogram

//RODO the z-pos of array should be set outside
Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=2.263;//to the physical end of array from the target (-20 to si start)

//only use when reaction.dat is loaded, to calculate Ex from e-z plot to Ex-thetaCM using inverse transformation.
double a = 11.5 ; // perpendicular distance of detector to axis [mm]
double Bfield = 2.4996828363;

/*** ======================= end of user setting */

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
TGraph ** graphRateCut; //!
TMultiGraph * rateGraph; //!
TGraph *graphRate;// = new TGraph(n,x,y);

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
TH1F* hStat[numDet];
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

//====== TAC
TH1F* htacE;
TH1I* htacArray[numDet];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

//======= Others
TH2F* hrdt[4];
TH2F* hrdtg[4];
TH2F* helum[2];

TH1F* helum4; //iss631 downstream dE
TH2F* helum4Entry; //iss631
TH1F* helum4D; // iss631, deuteron
TH1F* helum4C; // iss631, carbon

TH2F* he0dee;//ezero
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;

/***************************
 ***************************/
//==== time in sec
Float_t timeZero=0;
Float_t timeCurrent=0;
Float_t timeRef=0;

//==== global variables
Float_t x[numDet],z[numDet];
Float_t xcal[numDet],xfcal[numDet],xncal[numDet],eCal[numDet],ezero[10];
Int_t tacA[numDet];

//==== correction parameters
Float_t xnCorr[numDet];
Float_t xfxneCorr[numDet][2];
Float_t eCorr[numDet][2];

//==== parameters for Ex and thetaCM calcualtion
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

   //Generate all of the histograms needed for drawing later on
   heVID    = new TH2F("heVID",    "Raw e vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hringVID = new TH2F("hringVID", "Raw Ring vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxfVID   = new TH2F("hxfVID",   "Raw xf vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
   hxnVID   = new TH2F("hxnVID",   "Raw xn vs channel", numDet, 0, numDet, 500, rawEnergyRange[0], rawEnergyRange[1]);
  
   for (Int_t i=0;i<numDet;i++) {//array loop
      
      hStat[i] = new TH1F(Form("hStat%d", i),
                          Form("Hit Statistics (ch=%d)", i),
                          7, 1, 8); // 0 = no hit, 1 = e, 2 = ring, 3 = xf, 4 = xn, 5 = xf + xn, 6 = xf +xn + e, 7 = xf + xn + e + ring
      
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
                           100,-0.1,1.1,100,rawEnergyRange[0], rawEnergyRange[1]);
                           
      heCalVxCal[i] = new TH2F(Form("heCalVxCal%d",i),
                           Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),
                           500,-0.25,5.25,500,energyRange[0], energyRange[1]);
   }
   
   heCalID = new TH2F("heCalID", "Corrected E vs detID; detID; E / 10 keV", 24, 0, 24, 500, 2, 7);
   
   //E-Z plot
   heCalVz = new TH2F("heCalVz","E vs. Z;Z (mm);E (MeV)",400,-500, 0,400,0,10);
   heCalVzGC = new TH2F("heCalVzGC","E vs. Z gated;Z (cm);E (MeV)",700,-55,0,750,0,3000);
   
   for( int i = 0; i < numRow; i++){
      hecalVzRow[i] = new TH2F(Form("heCalVz%d", i),
                               Form("E vs. Z (ch=%d-%d); Z (cm); E (MeV)", 6*i, 6*i+5),
                               500, -55, 0, 500, 0, 7);
   }

   //Recoils
   for (Int_t i=0;i<4;i++) {
      hrdt[i] = new TH2F(Form("hrdt%d",i),
                         Form("Raw Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
                         1000,0,10000,1000,0,4000);
      hrdtg[i] = new TH2F(Form("hrdtg%d",i),
                          Form("Gated Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
                          1000,0,10000,1000,0,4000);
   }

   //ELUM
   helum[0] = new TH2F("helum0","Elum Ring Energies; E (channels); Ring Number", 900, 0, 9000,16,0,16);
   helum[1] = new TH2F("helum1","Elum Wedge Energies; E (channels); Ring Number", 900, 0, 9000,16,0,16);

   helum4=new TH1F("helum4", "downstream dE; E (channels); count / 10 channels", 800, 100, 8100);
   
   double timeRange[2] = {1700, 7000} ;// startTime, length [min]
   helum4Entry = new TH2F("helum4Entry", "downstream dE vs time; time [min]; dE channel", timeRange[1], timeRange[0], timeRange[0]+timeRange[1], 500, 0, 7000);
   helum4D = new TH1F("helum4D", "downstream dE (deuteron); time [min]; count / 1 min ",  timeRange[1], timeRange[0], timeRange[0]+timeRange[1]);
   helum4C = new TH1F("helum4C", "downstream dE (carbon) ; time [min]; count / 1 min",    timeRange[1], timeRange[0], timeRange[0]+timeRange[1]);
   
   //TAC
   htac[0] = new TH1F("htac0","Array-RDT0 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[1] = new TH1F("htac1","Array-RDT1 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[2] = new TH1F("htac2","Array-RDT2 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[3] = new TH1F("htac3","Array-RDT3 TAC; DT [clock ticks]; Counts",6,0,6);

   htacE = new TH1F("htacE","Elum-RDT TAC; DT [clock ticks]; Counts",4,0,4);

   //energy spectrum
   double exRange[3] = {25, -1, 4}; // bin [keV], low[MeV], high[MeV]
   hEx = new TH1F("hEx",Form("excitation spectrum; E [MeV] ; Count / %4.0f keV", exRange[0]), (int) (exRange[2]-exRange[1])/exRange[0]*1000, exRange[1], exRange[2]);
   hexR = new TH1F("hexR","excitation spectrum with Recoil",500,-5,10);

   for (Int_t i=0;i<numDet;i++) {
      htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
   }

   //EZERO
   he0dee = new TH2F("he0dee","EZERO DE-E; E [ch]; DE [ch]",500,0,8000,500,0,8000);//ezero
   he0det = new TH2F("he0det","EZERO DE-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   he0et = new TH2F("he0et","EZERO E-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
   h0detet = new TH1F("h0detet","EZERO DE Time - E Time; DET-ET [ch]",500,-250,250);//
   h0dettact = new TH1F("h0dettact","EZERO DE Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0ettact = new TH1F("h0ettact","EZERO E Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
   h0de = new TH1F("h0de","EZERO DE ; DE [ch]",500,50,4050);//
   h0e = new TH1F("h0e","EZERO - E; E [ch]",500,50,4050);//
   h0tac = new TH1F("h0tac","EZERO RF; RF [ch]",500,50,4050);//

   rateGraph = new TMultiGraph();
   graphRate = new TGraph();
   graphRate->SetTitle("Raw Total Rate");
   graphRate->SetMarkerColor(4);
   graphRate->SetMarkerStyle(20);
   graphRate->SetMarkerSize(1);

   printf("################## loading parameter files\n"); 

   //rateGraph->Add(graphRate);
   rateGraph->SetTitle("Instantaneous Beam rate [pps]; Delta Time [sec]; Rate [pps]");
	
   //Get any cuts;
   TFile * fCut = new TFile("rdtCuts.root");		   // open file
   isCutFileOpen = fCut->IsOpen(); 
   numCut = 0 ;
   if( isCutFileOpen ){
      cutList = (TObjArray *) fCut->FindObjectAny("cutList");
      numCut = cutList->GetEntries();
      printf("=========== found %d cutG in %s \n", numCut, fCut->GetName());

      cutG = new TCutG();
      graphRateCut = new TGraph * [numCut];
      for(int i = 0; i < numCut ; i++){
         printf(" cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
            cutList->At(i)->GetName(),
            ((TCutG*)cutList->At(i))->GetVarX(),
            ((TCutG*)cutList->At(i))->GetVarY(),
            ((TCutG*)cutList->At(i))->GetN());
         countFromCut.push_back(0);	
      
         graphRateCut[i] = new TGraph();
         graphRateCut[i]->SetMarkerColor(i+1);	 
         graphRateCut[i]->SetMarkerStyle(20+i);
         graphRateCut[i]->SetMarkerSize(1);
         rateGraph->Add(graphRateCut[i]);
      }
   }
  
   //printf("======== number of cuts found : %d \n", numCut);
   
   //===================================================== loading parameter
   //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   ifstream file;
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   
   printf("----- loading xf/xn-e correction.");
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
   
   printf("----- loading e correction.");
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
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
   }
   file.close();
   
   //========================================= reaction parameters
   printf("----- loading reaction parameter.");
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
      //printf("============\n");
      //printf("mass-b  : %f MeV/c2 \n", mass);
      //printf("charge-b: %f \n", q);
      //printf("E-total : %f MeV \n", Et);
      //printf("mass-B  : %f MeV/c2 \n", massB);      
      //printf("beta    : %f \n", beta);
      //printf("B-field : %f T \n", Bfield);
      //printf("alpha   : %f MeV/mm \n", alpha);
      //printf("a       : %f mm \n", a);
      //printf("G       : %f MeV \n", G);
      //printf("============\n");

   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();
   printf("======================================\n");

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


    /*********** Array ************************************************/ 
    //Do calculations and fill histograms
    //Array calcs first
    //int multiHit = 0;
    for (Int_t i = 0; i < numDet; i++) {

      //======================= fill raw data
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
      
      if( !TMath::IsNaN(e[i])  && !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) && !TMath::IsNaN(ring[i]) ) hStat[i]->Fill(7);
      if( !TMath::IsNaN(e[i])  && !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(6);
      if( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(5);
      if( !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(4);
      if( !TMath::IsNaN(xf[i]) ) hStat[i]->Fill(3);
      if( !TMath::IsNaN(ring[i]) ) hStat[i]->Fill(2);
      if( !TMath::IsNaN(e[i])  ) hStat[i]->Fill(1);
      if( TMath::IsNaN(e[i]) && TMath::IsNaN(ring[i]) && TMath::IsNaN(xf[i]) && TMath::IsNaN(xn[i]) ) hStat[i]->Fill(0);
      
      //==================== Calibrations go here
      xfcal[i] = xf[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      xncal[i] = xn[i]*xnCorr[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      eCal[i] = e[i]/eCorr[i][0]+eCorr[i][1];

      //if( eCal[i] < 1.5 ) continue;

      //===================== fill Calibrated  data
      heCal[i]->Fill(eCal[i]);
      hxfCalVxnCal[i]->Fill(xfcal[i], xncal[i]);
      heVxsCal[i]->Fill(xncal[i] + xfcal[i], e[i]);
      
      //===================== calculate X
      if( (xf[i] > 0 || !TMath::IsNaN(xf[i])) && ( xn[i]>0 || !TMath::IsNaN(xn[i])) ) {
        //x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;
        x[i] = 0.5*((xf[i]-xn[i]) / e[i])+0.5;
      }
      if (xfcal[i]>0.5*e[i]) {
        xcal[i] = xfcal[i]/e[i];
      }else if (xncal[i]>=0.5*e[i]) {
        xcal[i] = 1.0 - xncal[i]/e[i];
      }else{
        xcal[i] = TMath::QuietNaN();
      }
      
      //==================== calculate Z
      //z[i] = 5.0*(xcal[i]-0.5) + z_off + z_array_pos[i%6];//for downstream?
      z[i] = 5.0*(xcal[i]-0.5) - z_off - z_array_pos[i%numCol];
      
      //if( !TMath::IsNaN(z[i]) ) multiHit ++;
      
      //=================== Array fill next
      if( -1.1 < x[i] && x[i] < 1.1 && e[i]>100 && (xn[i]>0||xf[i]>0)) {
        heVx[i]->Fill(x[i],e[i]);
        heCalVxCal[i]->Fill(xcal[i]*5.0,eCal[i]);
        heCalVz->Fill(z[i]*10.,eCal[i]);

        for (Int_t ii=0;ii<4;ii++) hrdtg[ii]->Fill(rdt[ii+4],rdt[ii]);
      }
      
    }//end of array loop
    
    //if (multiHit != 1) return kTRUE;
    for( int i = 0; i < numRow; i++){
      for(int j = 0; j < numCol; j++){
         int k = 6*i+j;
         if( ((xf[k] > 0 || !TMath::IsNaN(xf[k]))  && ( xn[k]>0 || !TMath::IsNaN(xn[k]))) ) hecalVzRow[i] -> Fill( z[k], eCal[k]);
      }
    }
    
    /*********** RECOILS ************************************************/ 
    for (Int_t ii=0;ii<4;ii++) hrdt[ii]->Fill(rdt[ii+4],rdt[ii]);

    /*********** ELUM ************************************************/ 
    for (Int_t i = 0; i < 16; i++) {
      helum[0]->Fill(elum[i],i);
      helum[1]->Fill(elum[i+16],i);
      for(Int_t j=0;j<4;j++){
        if(rdt[j]-elum_t[i]>-10&&rdt[j]-elum[i]<10) htacE->Fill(0.5+j);
      }
    }

    helum4->Fill(elum[4]);
    
    helum4Entry->Fill(elum_t[4]/1e8/60., elum[4]);
    
    if( 1000 < elum[4]  && elum[4] < 1250 ) helum4D->Fill(elum_t[4]/1e8/60.); 
    if( 3500 < elum[4]  && elum[4] < 7000 ) helum4C->Fill(elum_t[4]/1e8/60.); 

    /*********** EZERO ************************************************/ 
    he0dee->Fill(ezero[1],ezero[0]);
    he0det->Fill(TMath::Abs(tac[0]),ezero[0]);
    he0et->Fill(TMath::Abs(tac[0]),ezero[1]);
    h0detet->Fill((int)(ezero_t[0]-ezero_t[1]));
    h0dettact->Fill((int)(ezero_t[0]-tac_t[0]));
    h0ettact->Fill((int)(ezero_t[1]-tac_t[0]));
    h0de->Fill(ezero[0]);
    h0e->Fill(ezero[1]);
    h0tac->Fill(TMath::Abs(tac[0]));
  
    /*********** Ex and thetaCM ************************************************/ 
    for(Int_t i = 0; i < numRow ; i++){
      for(Int_t j = 0; j < numCol; j++){
        
         int detID = i*numCol+j;
         if( eCal[detID] < 1) continue;
         
         if( isReaction ){
            //======== Ex calculation by Ryan 
            double y = eCal[detID] + mass; // to give the KE + mass of proton;
            double Z = alpha * gamm * beta * z[detID] * 10.;
            double H = TMath::Sqrt(TMath::Power(gamm * beta,2) * (y*y - mass * mass) ) ;
                 
            if( TMath::Abs(Z) < H ) {            
               //using Newton's method to solve 0 ==  H * sin(phi) - G * tan(phi) - Z = f(phi) 
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
        
        //CUTS
        if( isCutFileOpen){
          for( int k = 0 ; k < numCut; k++ ){
            cutG = (TCutG *)cutList->At(k) ;
            if( cutG->IsInside(rdt[k+4], rdt[k]) ) { //CRH
              for (Int_t kk = 0; kk < 4; kk++) { 
                tacA[detID]= (int)(rdt_t[kk]-e_t[detID]);
                if(-30 < tacA[detID] && tacA[detID] < 30) {
                  hexR->Fill(Ex);
                  heCalVzGC->Fill(z[detID],eCal[detID]);

                }
              }
            }
          }
        }
        
        if(e[detID]>100){
          for (Int_t k = 0; k < 4; k++) {
            tacA[detID]= (int)(rdt_t[k]-e_t[detID]);
            htacArray[detID]->Fill(tacA[detID]);
            if(-5 < tacA[detID] && tacA[detID] < 5 && rdt[k]>50 ){
              htac[k]->Fill(j+0.5);
            }
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
   //int strLen = canvasTitle.Sizeof();
   //canvasTitle.Remove(strLen-3);
   //cCanvas  = new TCanvas("cCanvas",canvasTitle,1250,1300);
   //cCanvas->Modified(); cCanvas->Update();
   //cCanvas->cd(); cCanvas->Divide(2,4);
   
   gStyle->SetOptStat("neiou");
   
   
   //TFile * transfer = new TFile("transfer.root");
   //TTree * treeT = (TTree *) transfer->FindObjectAny("tree"); 
   //TObjArray * gList = (TObjArray *) transfer->FindObjectAny("gList");
   //TObjArray * fxList = (TObjArray *) transfer->FindObjectAny("fxList");
   
   //cCanvas->cd(1);
   //treeT->Draw("thetaCM >> c0", "hit == 1 && ExID == 0", "");
   //treeT->Draw("thetaCM >> c1", "hit == 1 && ExID == 1", "");
   //treeT->Draw("thetaCM >> c2", "hit == 1 && ExID == 2", "");
   //treeT->Draw("thetaCM >> c3", "hit == 1 && ExID == 3", "");
   
   //cCanvas->cd(1); helum4Entry->Draw("colz");
   //cCanvas->cd(3); helum4D->Draw("");
   //cCanvas->cd(5); helum4C->Draw("");
   //cCanvas->cd(7); cCanvas->cd(10)->SetLogy(); helum4->Draw("colz");
   
   
   //cCanvas->cd(2); 

   //cCanvas->cd(4); hEx->Draw("");
   //hexoutEBIS->SetLineColor(16);
   //hexoutEBIS->SetFillColor(18);
   //hexoutEBIS->Draw("same");
   
   //cCanvas->cd(6); 
   //gList->At(0)->Draw("same");
   //fxList->At(0)->Draw("same");
   
   //cCanvas->cd(8); heCalVz->Draw("colz"); 
   //gList->At(0)->Draw("same");
   //gList->At(10)->Draw("same");
   //gList->At(20)->Draw("same");
   //fxList->At(0)->Draw("same");
   //fxList->At(1)->Draw("same");
   //fxList->At(2)->Draw("same");
   //fxList->At(3)->Draw("same");
   
   //TCanvas * cAux = new TCanvas("cAux", "Aux", 2000, 1000, 600, 400);
   //cAux->cd();
   //heCalID->Draw("colz");
   
   StpWatch.Start(kFALSE);
   
   gROOT->ProcessLine(".L Utils.C");
   //gROOT->ProcessLine(Form("FindBesCanvasDivision(%d)", numDet));
   printf("=============== loaded Utils.C\n");
   gROOT->ProcessLine(".L ../Armory/AutoFit.C");
   printf("=============== loaded Armory/AutoFit.C\n");
   gROOT->ProcessLine("listDraws()");
   
}
