#define Apollo_cxx

#include "Apollo.h"
#include <TH2.h>
#include <TStyle.h>
#include <TMath.h>
#include <TRandom.h>

///============== Settings
int Zb = 1;
double Bfield = 2.25;
const double c =  299.792458; //mm/ns
const double massb = 1875.61284;

///============== Apollo setting
double inch2mm = 25.4 ; // mm
double innerR = 4.4 * inch2mm;
double outerR = 5.0 * inch2mm;
double holeR = 2.17 / 2 * inch2mm;
std::vector<double> holeTheta = { 30, 60, 90};
std::vector<int> holeNum = {5, 10, 10};
std::vector<double> holePhiPhase = {0, 18, 0};
std::vector<std::vector<double>> holePhi;
const double zOffset = holeR + 15; // mm


//=============== Some functions
double calRho(double KE, double theta){
   double E = massb + KE;
   double pt = TMath::Sqrt(E*E - massb*massb) * TMath::Sin(theta); //transver momentum
   return  pt / Bfield/ Zb/ c * 1000 ; //mm
}
double XPos(double Zpos, double theta, double phi, double rho, int sign){
   return rho * ( TMath::Sin( TMath::Tan(theta) * Zpos / rho - sign * phi ) + sign * TMath::Sin(phi) );
}
double YPos(double Zpos, double theta, double phi, double rho, int sign){
   return rho * sign * (TMath::Cos( TMath::Tan(theta) * Zpos / rho - sign * phi ) - TMath::Cos(phi));
}
double RPos(double Zpos, double theta, double phi, double rho, int sign){
   double x =  XPos(Zpos, theta, phi, rho, sign) ;
   double y =  YPos(Zpos, theta, phi, rho, sign) ;
   return sqrt(x*x+y*y);
   //return rho * (2 - 2 * TMath::Cos( TMath::Tan(theta) * Zpos / rho));
}

//------- newton method
double f0(double thetab, double phib, double rho, double z, double theta, double phi){
   double xx = XPos(z, thetab, phib, rho, -1);
   double yy = YPos(z, thetab, phib, rho, -1);
   double zz = z - zOffset;
   double aa = TMath::Sin(theta) * TMath::Cos(phi);
   double bb = TMath::Sin(theta) * TMath::Sin(phi);
   double cc = TMath::Cos(theta);
   return xx * aa + yy * bb + zz * cc - innerR ;
}

double f1(double thetab, double phib, double rho, double z, double theta, double phi){
   double xx = TMath::Tan(thetab) * TMath::Cos(TMath::Tan(thetab) * z / rho + phib);
   double yy = TMath::Tan(thetab) * TMath::Sin(TMath::Tan(thetab) * z / rho + phib);
   double aa = TMath::Sin(theta) * TMath::Cos(phi);
   double bb = TMath::Sin(theta) * TMath::Sin(phi);
   double cc = TMath::Cos(theta); 
   return xx * aa + yy * bb + cc;
}

double nextZ(double thetab, double phib, double rho, double z, double theta, double phi){
   return z - f0(thetab, phib, rho, z, theta, phi )/f1(thetab, phib, rho,  z, theta, phi);
}

double g0(double thetab, double rho, double z){
   return rho * rho * (2 - 2 * TMath::Cos( TMath::Tan(thetab) * z / rho)) + (z - zOffset) * (z - zOffset) - innerR * innerR;
}
double g1(double thetab, double rho, double z){
   return 2 * rho * TMath::Tan(thetab) * TMath::Sin( TMath::Tan(thetab) * z / rho) + 2 * (z - zOffset);
}
double nextG(double thetab, double rho, double z ){
   return z - g0(thetab, rho, z)/g1(thetab, rho,  z);
}

double rhoMin;
double xHit, yHit, zHit; // these are hit z, y, z 
int iHit, jHit;

double FindZHit(double a0, double thetab, double phib, double rho, double theta, double phi){
   //============ Newton method;
   double da ;
   int count = 0;
   do{
      double a1 = nextZ(thetab, phib, rho, a0, theta, phi);
      da = abs(a1-a0);
      //printf("--------- %d | %.3f, %.3f, %.3f \n", count, a0, a1, da);
      count++;
      a0 = a1;
   }while( da > 0.001 && count < 500);

   return a0;
}

double FindZHitR(double a0, double thetab, double rho){
   //============ Newton method;
   double da ;
   int count = 0;
   do{
      double a1 = nextG(thetab,  rho, a0);
      da = abs(a1-a0);
      count++;
      //printf("--------- %d | %.3f, %.3f, %.3f \n", count, a0, a1, da);
      if( a1 < 0 ){ a1 = rho/TMath::Tan(thetab)/8 ;}
      a0 = a1;
   }while( da > 0.001 && count < 500);

   return a0;
}

