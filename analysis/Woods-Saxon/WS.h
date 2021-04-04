
/****************************************************************
  Eigen-energies for Wood-Saxon potential with Spin-Orbital.
  Using Rungu-Kuttan-4 method to solve the wavefunction numerically

  When the energy is not eigen-energyies, the wavefunction is diverge. 
  So, by checking the wavefunction at far-away, the eigen-energy can be found.
  
  Made by Ryan Tang (email: goluckyryan@gmail.com) 2019-1-31
*****************************************************************/

#include <stdio.h>
#include <string>
#include <stdlib.h> ///for atoi, atof
#include <cmath>
#include <vector> 
#include "RK4.h"

using namespace std;

class WoodsSaxon : public RKFourth{

public:
  WoodsSaxon(){}
  ~WoodsSaxon(){}
	
  ///=== output
  vector<double> energy;
  vector<string> orbString;
  vector<double> UatMaxdist; 
  vector<double> errorE; 
  vector<double> UatMaxdistratio; 

  float A, N; /// mass number, neutron number
  ///charge number already in class RKFourth
  double r0, rSO, rc; 

  void IsNeutron(){ this->mu = mn;}
  void IsProton(){ this->mu = mp;}

  void ClearVector(){
    energy.clear();
    orbString.clear();
    UatMaxdist.clear();
    errorE.clear();
    UatMaxdistratio.clear();
    wf.clear();
  }
  
  void SetZ(float Z) {
    this->Z = Z;
    if( Z != 0) {
      this->mu = mp;
    }else{
      this->mu = mn;
    }
  }

  void SetMass(double mass){
    this->mu = mass;
  }

  void CalRadius(){
    this->R0  = r0  * pow(A, 1./3.);
    this->RSO = rSO * pow(A, 1./3.);
    this->Rc  = rc  * pow(A, 1./3.);
  }
  
  void SetWSRadius(float A, float Z,  double r0, double rSO, double rc){
    this->A = A;
    this->Z = Z;
    this->N = A-Z;
    this->r0 = r0;
    this->rSO = rSO;
    this->rc = rc;

    this->R0  = r0  * pow(A, 1./3.);
    this->RSO = rSO * pow(A, 1./3.);
    this->Rc  = rc  * pow(A, 1./3.);

    if( Z != 0) {
      this->mu = mp;
    }else{
      this->mu = mn;
    }
  }

  double GetFreeWaveNumber(double Energy){
    if( Energy < 0 ) return 0;
    return sqrt(2*mn*Energy)/hbarc; 
  }

  void PrintEnergyLevels(){
    double rMax = dr * nStep;
    char uMaxStr[100];
    int nn = sprintf(uMaxStr, "U(%.0ffm)", rMax);
    char uRatioStr[100];
    nn = sprintf(uRatioStr, "UMax/U(%.0ffm)", rMax);

    printf("================ result\n");
    printf("  | %8s,  %12s | %12s | %12s | %12s\n", "orbital", "E [MeV]", "err(E) [keV]", uMaxStr,  uRatioStr);  
    for( int i = 0; i < (int) energy.size() ; i++){
      printf("%2d| %8s,  %12.6f | %12.4f | %12.4f | %12f\n",i, orbString[i].c_str(), energy[i], errorE[i]*1000., UatMaxdist[i], UatMaxdistratio[i]);  
    }
  }

