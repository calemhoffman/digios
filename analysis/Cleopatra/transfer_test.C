#include "HELIOS_LIB.h"
#include "TROOT.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TGraph.h"
#include "TMacro.h"
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <TObjArray.h>

void transfer_test(double t, double p, double bField, bool fromOutSide){
   
   TransferReaction reaction;
   reaction.SetA(14, 6);
   reaction.Seta( 2, 1);
   reaction.Setb( 1, 1);
   reaction.SetB(15, 6);
   
   reaction.SetExB(0);
   
   reaction.SetIncidentEnergyAngle(10, 0, 0);
   reaction.CalReactionConstant();
   
   HELIOS helios;
   helios.SetDetectorGeometry("../working/detectorGeo.txt");
   helios.OverrideMagneticField(bField);
   helios.SetDetectorOutside(fromOutSide);
   
   double beta = reaction.GetReactionBeta() ;
   
   double slope = 299.792458 * abs(helios.GetBField()) / TMath::TwoPi() * beta / 1000.; // MeV/mm
   
   double alpha = slope / beta;
   
   printf("===================================\n");
   printf("Mass A  :   %8.2f MeV/c2\n", reaction.GetMass_A());
   printf("Mass a  :   %8.2f MeV/c2\n", reaction.GetMass_a());
   printf("Mass b  :   %8.2f MeV/c2\n", reaction.GetMass_b());
   printf("Mass B  :   %8.2f MeV/c2\n", reaction.GetMass_B());
   printf("CM Mass :   %8.2f MeV\n", reaction.GetCMTotalEnergy());
   printf("CM beta :   %8.6f \n", beta);
   printf("slope   :   %8.6f MeV\n", alpha * beta);
   printf("alpha   :   %8.6f MeV\n", alpha);
   
   
   double thetaCM = t * TMath::DegToRad();
   double phiCM = - p * TMath::DegToRad();
   
   TLorentzVector * output = reaction.Event(thetaCM, phiCM);
   TLorentzVector Pb = output[2];
   TLorentzVector PB = output[3];
   
   Pb.Print();
   PB.Print();
   
   helios.CalArrayHit(Pb, 1);
   helios.CalRecoilHit(PB, 6);
      
   printf("+++++++++++++++++++++++++++++++++++++\n");
   
   int hitID = 2;
   while( hitID > 1 ){
      printf("==================== check accp.\n");
      hitID = helios.DetAcceptance();
      printf("-------------------- hitID %d\n", hitID);
   }
   
   PrintTrajectory(helios.GetTrajectory_b());
   
   
}
