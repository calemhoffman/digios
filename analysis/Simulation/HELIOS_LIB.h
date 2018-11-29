#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TGraph.h"
#include <vector>
#include <fstream>
#include "Isotope.h"

//=======================================================
//#######################################################
// Class for Transfer Reaction
// reaction notation A(a,b)B
// A = incident particle
// a = target
// b = light scattered particle
// B = heavy scattered particle
//======================================================= 
class TransferReaction {
public:
   TransferReaction();
   ~TransferReaction();
  
   void SetA(int A, int Z, double Ex = 0){
      Isotope temp (A, Z);
      mA = temp.Mass;
      AA = A;
      zA = Z;
      ExA = Ex;
      nameA = temp.Name;
      isReady = false;
      isBSet = true;
   }
   void Seta(int A, int Z){
      Isotope temp (A, Z);
      ma = temp.Mass;
      Aa = A;
      za = Z;
      namea = temp.Name;
      isReady = false;
      isBSet = false;
   }
   void Setb(int A, int Z){
      Isotope temp (A, Z);
      mb = temp.Mass;
      Ab = A;
      zb = Z;
      nameb = temp.Name;
      isReady = false;
      isBSet = false;
   }
   void SetB(int A, int Z){
      Isotope temp (A, Z);
      mB = temp.Mass;
      AB = A;
      zB = Z;
      nameB = temp.Name;
      isReady = false;
      isBSet = true;
   }
   void SetIncidentEnergyAngle(double KEA, double theta, double phi){
      this->TA = KEA;
      this->T = TA * AA;
      this->thetaIN = theta;
      this->phiIN = phi;
      isReady = false;
   }
   void SetExA(double Ex){
      this->ExA = Ex;
      isReady = false;
   }
   void SetExB(double Ex){
      this->ExB = Ex;
      isReady = false;
   }
   
   TString GetReactionName(){
      TString rName;
      rName.Form("%s(%s,%s)%s", nameA.c_str(), namea.c_str(), nameb.c_str(), nameB.c_str()); 
      return rName;
   }
   
   int GetAtomicNumber_A(){return AA;}
   int GetAtomicNumber_a(){return Aa;}
   int GetAtomicNumber_b(){return Ab;}
   int GetAtomicNumber_B(){return AB;}
   
   double GetMass_A(){return mA;}
   double GetMass_a(){return ma;}
   double GetMass_b(){return mb;}
   double GetMass_B(){return mB;}
   
   int GetCharge_A(){return zA;}
   int GetCharge_a(){return za;}
   int GetCharge_b(){return zb;}
   int GetCharge_B(){return zB;}
   
   double GetCMTotalKE() {return Etot - mA - ma;}
   double GetQValue() {return mA + ExA + ma - mb - mB - ExB;}
   double GetMaxExB() {return Etot - mb - mB;}
   
   TLorentzVector GetPA(){return PA;}
   TLorentzVector GetPa(){return Pa;}
   TLorentzVector GetPb(){return Pb;}
   TLorentzVector GetPB(){return PB;}
   
   void CalReactionConstant();
   TLorentzVector * Event(double thetaCM, double phiCM);
   
   int CalExThetaCM(double e, double z, double Bfield, double a); // return 0 for no-result, 1 for OK.
   double GetEx(){return Ex;}
   double GetThetaCM(){return thetaCM;}
   
   double GetMomentumbCM(){return p;}
   double GetReactionBeta(){return beta;}
   double GetReactionGamma(){return gamma;}
   double GetCMTotalEnergy(){return Etot;}
   
private:
   string nameA, namea, nameb, nameB;
   double thetaIN, phiIN;
   double mA, ma, mb, mB;
   int AA, Aa, Ab, AB;
   int zA, za, zb, zB;
   double TA, T; // TA = KE of A pre u, T = total energy
   double ExA, ExB;
   double Ex, thetaCM; //calculated Ex using inverse mapping from e and z to thetaCM
   
   bool isReady;
   bool isBSet;
   
   double k; // CM Boost momentum
   double beta, gamma; //CM boost beta
   double Etot;
   double p; // CM frame momentum of b, B
   
   TLorentzVector PA, Pa, Pb, PB;
};

TransferReaction::TransferReaction(){
   thetaIN = 0.;
   phiIN = 0.;
   SetA(12, 6, 0);
   Seta(2,1);
   Setb(1,1);
   SetB(13,6);
   TA = 6;
   T = TA * AA;
   
   ExA = 0;
   ExB = 0;
   
   Ex = TMath::QuietNaN();
   thetaCM = TMath::QuietNaN();
   
   CalReactionConstant();
   
   TLorentzVector temp (0,0,0,0);
   PA = temp;
   Pa = temp;
   Pb = temp;
   PB = temp;
   
}

TransferReaction::~TransferReaction(){

}

void TransferReaction::CalReactionConstant(){
   if( !isBSet){
      AB = AA + Aa - Ab;
      zB = zA + za - zb;
      Isotope temp (AB, zB);
      mB = temp.Mass;
      isBSet = true;
   }
   
   k = TMath::Sqrt(TMath::Power(mA + ExA + T, 2) - (mA + ExA) * (mA + ExA)); 
   beta = k / (mA + ExA + ma + T);
   gamma = 1 / TMath::Sqrt(1- beta * beta);   
   Etot = TMath::Sqrt(TMath::Power(mA + ExA + ma + T,2) - k * k);
   p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB + ExB,2)) * (Etot*Etot - TMath::Power(mb - mB - ExB,2)) ) / 2 / Etot;
   
   PA.SetXYZM(0, 0, k, mA + ExA);
   PA.RotateY(thetaIN);
   PA.RotateZ(phiIN);
   
   Pa.SetXYZM(0,0,0,ma);
   
   isReady = true;
}