  void PrintWSParas(){
    printf("================ Woods-Saxon parameters \n");
    printf(" Reduced Mass : %f MeV/c2\n", mu);
    printf("  A: %4.0f, N: %4.0f, dr:%5.3f fm, nStep: %3d, range: %5.3f fm \n", A, N, dr, nStep, dr * nStep);
    printf("-------------------------------\n");
    printf(" V0: %8.4f MeV,  R0: %8.4f(%4.2f) fm,  a0: %8.4f fm \n", V0,   R0,  r0 , a0);
    printf("VSO: %8.4f MeV, RS0: %8.4f(%4.2f) fm, aS0: %8.4f fm \n", VSO,  RSO, rSO, aSO);
    printf("  Z: %8.0f,      Rc: %8.4f(%4.2f) fm\n", Z, Rc, rc);
    printf("================================\n");
  }

  
  //============ find Coulomb Barrier
  double FindBarrier(int L){

    if( L == 0 ) return 0;
    
    ///finding the zero crossing point of derivative
    double step = 0.1;
    double rMax = R0;
    double t1 = 100, t2 = -1000;
    bool isFound = false;
    for( float r = R0; r < 20 * R0; r += step ) {

      ///using tracing method
      t1 = Pot(r) + pow( hbarc, 2) * L *(L + 1.) / pow(r,2) / 2 / mu;
      if( t1 > t2 ) t2 = t1;

      ///printf(" r : %f fm | t1 : %f | t2 : %f \n", r, t1 , t2);
      if( t1 < t2 ) {
        rMax = r;
        isFound = true;
        break;
      }
    }

    double barrier = 0;

    if( isFound ) {
      ///rMax = rMax + t1 / t2 * step; // when using 1st derivative
      barrier = Pot(rMax) + pow( hbarc, 2) * L *(L + 1.) / pow(rMax,2) / 2 / mu;
      if( barrier < 0 ) barrier = 0;
      ///printf(" max position = %f fm | %f  \n", rMax, barrier);
    }else{
      ///printf("max position not found for L = %d \n", L);
    }
    
    return  barrier;
    
  }


  //============= Potential
  double Pot(double r){
    /// Wood-Saxon
    double WSCentral = V0/(1+exp((r-R0)/a0));
    /// spin-orbital 
    double SO = - LS * VSO * exp((r-RSO)/aSO) / pow(1+exp((r-RSO)/aSO), 2) / aSO/ r ;
    /// Coulomb
    double Vc = 0;
    if( r > Rc ){
      Vc = Z * ee / r;
    }else{
      Vc = Z * ee * (3*Rc*Rc - r*r)/(2*Rc*Rc*Rc);
    }
    return WSCentral + SO + Vc;
  }

  //============= derivative of the Potential
  double dPot(double r){
    ///derivative of Wood-Saxon
    double dWSCentral = - V0 * exp((r-R0)/a0) / pow(1+exp((r-R0)/a0), 2) / a0;
    ///derivative of spin-orbital
    double dSO = LS * VSO * pow(1./cosh((r-RSO)/2./aSO),2) * ( aSO + r * tanh((r-RSO)/2./aSO) ) / 4 / aSO/ aSO / r / r;
    ///derivative of Coulomb
    double dVc = 0;
    if( r > Rc ){
      dVc = - Z * ee / r / r;
    }else{
      dVc = -Z * ee * r / Rc / Rc ; 
    }
    return dWSCentral + dSO + dVc;
  }

