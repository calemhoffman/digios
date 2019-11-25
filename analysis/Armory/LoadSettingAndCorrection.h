//////////////////////////////////////////////////////////
// still testing
//////////////////////////////////////////////////////////

#ifndef LoadSettingAndCorrection
#define LoadSettingAndCorrection

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TF1.h>
#include <string>
#include <fstream>
#include <TObjArray.h>
#include <TCutG.h>
#include "TClonesArray.h"

   
//===================================================== loading parameter

double Bfield;
double perpDist;
double length;
double firstPos;
double xnCorr[30]; // xn correction for xn = xf
double xfxneCorr[30][2]; //xf, xn correction for e = xf + xn
double xCorr[30]; // correction of x, scale to (-1,1)

double eCorr[30][2]; // e-correction
double rdtCorr[8]; //rdt-correction

double cTCorr[30][9]; // coinTime correction
TF1 ** f7 ; //!

bool isReaction;
double G, Z, H; // for excitation calcualtion
double mass, q;
double beta, gamma;
double alpha ;
double Et, massB;


void LoadGeometery(string detGeoFileName = "detectorGeo.txt" ){
   //========================================= detector Geometry
   printf("======================= loading parameters files .... \n");
   printf("loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0  )          Bfield = atof(x.c_str());
         if( i == 3  )        perpDist = atof(x.c_str());
         if( i == 5  )          length = atof(x.c_str());
         if( i == 14 )        firstPos = atof(x.c_str());
         if( i == 17 )            jDet = atoi(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      vector<double> posTemp = pos;
      for(int id = 0; id < iDet; id++){
        if( firstPos > 0 ) pos[id] = firstPos + posTemp[id];
        if( firstPos < 0 ) pos[id] = firstPos - posTemp[iDet-1-id];
      }
      
      printf(" Bfield       : %6.2f T\n", Bfield);
      printf(" PSD prepDist : %6.3f mm\n", perpDist);
      printf(" PSD length   : %6.3f mm\n", length);
      printf(" # row Det    : %6d \n", jDet);
      printf(" # col Det    : %6d \n", iDet);
      printf("----------- list of detector position\n");
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("==================================\n");
      
   }else{
       printf("... fail\n");
       Bfield = 2.5;
       firstPos =  0;
       perpDist = 9;
       length = 50.5;
       jDet = 4;
       iDet = 6;
       pos[0] = 0;
       pos[1] = 58.6;
       pos[2] = 117.9;
       pos[3] = 176.8;
       pos[4] = 235.8;
       pos[5] = 294.8;
   }
   
   numDet = iDet * jDet;
   
}

   //========================================= xf = xn correction
   printf("loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("................... done.\n");
   }else{
      printf("................... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   printf("loading xf/xn-e correction.");
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
      printf("................. done.\n");
   }else{
      printf("................. fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   printf("loading e correction.");
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
      printf("....................... done.\n");
      
   }else{
      printf("....................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
      //return;
   }
   file.close();
   
   
   //========================================= x-scale correction
   printf("loading x correction.");
   file.open("correction_scaleX.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a){
         if( i >= numDet) break;
         xCorr[i] = a;  
         i = i + 1;
      }
      printf("....................... done.\n");
      
   }else{
      printf("....................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         xCorr[i] = 1.;
      }
      //return;
   }
   file.close();
   
   
   //========================================= rdt correction
   printf("loading rdt correction.");
   file.open("correction_rdt.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a){
         if( i >= 8) break;
         rdtCorr[i] = a;  //
         i = i + 1;
      }
      printf("..................... done.\n");
      
   }else{
      printf("..................... fail.\n");
      for( int i = 0; i < numDet ; i++){
         rdtCorr[i] = 1.;
      }
   }
   file.close();
   
   //========================================= coinTime correction
   if( isTraceDataExist ){
      printf("loading coin-Time correction parameters.");
      file.open("correction_coinTime.dat");
      
      f7 = new TF1*[numDet];
      fList = new TObjArray();
      
      if( file.is_open() ){
         int d;
         double a0, a1, a2, a3, a4, a5, a6, a7, a8;
         int i = 0;
         while( file >>  d >> a0 >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8){
            if( i >= numDet) break;
            cTCorr[i][0] = a0;
            cTCorr[i][1] = a1;
            cTCorr[i][2] = a2;
            cTCorr[i][3] = a3;
            cTCorr[i][4] = a4;
            cTCorr[i][5] = a5;
            cTCorr[i][6] = a6;
            cTCorr[i][7] = a7;
            cTCorr[i][8] = a8; // this is the offset find by fitting the 1-D plot
            //printf("\n%2d, a0: %6.2f, a1: %6.2f .... a7: %6.2f", i, cTCorr[i][0], cTCorr[i][1], cTCorr[i][7]);
            i = i + 1;
         }
         printf(".... done.\n");
         
      }else{
         printf(".... fail.\n");
         for( int i = 0; i < numDet; i++){
            for( int j = 0 ; j < 9; j++) cTCorr[i][j] = 0.;
         }
      }

      TString name;
      for(int i = 0; i < numDet; i++){
         name.Form("f%d", i);
         f7[i] = new TF1(name, "pol7", -2, 2);
            
         for(int j = 0 ; j < 8; j++){
            f7[i]->SetParameter(j, cTCorr[i][j]);
         }

         if(file.is_open()) fList->Add(f7[i]);
      }

      if( file.is_open()){
         fList->Write("fList", TObject::kSingleKey);
      }

      file.close();
   }
   
   //========================================= reaction parameters
   printf("loading reaction parameter.");
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
      printf("................. done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.;
      gamma = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamma * beta * perpDist ;
      printf("============\n");
      printf("mass-b   : %f MeV/c2 \n", mass);
      printf("charge-b : %f \n", q);
      printf("E-total  : %f MeV \n", Et);
      printf("mass-B   : %f MeV/c2 \n", massB);      
      printf("beta     : %f \n", beta);
      printf("B-field  : %f T \n", Bfield);
      printf("alpha    : %f MeV/mm \n", alpha);
      printf("perpDist : %f mm \n", perpDist);
      printf("G        : %f MeV \n", G);


   }else{
      printf("................. fail.\n");
      isReaction = false;
   }
   file.close();

   //====================================== load RDT cut
   TFile * fileCut = new TFile("rdtCuts_trace_C2.root");   
   TObjArray * cutList = NULL;
   isRDTCutExist = false;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      if( cutList != NULL){
         isRDTCutExist = true;
         const int numCut = cutList->GetEntries();
         cut = new TCutG * [numCut];
         printf("=========== found %d cuts in %s \n", numCut, fileCut->GetName());
         for( int i = 0 ; i < numCut; i++){
            cut[i] = (TCutG* ) cutList->At(i);
            printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
         }
      }
   }

   printf("================================== numDet : %d \n", numDet);
   

}


#endif // #ifdef Cali_e_trace_cxx
