
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
#include "../Cleopatra/constant.h"

using namespace std;

class WoodsSaxon : public RKFourth{

public:
  WoodsSaxon();
  ~WoodsSaxon(){}

  ///=== output
  vector<double> energy;
  vector<int>    Lvalue;
  vector<double> Jvalue;
  vector<string> orbString;
  vector<double> UatMaxdist; 
  vector<double> errorE; 
  vector<double> UatMaxdistratio;

  //===== Normal declaration
  void SetNucleus(float A, float Z);
  void IsNeutron(){ this->mu = mn; isNeutron = true;}
  void IsProton() { this->mu = mp; isNeutron = false;}
  void SetWSPars(double V0, double r0, double a0, double VSO, double rSO, double aSO, double rc);

  void SetEnergy(double en) { this->T = en; }
  void SetAngularMomentum( int L, double J ) { this->L = L; this->J = J; }
  
  int CalWSEnergies(bool useBarrier , int maxL , double uTorr, double eTorr , int maxLoop, double dKE , bool debug );

  //====== indivuial parameter
  void SetV0(double v0) { this->V0 = v0 ;}
  void Setr0(double r0) { this->r0 = r0 ; this->R0  = r0  * pow(A, 1./3);}
  void SetR0(double R0) { this->R0 = R0 ; this->r0  = R0  / pow(A, 1./3);}
  void Seta0(double a0) { this->a0 = a0 ;}
  
  void SetVSO(double vso) { this->VSO = vso ;}
  void SetrSO(double rso) { this->rSO = rso ; this->RSO  = rSO  * pow(A, 1./3);}
  void SetRSO(double Rso) { this->RSO = Rso ; this->rSO  = RSO  / pow(A, 1./3); }
  void SetaSO(double aso) { this->aSO = aso ;}

  void Setrc(double rc) { this->rc = rc ; this->Rc  = rc  * pow(A, 1./3);}
  void SetRc(double Rc) { this->Rc = rc ; this->rc  = Rc  * pow(A, 1./3);}

  void SetMass(double mass);

  //======== Get parameter
  double GetV0() { return V0; }
  double GetR0() { return R0; }
  double Getr0() { return r0; }
  double Geta0() { return a0; }
  
  double GetVSO() { return VSO; }
  double GetRSO() { return RSO; }
  double GetrSO() { return rSO; }
  double GetaSO() { return aSO; }

  double GetRc() { return Rc; }
  double Getrc() { return rc; }

  //======= other calculations
  void CalRadius();

  double CoulombBarrier(int L);
  double FreeWaveNumber(double Energy);
  
  double RMSRadius(double * radialWF);
  vector<double> CalMomentumWF(double * radialWF, int L);

  void ClearVector();

  void PrintWSParas();
  void PrintEnergyLevels();
  void PrintWaveFunction();

  void SaveWaveFunction(string saveFileName);

private:

  float A, Z, N; /// mass number, charge, neutron number
  double mu; /// nucleon mass;

  /// energy in MeV, distance in fm
  double V0, R0, a0, r0;
  double VSO, RSO, aSO, rSO;
  double Rc, rc;
  int L;
  double J;
  double T;
  double LS;  /// spin-orbital  factor

  bool isNeutron;

  double SphericalBessel(double x, int L );


  //=============== The G-function, d^2u/dr^2 = G(u,r)
  virtual double G(double r, double y, double dy){
    return (2*mu/pow(hbarc,2))*(Pot(r)-T)*y + L*(L+1)*y/pow(r,2);
  }

