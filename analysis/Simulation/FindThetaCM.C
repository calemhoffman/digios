#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"

//TODO make it independence from transfer.root

void FindThetaCM(int ExID, double XRATION = 0.95){
   TFile * ft1 = new TFile("transfer.root");      // 206Hg simulation
   
   if( !ft1.IsOpen() ) {
      printf(" Please generate transfer.root with suitable excitation energy from transfer.C \n");
      return ;
   }
   
   TTree * tree = (TTree *) ft1->FindObjectAny("tree");

   TObjArray * txList = (TObjArray*) ft1->FindObjectAny("txList");
   
   TGraph * tx = (TGraph *) txList->At(ExID);
   
   TH1F * h1 = new TH1F("h1", "Ex", 1100, -1, 10);
   tree->Draw("Ex>>h1", Form("ExID == %d", ExID));
   double Ex = h1->GetMean();
   
   /**///========================================================= load files
   vector<double> pos;
   double length = 50.5;
   double firstPos = 0;
   int iDet = 6;
   int jDet = 4;

   string detGeoFileName = "detectorGeo.txt";
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file(detGeoFileName.c_str(), std::ifstream::in);
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return ;
   }
   
   printf("======== x-ratio : %f \n", XRATION);
   
   
   vector<double> midPos;
   
   for(int i = 0; i < iDet; i++){
      if( firstPos > 0 ){
         midPos.push_back(pos[i]+length/2.);
      }else{
         midPos.push_back(pos[i]-length/2.);
      }
   }
   
   /**///========================================================= load files
   
   printf("=================== ExID : %d , %6.4f MeV\n", ExID, Ex);
   for( int i = 0; i < iDet; i++){
         printf(" det-%d:  %6.2f - %6.2f \n", i, tx->Eval(midPos[i]-length*XRATION/2.), tx->Eval(midPos[i]+length*XRATION/2.));
   }
   printf("=================================== \n");
   
   gROOT->ProcessLine(".q");

}
