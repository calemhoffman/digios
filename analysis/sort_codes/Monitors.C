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

TH1F* he[24];
TH2F* hxfxn[24];
TH2F* hxfxne[24];
TH2F* heVx[24];
TH1F* heC[24];
TH2F* hxfxnC[24]; 
TH2F* hxfxneC[24];
TH2F* hecalVxcal[24];
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
TH1I* htacArray[24];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

Float_t x[24],z[24];
Float_t xcal[24],ecal[24],xfcal[24],xncal[24],ecrr[24],ezero[10];
Int_t tacA[24];
Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=6.5;//to the physical end of array from the target (-20 to si start)
Float_t xnCorr[24] = { 
 0.927777,
 0.997416,
 0.983927,
 0.921836,
 1.031119,
 0.932300,
 0.924544,
 0.964924,
 1.031305,
 0.948247,
 0.993230,
 0.993230,
 0.982570,
 0.996820,
 1.025245,
 0.968476,
 1.032115,
 0.916941,
 0.968057,
 0.934456,
 0.903874,
 0.969678,
 1.029105,
 1.088023};
Float_t xfxneCorr[24][2] = {
{21.734893,   0.903570},
{16.983617,   0.926099},
{11.953286,   1.037609},
{11.118699,   1.068145},
{12.792129,   0.886603},
{10.134575,   0.959260},
{ 6.012734,   1.014910},
{ 5.246817,   0.965618},
{ 4.760088,   0.982979},
{ 5.140974,   0.975379},
{ 3.307871,   0.910984},
{ 0.000000,   1.000000},
{ 6.678441,   0.787783},
{11.383868,   0.917190},
{11.139707,   0.927444},
{10.806677,   0.981076},
{ 9.559186,   0.983702},
{ 8.281657,   0.977478},
{ 8.727315,   0.898558},
{ 6.783902,   0.981707},
{15.131056,   1.042929},
{14.245593,   0.893980},
{16.216401,   1.102147},
{ 2.493960,   0.884956}};

Float_t eCorr[24][2] = {
{256.060637,	-0.003816},
{253.083810,	-0.015280},
{278.735623,	 0.021721},
{267.979831,	-0.008818},
{250.108256,	-0.001003},
{247.134021,	 0.013641},
{262.017938,	-0.006414},
{253.083810,	 0.010404},
{262.017938,	-0.006414},
{256.060637,	-0.003816},
{239.341772,	-0.009266},
{ 1.000000,	 0.000000   },
{213.689980,	-0.026390},
{250.108256,	-0.001003},
{244.161153,	 0.002046},
{253.083810,	 0.010404},
{263.857355,	 0.017556},
{259.038694,	 0.032498},
{252.842600,  -0.036309},
{254.005728,  -0.141253},
{265.202952,  -0.048925},
{255.575575,   0.092847},
{289.613274,  -0.078108},
{242.487892,   0.047080}};
   
Float_t exCorr[6] = { 938.272,  // mass of proton
	                   1,        // charge of proton
	                   27954.0982, // cm frame total energy
	                   26996.5929, // mass of recoil
	                   0.132178, // beta to CM frame
	                   2.5}; // Bfield [T]
double a = 11.5 ; // perpendicular distance of detector to axis [mm]
double Ex, thetaCM;

double alpha, Et, beta, gamm, G, massB, mass; //variables for Ex calculation
      

Float_t tempTime=-1000;
Long64_t tempTimeLong=10001;

void Monitors::Begin(TTree *tree)
{
   TString option = GetOption();
   NumEntries = tree->GetEntries();

   //Generate all of the histograms needed for drawing later on
  
   for (Int_t i=0;i<24;i++) {//array loop
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

   for (Int_t i=0;i<24;i++) {
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
  
   alpha = 299.792458 * exCorr[5] * exCorr[1] / TMath::TwoPi() / 1000; //MeV/mm
   beta = exCorr[4];
   gamm = 1./TMath::Sqrt(1-beta*beta);
   G = alpha * gamm * beta * a;
   massB = exCorr[3];
   mass = exCorr[0];
   Et = exCorr[2];
  
   printf("======== number of cuts found : %d \n", numCut);

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
    for (Int_t i = 0; i < 24; i++) {
      
      //fill raw data
      he[i]->Fill(e[i]);
      hxfxn[i]->Fill(xf[i],xn[i]);
      hxfxne[i]->Fill(xf[i]+xn[i], e[i]);
      
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
   
   gROOT->ProcessLine(".L ~/experiments/iss000/analysis/ryanAnaCodes/AutoCali/Utils.C");
   printf("=============== loaded Utils.C\n");
   gROOT->ProcessLine("listDraws()");
}