TLorentzVector * TransferReaction::Event(double thetaCM, double phiCM)
{
   if( isReady == false ){
      CalReactionConstant();
   }

   //TLorentzVector Pa(0, 0, 0, ma);
   
   //---- to CM frame
   TLorentzVector Pc = PA + Pa;
   TVector3 b = Pc.BoostVector();
   TVector3 v0 (0,0,0);
   TVector3 nb = v0 - b;
   
   TLorentzVector PAc = PA; 
   PAc.Boost(nb);
   TVector3 vA = PAc.Vect();
   
   TLorentzVector Pac = Pa;
   Pac.Boost(nb);
   TVector3 va = Pac.Vect();
   
   //--- from vb
   TVector3 vb = va;
   vb.SetMag(p);

   TVector3 ub = vb.Orthogonal();
   vb.Rotate(thetaCM, ub);
   vb.Rotate(phiCM + TMath::PiOver2(), va); // somehow, the calculation turn the vector 90 degree.
   
   //--- from Pb
   TLorentzVector Pbc;
   Pbc.SetVectM(vb, mb);
   
   //--- from PB
   TLorentzVector PBc;
   //PBc.SetVectM(vB, mB + ExB);
   PBc.SetVectM(-vb, mB + ExB);
   
   //---- to Lab Frame
   TLorentzVector Pb = Pbc;
   Pb.Boost(b);
   TLorentzVector PB = PBc;
   PB.Boost(b);
   
   TLorentzVector * output = new TLorentzVector[4];
   output[0] = PA;
   output[1] = Pa;
   output[2] = Pb;
   output[3] = PB;
   
   this->PA = PA;
   this->Pa = Pa;
   this->Pb = Pb;
   this->PB = PB;
   
   return output;   
}


int TransferReaction::CalExThetaCM(double e, double z, double Bfield, double a){
   double mass = mb;
   double massB = mB;
   double y = e + mass;
   double slope = 299.792458 * zb * Bfield / TMath::TwoPi() * beta / 1000.; // MeV/mm;
   double alpha = slope/beta;
   double G =  alpha * gamma * beta * a ;
   double Z = alpha * gamma * beta * z;
   double H = TMath::Sqrt(TMath::Power(gamma * beta,2) * (y*y - mass * mass) ) ;
   double Et = Etot;

   if( TMath::Abs(Z) < H ) {            
      //using Newton's method to solve 0 ==  H * sin(phi) - G * tan(phi) - Z = f(phi) 
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
         double x = TMath::ACos( mass / ( y * gamma - K));
         double k = mass * TMath::Tan(x); // momentum of particel b or B in CM frame
         double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(k*k + mass * mass));
         Ex = EB - massB;

         double hahaha1 = gamma* TMath::Sqrt(mass * mass + k * k) - y;
         double hahaha2 = gamma* beta * k;
         thetaCM = TMath::ACos(hahaha1/hahaha2) * TMath::RadToDeg();

         //double pt = k * TMath::Sin(thetaCM * TMath::DegToRad());
         //double pp = gamma*beta*TMath::Sqrt(mass*mass + k*k) - gamma * k * TMath::Cos(thetaCM * TMath::DegToRad());
         //thetaLab = TMath::ATan(pt/pp) * TMath::RadToDeg();

      }else{
         Ex = TMath::QuietNaN();
         thetaCM = TMath::QuietNaN();
         //thetaLab = TMath::QuietNaN();
         
         return 0;
      }

   }else{
      Ex = TMath::QuietNaN();
      thetaCM = TMath::QuietNaN();  
      //thetaLab = TMath::QuietNaN();
      return 0;
   }
   
   return 1; 
}
//=======================================================
//#######################################################
//Class for HELIOS
//input Lorentz vector, detector configuration
//output e, z, Ex, thetaCM, etc
//======================================================= 
class HELIOS{
public:

   HELIOS();
   ~HELIOS();
   
   void SetCoincidentWithRecoil(bool TorF){
      this->isCoincidentWithRecoil = TorF;
   }
   bool SetDetectorGeometry(string filename);
   
   void OverrideMagneticField(double BField){ this->Bfield = BField;}
   void OverrideMagneticFieldDirection(double BfieldTheta){ this->BfieldTheta = BfieldTheta * TMath::DegToRad();} // TODO move to detectorGeo.txt
   void OverrideFirstPos(double firstPos){
      overrideFirstPos = true;
      printf("------ Overriding FirstPosition to : %8.2f mm \n", firstPos);
      this->firstPos = firstPos;
   }
   void OverrideDetectorDistance(double a){
      overrideDetDistance = true;
      printf("------ Overriding Detector Distance to : %8.2f mm \n", a);
      this->a = a;
   }
   
