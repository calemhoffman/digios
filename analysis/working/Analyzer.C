#define Analyzer_cxx

#include "Analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TCutG.h>
#include <TObjArray.h>
///#include "../AutoCali/AutoFit.C"


int numRow = 6;
int numCol = 5;
const int numDet = numRow * numCol ;

//######################################## User Inputs

double rangeEx[3] = { 100, -2, 4}; ///resol. [keV], low Ex, high Ex
double rangeCM[3] = {1, 0, 45}; ///resol. [deg], low deg, high deg
double rangeZ[2] = { 700, 1000};

bool isExOffset = true;
double ExOffset[30] = { ///calibrated by h064_15N, (d,p), run013
   0.0000,  0.0000,  0.0000,  0.0000,  0.0000, 
   0.0000,  0.1590,  0.0000,  0.0570,  0.0000, 
   0.0000,  0.0000,  0.4800,  0.2700,  0.0000,
   0.0000,  0.0601,  0.0000,  0.0000,  0.0000,
   0.0000,  0.0000,  0.0400,  0.0000, -0.0100,
   0.0000,  0.1485,  0.1900,  0.0000,  0.0000};

//16N(d,3He)
//vector<int> listOfBadDet = {0, 1, 2, 3, 4,  6, 7, 9,  11, 14, 18, 19, 23, 28, 29};
//TString rdtCutFile = "rdtCuts_15C_tight.root"; //"rdt_15N_degraded.root";
//double coinTimeGate[2] = {17, 9}; // mean, half-width

//16N(d,t)
vector<int> listOfBadDet = {0, 1, 2, 3, 4,  7, 8, 9,  11, 14, 18, 19, 23, 24, 28, 29};
TString rdtCutFile = "rdtCuts_15N_2.root"; 
double coinTimeGate[2] = {65, 15}; // mean, half-width

//######################################## End of User Input

//=========================== Global histograms
TH1F * hRun; /// runID
TH1F * hMultiHit ; 

TH1F ** hEBISi; /// EBIS for each detector

TH1F ** hei; /// e
TH2F ** hexi; /// e vs x for each detector

TH1F ** hxi; ///x for each detector

TH2F * hez;  ///e vs z
TH2F * heza;  ///e vs z, |x| < 0.9
TH2F ** hezr; ///e vs z for each row

TH2F ** hxExi; ///x vs Ex for each detector

TH1F * hEx; ///Ex
TH1F ** hExi; ///Ex for each detector

TH1F ** hExc; ///Ex for each column
TH1F ** hExcA; ///Ex for each column more detectors summed
TH1F ** hExca; ///Ex gated x<0
TH1F ** hExcb; ///Ex gated x>0

TH2F ** hExzr; ///Ex vs z

TH2F * hExT; ///Ex vs thetaCM
TH2F * hExTa; ///Ex vs thetaCM, |x| < 0.9

TH1F ** hExr; ///Ex for each row
TH2F ** hExTr; ///Ex vs thetaCM for each row 

TH1F * hT; ///thetaCM
TH2F * hTz; ///thetaCM vs z
TH1F * hTa; ///thetaCM, |x| < 0.9


TCanvas * cAna;
Int_t totnumEntry;
TBenchmark gClock;
Bool_t shown;

//======= Recoil Cut
TString cutName("cut1");
TCutG* cutG; //!
TObjArray * cutList;
TString cutTag;
Bool_t isCutFileOpen;
int numCut;
vector<int> countFromCut;