bool ApolloAcceptance(double thetab, double energy, double phib){

   double rho = calRho(energy, thetab);
   double z0 = TMath::TwoPi() * rho/ TMath::Tan(thetab);

   if( z0 < zOffset) return false;
   if( rho < rhoMin) return false;

   //printf("...... rho %f \n", rho);
   iHit = -1;
   jHit = -1;

   xHit = 0;
   yHit = 0;
   zHit = 0;

   for( int i = 0 ; i < (int) holeTheta.size() ; i++){
      for( int j = 0 ; j < (int) holePhi[i].size(); j++){

         //printf("================ %d %d, %.0f, %.0f\n", i, j ,  holeTheta[i],  holePhi[i][j]);
         double a0 = innerR * TMath::Cos(holeTheta[i] * TMath::DegToRad()); /// initial guess
         zHit = FindZHit(a0, thetab, phib, rho, holeTheta[i] * TMath::DegToRad(), holePhi[i][j] * TMath::DegToRad() );
         
         //check is the slop of f0(z) is positive. i.e. hit from inside
         double slope = f1(thetab, phib, rho, zHit, holeTheta[i] * TMath::DegToRad(), holePhi[i][j] * TMath::DegToRad() );
         if( slope < 0 ) { // move serach - z0 /2 

            zHit = FindZHit(zHit - z0/2, thetab, phib, rho, holeTheta[i] * TMath::DegToRad(), holePhi[i][j] * TMath::DegToRad() );          
            slope = f1(thetab, phib, rho, a0, holeTheta[i] * TMath::DegToRad(), holePhi[i][j] * TMath::DegToRad() );

            if( slope < 0 ) continue;
         }

         double z = zHit - zOffset;  

         if( z < -holeR) continue;
         if( z > innerR) continue;

         xHit = XPos(zHit, thetab, phib, rho, -1);
         yHit = YPos(zHit, thetab, phib, rho, -1);
         //printf(" hit (x, y, z) : (%f, %f, %f)\n", xHit, yHit, zHit);

         // using dot product the get the perpendicular distance
         // normal vector of the hole
         double xN = TMath::Sin(holeTheta[i]*TMath::DegToRad()) * TMath::Cos(holePhi[i][j] * TMath::DegToRad());
         double yN = TMath::Sin(holeTheta[i]*TMath::DegToRad()) * TMath::Sin(holePhi[i][j] * TMath::DegToRad());
         double zN = TMath::Cos(holeTheta[i]*TMath::DegToRad());
         double proj = xHit*xN + yHit * yN + z * zN;

         double dist = TMath::Sqrt(xHit*xHit + yHit*yHit + z*z - innerR*innerR);
         //printf("--- %d | proj : %f (%f)  | %f (%f)\n", j, proj, innerR, dist, holeR);
         if( abs(proj - innerR) > 2 ) continue;

         if( dist < holeR) {
            iHit = i;
            jHit = j;
            //printf("<<<<<<<<<<<<< hit, %f\n", dist);
            return true;
         }
      }
   }

   return false;
}


//^=====================================================

void Apollo::Begin(TTree * /*tree*/){

   TString option = GetOption();

   holePhi.clear();
   std::vector<double> tempPhi;
   for( int i = 0; i < (int) holeTheta.size(); i++){
      tempPhi.clear();
      for( int j = 0; j < holeNum[i]; j++){
         double x = holePhiPhase[i] + 360./holeNum[i] * j;
         tempPhi.push_back(x);
      }
      holePhi.push_back(tempPhi);
   }

   rhoMin = innerR * TMath::Sin(30 * TMath::DegToRad() ) - holeR * TMath::Cos(30 * TMath::DegToRad());

}

void Apollo::SlaveBegin(TTree * /*tree*/){
   TString option = GetOption();

}

Long64_t processed = 0;
Bool_t Apollo::Process(Long64_t entry){
   b_thetab->GetEntry(entry);
   b_phib->GetEntry(entry);
   b_Tb->GetEntry(entry);
   b_thetaCM->GetEntry(entry);
   

   pTheta = thetab;
   pPhi = phib;
   pE = Tb;
   pThetaCM = thetaCM;

   //if( Tb < 1.0 ) return kTRUE;
   processed ++;
   //if( processed > 10 ) return kTRUE;
   if( processed % 10000 == 0) printf("processed : %lld \n", processed);

   //printf("######################## thetab %f deg, phib %f deg, E: %f MeV\n", thetab, phib, Tb);
   thetab = thetab * TMath::DegToRad();
   phib = phib * TMath::DegToRad();

   pRho = calRho(Tb, thetab);
   bool OK = ApolloAcceptance(thetab, Tb, phib);

   if( OK ){
      //printf("(%7.2f, %7.2f, %7.2f), (%d, %2d), (%2.0f, %3.0f), (%7f, %7f, %7f)\n",  
      //                xHit, yHit, zHit, iHit, jHit, holeTheta[iHit], holePhi[iHit][jHit], 
      //                 thetab *TMath::RadToDeg(), phib*TMath::RadToDeg(), Tb
      //                );

      xH = xHit;
      yH = yHit;
      zH = zHit;

      iH = iHit;
      jH = jHit;

      //save tree
      //saveFile->cd();
      //newTree->Fill();

   }else{
      xH = 0;
      yH = 0;
      zH = 0;

      iH = -1;
      jH = -1;

      //=========== calculate the hit point on a sphere with radius innerR
      if( pRho > rhoMin) {
         double z0 = TMath::TwoPi() * pRho/ TMath::Tan(thetab);
         zH = FindZHitR(z0/4, thetab, pRho);
         xH = XPos(zH, thetab, phib,  pRho, -1);
         yH = YPos(zH, thetab, phib,  pRho, -1);
         //printf("(%f , %f, %f) \n", xH, yH, zH);
         //printf("(%7f, %7f, %7f)\n",  thetab *TMath::RadToDeg(), phib*TMath::RadToDeg(), Tb );

         double RRR = TMath::Sqrt((zH-zOffset)*(zH-zOffset) + xH*xH + yH*yH);
         if( abs(RRR -innerR ) > 1 ) {
            printf("%f \n", RRR);
            iH = -3;
         }

      }else{
         iH = -2;
      }
   }

   //save tree
   saveFile->cd();
   newTree->Fill();



   return kTRUE;
}

void Apollo::SlaveTerminate(){

}

void Apollo::Terminate(){
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();
}