   int CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB, double xOff = 0, double yOff = 0 ); // return 0 for no hit, 1 for hit
   
   int GetNumberOfDetectorsInSamePos(){return mDet;}
   double GetEnergy(){return e;}
   double GetZ(){return z;}
   double GetX(){return x;} // position in each detector, range from -1, 1
   int GetDetID(){return detID;}
   int GetDetRowID(){return detRowID;}
   double GetTime(){return t;}
   double GetRho(){return rho;}
   double GetRhoHit(){return rhoHit;} // radius of particle-b hit recoil detector 
   double GetdPhi(){return dphi;}
   int GetLoop(){return loop;}
   double GetVt(){return vt0;}
   double GetVp(){return vp0;}
   // anti-clockwise rotation for positive ion in B-field to z-axis
   double GetXPos(double ZPos){
      return rho * (TMath::Sin(TMath::Tan(theta) * ZPos / rho + phi) - TMath::Sin(phi)) ;
   }
   double GetYPos(double ZPos){
      return rho * (TMath::Cos(phi) - TMath::Cos(TMath::Tan(theta) * ZPos / rho + phi) );
   }
   
   double GetR(double ZPos){
      return rho * TMath::Sqrt(2 - 2* TMath::Cos( TMath::Tan(theta) * ZPos / rho));
   }
   
   double GetRecoilEnergy(){return eB;}
   double GetRecoilZ(){return zB;}
   double GetRecoilTime(){return tB;}
   double GetRecoilRho(){return rhoB;}
   double GetRecoilRhoHit(){return rhoBHit;}
   double GetRecoilVt(){return vt0B;}
   double GetRecoilVp(){return vp0B;}
   // anti-clockwise rotation for positive ion in B-field to z-axis
   double GetRecoilXPos(double ZPos){
      return rhoB * (TMath::Sin(TMath::Tan(thetaB) * ZPos / rhoB + phiB) - TMath::Sin(phiB));
   }   
   double GetRecoilYPos(double ZPos){
      return rhoB * (TMath::Cos(phiB) - TMath::Cos(TMath::Tan(thetaB) * ZPos / rhoB + phiB) );
   }
   double GetRecoilR(double ZPos){
      return rhoB * TMath::Sqrt(2 - 2* TMath::Cos( TMath::Tan(thetaB) * ZPos / rhoB));
   }
   
   double GetRecoilXHit(){return rxHit;}
   double GetRecoilYHit(){return ryHit;}
   
   double GetZ0(){return z0;}  // infinite detector
   double GetTime0(){return t0;}
   double GetBField() {return Bfield;}
   double GetDetectorA() {return a;}
   
private:
   double theta, phi; // polar angle of particle 
   double e, z, x, rho, dphi, t;
   double vt0, vp0;
   double rhoHit; // radius of particle-b hit on recoil detector
   int detID, loop, detRowID;   // multiloop
   
   double thetaB, phiB;
   double eB, zB, rhoB, tB;
   double vt0B, vp0B;
   double rhoBHit; // particle-B hit radius
   double rxHit, ryHit; // particle-B hit position
   bool isDetReady;
   
   double z0, t0; // infinite detector 
   
   //detetcor Geometry
   double Bfield; // T
   double BfieldTheta; // rad, 0 = z-axis, pi/2 = y axis, pi = -z axis
   double bore; // bore , mm
   double a; // distance from axis
   double w; // width   
   double posRecoil; // recoil, downstream
   double rhoRecoil; // radius recoil
   double l; // length
   double support;
   double firstPos; // m 
   vector<double> pos; // near position in m
   int nDet, mDet; // nDet = number of different pos, mDet, number of same pos

   bool overrideDetDistance;
   bool overrideFirstPos;
   bool isCoincidentWithRecoil;
};

HELIOS::HELIOS(){
   theta = TMath::QuietNaN();
   phi = TMath::QuietNaN();
   e = TMath::QuietNaN();
   z = TMath::QuietNaN();
   x = TMath::QuietNaN();
   t = TMath::QuietNaN();
   rho = TMath::QuietNaN();
   rhoHit = TMath::QuietNaN();
   dphi = TMath::QuietNaN();
   detID = -1;
   detRowID = -1;
   loop = -1;
   
   thetaB = TMath::QuietNaN();
   phiB = TMath::QuietNaN();
   eB = TMath::QuietNaN();
   zB = TMath::QuietNaN();
   rhoB = TMath::QuietNaN();
   rhoBHit = TMath::QuietNaN();
   rxHit = TMath::QuietNaN();
   ryHit = TMath::QuietNaN();
   tB = TMath::QuietNaN();
   isDetReady = false;

   z0 = TMath::QuietNaN();
   t0 = TMath::QuietNaN();

   Bfield = 0;
   BfieldTheta = 0;
   bore = 1000;
   a = 10;
   w = 10;
   posRecoil = 0;
   rhoRecoil = 0;
   l = 0;
   support = 0;
   firstPos = 0;
   pos.clear();
   nDet = 0;
   mDet = 0;
   
   overrideDetDistance = false;
   overrideFirstPos = false;
   isCoincidentWithRecoil = true;
}

HELIOS::~HELIOS(){
}