void Analyzer::Begin(TTree *tree)
{
   TString option = GetOption();

   totnumEntry = tree->GetEntries();
   hRun = new TH1F("hRun", "runID with gate; runID; count / run", 50, 20, 70);
   hMultiHit = new TH1F("hMultiHit", "MultiHit; hit; count / hit ", 10, 0, 10);
   
   ///ex spectrum
   hEx   = new TH1F("hEx"  ,  Form("Ex; Ex[MeV]; count / %2.0f keV", rangeEx[0]) , (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);

   /// thetaCM spectrum
   hT  = new TH1F("hT",  Form("thetaCM ; thetaCM [deg]; count / %2.0f deg", rangeCM[0])     , (rangeCM[2]-rangeCM[1])/rangeCM[0], rangeCM[1], rangeCM[2]);
   hTa = new TH1F("hTa", Form("thetaCM (|x|<0.9); thetaCM [deg]; count / %2.0f deg", rangeCM[0]), (rangeCM[2]-rangeCM[1])/rangeCM[0], rangeCM[1], rangeCM[2]);

   ///E vs Z
   hez  = new TH2F("hez" , "e - z; z [mm]; e [MeV]", 450, rangeZ[0], rangeZ[1], 400, 0, 10);
   heza = new TH2F("heza", "e - z (|x|<0.9); z [mm]; e [MeV]", 450, rangeZ[0], rangeZ[1], 400, 0, 10);
   
   ///Ex vs thetaCM
   hExT  = new TH2F("hExT" , "Ex - thetaCM; thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);
   hExTa = new TH2F("hExTa", "Ex - thetaCM (|x|<0.9); thetaCM [deg]; Ex [MeV]", 400, 0, 50, 400, -1, 5);


   hTz = new TH2F("hTz", "thetaCM vs z; z [mm]; thetaCM [deg]", 450, rangeZ[0], rangeZ[1], 500, rangeCM[1], rangeCM[2]);

   hEBISi = new TH1F *[numDet];
   hei = new TH1F *[numDet];
   hexi = new  TH2F *[numDet];
   hxi = new  TH1F *[numDet];
   hxExi = new TH2F *[numDet];
   hExi = new TH1F *[numDet];

   for( int i = 0; i < numDet; i ++){
      hei[i]     = new TH1F(Form("hei%d", i) , Form("e (det-%d); e [MeV]; count/ 10 keV", i), 1000, 0, 10);
      hxi[i]     = new TH1F(Form("hxi%d", i) , Form("x (det-%d); x [a.u.]; count ", i), 400, -1.2, 1.2);
      hEBISi[i]  = new TH1F(Form("hEBISi%d", i) , Form("e_t - EBIS_t (det-%d); time [us]; count/ 1 us", i), 1500, 0, 1500);
      hExi[i]    = new TH1F(Form("hExi%d", i) , Form("Ex (det-%d); Ex[MeV]; count/ %2.0f keV", i,  rangeEx[0]), (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);
      hexi[i]    = new TH2F(Form("hexi%d", i) , Form("e - x (det-%d); x [a.u]; e [MeV]", i), 450, -1.2, 1.2, 400, 0, 10);
      hxExi[i]   = new TH2F(Form("hxExi%d", i), Form("x - Ex (det-%d); Ex [MeV); x [a.u.]", i), 400, -1, 5, 400, -1.2, 1.2);         
   }

   hezr = new TH2F * [numRow];
   hExTr = new TH2F * [numRow];
   hExzr = new TH2F * [numRow];
   hExr = new TH1F * [numRow];
   
   for( int i = 0; i < numRow; i++){
      hExTr[i] = new TH2F(Form("hExTr%d", i), Form("Ex - thetaCM (row-%d); thetaCM [deg]; Ex [MeV]", i) , 400, 0, 50, 400, -1, 5);
      hExzr[i] = new TH2F(Form("hExzr%d", i), Form("Ex - z (row-%d); z [mm]; Ex [MeV]", i) , 400, rangeZ[0], rangeZ[1], 400, -1, 5);
      hezr[i]  = new TH2F(Form("hezr%d", i) , Form("e - z (row-%d); z [mm]; e [MeV]", i) , 400, rangeZ[0], rangeZ[1], 400, 0, 13);
      hExr[i]  = new TH1F(Form("hExr%d", i) , Form("Ex (row-%d); e [MeV] ; count / %2.0f keV", i, rangeEx[0]) , (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);
   }

   hExc = new TH1F * [numCol];
   hExcA = new TH1F * [numCol];
   hExca = new TH1F * [numCol];
   hExcb = new TH1F * [numCol];

   for( int i = 0; i < numCol; i++){
      hExc[i] = new TH1F(Form("hExc%d", i), Form("Ex (col-%d); Ex [MeV]; count / %2.0f keV", i, rangeEx[0]) , (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);
      hExcA[i] = new TH1F(Form("hExcA%d", i), Form("Ex (col-%d) All; Ex [MeV]; count / %2.0f keV", i, rangeEx[0]) , (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);
      hExca[i]  = new TH1F(Form("hExca%d",i) ,  Form("Ex (x<0, col-%d); Ex[MeV]; count / %2.0f keV",i, rangeEx[0]), (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);
      hExcb[i]  = new TH1F(Form("hExcb%d",i) ,  Form("Ex (x>0, col-%d); Ex[MeV]; count / %2.0f keV",i, rangeEx[0]), (rangeEx[2]-rangeEx[1])/rangeEx[0]*1000., rangeEx[1], rangeEx[2]);   
   }
   
   
   ///================  Get Recoil cuts;
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
   
   gClock.Reset();
   gClock.Start("timer");
   shown = 0;
}

void Analyzer::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

Bool_t Analyzer::Process(Long64_t entry)
{
   b_run->GetEntry(entry, 0);
   b_e->GetEntry(entry,0);
   ///b_xf->GetEntry(entry,0);
   ///b_xn->GetEntry(entry,0);
   b_x->GetEntry(entry,0);
   b_z->GetEntry(entry,0);
   b_ring->GetEntry(entry,0);
   b_Ex->GetEntry(entry, 0);
   b_thetaCM->GetEntry(entry, 0);
   b_e_t->GetEntry(entry, 0);
   ///b_EBIS_t->GetEntry(entry, 0);
   ///b_hitID->GetEntry(entry, 0);

   if( isTraceDataExist ) {
     b_te->GetEntry(entry,0);
     b_te_t->GetEntry(entry,0);
     b_te_r->GetEntry(entry,0);
     b_trdt->GetEntry(entry,0);
     b_trdt_t->GetEntry(entry,0);
     b_trdt_r->GetEntry(entry,0);
     b_coinTime->GetEntry(entry, 0);
   }
   
   //#################### recoil gate
   bool rdtgate = false;
   if( isCutFileOpen ) {
      for(int i = 0 ; i < numCut ; i++ ){
         cutG = (TCutG *)cutList->At(i) ;
         if(cutG->IsInside(trdt[2*i],trdt[2*i+1])) {
            rdtgate= true;
            break; // only one is enough
         }
      }
   }else{
      rdtgate = true;
   }
   if( !rdtgate ) return kTRUE; 
   
   rdtgate = true;
   
   hRun->Fill(run);
   
   int multiHit = 0;

   for( int i = 0 ; i < numDet ; i++){
      
      //################################################## gate
      
      ///if( e_t[i] == 0 ) continue;
      
            
      ///hEBISi[i]->Fill( (e_t[i]-ebis_t)/1e2 );

      ///======= cut ring
      //if( ring[i] > 50 ) continue;

      ///======== cut-EBIS
      ///if( !(100 < (e_t[i]-ebis_t)/1e2 && (e_t[i]-ebis_t)/1e2 < 900 )) continue;
      ///if( 11000 < (e_t-EBIS_t)/1e2 && (e_t-EBIS_t)/1e2 < 19000 ) 
      
      ///======== cut-e, z
      if( TMath::IsNaN(e[i]) ) continue;
      if( TMath::IsNaN(z[i]) ) continue;
     
      if( e[i] < 1 ) continue;
     
      ///======== cut-coinTime;
      if( abs(coinTime-coinTimeGate[0]) > coinTimeGate[1] ) continue;
     
      ///======== cut-x, = (-1,1)
      ///if( !(TMath::Abs(x[i]) < 0.95) ) continue;      
      

      ///======== cut-hitID
      ///if( hitID[i] != 0 ) continue;
      
      ///======== cut-thetaCM
      if( thetaCM < 10 ) continue;
      
      ///======== cut-det
      bool badDetFlag = false;
      for( int p = 0 ; p < (int) listOfBadDet.size(); p ++){
        if( i == listOfBadDet[p] ) badDetFlag = true;
      }
      if( badDetFlag ) continue;
      
      double scaling = 1;
      ///TODO auto calculate from a list-of-bad-det
      ///if( i%numCol == 0 ) scaling = numRow/3.;
      ///if( i%numCol == 1 ) scaling = numRow/6.;
      ///if( i%numCol == 2 ) scaling = numRow/5.;
      ///if( i%numCol == 3 ) scaling = numRow/5.;
      ///if( i%numCol == 4 ) scaling = numRow/3.;

      ///======== special gate;
      ///if( i == 16 && e[i] < 6 ) continue;

      //################################################## 
      
      Ex = ExCal( e[i] , z[i] );
      //if( isExOffset) Ex = Ex - ExOffset[i];
      
      //============ 16N(d,3He)
      //if( i ==  5 ) Ex = 1.10 * Ex - 0.023;
      //if( i == 10 ) Ex = 1.10 * Ex - 0.078;
      //if( i == 15 ) Ex = 1.00 * Ex + 0.035;
      //if( i == 20 ) Ex = 1.07 * Ex - 0.107;
      //if( i == 21 ) Ex = 0.89 * Ex - 0.054;
      //if( i == 25 ) Ex = 1.07 * Ex - 0.062;
      //if( i == 26 ) Ex = Ex - 0.22;
      //if( i == 12 ) Ex = Ex - 0.64;
      //if( i ==  8 ) Ex = Ex - 0.14;
      //if( i == 13 ) Ex = Ex - 0.24;
      //if( i == 22 ) Ex = Ex - 0.10;
      //if( i == 27 ) Ex = Ex - 0.30;
      
      
      //for 16N g.s. -> 15C(0.74)
      //if ( !( 2 > Ex && Ex > 0.4 ) ) continue;
      //if( i%5 == 0 && !(x[i] < 0 )) continue; //16N g.s. -> 15C(0.74), only the 2nd half is used.

      //if ( !( Ex < 0.4 ) ) continue;//16N iso -> 15C(0.00)
      
      //for total spectrum
      //if( 2 > Ex && Ex > 0.4 && i%5 == 0 && !(x[i] < 0 )) continue; //16N g.s. -> 15C(0.74), only the 2nd half is used.
      
      // compare the yield for thetaCM from 15.63 deg to 17.30 deg
      //if( i%5 != 0 ) continue;
      //if( 2 > Ex && Ex > 0.4 && !( x[i] < -0.4545 )) continue;
      //if( Ex < 0.4 && !( x[i] > 0.4545 )) continue;
      
      multiHit ++;
      
      hei[i]->Fill( e[i] );
      hxi[i]->Fill( x[i] );
      hexi[i]->Fill(x[i], e[i]);
      
      hExi[i]->Fill(Ex);
      hxExi[i]->Fill(Ex, x[i]);

      hez->Fill(z[i],e[i]);
      hExT->Fill(thetaCM,Ex);
      hT->Fill(thetaCM);      
      
      hTz->Fill(z[i], thetaCM);
      
      heza->Fill(z[i],e[i]);
      hExTa->Fill(thetaCM,Ex);
      hTa->Fill(thetaCM);
      
      int row = -1;
      for( int ri = 0; ri < numRow; ri++){
        if( ri*numCol <= i && i < (ri+1)*numCol) row = ri;
      }
      hezr[row]->Fill(z[i], e[i]);
      hExTr[row]->Fill(thetaCM, Ex);
      hExzr[row]->Fill(z[i], Ex);
      
      hExcA[i%numCol]->Fill(Ex);
      hExr[row]->Fill(Ex, scaling);
      
      //======== special gate;
      ///if( i == 1 || i == 5 || i == 17 || i == 6 || i == 7 || i == 8 || i == 12 || i == 18 || i == 19) continue;  ///for beyound ground state
      
      hExc[i%numCol]->Fill(Ex);
      hEx->Fill(Ex, scaling);
      
      if( x[i] < 0) {
         hExca[i%numCol]->Fill(Ex);
      }else{
         hExcb[i%numCol]->Fill(Ex);
      }
      
   }
   
   hMultiHit->Fill(multiHit);
   
   //#################################################################### Timer  
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   gClock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%3d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(entry+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Analyzer::SlaveTerminate()
{
}

double Analyzer::ExCal(double e, double z){
   
   double mass = 2808.3914;
   double q = 1;
   double beta = 0.140241;
   
   double Et = 16802.7129;
   double massB = 13968.9353;
   
   double Bfield = 2.0;
   double a = 11.5;

   double gamm = 1./TMath::Sqrt(1-beta*beta);

   double alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.; //MeV/mm
   double G = alpha * gamm * beta * a ;

   double Ex, thetaCM;
   
   //======== Ex calculation by Ryan 
   double y = e + mass; // to give the KE + mass of proton;
   double Z = alpha * gamm * beta * z;
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
   return Ex;
}

double Analyzer::findXMax(TH1F ** hist){
  
  double max = 0;
  int maxDetID = -1;
  for( int i = 0; i < numDet; i++){
    if( i == 11 ) continue;
    double temp = hist[i]->GetMaximum();
    if( temp > max) {
      max = temp;
      maxDetID = i;
    }
  }
  printf("====== detID %d is max : %f \n", maxDetID, max);
  
  return max;
}

void Analyzer::Terminate()
{
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   printf("%10d[%3d%%]|%3.0f min %5.2f sec | finished.\n", 
               totnumEntry, 
               100,
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.);
  
   gStyle->SetOptStat("neiou");
   
   //=========== load transfer.root
   TFile * ft = new TFile("transfer.root");
   TObjArray * fxList = (TObjArray *) ft->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray *) ft->FindObjectAny("gList");
   
   //=========== load AutoFit.C
   gROOT->ProcessLine(".L ../Armory/AutoFit.C");
  
   //=========== Plot
   int nX = 3, nY = 2;
   int sizeX = 300, sizeY = 300;
   cAna = new TCanvas("cAna", "Analyzer", nX * sizeX, nY * sizeY);
   cAna->Divide(nX,nY);
   for( int i = 1; i <= nX * nY ; i++) cAna->cd(i)->SetGrid();
   if( cAna->GetShowEditor() ) cAna->ToggleEditor(); 
   
   ///double max = findXMax(hei)
   double max = findXMax(hExi);
   
   //cAna->cd(1); hExT->Draw("colz");
   //hEx->Draw();
   
   cAna->cd(1); hEx->Draw();
   cAna->cd(2); hxExi[5]->Draw();
   cAna->cd(3); hxExi[10]->Draw();
   cAna->cd(4); hxExi[15]->Draw();
   cAna->cd(5); hxExi[20]->Draw();
   cAna->cd(6); hxExi[25]->Draw();
   
   /*
   //for( int i = 5; i < numDet; i++){
   for( int r = 1; r < 6; r++){
      for( int c = 0; c < 5; c++){
      int iDet = 5*r + c ;
         cAna->cd( 5*c + r );
         //cAna->cd(i+1)->SetGrid();
         //cAna->cd(i+1)->SetLogy();
         //hei[i]->GetYaxis()->SetRangeUser(0.5, max * 1.1);
         //hei[i]->Draw();

         //hxi[i]->Draw();

         //hEBISi[i]->Draw();
         //hExi[i]->GetYaxis()->SetRangeUser(0, max*1.2);
         if( iDet == 9 ) {
           hEx->Draw();
         }else{
           hExi[iDet]->Draw();
         }
         //fitAuto(hExi[i]);
         //fit2GaussP1(hExi[i], 0.0, 0.05, 1.2, 0.05, -1, 1.5, 0);

         //hExPi[i]->SetLineColor(2);
         //hExPi[i]->Draw("same");

         //2-D 
         //hexi[i]->Draw(); 
         //hxExi[i]->SetMarkerStyle(7);
         //hxExi[i]->SetMarkerColor(4);
         //hxExi[i]->Draw("scat");
         //hxExi[i]->Draw("colz");
      } 
   }*/
   
   ///for( int i = 0; i < numDet; i++){
   ///  //hei[i]->GetYaxis()->SetRangeUser(0, max);
   ///  hExi[i]->GetYaxis()->SetRangeUser(0, max);
   ///  if( i < 6 ) {
   ///     cAna->cd(i+1);
   ///     hExi[i]->Draw();
   ///  }
   ///  if( 6 <= i && i < 12) {
   ///     cAna->cd(i-6+1);
   ///     hExi[i]->SetLineColor(2); 
   ///     hExi[i]->Draw("same");
   ///  }
   ///  if(12 <= i && i < 18) {
   ///     cAna->cd(i-12+1);
   ///     hExi[i]->SetLineColor(kGreen+3); 
   ///     hExi[i]->Draw("same");
   ///  }
   ///  if(18 <= i && i < numDet) {
   ///     cAna->cd(i-18+1);
   ///     hExi[i]->SetLineColor(6); 
   ///     hExi[i]->Draw("same");
   ///  }
   ///} 
   
   ///for( int i = 0; i < 6; i++){
   ///  cAna->cd(i+1);
   ///  //cAna->cd(i+1)->SetGrid();
   ///  //hezr[i]->Draw("colz");
   ///  //fxList->At(0)->Draw("same");
   ///  //fxList->At(2)->Draw("same");
   ///  //fxList->At(5)->Draw("same");
   ///  //fxList->At(7)->Draw("same");
   ///  //gList->At(0)->Draw("same");
   ///  //gList->At(10)->Draw("same");
   ///  //hezr[i]->Draw("colz same");
   ///  
   ///  hExTr[i]->Draw("colz");
   ///  //hExzr[i]->Draw("colz");
   ///  //hExPzr[i]->Draw("colz");
   ///}
   
   ///for( int i = 0; i < 6; i++){
   ///  cAna->cd(i+1);
   ///  cAna->cd(i+1)->SetGrid();
   ///  hExc[i]->Draw();
   ///}
   
   /**/
}
