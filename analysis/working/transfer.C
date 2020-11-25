#include "../Cleopatra/Isotope.h"
#include "TMath.h"


//==== reaction constant;
double beta, gamm, Mc, alpha;

double massA;
double massa;
double massb;
double massB;

double zPos0, zPos, xPos, yPos;

int detRowID, detID;

double loop;

//double c = 299.792458; // mm/ns

double kcm(double Ex) {
   return sqrt((Mc*Mc - pow(massb+massB+Ex,2)) * ( Mc*Mc - pow(massb - massB - Ex,2) )  )/2./Mc;
}

double * Pb(double thetaCM, double phiCM, double Ex ){
   
   double * vec = new double[4];
   
   double k = kcm(Ex);
   double q = sqrt(massb*massb + k *k );
   
   vec[0] = gamm * q - gamm * beta * k * cos(thetaCM);
   vec[1] = gamm * beta * q - gamm * k * cos(thetaCM);
   vec[2] = k * sin(thetaCM) * cos(phiCM);
   vec[3] = k * sin(thetaCM) * sin(phiCM);
   
   return vec;
   
}

double rho(double thetaCM, int Q, double Bfield, double Ex) { 
   return kcm(Ex) * sin(thetaCM) / abs(Bfield) / Q / c * 1000 ; 
}

double z0(double thetaCM, int Q, double Ex){ 
   double k = kcm(Ex);
   double q = sqrt(massb*massb + k *k );
   return (gamm * beta * q - gamm * k * cos(thetaCM) )/alpha;
}

double zHit(double thetaCM, int Q, double Ex, int n,double prepdist, double Bfield, double dphi){
   
   double z00 = z0(thetaCM, Q, Ex);
   int s = (Bfield > 0 ? 1 : -1);
   double rho0 = rho(thetaCM, Q, Bfield, Ex);
   double hahaha = asin( prepdist/ rho0 - s * sin(dphi));
   
   return z00/ TMath::TwoPi() * ( s * dphi + n * TMath::Pi() + pow(-1, n) * hahaha );
}

double xHit(double thetaCM, double phi, int Q, double Ex, double prepdist, double Bfield,  double z){

   double z00 = z0(thetaCM, Q, Ex);
   int s = (Bfield > 0 ? 1 : -1);
   double rho0 = rho(thetaCM, Q, Bfield, Ex);
   
   return rho0 * ( sin(z/z00 * TMath::TwoPi() - s * phi)  + s * sin(phi));
}

double yHit(double thetaCM, double phi, int Q, double Ex, double prepdist, double Bfield,  double z){

   double z00 = z0(thetaCM, Q, Ex);
   int s = (Bfield > 0 ? 1 : -1);
   double rho0 = rho(thetaCM, Q, Bfield, Ex);
   
   return rho0 * ( cos(z/z00 * TMath::TwoPi() - s * phi)  - s * cos(phi));
}

void HitCal(double thetaCM, double phi, int nSide, int Q, double Ex, double prepdist, double Bfield, int nLoop, bool inOut,  bool debug){

   double z00 = z0(thetaCM, Q, Ex);
   
   zPos0 = z00;
   
   double rho0 = rho(thetaCM, Q, Bfield, Ex);
   
   if( debug ) {
      printf("===================================\n");
      printf("thetaCM : %f deg, phi : %f deg \n", thetaCM * TMath::RadToDeg(), phi * TMath::RadToDeg());
      printf("z0: %f mm, rho : %f mm \n", z00, rho0);
      printf("target loop : %d \n", nLoop);
      printf("-----------------------------------\n");
   }
   vector<double> zCan;
   vector<int> dID; //detRowID
   
   for( int i = 0; i < nSide *2 ; i++){
      
      double phiD = TMath::TwoPi()/nSide * i ;
      
      double zP = zHit(thetaCM, Q, Ex, 2*nLoop+1, prepdist, Bfield, phi - phiD);
      double xP = xHit(thetaCM, phi, Q, Ex, prepdist, Bfield, zP);
      double yP = yHit(thetaCM, phi, Q, Ex, prepdist, Bfield, zP);
      
      if( debug) {
         printf("phiD: %4.0f, dphi: %6.1f, Loop : %9.5f, zHit : %8.3f mm, (x,y) = (%7.2f, %7.2f) \n",
                  phiD * TMath::RadToDeg(), 
                  (phi-phiD) * TMath::RadToDeg(), 
                  zP/z00, zP, xP, yP );
      }
      
      ///Selection
      if( !TMath::IsNaN(zP) && 0< zP/z00 && nLoop-1 < zP/z00 && zP/z00 < nLoop ) {
         zCan.push_back(zP);
         dID.push_back(i);
      }
   }
   
   if( zCan.size() == 0 ){
      zPos = TMath::QuietNaN();
      xPos = TMath::QuietNaN();
      yPos = TMath::QuietNaN();
      loop = TMath::QuietNaN();
      detRowID = -1;
   }
   
   //find the 
   if( debug ) printf("-----------------------------------\n");
   double dMin = 1;   
   for( int i = 0; i < (int) zCan.size(); i++){
      
      double dd = abs(zCan[i]/z00 - nLoop);
      
      if( debug) printf(" %d | zP : %8.3f mm; loop : %9.5f ", i, zCan[i], zCan[i]/z00); 
      
      if( dd < dMin) {
         zPos = zCan[i];
         dMin = dd;
         xPos = xHit(thetaCM, phi, Q, Ex, prepdist, Bfield, zPos);
         yPos = yHit(thetaCM, phi, Q, Ex, prepdist, Bfield, zPos);
         loop = zCan[i]/z00;
         detRowID = dID[i];
         
         double phiD = TMath::TwoPi()/nSide * dID[i] ;
         
         double a = xPos * cos(phiD) + yPos * sin(phiD);
         
         if( debug ) printf(" a : %f ", a);
         
         if( abs(a - prepdist) > 0.01) { // this must be false, otherwise, calculation error
             printf(" != %f ", prepdist);
             zPos = TMath::QuietNaN();
             xPos = TMath::QuietNaN();
             yPos = TMath::QuietNaN();
             loop = TMath::QuietNaN();
             detRowID = -1;
         }
         if( debug) printf(" <--- pick");
      }
      
      if( debug) printf("\n");
   }
   
   if( debug ) printf("===================================\n");

}