bool HELIOS::SetDetectorGeometry(string filename){
   
   pos.clear();
   
   printf("----- loading detector geometery : %s.", filename.c_str());
   ifstream file;
   file.open(filename.c_str());
   string line;
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         
         if( i == 0 )                         Bfield    = atof(x.c_str());
         if( i == 1 )                         bore      = atof(x.c_str());
         if( i == 2 && !overrideDetDistance ) a         = atof(x.c_str());
         if( i == 3 )                         w         = atof(x.c_str());
         if( i == 4 )                         posRecoil = atof(x.c_str());
         if( i == 5 )                         rhoRecoil = atof(x.c_str());
         if( i == 6 )                         l         = atof(x.c_str());
         if( i == 7 )                         support   = atof(x.c_str());
         if( i == 8 && !overrideFirstPos )    firstPos  = atof(x.c_str());
         if( i == 9 )                         mDet      = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      nDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < nDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      printf("================ B-field: %8.2f T, Theta : %6.2f deg \n", Bfield, BfieldTheta * TMath::RadToDeg());
      printf("==== Recoil detector pos: %8.2f mm, radius: %6.2f mm \n", posRecoil, rhoRecoil);
      printf("========= First Position: %8.2f mm \n", firstPos);
      printf("====== gap of multi-loop: %8.2f mm \n", firstPos > 0 ? firstPos - support : firstPos + support );
      for(int i = 0; i < nDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i], pos[i] + l);
         }else{
            printf("%d, %8.2f mm - %8.2f mm \n", i, pos[i] - l , pos[i]);
         }
      }
      printf("=======================\n");
		isDetReady = true;
   
   }else{
       printf("... fail\n");
       printf("    ----> Use no-detector setting.\n");
       isDetReady = false;
   }
   
   return isDetReady;  
}

