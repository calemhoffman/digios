#define Monitors_cxx

#include "Monitors.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TString.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <fstream>

#define NUMPRINT 20 //>0
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar
TStopwatch StpWatch;

TCanvas *cCanvas;
TCanvas *cEX;
TGraph *graphRate;// = new TGraph(n,x,y);
Int_t n=1;

TString cutName("cut1");
TCutG* cutG; //!
TObjArray * cutList;
TString cutTag;
Bool_t isCutFileOpen;
int numCut;
vector<int> countFromCut;
TGraph ** graphRateCut; //!
TMultiGraph * rateGraph; //!

const int numDet = 24;

TH1F* hStat[numDet];
TH1F* he[numDet];
TH2F* hxfxn[numDet];
TH2F* hxfxne[numDet];
TH2F* heVx[numDet];
TH1F* heC[numDet];
TH2F* hxfxnC[numDet]; 
TH2F* hxfxneC[numDet];
TH2F* hecalVxcal[numDet];
TH2F* hecalVz;
TH2F* hecalVzRow[4];
TH2F* hecalVzR;
TH2F* hrdt[4];
TH2F* hrdtg[4];
TH2F* helum[2];
TH2F* he0dee;//ezero
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;

//time in sec
Float_t timeZero=0;
Float_t timeCurrent=0;
Float_t timeRef=0;

TH1F* htacE;
TH1F* hexC;
TH1F* hexR;
TH1I* htacArray[numDet];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

Float_t x[numDet],z[numDet];
Float_t xcal[numDet],ecal[numDet],xfcal[numDet],xncal[numDet],ecrr[numDet],ezero[10];
Int_t tacA[numDet];

Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=6.5;//to the physical end of array from the target (-20 to si start)

Float_t xnCorr[numDet];
Float_t xfxneCorr[numDet][2];
Float_t eCorr[numDet][2];

double a = 11.5 ; // perpendicular distance of detector to axis [mm]
double Bfield = 2.5 ; // in T
double Ex, thetaCM;
double q, alpha, Et, beta, gamm, G, massB, mass; //variables for Ex calculation
bool isReaction;
      

Float_t tempTime=-1000;
Long64_t tempTimeLong=10001;

