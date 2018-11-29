#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <fstream>

void Check_alignment(TString rootfileAlpha){ 

   //const char* rootfileAlpha="A_gen_run107.root";
   const char* rootfileSim="transfer.root";
   
   double eRange[3] = {200, 0, 10};
   
   int mode = 0; // 0 == all, 1 == column
   int detID = 1; // use when mode = 1
      
/**///========================================================== tree and canvas   
   printf("################### Check_aligment.C ######################\n");
   
   TFile *fa = new TFile (rootfileAlpha, "read"); 
   TTree * atree = (TTree*)fa->Get("tree");

   TFile *fs = new TFile (rootfileSim, "read"); 
   TTree * sTree = (TTree*)fs->Get("tree");
   
   Int_t Div[2] = {6,1};  //x,y
   Int_t size[2] = {200,600}; //x,y
   if( mode == 1) {
      Div[0] = 4;
   }
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   
/**///========================================================== load parameter
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   int numDet;
   int rDet = 6; // number of detector at different position, row-Det
   int cDet = 4; // number of detector at same position, column-Det
   vector<double> pos;
   double length = 50.5;
   double firstPos = -110;
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) cDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < rDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < rDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return;
   }
   
   numDet = rDet * cDet;
   
/**///========================================================== analysis  

   if( mode == 0 ){

      TH2F ** h1 = new TH2F*[rDet];
      TH2F ** h2 = new TH2F*[rDet];
      TH2F ** h3 = new TH2F*[rDet];
      TH2F ** h4 = new TH2F*[rDet];

      for( int iDet = 0 ; iDet < rDet ; iDet++){

         double zRange[2];
         
         if( firstPos > 0 ){
            zRange[0] = pos[iDet] - 10. ;
            zRange[1] = pos[iDet] + length + 10. ;
         }else{
            zRange[0] = pos[iDet] - length - 10.;
            zRange[1] = pos[iDet] + 10.;
         }
         
         cScript->cd(iDet+1);
         TString name;
         name.Form("h1%d", iDet); h1[iDet] = new TH2F (name, "e:z; z [mm]; e [a.u.]", 100, zRange[0], zRange[1], eRange[0], eRange[1], eRange[2]);
         name.Form("h2%d", iDet); h2[iDet] = new TH2F (name, "e:z; z [mm]; e [a.u.]", 100, zRange[0], zRange[1], eRange[0], eRange[1], eRange[2]);
         name.Form("h3%d", iDet); h3[iDet] = new TH2F (name, "e:z; z [mm]; e [a.u.]", 100, zRange[0], zRange[1], eRange[0], eRange[1], eRange[2]);
         name.Form("h4%d", iDet); h4[iDet] = new TH2F (name, "e:z; z [mm]; e [a.u.]", 100, zRange[0], zRange[1], eRange[0], eRange[1], eRange[2]);

         h1[iDet]->SetMarkerColor(2);
         h2[iDet]->SetMarkerColor(4);
         h3[iDet]->SetMarkerColor(8);
         h4[iDet]->SetMarkerColor(7);

         TString gate, express;
         gate.Form("detID == %d", iDet+18); express.Form("e:z >> h1%d", iDet); atree->Draw(express, gate);   
         gate.Form("detID == %d", iDet+12); express.Form("e:z >> h2%d", iDet); atree->Draw(express, gate, "same");  
         gate.Form("detID == %d", iDet+6);  express.Form("e:z >> h3%d", iDet); atree->Draw(express, gate, "same");  
         gate.Form("detID == %d", iDet);    express.Form("e:z >> h4%d", iDet); atree->Draw(express, gate, "same"); 
       
      }
   
   }  
   
   if( mode == 1){
         int iDet = detID;
         double zRange[2];
         
         if( firstPos > 0 ){
            zRange[0] = pos[iDet] - 10. ;
            zRange[1] = pos[iDet] + length + 10. ;
         }else{
            zRange[0] = pos[iDet] - length - 10.;
            zRange[1] = pos[iDet] + 10.;
         }
         
         
   
   }
}
