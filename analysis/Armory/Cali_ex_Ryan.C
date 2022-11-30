#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TProfile.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TMath.h>
#include "../Armory/AnalysisLibrary.h"
#include "../Armory/AutoFit.C"

void Cali_ex_Ryan(TTree *tree){
   
/**///======================================================== initial input
   int energyRange[2] = {500, 5000};
   
   gStyle->SetOptStat(11111111);
   
   printf("============================================================= \n");
   printf("=====================  Cali_ex_Ryan.C  ====================== \n");
   printf("============================================================= \n");
   printf("==== flatten e - x  \n");
   printf("------------------------------------------------------------- \n");
   printf("=========== Total #Entry: %10lld \n", tree->GetEntries());


/**///========================================================= load parameters

   //========================================= detector Geometry
   printf("======================= loading parameters files .... \n");
   string detGeoFileName = "detectorGeo.txt";
   printf("loading detector geometery : %s.", detGeoFileName.c_str());

   DetGeo detGeo;

   TMacro * haha = new TMacro();
   if( haha->ReadFile(detGeoFileName.c_str()) > 0 ) {

      detGeo = LoadDetectorGeo(haha);

      PrintDetGeo(detGeo);
      
      printf("... done.\n");
   }else{
      printf("... fail\n");
      return;
   }
   
   double length = detGeo.detLength;
   vector<double> pos = detGeo.detPos;
   
   int colDet = detGeo.nDet;
   int rowDet = detGeo.mDet;
      
   int nDet = colDet * rowDet;

   delete haha;


   double xnCorr[nDet];
   
   //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   ifstream file;
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= nDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for( int i = 0 ; i < nDet ; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
/**///======================================================== Browser or Canvas

   Int_t Div[2] = {colDet,rowDet};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cCali_xf_xn_e = new TCanvas("cCali_xf_xn_e", "cCali_xf_xn_e", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCali_xf_xn_e->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCali_xf_xn_e->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   if(cCali_xf_xn_e->GetShowEditor()  )cCali_xf_xn_e->ToggleEditor();
   if(cCali_xf_xn_e->GetShowToolBar() )cCali_xf_xn_e->ToggleToolBar();

   
   
/**///========================================================= Analysis




}