void Monitors::Begin(TTree *tree)
{
   TString option = GetOption();
   NumEntries = tree->GetEntries();

   //Generate all of the histograms needed for drawing later on
  
   for (Int_t i=0;i<numDet;i++) {//array loop
      
      hStat[i] = new TH1F(Form("hStat%d", i),
                          Form("Hit Statistics (ch=%d)", i),
                          5, 1, 6); // 0 = no hit, 1 = e, 2 = xf, 3 = xn, 4 = xf + xn, 5 = xf +xn + e
      
      he[i] = new TH1F(Form("he%d", i), 
                       Form("Raw e (ch=%d); e (channel); count", i),
                       500, -500, 3500);
      
      hxfxn[i] = new TH2F(Form("hxfxn%d",i),
                           Form("Raw PSD XF vs. XN (ch=%d);XF (channel);XN (channel)",i),
                           500,0,2000,500,0,2000);
      
      hxfxne[i] = new TH2F(Form("hxfxne%d", i), 
                       Form("Raw e vs xf+xn (ch=%d); xf+xn (channel); e (channel)", i),
                       500, -500, 2000, 500, -500, 2000);
      
      heC[i] = new TH1F(Form("heC%d", i), 
                       Form("Corrected e (ch=%d); e (MeV); count", i),
                       500, 1, 7);
      
      hxfxnC[i] = new TH2F(Form("hxnC%d",i),
                           Form("Corrected XF vs. XN (ch=%d);XF (channel);XN (channel)",i),
                           500,0,2000,500,0,2000);      
                           
      hxfxneC[i] = new TH2F(Form("hxfxneC%d", i), 
                       Form("Raw e vs Corrected xf+xn (ch=%d); corrected xf+xn (channel); Raw e (channel)", i),
                       500, -500, 2000, 500, -500, 2000);           
      
      heVx[i] = new TH2F(Form("heVx%d",i),
                           Form("Raw PSD E vs. X (ch=%d);X (channel);E (channel)",i),
                           500,-0.1,1.1,500,0,4000);
                           
      hecalVxcal[i] = new TH2F(Form("hecalVxcal%d",i),
                           Form("Cal PSD E vs. X (ch=%d);X (cm);E (MeV)",i),
                           500,-0.25,5.25,500,0,20);
   }
   
   //array loop
   hecalVz = new TH2F("hecalVz","E vs. Z;Z (cm);E (MeV)",700,-55,0,750,0,10);
   hecalVzR = new TH2F("hecalVzR","E vs. Z gated;Z (cm);E (MeV)",700,-55,0,750,0,10);
   
   for( int i = 0; i < 4; i++){
      hecalVzRow[i] = new TH2F(Form("hecalVz%d", i),
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
   helum[0] = new TH2F("helum0","Elum Ring Energies; E (channels); Ring Number", 500,100,4000,16,0,16);
   helum[1] = new TH2F("helum1","Elum Wedge Energies; E (channels); Ring Number", 500,100,4000,16,0,16);

   //TAC
   htac[0] = new TH1F("htac0","Array-RDT0 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[1] = new TH1F("htac1","Array-RDT1 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[2] = new TH1F("htac2","Array-RDT2 TAC; DT [clock ticks]; Counts",6,0,6);
   htac[3] = new TH1F("htac3","Array-RDT3 TAC; DT [clock ticks]; Counts",6,0,6);

   htacE = new TH1F("htacE","Elum-RDT TAC; DT [clock ticks]; Counts",4,0,4);

   hexC = new TH1F("hexC","excitation spectrum",500,-5,10);
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
      printf("============\n");
      printf("mass-b  : %f MeV/c2 \n", mass);
      printf("charge-b: %f \n", q);
      printf("E-total : %f MeV \n", Et);
      printf("mass-B  : %f MeV/c2 \n", massB);      
      printf("beta    : %f \n", beta);
      printf("B-field : %f T \n", Bfield);
      printf("alpha   : %f MeV/mm \n", alpha);
      printf("a       : %f mm \n", a);
      printf("G       : %f MeV \n", G);
      printf("============\n");

   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();

   StpWatch.Start();
}

void Monitors::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

/*###########################################################
 * Process
###########################################################*/
Bool_t Monitors::Process(Long64_t entry)
{
   ProcessedEntries++;
    
      if (ProcessedEntries>NumEntries*Frac-1) {
         printf(" %3.0f%% (%llu/%llu k) processed in %6.1f seconds\n",
                  Frac*100,ProcessedEntries/1000,NumEntries/1000,StpWatch.RealTime());
         StpWatch.Start(kFALSE);
         Frac+=0.1;
      }

    b_Energy->GetEntry(entry);
    b_XF->GetEntry(entry);
    b_XN->GetEntry(entry);
    b_RDT->GetEntry(entry);
    b_TAC->GetEntry(entry);
    b_ELUM->GetEntry(entry);
    b_EZERO->GetEntry(entry);
    b_EnergyTimestamp->GetEntry(entry);
    b_RDTTimestamp->GetEntry(entry);
    b_TACTimestamp->GetEntry(entry);
    b_ELUMTimestamp->GetEntry(entry);
    b_EZEROTimestamp->GetEntry(entry);

    //Do calculations and fill histograms
    //Array calcs first
    for (Int_t i = 0; i < numDet; i++) {
      
      //fill raw data
      he[i]->Fill(e[i]);
      hxfxn[i]->Fill(xf[i],xn[i]);
      hxfxne[i]->Fill(xf[i]+xn[i], e[i]);
      
      if( !TMath::IsNaN(e[i])  && !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(5);
      if( !TMath::IsNaN(xf[i]) && !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(4);
      if( !TMath::IsNaN(xn[i]) ) hStat[i]->Fill(3);
      if( !TMath::IsNaN(xf[i]) ) hStat[i]->Fill(2);
      if( !TMath::IsNaN(e[i])  ) hStat[i]->Fill(1);
      if( TMath::IsNaN(e[i])  && TMath::IsNaN(xf[i]) && TMath::IsNaN(xn[i]) ) hStat[i]->Fill(0);
      
      //Calibrations go here
      xfcal[i] = xf[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      xncal[i] = xn[i]*xnCorr[i]*xfxneCorr[i][1]+xfxneCorr[i][0];
      ecal[i] = e[i]/eCorr[i][0]+eCorr[i][1];
      ecrr[i] = e[i]/eCorr[i][0]+eCorr[i][1];
      
      //fill corrected data
      heC[i]->Fill(ecrr[i]);
      hxfxnC[i]->Fill(xfcal[i], xncal[i]);
      hxfxneC[i]->Fill(xncal[i] + xfcal[i], e[i]);
      
      //calculate X
      if( (xf[i] > 0 || !TMath::IsNaN(xf[i])) && ( xn[i]>0 || !TMath::IsNaN(xn[i])) ) {
        x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;
        //x[i] = 0.5*((xf[i]-xn[i]) / e[i])+0.5;
      }
      
      if (xfcal[i]>0.5*e[i]) {
        xcal[i] = xfcal[i]/e[i];
      }else if (xncal[i]>=0.5*e[i]) {
        xcal[i] = 1.0 - xncal[i]/e[i];
      }else{
        xcal[i] = TMath::QuietNaN();
      }
      
      //calculate Z
      //z[i] = 5.0*(xcal[i]-0.5) + z_off + z_array_pos[i%6];//for downstream?
      z[i] = 5.0*(xcal[i]-0.5) - z_off - z_array_pos[i%6];
      
      //Array fill next
      if( -1.1 < x[i] && x[i] < 1.1 && e[i]>100 && (xn[i]>0||xf[i]>0)) {
        heVx[i]->Fill(x[i],e[i]);
        hecalVxcal[i]->Fill(xcal[i]*5.0,ecrr[i]);
        hecalVz->Fill(z[i],ecrr[i]);
        for (Int_t ii=0;ii<4;ii++) hrdtg[ii]->Fill(rdt[ii+4],rdt[ii]);
      }
      
    }//array loop
    
    for( int i = 0; i < 4; i++){
      for(int j = 0; j < 6; j++){
         int k = 6*i+j;
         if( ((xf[k] > 0 || !TMath::IsNaN(xf[k]))  && ( xn[k]>0 || !TMath::IsNaN(xn[k]))) ) hecalVzRow[i] -> Fill( z[k], ecrr[k]);
      }
    }
    
    /* RECOILS */
    for (Int_t ii=0;ii<4;ii++) hrdt[ii]->Fill(rdt[ii+4],rdt[ii]);

    /* ELUM */
    for (Int_t i = 0; i < 16; i++) {
      helum[0]->Fill(elum[i],i);
      helum[1]->Fill(elum[i+16],i);
      for(Int_t j=0;j<4;j++){
        if(rdt[j]-elum_t[i]>-10&&rdt[j]-elum[i]<10) htacE->Fill(0.5+j);
      }
    }

    //EZERO
    he0dee->Fill(ezero[1],ezero[0]);
    he0det->Fill(TMath::Abs(tac[0]),ezero[0]);
    he0et->Fill(TMath::Abs(tac[0]),ezero[1]);
    h0detet->Fill((int)(ezero_t[0]-ezero_t[1]));
    h0dettact->Fill((int)(ezero_t[0]-tac_t[0]));
    h0ettact->Fill((int)(ezero_t[1]-tac_t[0]));
    h0de->Fill(ezero[0]);
    h0e->Fill(ezero[1]);
    h0tac->Fill(TMath::Abs(tac[0]));
    
    //TACs
    for(Int_t i = 0; i < 4 ; i++){
      for(Int_t j = 0; j < 6; j++){
        
         int detID = i*6+j;
        
         if( isReaction ){
            //======== Ex calculation by Ryan 
            double y = ecrr[detID] + mass; // to give the KE + mass of proton;
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
        hexC->Fill(Ex);
        
        //CUTS
        if( isCutFileOpen){
          for( int k = 0 ; k < numCut; k++ ){
            cutG = (TCutG *)cutList->At(k) ;
            if( cutG->IsInside(rdt[k+4], rdt[k]) ) { //CRH
              for (Int_t kk = 0; kk < 4; kk++) { 
                tacA[detID]= (int)(rdt_t[kk]-e_t[detID]);
                if(-30 < tacA[detID] && tacA[detID] < 30) {
                  hexR->Fill(Ex);
                  hecalVzR->Fill(z[detID],ecrr[detID]);
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
   //when recoils are available...
   /*
   cCanvas  = new TCanvas("cCanvas","Plots",1250,1000);
   cCanvas->Modified(); cCanvas->Update();
   cCanvas->cd(); cCanvas->Divide(1,2);
   cCanvas->cd(1); gPad->Divide(4,1);
   
   gStyle->SetOptStat("neiou");
   
   for (Int_t i=0;i<4;i++) {
      cCanvas->cd(1);gPad->cd(i+1); hrdtg[i]->Draw("box colz");
      if( isCutFileOpen ) {
         cutG = (TCutG *)cutList->At(i);
         cutG->Draw("same");
      }
   }
   cCanvas->cd(2); gPad->Divide(2,1);
   cCanvas->cd(2);gPad->cd(1); hecalVz->Draw("colz");
   cCanvas->cd(2);gPad->cd(2); hecalVzR->Draw("colz box");//hexC->Draw();
   cCanvas->cd();
   */
   StpWatch.Start(kFALSE);
   
   gROOT->ProcessLine(".L /Users/heliosdigios/experiments/iss631/ryanAnaCodes/AutoCali/Utils.C");
   printf("=============== loaded Utils.C\n");
   gROOT->ProcessLine("listDraws()");
}