int HELIOS::CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB, double xOff, double yOff){
   
   //initialization
   int hit = 0;
   const double c = 299.792458; // mm/ns, standard speed for MeV conversion
   theta = TMath::QuietNaN();
   e = TMath::QuietNaN();
   z = TMath::QuietNaN();
   x = TMath::QuietNaN();
   t = TMath::QuietNaN();
   rho = TMath::QuietNaN();
   dphi = TMath::QuietNaN();
   detID = -1;
   loop = -1;
   detRowID = -1;
   
   eB = TMath::QuietNaN();
   zB = TMath::QuietNaN();
   tB = TMath::QuietNaN();
   rhoB = TMath::QuietNaN();
   rxHit = TMath::QuietNaN();
   ryHit = TMath::QuietNaN();
   phiB = TMath::QuietNaN();
   
   //rotate Pb and PB to B-Field 
   Pb.RotateX(BfieldTheta);
   PB.RotateX(BfieldTheta);

   //====================== X-Y plane
   rho = Pb.Pt()  / Bfield / Zb / c * 1000; //mm
   theta = Pb.Theta();
   phi = Pb.Phi();

   if( isDetReady == false ) {
      //====================== infinite small detector   
      vt0 = Pb.Beta() * TMath::Sin(theta) * c ; // mm / nano-second  
      vp0 = Pb.Beta() * TMath::Cos(theta) * c ; // mm / nano-second  
      t = TMath::TwoPi() * rho / vt0; // nano-second   
      z = vp0 * t; // mm        
      e = Pb.E() - Pb.M();
      dphi = TMath::TwoPi();
      
		//======================  recoil detector
      thetaB = PB.Theta();
      phiB = PB.Phi();
      rhoB = PB.Pt() / Bfield / ZB / c * 1000; //mm
      vt0B = Pb.Beta() * TMath::Sin(thetaB) * c ; // mm / nano-second  
      vp0B = PB.Beta() * TMath::Cos(thetaB) * c ; // mm / nano-second  
      tB   = TMath::TwoPi() * rhoB / vt0B; // nano-second
      eB   = PB.E() - PB.M();
      zB   = vp0 * tB;
     
      return 0;
   }
   
   if( bore > 2 * rho && ((firstPos > 0 && theta < TMath::PiOver2())  || (firstPos < 0 && theta > TMath::PiOver2())) ){
      //====================== infinite small detector   
      vt0 = Pb.Beta() * TMath::Sin(theta) * c ; // mm / nano-second  
      vp0 = Pb.Beta() * TMath::Cos(theta) * c ; // mm / nano-second  
      t0 = TMath::TwoPi() * rho / vt0; // nano-second   
      z0 = vp0 * t0; // mm        
      
      //======================  recoil detector
      thetaB = PB.Theta();
      phiB = PB.Phi();
      rhoB = PB.Pt() / Bfield / ZB / c * 1000; //mm
      tB   = posRecoil / (PB.Beta() * TMath::Cos(thetaB) * c ); // nano-second
      eB   = PB.E() - PB.M();
      zB   = vp0 * tB;
      
      //========= check is particle-b was blocked by recoil detector
      rhoHit = GetR(posRecoil) ;// radius of light particle b at recoil detector
      if( z0 > 0 && posRecoil > 0 && z0 > posRecoil && rhoHit < rhoRecoil) {
          return -1 ;  // when particle-b blocked by recoil detector
      }
      if( z0 < 0 && posRecoil < 0 && z0 < posRecoil && rhoHit < rhoRecoil) {
          return -1 ;
      }
      //========== calculate particle-B hit radius on recoil dectector
      vt0B = PB.Beta() * TMath::Sin(thetaB) * c ; // mm / nano-second  
      vp0B = PB.Beta() * TMath::Cos(thetaB) * c ; // mm / nano-second  
      rhoBHit = GetRecoilR(posRecoil);
      rxHit = GetRecoilXPos(posRecoil);
      ryHit = GetRecoilYPos(posRecoil);
      if(isCoincidentWithRecoil && rhoBHit > rhoRecoil ) return -2; // when particle-B miss the recoil detector

      //================ Calulate z hit
      double zHit = TMath::QuietNaN();
      bool isHit = false;
      bool isHitFromOutside = false;
      bool redoFlag = false;
      
      loop = 0;
      int startJ = (int) fmod(TMath::Ceil(mDet*phi/TMath::TwoPi() - 0.5) ,mDet) ;

      do{
         loop += 1;
         int n = 2*loop -1;
         
         if( loop > 50 ) {
            return -3;  // when loop > 50
            break; // maximum 50 loops
         }
         
         for( int j = startJ ; j < startJ + mDet; j++){
         
            double phiDet = TMath::TwoPi() / mDet * (j); // detector plane angle 
            redoFlag = false;
            isHitFromOutside = false;
            
            //========== calculate zHit
            double aEff = a - (xOff * TMath::Cos(phiDet) + yOff * TMath::Sin(phiDet));
            
            zHit = rho / TMath::Tan(theta) * ( phiDet - phi + TMath::Power(-1, n) * TMath::ASin(aEff/rho + TMath::Sin(phi-phiDet)) + TMath::Pi() * n );
            if( firstPos > 0 ){
               if( zHit < pos[0] )  redoFlag = true;
               if(zHit > pos[nDet-1] + l) return -4; // since the zHit is mono-increse, when zHit shoot over the detector
            }else{
               if( pos[nDet-1] < zHit )redoFlag = true;
               if( zHit < pos[0] - l) return -4; 
            }
            
            //======== this is the particel direction (normalized) dot normal vector of the detector plane
            double dir = TMath::Cos( TMath::Tan(theta) * zHit/ rho + phi - phiDet);
            if( dir < 0) isHitFromOutside = true;
            // when dir == 0, no solution

            // calculate the distance from middle of detector
            double xHit = GetXPos(zHit) + xOff;
            double yHit = GetYPos(zHit) + yOff;
            double sHit = TMath::Sqrt(xHit*xHit + yHit*yHit - a*a);
         
            //======= check Block
            if( firstPos > 0 ){
               if( pos[0] > zHit && zHit > pos[0] - support && sHit < a/2. ) return -5; // blocked by support
            }else{
               if( pos[nDet-1] < zHit && zHit < pos[nDet-1] + support && sHit < a/2.) return -5;
            }
            
            //====== check hit
            if( !redoFlag && isHitFromOutside && sHit < w/2.){      
               isHit = true;
               redoFlag = false;
               detRowID = (j+mDet) % mDet;
               break;     // if isHit, break, don't calculate for the rest of the detector
            }else{
               redoFlag = true;
            }
         }      
      }while(redoFlag); 
      
      if( !isHit ) return -6; // zHit falls outside the detector, but could be in the gap of detector
      
      //===== final calculation for light particle
      e = Pb.E() - Pb.M();
      z = zHit;
      t = zHit / vp0;
      dphi = t * vt0 / rho;
      
      //=========== check hit on detector gap
      for( int i = 0 ; i < nDet ; i++){
         if( firstPos > 0 ){
            if( pos[i] < z  && z < pos[i] + l ){
               detID = i;
               x = ( z - (pos[i] + l/2 ))/ l*2 ;// range from -1 , 1 
            }
         }else{
            if( pos[i] - l < z  && z < pos[i] ){
               detID = i;
               x = ( z - (pos[i] - l/2 ))/ l*2 ;// range from -1 , 1 
            }
         }
      }
      
      if( detID >=0  ){
         hit = 1;      
      }else{
         hit = -7; // particle-b hit the gap
      }
   }else{
      hit = -8;
   }

   return hit;
}

//=======================================================
//#######################################################
// Class for multi-scattering of the beam inside target
// using SRIM to generate stopping power
// input : TLorentzVector, data_files
// output : scattered TLorentzVector
//=======================================================
class TargetScattering{

public:
   TargetScattering();
   ~TargetScattering();
   
   double GetKE0(){return KE0;}
   double GetKE() {return KE;}
   double GetKELoss() {return KE0-KE;}
   double GetDepth() {return depth;}
   double GetPathLength() {return length;}
   
   void LoadStoppingPower(string file);
   
   void SetTarget(double density, double depth){
      this->density = density;
      this->depth = depth;
      isTargetSet = true;
      //printf("===== Target, density: %f g/cm3, depth: %f um \n", density, depth * 1e+4 );
   }
   
