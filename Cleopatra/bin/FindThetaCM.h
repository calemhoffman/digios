/***********************************************************************
 * 
 *  This is FindThetaCM.C, To calculate the thetaCM convrage for each detector
 *  
 *  This required two inout files: basicReactionConfig.txt
 *                                 detectorGeo.txt
 * 
 *-------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "HELIOS_LIB.h"

void FindThetaCM(double Ex, double XRATION = 0.95, string basicConfig="basicReactionConfig.txt",  string detGeoFileName = "detectorGeo.txt"){
   
   //---- reaction
   int AA = 206, zA = 80;
   int Aa = 2,  za = 1;
   int Ab = 1,  zb = 1;
   
   //---- beam
   double KEAmean = 7.0; // MeV/u 
   double thetaMean = 0.; // mrad 
   double xBeam = 0; // mm
   double yBeam = 0; // mm
   
   ifstream cFile;
   cFile.open(basicConfig.c_str());
   bool isOverided = false;
   if( cFile.is_open() ){
      string line;
      int i = 0;
      while( cFile >> line){
         //printf("%d, %s \n", i,  line.c_str());
         if( line.substr(0,2) == "//" ) continue;
         if( i == 0 ) AA = atoi(line.c_str());
         if( i == 1 ) zA = atoi(line.c_str());
         if( i == 2 ) Aa = atoi(line.c_str());
         if( i == 3 ) za = atoi(line.c_str());
         if( i == 4 ) Ab = atoi(line.c_str());
         if( i == 5 ) zb = atoi(line.c_str());
         if( i == 6 ) KEAmean = atof(line.c_str());
         if( i == 10 ) xBeam = atof(line.c_str());
         if( i == 11 ) yBeam = atof(line.c_str());
         i = i + 1;
      }
      cFile.close();
      isOverided = true;
   }
   
   /**///========================================================= load files
   vector<double> pos;
   double a = 10.5;
   double length = 50.5;
   double firstPos = 0;
   int iDet = 6;
   int jDet = 4;
   double BField = 0;
   
   //=============================================================
   //=============================================================
   //=============================================================
   //===== Set Reaction
   TransferReaction reaction;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   reaction.SetIncidentEnergyAngle(KEAmean, 0, 0);
   reaction.SetExB(Ex);
   reaction.CalReactionConstant();
   
   printf("===================================================\n");
   printf("=========== %27s ===========\n", reaction.GetReactionName().Data());
   printf("===================================================\n");
   if( isOverided )printf("----- loading reaction from : %s. \n", basicConfig.c_str());
   printf("         KE: %7.4f \n", KEAmean);
   printf("      theta: %7.4f \n", thetaMean);
   printf("offset(x,y): %7.4f, %7.4f mm \n", xBeam, yBeam);
   printf("   Q-value : %7.4f MeV \n", reaction.GetQValue() );
   printf("     Max Ex: %7.4f MeV \n", reaction.GetMaxExB() );
   printf("===================================================\n");
   
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file(detGeoFileName.c_str(), std::ifstream::in);
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) BField   = atof(x.c_str());
         if( i == 2 )  a       = atof(x.c_str());
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
      printf("========================================\n");
      
   }else{
       printf("... fail\n");
       return ;
   }
   
   vector<double> midPos;
   
   for(int i = 0; i < iDet; i++){
      if( firstPos > 0 ){
         midPos.push_back(pos[i]+length/2.);
      }else{
         midPos.push_back(pos[i]-length/2.);
      }
   }
   
   //calculate a TGraph for thetaCM vs z
   double px[100];
   double py[100];
   
   double mb = reaction.GetMass_b();
   double kCM = reaction.GetMomentumbCM();
   double q = TMath::Sqrt(mb*mb + kCM * kCM );
   double beta = reaction.GetReactionBeta() ;
   double slope = 299.792458 * zb * BField / TMath::TwoPi() * beta / 1000.; // MeV/mm
   double gamma = reaction.GetReactionGamma();
   for(int i = 0; i < 100; i++){
      double thetacm = (i + 8.) * TMath::DegToRad();
      double temp = TMath::TwoPi() * slope / beta / kCM * a / TMath::Sin(thetacm); 
      px[i] = beta /slope * (gamma * beta * q - gamma * kCM * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi());
      py[i] = thetacm * TMath::RadToDeg();   
   }
   
   TGraph * tx = new TGraph(100, px, py);
   tx->SetName(Form("tx"));
   tx->SetLineColor(4);
   
   /**///========================================================= result
      
   printf("==== ThetaCM in degree =================\n");
   printf("========================= x-ratio : %f \n", XRATION);
   printf("=========================      Ex : %6.4f MeV\n", Ex);
   printf("         %6s - %6s |  %6s, %6s\n", "Min",  "Max", "Mean", "Half");
   printf("-------------------------------------------------\n");
   for( int i = 0; i < iDet; i++){
      double tMin = tx->Eval(midPos[i]-length*XRATION/2.);
      double tMax = tx->Eval(midPos[i]+length*XRATION/2.);
      
      double tMean = (tMax + tMin)/2.;
      double tHalf = (tMax - tMin)/2.;
      
      printf(" det-%d:  %6.2f - %6.2f |  %6.2f, %6.2f\n", i,tMin,  tMax, tMean, tHalf);
   }
   printf("================================================= \n");

}