  //============= Potential
  double Pot(double r){
    /// Wood-Saxon
    double WSCentral = V0/(1+exp((r-R0)/a0));
    /// spin-orbital 
    double SO = - LS * VSO * exp((r-RSO)/aSO) / pow(1+exp((r-RSO)/aSO), 2) / aSO/ r ;
    /// Coulomb
    double Vc = 0;
    if( !isNeutron ){
      if( r > Rc ){
        Vc = Z * ee / r;
      }else{
        Vc = Z * ee * (3*Rc*Rc - r*r)/(2*Rc*Rc*Rc);
      }
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
    if( !isNeutron ){
      if( r > Rc ){
        dVc = - Z * ee / r / r;
      }else{
        dVc = -Z * ee * r / Rc / Rc ; 
      }
    }
    return dWSCentral + dSO + dVc;
  }
};


WoodsSaxon::WoodsSaxon(){

  A = 16;
  N =  8;
  Z =  8;

  V0 = -47;
  r0 =  1.25;
  a0 =  0.67;

  VSO = -47;
  rSO =  1.25;
  aSO =  0.67;

  R0  = r0  * pow(A, 1./3);
  RSO = rSO * pow(A, 1./3);
  Rc  = rc  * pow(A, 1./3);

  L = 0;
  J = 1./2;

  T = -4;
  LS = 0;

  isNeutron = true;

  sol.clear();

}

void WoodsSaxon::SetNucleus(float A, float Z){
  this->A = A;
  this->Z = Z;
  this->N = A-Z;
}

void WoodsSaxon::SetWSPars(double V0, double r0, double a0, double VSO, double rSO, double aSO, double rc = 0){
  this->V0 = V0;
  this->r0 = r0;
  this->a0 = a0;
  
  this->VSO = VSO;
  this->rSO = rSO;
  this->aSO = aSO;

  this->rc = rc;

  this->R0  = r0  * pow(A, 1./3.);
  this->RSO = rSO * pow(A, 1./3.);
  this->Rc  = rc  * pow(A, 1./3.);
}


void WoodsSaxon::ClearVector(){
  energy.clear();
  orbString.clear();
  UatMaxdist.clear();
  errorE.clear();
  UatMaxdistratio.clear();
  sol.clear();
}

void WoodsSaxon::SetMass(double mass){
  this->mu = mass;
}
  
//================  the potential parameters are global values
int WoodsSaxon::CalWSEnergies(bool useBarrier = false, int maxL = 7, double uTorr = 500, double eTorr = 1e-6, int maxLoop = 100, double dKE = 0.2, bool debug = false){

  ///debug = 1;

  energy.clear();
  orbString.clear();
  UatMaxdist.clear();
  errorE.clear();
  UatMaxdistratio.clear();
  
  double u = 0;
  double u2 = 0;
  double uOld = 0;
  double uOld2 = 0;

  if( A < 40 ) maxL = 3;
  if( N < 20 || Z < 20 ) maxL = 2;

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

      SetAngularMomentum(L, J);

      if( VSO == 0. &&  J < L + 0.5 ) continue;

      LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;
      if( debug ) printf(" ----------------  L = %d , J = %3.1f | LS : %f \n", L, J, LS); 
      int nValue = 0;

      for( float KE = V0 ; KE < 0 ; KE = KE + dKE ){ /// the KE is not kinetic energy, just a name

        SetEnergy(KE);
        SolveRK4(0);

        u = GetLastPoint();
        u2 = GetPoint(nStep - 30);
        
        ///vector<double> outputRK = SolveRadialFunction(KE, L); /// RKfouth will output the wavefunction at the furtherst distant.
        ///u = outputRK[0];
        ///u2 = outputRK[2];///wave at r = Max - 50 (for nStep >= 200 ), or Max - 100  for nStep >= 300

        if( KE == V0 ) {
          uOld = u;
          uOld2 = u2;
          continue;
        }

        ///check crossing point 
        if( u * uOld < 0 ) {
          ///use bi-section method to find zero point 
          double e1 = KE - dKE;
          double e2 = KE;
          double w , wr; /// wave at r=Max, and wave ratio = waveMax/wave
          double w2 ;  ///wave at r = Max - 50 (for nStep >= 200 ), or Max - 100  for nStep >= 300
          double sumW , sumWOld; /// w + w2
          double e , de;
          int loop = 0;

          if( e1 <= V0 ) continue;

          if( debug ) printf(" ====  KE , uOld,  u : %f , %f,  %f \n", KE, uOld,  u);

          sumWOld = abs(uOld + uOld2);

          do{
            loop ++;
            e = (e1 + e2)/2.;
            de = abs( e-e1);

            SetEnergy(e);
            SolveRK4(0);
            w = GetLastPoint();
            w2 = GetPoint(nStep - 30);
            

            sumW = abs(w + w2);
            wr = w/GetMaxSolution();
            if( loop > maxLoop ) break;
            
            if( de < 1e-10 ) {
              loop = -404;
              break;
            }
            
            if( debug ) printf("         %3d|  %9.2E< e = %9.2E< %9.2E | de = %.2E (eTorr:%.1E) | w : %9.2E. %9.2E | sumW : %9.2E\n", loop,  e1, e, e2, de, eTorr, w, w2, sumW);

            if( uOld * w < 0 ) {
              e2 = e;
            }else if( w * u < 0 ){
              e1 = e;
            }

            //if( sumW > sumWOld ) break; //TODO, use the minimum of 2 points for best energy.

            sumWOld = sumW;
            
          }while( abs(w) > uTorr || de > eTorr );

          char buffer[100];
          int nn = sprintf(buffer, "%d%s%d/2", nValue, orbital.c_str(), J > L ? 2*L+1 : 2*L-1);

          Lvalue.push_back(L);
          Jvalue.push_back(J);
      
          string temp = buffer;
          orbString.push_back(temp);
          if( useBarrier ) {
            double barrier = CoulombBarrier(L);
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
        uOld2 = u2;

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

        temp = Jvalue[j+1];
        Jvalue[j+1] = Jvalue[j];
        Jvalue[j] = temp;

        int tempInt = Lvalue[j+1];
        Lvalue[j+1] = Lvalue[j];
        Lvalue[j] = tempInt;

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

      Lvalue.erase(Lvalue.begin() + i);
      Jvalue.erase(Jvalue.begin() + i);
      orbString.erase(orbString.begin() + i);
      energy.erase(energy.begin() + i);
      UatMaxdist.erase(UatMaxdist.begin() + i);
      errorE.erase(errorE.begin() + i);
      UatMaxdistratio.erase(UatMaxdistratio.begin() + i);

    }
  }
  return 0;
}


double FreeWaveNumber(double Energy){
  if( Energy < 0 ) return -404;
  return sqrt(2*mn*Energy)/hbarc; 
}

double WoodsSaxon::CoulombBarrier(int L){

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


void WoodsSaxon::PrintEnergyLevels(){
  double rMax = rStart + dr * nStep;
  char uMaxStr[100];
  int nn = sprintf(uMaxStr, "U(%.0ffm)", rMax);
  char uRatioStr[100];
  nn = sprintf(uRatioStr, "U(%.0ffm)/UMax", rMax);

  printf("================ result\n");
  printf("  | %8s,  %12s | %12s | %12s | %12s\n", "orbital", "E [MeV]", "err(E) [keV]", uMaxStr,  uRatioStr);  
  for( int i = 0; i < (int) energy.size() ; i++){
    printf("%2d| %8s,  %12.6f | %12.4E | %12.4f | %12f\n",i, orbString[i].c_str(), energy[i], errorE[i]*1000., UatMaxdist[i], UatMaxdistratio[i]);  
  }
}

void WoodsSaxon::PrintWSParas(){
  printf("================ Woods-Saxon parameters \n");
  printf(" Reduced Mass : %f MeV/c2\n", mu);
  printf("  A: %4.0f, N: %4.0f, dr:%5.3f fm, nStep: %3d, range: %5.3f fm \n", A, N, dr, nStep, dr * nStep);
  printf("-------------------------------\n");
  printf(" V0: %8.4f MeV,  R0: %8.4f(%4.2f) fm,  a0: %8.4f fm \n", V0,   R0,  r0 , a0);
  printf("VSO: %8.4f MeV, RS0: %8.4f(%4.2f) fm, aS0: %8.4f fm \n", VSO,  RSO, rSO, aSO);
  printf("  Z: %8.0f,      Rc: %8.4f(%4.2f) fm\n", Z, Rc, rc);
  printf("================================\n");
}

void WoodsSaxon::PrintWaveFunction(){

  printf("=================== energy : %f MeV, L: %d, J: %.0f/2 \n", T, L, 2*J);

  for( int i = 0 ; i < nStep ; i++){
    double r = rStart + i * dr;
    if( i%50 == 0 ) printf("%12.4f, %12.4f\n", r, sol[i]);
  
  }

}

void WoodsSaxon::SaveWaveFunction(string saveFileName){

  printf("-----------------------------------------------\n");
  printf(" save radial functions in %s \n", saveFileName.c_str());
  
  FILE * paraOut = fopen (saveFileName.c_str(), "w");
  fprintf(paraOut, "#  V0: %6.2f MeV,  R0: %6.2f fm,  a0: %6.2f fm\n", V0,  R0, a0);
  fprintf(paraOut, "# VSO: %6.2f MeV, RSO: %6.2f fm, aSO: %6.2f fm\n", VSO, RSO, aSO);
  fprintf(paraOut, "#  dr: %5.3f fm, nStep: %3d \n", dr, nStep);
  fprintf(paraOut, "#==================================================================\n");

  ///------ recaluclate the orbital.
  int nOrbital = orbString.size();
  double rad[nOrbital][nStep];

  fprintf(paraOut,"#%7s, ", "r[fm]");

  double norm[nOrbital] ;
  
  for( int i = 0; i < nOrbital; i++){

    sol.clear();
    SetEnergy(energy[i]);
    SetAngularMomentum(Lvalue[i], Jvalue[i]);
        
    LS = (J*(J+1) - L*(L+1) - 0.5*(1.5))/2.;

    SolveRK4(0);
    ///PrintWSParas();
    ///PrintWaveFunction();
    
    norm[i] = 0;
    bool setZero = false;
    for( int j = 0; j < nStep; j++) {
      double r = rStart + (j ) * dr;
      if( !setZero ) {
        rad[i][j] = sol[j]/r;
      }else{
        rad[i][j] = 0;
      }
      
      if( dr*j < 15.  ) { /// wave function for > 15 fm should zero
        norm[i] += pow(rad[i][j] * r, 2);
      }else{
        if( abs(rad[i][j-1]) > abs(rad[i][j]) ) {
          norm[i] += pow(rad[i][j] * r, 2);
        }else{
          setZero = true;
        }
      }
    }
    if( L == 0 ) {
      rad[i][0] = rad[i][1];
    }else{
      rad[i][0] = 0;
    }
    norm[i] = sqrt(norm[i]*dr);
    fprintf(paraOut, "%6s(%7.3f), ", orbString[i].c_str(), T);

  }

  fprintf(paraOut,"%8s, ", "k[MeV/c]");
  fprintf(paraOut, "\n");

  ///--------- normalize the wave function
  for( int i = 0 ; i < nOrbital ; i++){
    for( int j = 0; j < nStep; j++) {
      rad[i][j] = rad[i][j]/norm[i];
    }
  }

  ///-------- Calculate wave function in momentum space
  vector<vector<double>> wfK;
  for(  int i = 0 ; i < nOrbital ; i++){
      int L = Lvalue[i];
      vector<double> temp = CalMomentumWF(rad[i], L);
      wfK.push_back(temp);

      printf("   %2d | %12.4f MeV  | %12.4f fm \n", i, energy[i] , RMSRadius(rad[i])); 

  }
  
  
  for( int j = 0; j < nStep; j++){
    fprintf(paraOut, "%8.2f, ", rStart + j * dr);
    for( int i = 0; i < nOrbital ; i ++ ) fprintf(paraOut, "%15.6f, ", rad[i][j] );
    
    fprintf(paraOut, "%8.2f, ", j*0.01 * 197.327);
    for( int i = 0; i < nOrbital ; i ++ ) fprintf(paraOut, "%15.6f, ", wfK[i][j] );

    fprintf(paraOut, "\n");
  }
  
  
  fclose(paraOut);


}

double WoodsSaxon::SphericalBessel(double x, int L ){

  if( x == 0.0 ) {
    return L == 0 ? 1.0 : 0.0;
  }

  double fs = 0;
  double fc = 0;
  switch (L) {
    case 0 : fs =      1.0                                             ; fc =       0.0                                         ; break;
    case 1 : fs =      1.0                                             ; fc =      -1.0                                         ; break;
    case 2 : fs =      3. -        x*x                                 ; fc =      -3.0                                         ; break;
    case 3 : fs =     15. -     6.*x*x                                 ; fc =     -15. +        x*x                             ; break;
    case 4 : fs =    105. -    45.*x*x +       pow(x,4)                ; fc =    -105. +    10.*x*x                             ; break;
    case 5 : fs =    945. -   420.*x*x +   15.*pow(x,4)                ; fc =    -945. +   105.*x*x -      pow(x,4)             ; break;
    case 6 : fs =  10395. -  4725.*x*x +  210.*pow(x,4) -     pow(x,6) ; fc =  -10395. +  1260.*x*x -  21.*pow(x,4)             ; break;
    case 7 : fs = 135135. - 62370.*x*x + 3150.*pow(x,4) - 28.*pow(x,6) ; fc = -135135. + 17325.*x*x - 378.*pow(x,4) + pow(x,6)  ; break;
  }

  return fs * sin(x) / pow(x,L+1) + fc * cos(x) / pow(x,L);

}


double WoodsSaxon::RMSRadius(double * radialWF){

  double rms = 0;
  double norm = 0;
  
  for( int i = 1 ; i < nStep ; i++ ){
    double r = rStart + (i ) * dr;
    rms += pow(radialWF[i] * r * r, 2);
    norm += pow(radialWF[i] * r, 2);
  }

  ///printf(" rms : %f , norm : %f \n", rms, norm);
  
  rms = sqrt(rms / norm);

  return rms;

}


vector<double> WoodsSaxon::CalMomentumWF(double * radialWF, int L){

  vector < double> mont;
  mont.clear();

  //TODO fast -fouirer transform??

  for( int k = 0; k < nStep; k ++){

    double integral = 0;
    
    for( int j = 1; j < nStep ; j ++){ 
      double r = rStart + j * dr;
      integral += SphericalBessel(k*r*0.01, L) * radialWF[j] * r * r;
    }

    mont.push_back( 0.797884560803 * integral); /// the coef is sqrt(2/pi)
    
  }

  return mont;
}