   TLorentzVector Scattering(TLorentzVector P){
      double mass = P.M();
      KE0 = (P.E() - mass);
      KE = KE0;
      double theta = P.Theta();   
      this->length = TMath::Abs(depth/TMath::Cos(theta));
      //printf("------- theta: %f deg, length: %f um, KE: %f MeV \n", theta * TMath::RadToDeg(), this->length * 1e+4, KE);
      
      //integrate the energy loss within the depth of A
      double dx = length/100.; 
      double x = 0;
      double densityUse = density;
      if( unitID == 0 ) densityUse = 1.;
      do{
         //assume the particle will not be stopped
         //printf(" x: %f, KE:  %f, S: %f \n", x, KE, gA->Eval(KE));
         KE = KE - densityUse * gA->Eval(KE) * 10. * dx ; // factor 10, convert MeV/mm -> MeV/cm
         x = x + dx;
      }while(x < length);
      
      //printf(" depth: %f cm = %f um, KE : %f -> %f MeV , dE = %f MeV \n", depth, depth * 1e+4, KE0, KE, KE0 - KE);
      
      double newk = TMath::Sqrt(TMath::Power(mass+KE,2) - mass * mass);
      
      TVector3 vb = P.Vect();
      vb.SetMag(newk);
      
      TLorentzVector Pnew;
      Pnew.SetVectM(vb,mass);
      
      return Pnew;
   }
   
private:
   bool isTargetSet;
   double density; // density [mg/cm2]
   int unitID; // 0 = MeV /mm or keV/um , 1 = MeV / (ug/cm2) 
   
   double depth; // depth in target [cm]
   double length; // total path length in target [cm]
   double KE0, KE;
   
   TGraph * gA; // stopping power of A, b, B, in unit of MeV/(mg/cm2)
      
};

TargetScattering::TargetScattering(){
   isTargetSet = false;
   density = 1; // mg/cm2
   unitID = 0; 
   KE0 = 0;
   KE = 0;
   depth = 0;
   length = 0;
   gA = NULL;
}

TargetScattering::~TargetScattering(){
   delete gA;
}

void TargetScattering::LoadStoppingPower(string filename){
   
   printf("===== loading Stopping power: %s.", filename.c_str());
   
   ifstream file;
   file.open(filename.c_str());
   
   vector<double> energy;
   vector<double> stopping;
   
   if( file.is_open() ){
      printf("... OK\n");
      char lineChar[16635];
      string line;
      while( !file.eof() ){
         file.getline(lineChar, 16635, '\n');
         line = lineChar;

         size_t found = line.find("Target Density");
         if( found != string::npos ){
            printf("    %s\n", line.c_str());
         }
         
         found = line.find("Stopping Units =");
         if( found != string::npos){         
            printf("    %s\n", line.c_str());
            if( line.find("MeV / mm") != string::npos ) { 
               unitID = 0;
            }else if( line.find("keV / micron") != string::npos ){
               unitID = 0;
            }else if( line.find("MeV / (mg/cm2)") != string::npos ) {
               unitID = 1;
            }else{
               printf("please using MeV/mm, keV/um, or MeV/(mg/cm2) \n");  
            }
         }
         
         found = line.find("keV   ");
         if ( found != string::npos){
            energy.push_back( atof(line.substr(0,7).c_str()) * 0.001);
            double a = atof(line.substr(14,10).c_str());
            double b = atof(line.substr(25,10).c_str());
            stopping.push_back(a+b);
         }
         
         found = line.find("MeV   ");
         if ( found != string::npos){
            energy.push_back( atof(line.substr(0,7).c_str()));
            double a = atof(line.substr(14,10).c_str());
            double b = atof(line.substr(25,10).c_str());
            stopping.push_back(a+b);
         }
      }
      
   }else{
       printf("... fail\n");
   }
   
   gA = new TGraph(energy.size(), &energy[0], &stopping[0]);
   
}
//=======================================================
//#######################################################
// Class for Particle Decay
// B --> d + D
//  input : TLorentzVector, emitting particle
// output : scattered TLorentzVector
//=======================================================
class Decay{
public:
   Decay();
   ~Decay();
   
   double GetQValue() { return Q;}
   
   double GetAngleChange(){
      TVector3 vD = PD.Vect();
      TVector3 vB = PB.Vect();
      vD.SetMag(1);
      vB.SetMag(1);
      double dot = vD.Dot(vB);
      return TMath::ACos(dot)*TMath::RadToDeg() ;
   }
    
   double GetCMMomentum(){ return k;}
   TLorentzVector GetDaugther_d() {return Pd;}
   TLorentzVector GetDaugther_D() {return PD;}
   
   void SetMotherDaugther(int AB, int zB, int AD, int zD){
      Isotope Mother(AB, zB);
      Isotope Daugther_D(AD, zD);
      Isotope Daugther_d(AB-AD, zB-zD);
      
      printf("====== decay mode : %s --> %s + %s \n", Mother.Name.c_str(), Daugther_d.Name.c_str(), Daugther_D.Name.c_str());

      mB = Mother.Mass;
      mD = Daugther_D.Mass;
      md = Daugther_d.Mass;
      
      isMotherSet = true;
   }
   
