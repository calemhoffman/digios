/***********************************************************************
 * 
 *  This is FindThetaCM.h, To calculate the thetaCM convrage for each detector
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
#include "TMacro.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "../Cleopatra/HELIOS_LIB.h"

void FindThetaCM(double Ex, int nDivision=1, double XRATION = 0.95, 
            string basicConfig="reactionConfig.txt",  
            string detGeoFileName = "detectorGeo.txt"){

  //---- reaction
  int AA, zA; //beam
  int Aa, za; //target
  int Ab, zb; //recoil-1
  double ExA; 

  //---- beam
  double KEAmean, KEAsigma; // MeV/u , assume Guassian
  double thetaMean, thetaSigma; // mrad , assume Guassian due to small angle
  double xBeam, yBeam; // mm

   /**///========================================================= load files
  ReactionConfig reactionConfig;
  DetGeo detGeo;
  TMacro * haha = new TMacro();
  if( haha->ReadFile(basicConfig.c_str()) > 0 ){
    reactionConfig = LoadReactionConfig(haha);

    PrintReactionConfig(reactionConfig);

    KEAmean = reactionConfig.beamEnergy;
    KEAsigma = reactionConfig.beamEnergySigma;

    thetaMean = reactionConfig.beamAngle;
    thetaSigma = reactionConfig.beamAngleSigma;

    xBeam = reactionConfig.beamX;
    yBeam = reactionConfig.beamY;

    AA = reactionConfig.beamA;        zA = reactionConfig.beamZ;
    Aa = reactionConfig.targetA;      za = reactionConfig.targetZ;
    Ab = reactionConfig.recoilLightA; zb = reactionConfig.recoilLightZ;

    ExA = reactionConfig.beamEx[0];
    
  }else{
    printf("cannot load %s \n", basicConfig.c_str());
    return;
  }
  
   vector<double> pos;
   double a = 11.5;
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
   reaction.SetExA(ExA);
   reaction.CalReactionConstant();
   
   printf("===================================================\n");
   printf("=========== %27s ===========\n", reaction.GetReactionName().Data());
   printf("===================================================\n");
   printf("----- loading reaction from : %s. \n", basicConfig.c_str());
   printf("       Ex A: %7.3f MeV\n", ExA);
   printf("         KE: %7.4f \n", KEAmean);
   printf("      theta: %7.4f \n", thetaMean);
   printf("offset(x,y): %7.4f, %7.4f mm \n", xBeam, yBeam);
   printf("    Q-value: %7.4f MeV \n", reaction.GetQValue() );
   printf("     Max Ex: %7.4f MeV \n", reaction.GetMaxExB() );
   printf("===================================================\n");
   
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   TMacro * kaka = new TMacro();
   if( kaka->ReadFile(detGeoFileName.c_str()) > 0 ){
     detGeo = LoadDetectorGeo(kaka);
     
     pos = detGeo.detPos;
     a = detGeo.detPerpDist;
     length = detGeo.detLength;
     firstPos = detGeo.firstPos;
     iDet = detGeo.nDet;
     jDet = detGeo.mDet;
     BField = detGeo.Bfield;
     
     printf("... done.\n");

     PrintDetGeo(detGeo);
     
   }else{
     printf("... fail\n");
     return;
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
   double slope = 299.792458 * zb * abs(BField) / TMath::TwoPi() * beta / 1000.; // MeV/mm
   double gamma = reaction.GetReactionGamma();
   for(int i = 0; i < 100; i++){
      double thetacm = (i + 5.) * TMath::DegToRad();
      double temp = TMath::TwoPi() * slope / beta / kCM * a / TMath::Sin(thetacm); 
      px[i] = beta /slope * (gamma * beta * q - gamma * kCM * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi());
      py[i] = thetacm * TMath::RadToDeg();   
   }

   //find minimum z position
   TGraph * xt = new TGraph(100, py, px);
   xt->SetName("xt");
   ///double zMin0 = xt->Eval(0);
   ///printf("z for thetaCM = 0 : %f mm \n", zMin0);
   
   ///xt->Draw("AC*");
   
   /// find the minimum z position and the corresponding theta
   double zMin0 = 0;
   double tMin0 = 0;
   for( double ttt = 3 ; ttt < 20 ; ttt += 0.1 ){
     double zzz = xt->Eval(ttt);
     if( zzz < zMin0 ) {
       zMin0 = zzz;
       tMin0 = ttt;
     }
   }   
   printf(" z min %f mm at thetaCM %f deg \n", zMin0, tMin0);


   TGraph * tx = new TGraph(100, px, py);
   tx->SetName(Form("tx"));
   tx->SetLineColor(4);

   //tx->Draw("AC*");
   
   /**///========================================================= result
      
   printf("==== ThetaCM in degree =================\n");
   printf("========================= x-ratio : %f, number of division : %d \n", XRATION, nDivision);
   printf("\n");
   for( int j = 0; j < nDivision + 1; j++) printf("%5.2f   ", -XRATION + 2*XRATION/nDivision*j);
   printf(" <<-- in X \n");
   for( int j = 0; j < nDivision + 1; j++) printf("%5s   ", "  |  ");
   printf("\n");
   for( int j = 0; j < nDivision + 1; j++) printf("%5.2f   ", length/2 -length*XRATION/2 + length*XRATION/nDivision*j);
   printf(" <<-- in cm \n\n");
   printf("=========================      Ex : %6.4f MeV\n", Ex);
   printf("            %6s - %6s |  %6s, %6s, %6s\n", "Min",  "Max", "Mean", "Dt", "sin(x)dx * 180/pi");
   printf("-------------------------------------------------\n");
   for( int i = 0; i < iDet; i++){
     double zMin = midPos[i]-length*XRATION/2.;
     double zMax = midPos[i]+length*XRATION/2.;
     double zLength = zMax - zMin; 
     double zStep = zLength/(nDivision);
     for( int j = 0 ; j < nDivision ; j++){
       
       double tMin =  (zMin +     j*zStep > zMin0) ? tx->Eval(zMin +     j*zStep) : TMath::QuietNaN();
       double tMax =  (zMin + (j+1)*zStep > zMin0) ? tx->Eval(zMin + (j+1)*zStep) : TMath::QuietNaN();
       
       double tMean = (tMax + tMin)/2.;
       double dt = (tMax - tMin);
       
       double sintdt = TMath::Sin(tMean * TMath::DegToRad()) * dt ;

         
       printf(" det-%d[%d]:  %6.2f - %6.2f |  %6.2f, %6.2f, %6.4f\n", i, j, tMin,  tMax, tMean, dt, sintdt);
       
     }
     if( nDivision > 0 ) printf("--------------\n");
   }
   printf("================================================= \n");

}