  //================  the potential parameters are global values
  int CalWSEnergies(bool useBarrier = false, int maxL = 7, double uTorr = 500, double eTorr = 0.001, int maxLoop = 100, double dKE = 0.2, bool debug = false){

    energy.clear();
    orbString.clear();
    UatMaxdist.clear();
    errorE.clear();
    UatMaxdistratio.clear();
    
    double u = 0;
    double uOld = 0;

    if( A < 40 ) maxL = 3;
    if( N < 20 ) maxL = 2;
  
    for( int L = 0 ; L <= maxL; L++ ){

      string orbital;

      if( L == 0 ) orbital = "s";
      if( L == 1 ) orbital = "p";
      if( L == 2 ) orbital = "d";
      if( L == 3 ) orbital = "f";
      if( L == 4 ) orbital = "g";
      if( L == 5 ) orbital = "h";
      if( L == 6 ) orbital = "i";
      if( L == 7 ) orbital = "j";
      if( L == 8 ) orbital = "k";

      for( float J = L + 0.5 ;  J >= abs(L - 0.5) ; J = J - 1. ){

        if( VSO == 0. &&  J < L + 0.5 ) continue;

        LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;
        if( debug ) printf(" ----------------  L = %d , J = %3.1f | LS : %f \n", L, J, LS); 
        int nValue = 0;

        for( float KE = V0 ; KE < 0 ; KE = KE + dKE ){
              
          vector<double> outputRK = SolveRadialFunction(KE, L); // RKfouth will output the wavefunction at the furtherst distant.
          u = outputRK[0];

          if( KE == V0 ) {
            uOld = u;
            continue;
          }

          ///check crossing point 
          if( u * uOld < 0 ) {
            ///use bi-section method to find zero point 
            double e1 = KE - dKE;
            double e2 = KE;
            double w , wr; /// wave, and wave ratio = waveMax/wave
            double e , de;
            int loop = 0;

            if( e1 <= V0 ) continue;

            if( debug ) printf(" ====  KE , uOld,  u : %f , %f,  %f \n", KE, uOld,  u);

            do{
              loop ++;
              e = (e1 + e2)/2.;
              de = abs( e-e1);
              vector<double> out = SolveRadialFunction(e, L);
              w = out[0];
              wr = out[0]/out[1];
              if( loop > maxLoop ) break;
              
              if( de < 1e-6 ) {
                loop = -404;
                break;
              }
              
              if( debug ) printf("         %3d|  %f< e = %f< %f | de = %.2E (eTorr:%.1E) | w : %f\n", loop,  e1, e, e2, de, eTorr, w);

              if( uOld * w < 0 ) {
                e2 = e;
                continue;
              }

              if( w * u < 0 ){
                e1 = e;
                continue;
              }
            }while( abs(w) > uTorr || de > eTorr );

            char buffer[100];
            int nn = sprintf(buffer, "%d%s%d/2", nValue, orbital.c_str(), J > L ? 2*L+1 : 2*L-1);

            string temp = buffer;
            orbString.push_back(temp);
            if( useBarrier ) {
              double barrier = FindBarrier(L);
              if( debug ) printf(" J=%3.1f, L = %d,  barrier = %f \n", J, L, barrier); 
              energy.push_back(e - barrier);
            }else{
              energy.push_back(e);
            }
            UatMaxdist.push_back(w);
            errorE.push_back(de);
            UatMaxdistratio.push_back(wr); /// It seems that wRatio is useless
               
            if( (0 < loop && loop < maxLoop) || (abs(w) < uTorr && de < eTorr && wr < 0.01)) nValue++;
               
            if( debug ) printf(" %s : %12.6f (Last u = %f ), loop %d, de %f, wr %f \n", temp.c_str(),  e, w , loop, de, wr);
          }

          uOld = u;

        } /// end of KE-loop
      } /// end of J-loop
    } /// end of L-loop
	  
    //==================sorting energy, dubble sort
    int i, j ;
    bool swapped;
    int size = energy.size();
    for (i = 0; i < size-1; i++) { 
      swapped = false; 
      for (j = 0; j < size-i-1; j++) { 
        if (energy[j] > energy[j+1]) { 
          double temp = energy[j+1];
          energy[j+1] = energy[j];
          energy[j] = temp;

          string tempstr = orbString[j+1];
          orbString[j+1] = orbString[j];
          orbString[j] = tempstr;

          temp = UatMaxdist[j+1];
          UatMaxdist[j+1] = UatMaxdist[j];
          UatMaxdist[j] = temp;
              
          temp = errorE[j+1];
          errorE[j+1] = errorE[j];
          errorE[j] = temp;
              
          temp = UatMaxdistratio[j+1];
          UatMaxdistratio[j+1] = UatMaxdistratio[j];
          UatMaxdistratio[j] = temp;

          swapped = true; 
        } 
      } 

      /// If no two elements were swapped by inner loop, then break 
      if (swapped == false) break; 
    } 

    ///Erase incorrect energy
    for( int i = energy.size() - 1 ; i >=0  ; i--){
      ///printf("%s, energy: %f, UatMaxdist : |%f|,  uTorr :  %f \n", orbString[i].c_str(), energy[i],  UatMaxdist[i], uTorr); 
      ///if( UatMaxdist[i] > uTorr || UatMaxdist[i] < -uTorr || errorE[i] > eTorr){
      if( abs(UatMaxdist[i]) > uTorr || errorE[i] > eTorr){

        orbString.erase(orbString.begin() + i);
        energy.erase(energy.begin() + i);
        UatMaxdist.erase(UatMaxdist.begin() + i);
        errorE.erase(errorE.begin() + i);
        UatMaxdistratio.erase(UatMaxdistratio.begin() + i);

      }
    }
    return 0;
  }

};