   int CalDecay(TLorentzVector P, double ExB, double ExD){
      if( !isMotherSet ) {
         return -1;
      }
      this->PB = P;
      
      double MB = mB + ExB;
      double MD = mD + ExD;
      Q = MB - MD - md;
      if( Q < 0 ) {
         this->PD = this->PB;
         dTheta = TMath::QuietNaN();
         k = TMath::QuietNaN();
         return -2;
      }
      
      //clear 
      TLorentzVector temp(0,0,0,0);
      PD = temp;
      Pd = temp;
      
      k = TMath::Sqrt((MB+MD+md)*(MB+MD-md)*(MB-MD+md)*(MB-MD-md))/2./MB;
      
      //in mother's frame, assume isotropic decay
      double theta = TMath::ACos(2 * gRandom->Rndm() - 1) ; 
      double phi = TMath::TwoPi() * gRandom->Rndm();
      PD.SetE(TMath::Sqrt(mD * mD + k * k ));
      PD.SetPz(k);
      PD.SetTheta(theta);
      PD.SetPhi(phi);
      
      Pd.SetE(TMath::Sqrt(md * md + k * k ));
      Pd.SetPz(k);
      Pd.SetTheta(theta + TMath::Pi());
      Pd.SetPhi(phi + TMath::Pi());
      
      //Transform to Lab frame;
      TVector3 boost = PB.BoostVector();
      
      PD.Boost(boost);
      Pd.Boost(boost);
      
      return 1;      
   }
   
private:
   TLorentzVector PB, Pd, PD;
   
   double mB, mD, md;
   
   bool isMotherSet;
   double Q;
   double k; // momentum in B-frame
   double dTheta; // change of angle
};

Decay::Decay(){
   TLorentzVector temp(0,0,0,0);
   PB = temp;
   Pd = temp;
   PD = temp;
  
   mB = TMath::QuietNaN();
   mD = TMath::QuietNaN();
   md = TMath::QuietNaN(); 
   
   k = TMath::QuietNaN();
   
   Q = TMath::QuietNaN();
   dTheta = TMath::QuietNaN();
   isMotherSet = false;
}

Decay::~Decay(){
   
}

//=======================================================
//#######################################################
// Class for Knockout Reaction
// A(a,12)B, A = B + b, a->1, b->2
// incident particle is A
// the calculation: 1) go to A's rest frame
//                  2) calculate the b = A - B
//                  3) go to CM frame
//=======================================================
class Knockout{
public:
   Knockout();
   ~Knockout();
   
   void SetA(int A, int Z){
      Isotope temp(A,Z);
      mA = temp.Mass;
      AA = A;
      ZA = Z;
      nameA = temp.Name;
   }
   
   void SetExA(double Ex){
      this->ExA = Ex; 
   }
   
   void Seta(int A, int Z){
      Isotope temp(A,Z);
      ma = temp.Mass;
      Aa = A;
      Za = Z;
      m1 = ma;
      A1 = A;
      Z1 = Z;
      namea = temp.Name;
      name1 = temp.Name;
      
      if( namea == "1H " ) {
         namea = "p";
         name1 = "p";
      }
   }

   void Set2(int A, int Z){
      Isotope temp(A,Z);
      m2 = temp.Mass;
      A2 = A;
      Z2 = Z;
      name2 = temp.Name;
      
      if( name2 == "1H " ) {
         name2 = "p";
      }
      
      AB = AA + Aa - A1 - A2;
      ZB = ZA + Za - Z1 - Z2;
      Isotope temp2(AB,ZB);
      mB0 = temp2.Mass;
      nameB = temp2.Name;
      
   }
   
   void SetBSpk(double S, double kb, double thetab, double phib){
      this->S = S;
      AB = AA + Aa - A1 - A2;
      ZB = ZA + Za - Z1 - Z2;
      Isotope temp(AB,ZB);
      mB0 = temp.Mass;
      nameB = temp.Name;
      this->kb = kb;
      this->thetab = thetab;
      this->phib = phib;
      
      mB = mA + ExA - m2 + S;
      
      ExB = mB - mB0;

      if( ExB < 0 && !isOverRideExNegative ){
         printf(" seperation energy is too small. \n");
      }
   }
   
   void OverRideExNegative(bool YN){
      isOverRideExNegative = YN;
   }   
   
   TString GetReactionName(){
      TString rName;
      
      TString normInv;
      if( isNormalKinematics ){
         normInv = "Normal Kinematics";
      }else{
         normInv = "Inverse Kinematics";
      }
      
      rName.Form("%s(%s,%s%s)%s, %s", nameA.c_str(), namea.c_str(), name1.c_str(), name2.c_str(), nameB.c_str(), normInv.Data()); 
   
      return rName;
   }
   
   void SetIncidentEnergyAngle(double KEA, double theta, double phi){
      this->KEA = KEA;
      this->thetaIN = theta;
      this->phiIN = phi;
   }
   
   void CalIncidentChannel(bool isNormalKinematics); 
   void CalReactionConstant(bool isNormalKinematics);
   void Event(double thetaCM, double phCM);
   
   double GetMass_A(){return mA;}
   double GetMass_a(){return ma;}
   double GetMass_b(){return mb;}
   double GetMass_B(){return mB;}
   double GetMass_Bgs(){return mB0;}
   double GetMass_1(){return m1;}
   double GetMass_2(){return m2;}
   
   TLorentzVector GetPA(){return PA;}
   TLorentzVector GetPa(){return Pa;}
   TLorentzVector GetPb(){return Pb;}
   TLorentzVector GetPB(){return PB;}
   TLorentzVector GetP1(){return P1;}
   TLorentzVector GetP2(){return P2;}
   
   double GetMomentumbNN(){return p;}
   double GetReactionBeta(){return beta;}
   double GetReactionGamma(){return gamma;}
   double GetNNTotalEnergy(){return Etot;}
   