int DetAcceptance(int nSide, double prepdist){
   

   vector<double> arrayPos = {0, 60, 120, 180};
   double dist = -100;
   double length = 50;
   double width = 10;
   
      
   //======= detector absolute position

   vector<double> absPos;
   int n = (int) arrayPos.size();
   for( int i = 0; i < n ; i++){
      
      if( dist < 0 ) {
         absPos.push_back( - arrayPos[n-1 -i] + dist );
         printf("%d | %8.2f - %8.2f \n", i, absPos.back() - length , absPos.back());
      }else{
         absPos.push_back( arrayPos[i] + dist );
         printf("%d | %8.2f - %8.2f \n", i, absPos.back() , absPos.back()+ length );
      }
   }
   
   //===== check zPos, 

   double sDist = sqrt(xPos*xPos + yPos*yPos - prepdist * prepdist);
   if( sDist > width/2 ) return -1; // when outside the detector width
   
   if( zPos * dist < 0 ) return -3; // other side of the target
   
   if( dist < 0 ){
      if( zPos < absPos[0] - length ) return -2; // when zPos further the range of whole array, more loop would not save
      if( absPos[n-1] < zPos) return 2; // when zPos less then the nearest position, more loop may hit
      for( int i = 0; i < n-1 ; i++){
         if( absPos[i] < zPos && zPos < absPos[i+1] - length ) return -3; // when zPos is between arrays.
      }      
   }else{
      if(  absPos[n-1] + length < zPos ) return -2; // when zPos further the range of whole array, more loop would not save
      if( zPos < absPos[0] ) return 2; // when zPos less then the nearest position, more loop may hit
      for( int i = 0; i < n-1 ; i++){
         if( absPos[i] + length < zPos && zPos < absPos[i+1] ) return -3; // when zPos is between arrays.
      }      
   }

   return 1;
   
}

void transfer(){

   //============== User input
   int AA = 14; int AZ = 6;
   int aA =  2; int aZ = 1;
   int bA =  1; int bZ = 1;

   double KEA = 10; // MeV/u of A
   double Bfield = 2.5; //T
   double prepdist = 11.5; // mm
   
   int arrayNSide = 4; //num of array sides
   
   //============== end of user input
   
   int BA = AA + aA - bA;
   int BZ = AZ + aZ - bZ;
   
   Isotope isoA(AA, AZ);
   Isotope isoa(aA, aZ);
   Isotope isob(bA, bZ);
   Isotope isoB(BA, BZ);
   
   massA = isoA.Mass;
   massa = isoa.Mass;
   massb = isob.Mass;
   massB = isoB.Mass;
   
   double T = AA * KEA;
   beta = sqrt(pow(massA+T,2)- massA*massA)/(massa+massA+T);
   
   gamm = 1/sqrt(1-beta*beta);
   
   Mc = sqrt(2*massa*T + pow(massa + massA,2));
   alpha = Bfield * bZ * c / TMath::TwoPi() / 1000; // MeV/mm / beta
   
   printf("===================================\n");
   printf("Mass A  :   %8.2f MeV/c2\n", massA);
   printf("Mass a  :   %8.2f MeV/c2\n", massa);
   printf("Mass b  :   %8.2f MeV/c2\n", massb);
   printf("Mass B  :   %8.2f MeV/c2\n", massB);
   printf("CM Mass :   %8.2f MeV\n", Mc);
   printf("CM beta :   %8.6f \n", beta);
   printf("slope   :   %8.6f MeV\n", alpha * beta);
   printf("alpha   :   %8.6f MeV\n", alpha);


   int targetLoop = 1;
   double thetaCM = 40 * TMath::DegToRad();
   double phi = 20 * TMath::DegToRad();

   HitCal(thetaCM, phi, arrayNSide, bZ, 0, prepdist, Bfield, targetLoop, true);
   
   int hitID = DetAcceptance(arrayNSide, prepdist);

   printf("(z,x,y) = (%8.3f, %8.3f, %8.3f) , loop : %9.6f, Row : %d , hitID : %d \n", 
       zPos, xPos, yPos, loop, detRowID, hitID);
   
   while(hitID > 1 && loop < 6){
      zPos = zPos + zPos0;
      loop += 1;
      targetLoop += 1;
      hitID = DetAcceptance(arrayNSide, prepdist);
      printf("(z,x,y) = (%8.3f, %8.3f, %8.3f) , loop : %9.6f, Row : %d , hitID : %d \n",  zPos, xPos, yPos, loop, detRowID, hitID);
   }
   
   HitCal(thetaCM, phi, arrayNSide, bZ, 0, prepdist, Bfield, targetLoop, true);
   
   printf("(z,x,y) = (%8.3f, %8.3f, %8.3f) , loop : %9.6f, Row : %d , hitID : %d \n", 
       zPos, xPos, yPos, loop, detRowID, hitID);
}