   double GetNNTotalKE() {return Etot - mb - ma;}
   double GetQValue() {return mA + ExA - m2 - mB;}
   double GetMaxExB() {return Etot - mb - mB0;}
   
private:   
   int AA, Aa, A1, A2, AB;
   int ZA, Za, Z1, Z2, ZB;
   double mA, ma, m1, m2, mB, mB0, mb;
   string nameA, namea, name1, name2, nameB;
   
   double S; // separation energy
   double kb; // momentum of b
   double thetab, phib;// direction of b
   
   TLorentzVector PA, Pa, P1, P2, PB, Pb; // lab
   
   double KEA, thetaIN, phiIN;
   double T;
   
   double k, beta, gamma, Etot, p; // reaction constant, in NN frame
   double ExA, ExB;

   bool isNormalKinematics;
   bool isOverRideExNegative;
   
};

Knockout::Knockout(){
   TLorentzVector temp(0,0,0,0);
   
   PA = temp;
   Pa = temp;
   P1 = temp;
   P2 = temp;
   PB = temp;
   Pb = temp;
   
   SetA(12,6);
   Seta(1,1);
   Set2(1,1);
   
   SetBSpk(1000, 0, 0, 0);
   SetIncidentEnergyAngle(10, 0, 0);
   
   ExA = 0;
   
   isNormalKinematics = false;
   isOverRideExNegative = false;
}

Knockout::~Knockout(){

}

void Knockout::CalIncidentChannel(bool isNormalKinematics){
   if( ExB < 0 && !isOverRideExNegative) return;
   
   this->isNormalKinematics = isNormalKinematics;
   
   if(!isNormalKinematics){
      //===== construct Lab frame 4-momentum
      this->T = KEA * AA;
      double kA = TMath::Sqrt(TMath::Power(mA + ExA + T, 2) - (mA + ExA) * (mA + ExA)); 
      PA.SetXYZM(0, 0, kA, mA + ExA);
      PA.RotateY(thetaIN);
      PA.RotateZ(phiIN);
      
      Pa.SetXYZM(0,0,0,ma);
      
   }else{
      //===== construct 4-momentum
      this->T = KEA * Aa;
      double ka = TMath::Sqrt(TMath::Power(ma + T, 2) - (ma) * (ma)); 
      Pa.SetXYZM(0, 0, ka, ma);
      Pa.RotateY(thetaIN);
      Pa.RotateZ(phiIN);
      
      PA.SetXYZM(0, 0, 0, mA + ExA);  
   }

}

void Knockout::CalReactionConstant(bool isNormalKinematics){
   if( ExB < 0 && !isOverRideExNegative) return;
   
   this->isNormalKinematics = isNormalKinematics;
   
   CalIncidentChannel(isNormalKinematics);
   
   if(!isNormalKinematics){
      
      //===== change to A's rest frame
      TVector3 bA = PA.BoostVector();
      PA.Boost(-bA);
      
      //===== constructe bounded nucleus b
      PB.SetXYZM(0, 0, -kb, mB);
      PB.RotateY(thetab);
      PB.RotateZ(phib);
      
      Pb = PA - PB;
      mb = Pb.M();
   
      //===== change to Lab frame
      Pb.Boost(bA);
      PA.Boost(bA);
      PB.Boost(bA);
      
   }else{
      
      //===== constructe bounded nucleus b
      PB.SetXYZM(0, 0, -kb, mB);
      PB.RotateY(thetab);
      PB.RotateZ(phib);
      
      Pb = PA - PB;
      mb = Pb.M();
      
   }
   
   //====== reaction constant
   k = (Pa+Pb).P();
   double E = (Pa+Pb).E();
   beta = (Pa+Pb).Beta();
   gamma = 1 / TMath::Sqrt(1- beta * beta);   
   Etot = TMath::Sqrt(TMath::Power(E,2) - k * k);
   p = TMath::Sqrt( (Etot*Etot - TMath::Power(m1 + m2,2)) * (Etot*Etot - TMath::Power(m1 - m2 ,2)) ) / 2 / Etot;

	//if( TMath::IsNaN(p) ){
	//	printf(" Mc: %f, m1+m2: %f, kb:%f, thetab:%f, phib:%f\n", Etot, m1+m2, kb, thetab, phib); 
	//}

}

void Knockout::Event(double thetaCM, double phiCM){
   if( ExB < 0 && !isOverRideExNegative ) return;
   
   //===== construct Pcm
   TLorentzVector Pc = Pb + Pa;
   TVector3 bc = Pc.BoostVector();
   
   TLorentzVector Pac = Pa;
   Pac.Boost(-bc);
   TVector3 va = Pac.Vect();
   
   TLorentzVector Pbc = Pb;
   Pbc.Boost(-bc);
   TVector3 vb = Pbc.Vect();
   
   //--- from P1
   TVector3 v1 = va;
   v1.SetMag(p);
   
   TVector3 u1 = va.Orthogonal();
   v1.Rotate(thetaCM, u1);
   v1.Rotate(phiCM + TMath::PiOver2(), va); // somehow, the calculation turn the vector 90 degree.
   
   TLorentzVector P1c;
   P1c.SetVectM(v1, m1);
   
   //--- from P2
   TLorentzVector P2c;
   P2c.SetVectM(-v1, m2);
   
   //---- to Lab Frame
   P1 = P1c;
   P1.Boost(bc);
   P2 = P2c;
   P2.Boost(bc);
   
}

